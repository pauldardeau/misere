// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <string>

#include <cstdio>

#include "HttpClient.h"
#include "HTTP.h"
#include "HttpResponse.h"
#include "Socket.h"
#include "BasicException.h"
#include "KeyValuePairs.h"
#include "Logger.h"

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


//******************************************************************************

HttpClient::HttpClient() noexcept
{
   Logger::logInstanceCreate("HttpClient");
}

//******************************************************************************

HttpClient::~HttpClient() noexcept
{
   Logger::logInstanceDestroy("HttpClient");
}

//******************************************************************************

void HttpClient::buildHeader(std::string& header,
                             const std::string& address,
                             int port,
                             const std::string& url,
                             const std::string& method,
                             const std::string& contentType,
                             unsigned long contentLength,
                             const KeyValuePairs& kvpAddlHeaders) noexcept
{
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
      host += std::to_string(port);
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
      header += std::to_string(contentLength);
      header += EOL;
   }

   std::vector<std::string> vecKeys;
   kvpAddlHeaders.getKeys(vecKeys);

   for (const std::string& key : vecKeys) {
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

std::string HttpClient::post(const std::string& address,
                             int port,
                             const std::string& url,
                             const std::string& postData,
                             const std::string& contentType,
                             const KeyValuePairs& kvpAddlHeaders)
{
   const auto postDataLength = postData.length();

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

std::string HttpClient::sendReceive(const std::string& address,
                                    int port,
                                    const std::string& sendBuffer)
{
   Socket socket(address.c_str(), port);
   socket.setTcpNoDelay(true);
   socket.setSendBufferSize(SOCKET_SEND_BUFFER_SIZE);
   socket.setReceiveBufferSize(SOCKET_RECV_BUFFER_SIZE);
   socket.setIncludeMessageSize(false);
   
   if (Logger::isLogging(Logger::LogLevel::Debug)) {
      Logger::log(Logger::LogLevel::Debug, "*** start of send data ***");
      Logger::log(Logger::LogLevel::Debug, sendBuffer);
      Logger::log(Logger::LogLevel::Debug, "*** end of send data ***");
   }

   socket.write(sendBuffer);

   HttpResponse response(socket);

   if (Logger::isLogging(Logger::LogLevel::Debug)) {
      Logger::log(Logger::LogLevel::Debug, "*** start of reply data ***");
      Logger::log(Logger::LogLevel::Debug, response.getBody());
      Logger::log(Logger::LogLevel::Debug, "*** end of reply data ***");
   }

   return std::string(response.getBody());
}

//******************************************************************************

