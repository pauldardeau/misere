// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_HTTPRESPONSE_H
#define MISERE_HTTPRESPONSE_H

#include <string>

#include "HttpTransaction.h"
#include "Socket.h"


namespace misere
{

/**
 * HttpRequest is used by an HTTP client for parsing an HTTP response.
 */
class HttpResponse : public HttpTransaction
{
   public:
      /**
       * Default constructor
       */
      HttpResponse();
   
      /**
       * Constructs and HttpResponse by reading from socket
       * @param socket the socket to read from
       * @see Socket()
       * @throw BasicException
       * @throw HttpException
       */
      explicit HttpResponse(chaudiere::Socket& socket);
   
      /**
       * Copy constructor
       * @param copy the source of the copy
       */
      HttpResponse(const HttpResponse& copy);
   
      /**
       * Destructor
       */
      ~HttpResponse();

      /**
       * Copy operator
       * @param copy the source of the copy
       * @return reference to the updated instance
       */
      HttpResponse& operator=(const HttpResponse& copy);
   
      /**
       * Initializes HttpResponse instance by reading data from socket
       * @param socket the socket to read from for initialization
       * @return boolean indicating whether initialization succeeded
       */
      virtual bool streamFromSocket(chaudiere::Socket& socket);

      /**
       * Retrieves the HTTP status code
       * @return HTTP status code value
       */
      int getStatusCode() const;
   
      /**
       * Sets the HTTP status code
       * @param statusCode HTTP status code value
       */
      void setStatusCode(int statusCode);
   
      /**
       * Retrieves the textual description of the HTTP status
       * @return textual description of HTTP status
       */
      const std::string& getReasonPhrase() const;
   
      /**
       * Determines if Content-encoding header field value exists
       * @return boolean indicating if header field exists
       */
      bool hasContentEncoding() const;
   
      /**
       * Determines if Content-type header field value exists
       * @return boolean indicating if header field exists
       */
      bool hasContentType() const;
   
      /**
       * Retrieves the value for the Content-encoding HTTP header field
       * @return the value for the Content-encoding field
       */
      const std::string& getContentEncoding() const;
   
      /**
       * Retrieves the value for the Content-type HTTP header field
       * @return the value for the Content-type field
       */
      const std::string& getContentType() const;
   
      /**
       * Sets the Content-encoding HTTP header field
       * @param contentEncoding the value to use for Content-encoding
       */
      void setContentEncoding(const std::string& contentEncoding);
   
      /**
       * Sets the Content-type HTTP header field
       * @param contentType the value to use for Content-type
       */
      void setContentType(const std::string& contentType);

      void close();

      int getContentLength() const; 

      void setContentLength(int contentLength);


   private:
      std::string m_statusCode;
      std::string m_reasonPhrase;
      int m_statusCodeAsInteger;

};

}

#endif


