// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef HTTPTRANSACTION_H
#define HTTPTRANSACTION_H


#include <string>
#include <vector>
#include <unordered_map>

class Socket;

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
      HttpTransaction() noexcept;
   
      /**
       * Copy constructor
       * @param copy the source of the copy
       */
      HttpTransaction(const HttpTransaction& copy) noexcept;
   
      /**
       * Move constructor
       * @param move the source of the move
       */
      HttpTransaction(HttpTransaction&& move) noexcept;
   
      /**
       * Destructor
       */
      virtual ~HttpTransaction() noexcept {}

      /**
       * Copy operator
       * @param copy the source of the copy
       * @return reference to the target of the copy
       */
      HttpTransaction& operator=(const HttpTransaction& copy) noexcept;
   
      /**
       * Move operator
       * @param move the source of the move
       * @return reference to the target of the move
       */
      HttpTransaction& operator=(HttpTransaction&& move) noexcept;

      /**
       * Initializes object by reading from socket
       * @param socket the socket to read the HTTP request or response
       * @see Socket()
       * @return boolean indicating whether the initialization from socket succeeded
       */
      virtual bool streamFromSocket(Socket& socket);

      /**
       * Retrieves the full set of HTTP headers as a single string
       * @return HTTP headers (unparsed)
       */
      const std::string& getRawHeader() const noexcept;
   
      /**
       * Retrieves the body (content) associated with the request or response
       * @return the body as text (empty string if none was set)
       */
      const std::string& getBody() const noexcept;
   
      /**
       * Sets the body (content) associated with the request or response
       * @param body the content for the request or response
       */
      void setBody(const std::string& body) noexcept;
   
      /**
       * Determines if the specified header key exists
       * @param headerKey the key being tested for existence in HTTP headers
       * @return boolean indicating if the specified key exists
       */
      bool hasHeaderValue(const std::string& headerKey) const noexcept;
   
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
      void getHeaderKeys(std::vector< std::string >& headerKeys) const noexcept;
   
      /**
       * Sets HTTP header key/value pair
       * @param key the key of the HTTP header being set
       * @param value the value of the HTTP header being set
       */
      void setHeaderValue(const std::string& key, const std::string& value) noexcept;
   
      /**
       * Retrieves the protocol (e.g., "HTTP/1.1") of the request
       * @return the protocol
       */
      const std::string& getProtocol() const noexcept;
   
      /**
       * Retrieves the HTTP method (e.g., "GET" or "POST")
       * @return the HTTP method for the request
       */
      const std::string& getRequestMethod() const noexcept;
   
      /**
       * Retrieves the path for the HTTP request
       * @return the HTTP request path
       */
      const std::string& getRequestPath() const noexcept;
   
      /**
       * Returns the first line (request line) of the HTTP request or response
       * @return the request line
       */
      const std::string& getRequestLine() const noexcept;
   
      /**
       * Retrieves the HTTP header key/value pairs
       * @param hashTable the map to populate with HTTP header key/values
       */
      void populateWithHeaders(std::unordered_map<std::string, std::string>& hashTable) noexcept;

   
   protected:
      /**
       * Retrieves the parsed values/tokens of the request line (the first line)
       * @return list of parsed tokens on request line
       */
      const std::vector<std::string>& getRequestLineValues() const noexcept;
   
      /**
       * Sets the protocol (e.g., "HTTP/1.1")
       * @param protocol the protocol being used
       */
      void setProtocol(const std::string& protocol) noexcept;
   
      /**
       * Parse the HTTP headers
       * @return boolean indicating whether the headers were successfully parsed
       */
      bool parseHeaders() noexcept;


   private:
      std::vector<std::string> m_vecHeaderLines;
      std::vector<std::string> m_vecRequestLineValues;
      std::string m_header;
      std::string m_body;
      std::string m_protocol;
      std::string m_requestLine;
      std::unordered_map<std::string,std::string> m_hashHeaders;
      std::string m_method;
      int m_contentLength;

};

#endif


