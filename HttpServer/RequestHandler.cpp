// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "RequestHandler.h"
#include "Socket.h"
#include "SocketRequest.h"
#include "HttpServer.h"
#include "HTTP.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Thread.h"
#include "BasicException.h"
#include "Logger.h"

static const std::string HTTP_CONTENT_TYPE    = "Content-Type:";
static const std::string HTTP_CONTENT_LENGTH  = "Content-Length:";
static const std::string HTTP_CONNECTION      = "Connection:";
static const std::string HTTP_SERVER          = "Server:";
static const std::string HTTP_DATE            = "Date:";

static const std::string CONNECTION_CLOSE     = "close";

static const std::string ZERO                 = "0";
static const std::string FAVICON_ICO          = "/favicon.ico";

static const std::string CONTENT_TYPE_HTML    = "text/html";


//******************************************************************************

RequestHandler::RequestHandler(HttpServer& server,
                               SocketRequest* socketRequest) noexcept :
   m_server(server),
   m_socket(nullptr),
   m_socketRequest(socketRequest),
   m_isThreadPooling(false)
{
   Logger::logInstanceCreate("RequestHandler");
}

//******************************************************************************

RequestHandler::RequestHandler(HttpServer& server, Socket* socket) noexcept :
   m_server(server),
   m_socket(socket),
   m_socketRequest(nullptr),
   m_isThreadPooling(false)
{
   Logger::logInstanceCreate("RequestHandler");
}

//******************************************************************************

RequestHandler::~RequestHandler() noexcept
{
   Logger::logInstanceDestroy("RequestHandler");

   if (m_socket) {
      m_socket->close();
      delete m_socket;
   }
   
   if (m_socketRequest) {
      delete m_socketRequest;
   }
}

//******************************************************************************

