// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_HTTPCLIENT_H
#define MISERE_HTTPCLIENT_H

#include <string>

#include "BasicException.h"
#include "KeyValuePairs.h"


#define THROW(exceptionType)


namespace misere
{

/**
 * HttpClient is used for constructing and executing HTTP requests.
 */
class HttpClient
{
   public:
      /**
       * Constructor
       */
      HttpClient() noexcept;
   
      /**
       * Destructor
       */
      ~HttpClient() noexcept;

      /**
       * Sends an HTTP post to HTTP server and returns response
       * @param address the server address (IP address or server name)
       * @param port the port that the server is listening on
       * @param url the url of the server
       * @param postData any POST payload data (may be empty)
       * @param contentType the HTTP Content-type header field value
       * @param kvpAddlHeaders collection of additional key/value pairs for headers
       * @see KeyValuePairs()
       * @throw HttpException
       * @throw BasicException
       * @return the HTTP response as a string
       */
      std::string post(const std::string& address,
                       int port,
                       const std::string& url,
                       const std::string& postData,
                       const std::string& contentType,
                       const chaudiere::KeyValuePairs& kvpAddlHeaders);

      /**
       * Opens a socket, sends the sendBuffer and returns the response
       * @param address the server address (IP address or server name)
       * @param port the port that the server is listening on
       * @param sendBuffer the full request including headers and payload
       * @throw HttpException
       * @throw BasicException
       * @return the HTTP response as a string
       */
      std::string sendReceive(const std::string& address,
                              int port,
                              const std::string& sendBuffer);

      /**
       * Build HTTP request headers from components
       * @param header the string to hold the constructed headers as text
       * @param address the server address (IP address or server name)
       * @param port the port that the server is listening on
       * @param url the url of the server
       * @param method the HTTP method
       * @param contentType the HTTP Content-type header value
       * @param contentLength the HTTP Content-length header value
       * @param kvpAddlHeaders collection of additional HTTP header key/value pairs
       * @see KeyValuePairs()
       */
      void buildHeader(std::string& header,
                       const std::string& address,
                       int port,
                       const std::string& url,
                       const std::string& method,
                       const std::string& contentType,
                       unsigned long contentLength,
                       const chaudiere::KeyValuePairs& kvpAddlHeaders) noexcept;

};

}

#endif




