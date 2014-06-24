// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>

#include "BasicException.h"


#define THROW(exceptionType)


class KeyValuePairs;


/*!
 * HttpClient is used for constructing and executing HTTP requests.
 */
class HttpClient
{
   public:
      HttpClient() noexcept;
      ~HttpClient() noexcept;

      // throws BasicException
      std::string post(const std::string& address,
                     int port,
                     const std::string& host,
                     const std::string& url,
                     const std::string& postData,
                     const std::string& contentType,
						const KeyValuePairs& kvpAddlHeaders);

      // throws BasicException
      std::string sendReceive(const std::string& address,
                              int port,
                              const std::string& sendBuffer);
      // throws BasicException
      std::string sendReceive(const std::string& address,
                              int port,
                              const std::string& url,
                              const std::string& host,
                              const std::string& postData);

      void buildHeader(std::string& header,
                     const std::string& address,
                     int port,
                     const std::string& url,
                     const std::string& method,
                     const std::string& contentType,
                     unsigned long contentLength,
						const KeyValuePairs& kvpAddlHeaders) noexcept;


      std::string buildHttpRequest(const std::string& host,
                                 const std::string& url,
                                 const std::string& postData) noexcept;


};

#endif




