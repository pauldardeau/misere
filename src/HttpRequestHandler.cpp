// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <stdio.h>
#include <string.h>

#include "HttpRequestHandler.h"
#include "Socket.h"
#include "SocketRequest.h"
#include "HttpServer.h"
#include "HTTP.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Thread.h"
#include "BasicException.h"
#include "Logger.h"
#include "StrUtils.h"


static const std::string HTTP_ACCEPT_ENCODING = "accept-encoding";
static const std::string HTTP_CONNECTION      = "Connection:";
static const std::string HTTP_CONTENT_LENGTH  = "content-length:";
static const std::string HTTP_CONTENT_TYPE    = "content-type:";
static const std::string HTTP_DATE            = "date:";
static const std::string HTTP_SERVER          = "server:";
static const std::string HTTP_USER_AGENT      = "User-Agent";

static const std::string CONNECTION_CLOSE     = "close";

static const std::string ZERO                 = "0";
static const std::string FAVICON_ICO          = "/favicon.ico";

static const std::string CONTENT_TYPE_HTML    = "text/html";

static const std::string COUNT_PATH           = "path";
static const std::string COUNT_USER_AGENT     = "user_agent";

static const std::string QUESTION_MARK        = "?";

static const std::string GZIP                 = "gzip";

using namespace misere;
using namespace chaudiere;

//******************************************************************************

HttpRequestHandler::HttpRequestHandler(HttpServer& server,
                                       SocketRequest* socketRequest) :
   RequestHandler(socketRequest),
   m_server(server) {
   Logger::logInstanceCreate("HttpRequestHandler");
}

//******************************************************************************

HttpRequestHandler::HttpRequestHandler(HttpServer& server,
                                       Socket* socket) :
   RequestHandler(socket),
   m_server(server) {
   Logger::logInstanceCreate("HttpRequestHandler");
}

//******************************************************************************

HttpRequestHandler::~HttpRequestHandler() {
   Logger::logInstanceDestroy("HttpRequestHandler");
}

//******************************************************************************