void RequestHandler::run()
{
   Socket* socket = nullptr;
   
   if (m_socket) {
      socket = m_socket;
   } else if (m_socketRequest) {
      socket = m_socketRequest->getSocket();
   }
   
   if (!socket) {
      Logger::error("no socket or socket request present in RequestHandler");
      return;
   }
   
   socket->setTcpNoDelay(true);
   socket->setSendBufferSize(m_server.getSocketSendBufferSize());
   socket->setReceiveBufferSize(m_server.getSocketReceiveBufferSize());

   const bool isLoggingDebug = Logger::isLogging(Logger::LogLevel::Debug);
   if (isLoggingDebug) {
      Logger::debug("starting parse of HttpRequest");
   }
   
   HttpRequest request(*socket);

   if (isLoggingDebug) {
      Logger::debug("ending parse of HttpRequest");
   }
   
   const std::string& method = request.getMethod();
   const std::string& protocol = request.getProtocol();
   const std::string& path = request.getPath();
   
   HttpHandler* pHandler = m_server.getPathHandler(path);
   bool handlerAvailable = false;
   
   if (pHandler == nullptr) {
      Logger::info("no handler for request: " + path);
   }
   
   // assume the worst
   std::string responseCode = HTTP::HTTP_RESP_SERV_ERR_INTERNAL_ERROR;
   
   const std::string systemDate = m_server.getSystemDateGMT();
   
   std::unordered_map<std::string, std::string> mapHeaders;
   
   mapHeaders[HTTP_CONNECTION] = CONNECTION_CLOSE;
   
   const std::string& serverString = m_server.getServerId();
   
   if (!serverString.empty()) {
      mapHeaders[HTTP_SERVER] = serverString;
   }
   
   mapHeaders[HTTP_DATE] = systemDate;
   mapHeaders[HTTP_CONTENT_TYPE] = CONTENT_TYPE_HTML;
   
   if (HTTP::HTTP_PROTOCOL != protocol) {
      responseCode = HTTP::HTTP_RESP_SERV_ERR_HTTP_VERSION_UNSUPPORTED;
      Logger::warning("unsupported protocol: " + protocol);
   } else if (nullptr == pHandler) { // path recognized?
      responseCode = HTTP::HTTP_RESP_CLIENT_ERR_BAD_REQUEST;
      Logger::warning("bad request: " + path);
   } else if (!pHandler->isAvailable()) { // is our handler available?
      responseCode = HTTP::HTTP_RESP_SERV_ERR_SERVICE_UNAVAILABLE;
      Logger::warning("handler not available: " + path);
   } else {
      handlerAvailable = true;
   }
   
   const std::string httpHeader = request.getRawHeader();
   const std::string httpBody = request.getBody();
   
   if (isLoggingDebug) {
      Logger::debug("HttpServer method: " + method);
      Logger::debug("HttpServer path: " + path);
      Logger::debug("HttpServer protocol: " + protocol);
      
      Logger::debug("HttpServer header:");
      Logger::debug(httpHeader);
      
      Logger::debug("HttpServer body:");
      Logger::debug(httpBody);
   }
   
   std::string responseBody;
   
   if ((nullptr != pHandler) && handlerAvailable) {
      try
      {
         HttpResponse response;
         pHandler->serviceRequest(request, response);
         responseCode = std::to_string(response.getStatusCode());
         responseBody = response.getBody();
         
         response.populateWithHeaders(mapHeaders);
      }
      catch (const BasicException& be)
      {
         responseCode = HTTP::HTTP_RESP_SERV_ERR_INTERNAL_ERROR;
         Logger::error("exception handling request: " + be.whatString());
      }
      catch (const std::exception& e)
      {
         responseCode = HTTP::HTTP_RESP_SERV_ERR_INTERNAL_ERROR;
         Logger::error("exception handling request: " + std::string(e.what()));
      }
      catch (...)
      {
         responseCode = HTTP::HTTP_RESP_SERV_ERR_INTERNAL_ERROR;
         Logger::error("unknown exception handling request");
      }
      
      
      if (!responseBody.empty()) {
         mapHeaders[HTTP_CONTENT_LENGTH] = std::to_string(responseBody.length());
      } else {
         mapHeaders[HTTP_CONTENT_LENGTH] = ZERO;
      }
   } else {
      // is it a GET request for /favicon.ico?
      if ((responseCode == HTTP::HTTP_RESP_CLIENT_ERR_METHOD_NOT_ALLOWED) &&
         (method == HTTP::HTTP_METHOD_GET) &&
         (path == FAVICON_ICO)) {
         // send a not found (404)
         responseCode = HTTP::HTTP_RESP_CLIENT_ERR_NOT_FOUND;
      }
   }
   
   std::string clientIPAddress;
   
   socket->getPeerIPAddress(clientIPAddress);
   
   // log the request
   if (m_isThreadPooling) {
      const std::string& runByWorkerThreadId = getRunByThreadWorkerId();
      
      if (!runByWorkerThreadId.empty()) {
         m_server.logRequest(clientIPAddress,
                            request.getRequestLine(),
                            responseCode,
                            runByWorkerThreadId);
      } else {
         m_server.logRequest(clientIPAddress,
                            request.getRequestLine(),
                            responseCode);
      }
   } else {
      m_server.logRequest(clientIPAddress,
                         request.getRequestLine(),
                         responseCode);
   }
   
   std::string response = m_server.buildHeader(responseCode, mapHeaders);
   
   if (!responseBody.empty()) {
      response += responseBody;
   }
   
   socket->write(response);
   
   /*
   if (isLoggingDebug) {
      Logger::debug("response written, calling read so that client can close first");
   }
   
   // invoke a read to give the client the chance to close the socket
   // first. this also lets us easily detect the close on the client
   // end of the connection.  we won't actually read any data here,
   // this is just a wait to allow the client to close first
   char readBuffer[5];
   socket->read(readBuffer, 4);
   */
   
   //if (m_socketRequest != nullptr) {
   //   m_socketRequest->requestComplete();
   //}
   
   socket->close();
}

//******************************************************************************

void RequestHandler::setThreadPooling(bool isThreadPooling) noexcept
{
   m_isThreadPooling = isThreadPooling;
}

//******************************************************************************

