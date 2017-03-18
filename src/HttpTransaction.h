// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_HTTPTRANSACTION_H
#define MISERE_HTTPTRANSACTION_H


#include <string>
#include <vector>

#include "KeyValuePairs.h"
#include "Socket.h"


namespace misere
{

/**
 * HttpTransaction is an abstract base class that provides common logic
 * for both HTTP requests and responses.
 */
class HttpTransaction
{
   public:
      /**
       * Default constructor
       */
      HttpTransaction();
   
      /**
       * Copy constructor
       * @param copy the source of the copy
       */
      HttpTransaction(const HttpTransaction& copy);
   
      /**
       * Destructor
       */
      virtual ~HttpTransaction() {}

      /**
       * Copy operator
       * @param copy the source of the copy
       * @return reference to the target of the copy
       */
      HttpTransaction& operator=(const HttpTransaction& copy);
   
      /**
       * Initializes object by reading from socket
       * @param socket the socket to read the HTTP request or response
       * @see Socket()
       * @return boolean indicating whether the initialization from socket succeeded
       */
      virtual bool streamFromSocket(chaudiere::Socket& socket);

      /**
       * Retrieves the full set of HTTP headers as a single string
       * @return HTTP headers (unparsed)
       */
      const std::string& getRawHeader() const;
   
      /**
       * Retrieves the body (content) associated with the request or response
       * @return the body as text (empty string if none was set)
       */
      const std::string& getBody() const;
   
      /**
       * Sets the body (content) associated with the request or response
       * @param body the content for the request or response
       */
      void setBody(const std::string& body);
   
      /**
       * Determines if the specified header key exists
       * @param headerKey the key being tested for existence in HTTP headers
       * @return boolean indicating if the specified key exists
       */
      bool hasHeaderValue(const std::string& headerKey) const;
   
      /**
       * Retrieves the header value associated with the specified key
       * @param headerKey the HTTP header key whose value is being retrieved
       * @throw InvalidKeyException
       * @return the header value associated with the specified key
       */
      const std::string& getHeaderValue(const std::string& headerKey) const;
   
      /**
       * Retrieves the keys of all the HTTP header key/value pairs
       * @param headerKeys list that will be populated with HTTP header keys
       */
      void getHeaderKeys(std::vector<std::string>& headerKeys) const;
   
      /**
       * Sets HTTP header key/value pair
       * @param key the key of the HTTP header being set
       * @param value the value of the HTTP header being set
       */
      void setHeaderValue(const std::string& key, const std::string& value);
   
      /**
       * Retrieves the protocol (e.g., "HTTP/1.1") of the request
       * @return the protocol
       */
      const std::string& getProtocol() const;
   
      /**
       * Retrieves the HTTP method (e.g., "GET" or "POST")
       * @return the HTTP method for the request
       */
      const std::string& getRequestMethod() const;
   
      /**
       * Retrieves the path for the HTTP request
       * @return the HTTP request path
       */
      const std::string& getRequestPath() const;
   
      /**
       * Returns the first line (request line) of the HTTP request or response
       * @return the request line
       */
      const std::string& getRequestLine() const;
   
      /**
       * Retrieves the HTTP header key/value pairs
       * @param headers HTTP header key/values
       */
      void populateWithHeaders(chaudiere::KeyValuePairs& headers);

   
   protected:
      /**
       * Retrieves the parsed values/tokens of the request line (the first line)
       * @return list of parsed tokens on request line
       */
      const std::vector<std::string>& getRequestLineValues() const;
   
      /**
       * Sets the protocol (e.g., "HTTP/1.1")
       * @param protocol the protocol being used
       */
      void setProtocol(const std::string& protocol);
   
      /**
       * Parse the HTTP headers
       * @return boolean indicating whether the headers were successfully parsed
       */
      bool parseHeaders();


   private:
      std::vector<std::string> m_vecHeaderLines;
      std::vector<std::string> m_vecRequestLineValues;
      std::string m_header;
      std::string m_body;
      std::string m_protocol;
      std::string m_requestLine;
      chaudiere::KeyValuePairs m_headers;
      std::string m_method;
      int m_contentLength;

};

}

#endif