void HttpRequestHandler::run() {
   Socket* socket = getSocket();
   
   if (NULL == socket) {
      Logger::error("no socket or socket request present in RequestHandler");
      return;
   }
   
   socket->setTcpNoDelay(true);
   socket->setSendBufferSize(m_server.getSocketSendBufferSize());
   socket->setReceiveBufferSize(m_server.getSocketReceiveBufferSize());

   const bool isLoggingDebug = Logger::isLogging(Debug);
   if (isLoggingDebug) {
      Logger::debug("starting parse of HttpRequest");
   }
   
   HttpRequest request(socket);

   if (request.isInitialized()) {
      if (isLoggingDebug) {
         Logger::debug("ending parse of HttpRequest");
      }
   
      const std::string& method = request.getMethod();
      const std::string& protocol = request.getProtocol();
      const std::string& path = request.getPath();
      
      std::string routingPath = path;
      
      std::string clientIPAddress;
      socket->getPeerIPAddress(clientIPAddress);
      
      if (StrUtils::containsString(path, QUESTION_MARK)) {
         // strip arguments from path
         const std::string::size_type posQuestionMark =
            path.find(QUESTION_MARK);
         if (posQuestionMark != std::string::npos) {
            routingPath = path.substr(0, posQuestionMark);
         }
      }

      Logger::countOccurrence(COUNT_PATH, routingPath);

      
      if (request.hasHeaderValue(HTTP_USER_AGENT)) {
         Logger::countOccurrence(COUNT_USER_AGENT,
                                 request.getHeaderValue(HTTP_USER_AGENT));
      }
   
      HttpHandler* pHandler = m_server.getPathHandler(routingPath);
      bool handlerAvailable = false;
   
      if (pHandler == NULL) {
         Logger::info("no handler for request: " + routingPath);
      }
   
      // assume the worst
      std::string responseCode = HTTP::HTTP_RESP_SERV_ERR_INTERNAL_ERROR;
      const std::string systemDate = m_server.getSystemDateGMT();
      KeyValuePairs headers;
      headers.addPair(HTTP_CONNECTION, CONNECTION_CLOSE);
      const std::string& serverString = m_server.getServerId();
   
      if (!serverString.empty()) {
         headers.addPair(HTTP_SERVER, serverString);
      }
   
      headers.addPair(HTTP_DATE, systemDate);
      //headers.addPair(HTTP_CONTENT_TYPE, CONTENT_TYPE_HTML);
   
      if ((HTTP::HTTP_PROTOCOL1_0 != protocol) &&
          (HTTP::HTTP_PROTOCOL1_1 != protocol)) {
         responseCode = HTTP::HTTP_RESP_SERV_ERR_HTTP_VERSION_UNSUPPORTED;
         Logger::warning("unsupported protocol: " + protocol);
      } else if (NULL == pHandler) { // path recognized?
         responseCode = HTTP::HTTP_RESP_CLIENT_ERR_NOT_FOUND;
         //Logger::warning("bad request: " + path);
      } else if (!pHandler->isAvailable()) { // is our handler available?
         responseCode = HTTP::HTTP_RESP_SERV_ERR_SERVICE_UNAVAILABLE;
         Logger::warning("handler not available: " + routingPath);
      } else {
         handlerAvailable = true;
      }
   
      const std::string httpHeader = request.getRawHeader();
   
      if (isLoggingDebug) {
         Logger::debug("HttpServer method: " + method);
         Logger::debug("HttpServer path: " + routingPath);
         Logger::debug("HttpServer protocol: " + protocol);
      
         Logger::debug("HttpServer header:");
         Logger::debug(httpHeader);
      }
   
      int contentLength = 0;
      HttpResponse response;
   
      if ((NULL != pHandler) && handlerAvailable) {
         try {
            pHandler->serviceRequest(request, response);
            responseCode = StrUtils::toString(response.getStatusCode());
            const ByteBuffer* responseBody = response.getBody();
            if (responseBody != NULL) {
               contentLength = responseBody->size();
            }

            if ((contentLength > 0) && !response.hasContentEncoding()) {
               if (request.hasAcceptEncoding()) {
                  /*
                  const std::string& acceptEncoding =
                     request.getAcceptEncoding();
                 
                  if (StrUtils::containsString(acceptEncoding, GZIP) &&
                      m_server.compressionEnabled() &&
                      m_server.compressResponse(response.getContentType()) &&
                      contentLength >= m_server.minimumCompressionSize()) {
                     
                     try {
                        const std::string compressedResponseBody =
                           StrUtils::gzipCompress(responseBody);
                        contentLength = compressedResponseBody.size();
                        response.setBody(compressedResponseBody);
                        response.setContentEncoding(GZIP);
                     } catch (const std::exception& e) {
                        Logger::error("unable to compress response");
                     }
                  }
                  */
               }
            }
         
            response.populateWithHeaders(headers);
         } catch (const BasicException& be) {
            responseCode = HTTP::HTTP_RESP_SERV_ERR_INTERNAL_ERROR;
            Logger::error("exception handling request: " + be.whatString());
         } catch (const std::exception& e) {
            responseCode = HTTP::HTTP_RESP_SERV_ERR_INTERNAL_ERROR;
            Logger::error("exception handling request: " + std::string(e.what()));
         } catch (...) {
            responseCode = HTTP::HTTP_RESP_SERV_ERR_INTERNAL_ERROR;
            Logger::error("unknown exception handling request");
         }
      }

      if (contentLength > 0) {
         headers.addPair(HTTP_CONTENT_LENGTH,
                         StrUtils::toString(contentLength));
      } else {
         headers.addPair(HTTP_CONTENT_LENGTH, ZERO);
      }
   
      // log the request
      if (isThreadPooling()) {
         const std::string& runByWorkerThreadId = getRunByThreadWorkerId();
      
         if (!runByWorkerThreadId.empty()) {
            m_server.logRequest(clientIPAddress,
                                request.getFirstHeaderLine(),
                                responseCode,
                                runByWorkerThreadId);
         } else {
            m_server.logRequest(clientIPAddress,
                                request.getFirstHeaderLine(),
                                responseCode);
         }
      } else {
         m_server.logRequest(clientIPAddress,
                             request.getFirstHeaderLine(),
                             responseCode);
      }
   
      std::string headersAsString =
         m_server.buildHeader(responseCode, headers);
      socket->write(headersAsString);
   
      if (contentLength > 0) {
         const ByteBuffer* body = response.getBody();
         if (body != NULL) {
            socket->write(body->const_data(), body->size());
         }
      }

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
   
      //if (m_socketRequest != NULL) {
      //   m_socketRequest->requestComplete();
      //}
       */
   } else {
      ::printf("error: unable to initialize HttpRequest\n");
   }
}

//******************************************************************************

