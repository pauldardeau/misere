// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <string>
#include <stdio.h>
#include <string.h>

#include "HttpClient.h"
#include "HTTP.h"
#include "HttpResponse.h"
#include "Socket.h"
#include "BasicException.h"
#include "KeyValuePairs.h"
#include "Logger.h"
#include "StrUtils.h"

static const int SOCKET_SEND_BUFFER_SIZE = 8192;
static const int SOCKET_RECV_BUFFER_SIZE = 8192;


static const std::string SPACE = " ";
static const std::string EOL   = "\r\n";
static const std::string COLON = ":";

// header keys
static const std::string HOST             = "Host: ";
static const std::string CONTENT_LENGTH   = "Content-Length: ";
static const std::string CONTENT_TYPE     = "Content-Type: ";
static const std::string CONNECTION       = "Connection: ";

static const std::string CONNECTION_CLOSE = "Connection: close";

using namespace misere;
using namespace chaudiere;

//******************************************************************************

HttpClient::HttpClient() {
   LOG_INSTANCE_CREATE("HttpClient")
}

//******************************************************************************

HttpClient::~HttpClient() {
   LOG_INSTANCE_DESTROY("HttpClient")
}

//******************************************************************************

Socket* HttpClient::socketForRequest(const HttpRequest& request)
{
   int port = request.port();
   if (port == 0) {
      port = 80;
   }
   return new Socket(request.host(), port);
}

HttpResponse* HttpClient::get(HttpRequest& request)
{
   HttpResponse* r = NULL;
   Socket* s = socketForRequest(request);
   if (s != NULL) {
      request.setMethod(HTTP::HTTP_METHOD_GET);
      if (request.write(s)) {
         r = new HttpResponse(s);
      }
   }

   return r;
}

HttpResponse* HttpClient::head(HttpRequest& request)
{
   HttpResponse* r = NULL;
   Socket* s = socketForRequest(request);
   if (s != NULL) {
      request.setMethod(HTTP::HTTP_METHOD_HEAD);
      if (request.write(s)) {
         r = new HttpResponse(s);
      }
   }

   return r;
}

HttpResponse* HttpClient::put(HttpRequest& request,
                              const std::string& buffer)
{
   HttpResponse* r = NULL;
   Socket* s = socketForRequest(request);
   if (s != NULL) {
      request.setMethod(HTTP::HTTP_METHOD_PUT);
      if (request.write(s, buffer.size()) &&
          s->write(EOL.c_str(), EOL.size()) &&
          s->write(buffer.c_str(), buffer.size())) {
         r = new HttpResponse(s);
      }
   }

   return r;
}

HttpResponse* HttpClient::put(HttpRequest& request,
                              const ByteBuffer& buffer)
{
   
   HttpResponse* r = NULL;
   Socket* s = socketForRequest(request);
   if (s != NULL) {
      request.setMethod(HTTP::HTTP_METHOD_PUT);
      if (request.write(s, buffer.size()) &&
          s->write(EOL.c_str(), EOL.size()) &&
          s->write((const char*) buffer.const_data(), buffer.size())) {
         r = new HttpResponse(s);
      }
   }

   return r;
}

HttpResponse* HttpClient::post(HttpRequest& request,
                               const std::string& buffer)
{
   HttpResponse* r = NULL;
   Socket* s = socketForRequest(request);
   if (s != NULL) {
      request.setMethod(HTTP::HTTP_METHOD_POST);
      if (request.write(s, buffer.size()) &&
          s->write(EOL.c_str(), EOL.size()) &&
          s->write(buffer.c_str(), buffer.size())) {
         r = new HttpResponse(s);
      }
   }

   return r;
}

HttpResponse* HttpClient::post(HttpRequest& request,
                               const ByteBuffer& buffer)
{
   HttpResponse* r = NULL;
   Socket* s = socketForRequest(request);
   if (s != NULL) {
      request.setMethod(HTTP::HTTP_METHOD_POST);
      if (request.write(s, buffer.size()) &&
          s->write(EOL.c_str(), EOL.size()) &&
          s->write((const char*) buffer.const_data(), buffer.size())) {
         r = new HttpResponse(s);
      }
   }

   return r;
}

HttpResponse* HttpClient::do_delete(HttpRequest& request)
{
   HttpResponse* response = NULL;
   Socket* s = socketForRequest(request);
   if (s != NULL) {
      request.setMethod(HTTP::HTTP_METHOD_DELETE);
      if (request.write(s)) {
         response = new HttpResponse(s);
      }
   }

   return response;
}

void HttpClient::buildHeader(std::string& header,
                             const std::string& address,
                             int port,
                             const std::string& url,
                             const std::string& method,
                             const std::string& contentType,
                             unsigned long contentLength,
                             const KeyValuePairs& kvpAddlHeaders) {
   bool haveContent = false;  // assume we don't

   if ((method == HTTP::HTTP_METHOD_POST) && (contentLength > 0)) {
      haveContent = true;
   }

   // first line
   // Example: "POST /SomeURL HTTP/1.1"
   header = method;
   header += SPACE;
   header += url;
   header += SPACE;
   header += HTTP::HTTP_PROTOCOL1_0;  // HTTP/1.1
   header += EOL;

   // host
   header += HOST;

   if (80 == port) {
      header += address;
   } else {
      std::string host = address;
      host += COLON;
      host += StrUtils::toString(port);
      header += host;
   }

   header += EOL;

   // close connection
   header += CONNECTION_CLOSE;
   header += EOL;

   if (haveContent) {
      // content type
      header += CONTENT_TYPE;
      header += contentType;
      header += EOL;

      // content length
      header += CONTENT_LENGTH;
      header += StrUtils::toString(contentLength);
      header += EOL;
   }

   std::vector<std::string> vecKeys;
   kvpAddlHeaders.getKeys(vecKeys);
   const std::vector<std::string>::const_iterator itEnd = vecKeys.end();
   std::vector<std::string>::const_iterator it = vecKeys.begin();

   for (; it != itEnd; it++) {
      const std::string& key = *it;
      const std::string& value = kvpAddlHeaders.getValue(key);

      header += key;
      header += COLON;
      header += SPACE;
      header += value;
      header += EOL;
   }

   header += EOL;
}

//******************************************************************************

HttpResponse* HttpClient::post(const std::string& address,
                             int port,
                             const std::string& url,
                             const std::string& postData,
                             const std::string& contentType,
                             const KeyValuePairs& kvpAddlHeaders) {
   const std::size_t postDataLength = postData.length();

   std::string requestPayload;

   buildHeader(requestPayload,
               address,
               port,
               url,
               HTTP::HTTP_METHOD_POST,
               contentType,
               postDataLength,
               kvpAddlHeaders);

   if (postDataLength > 0) {
      requestPayload += postData;
   }

   return sendReceive(address, port, requestPayload);
}

//******************************************************************************

HttpResponse* HttpClient::sendReceive(const std::string& address,
                                    int port,
                                    const std::string& sendBuffer) {
   Socket* socket = new Socket(address.c_str(), port);
   socket->setTcpNoDelay(true);
   socket->setSendBufferSize(SOCKET_SEND_BUFFER_SIZE);
   socket->setReceiveBufferSize(SOCKET_RECV_BUFFER_SIZE);
   socket->setIncludeMessageSize(false);
   
   if (Logger::isLogging(LogLevel::Debug)) {
      LOG_DEBUG("*** start of send data ***")
      LOG_DEBUG(sendBuffer)
      LOG_DEBUG("*** end of send data ***")
   }

   socket->write(sendBuffer);

   return new HttpResponse(socket);
}

//******************************************************************************

