// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_HTTPREQUEST_H
#define MISERE_HTTPREQUEST_H

#include <string>
#include <vector>
#include <map>

#include "HttpTransaction.h"
#include "KeyValuePairs.h"
#include "Socket.h"


namespace misere
{

/**
 * HttpRequest is used by an HTTP server for parsing an HTTP request.
 */
class HttpRequest : public HttpTransaction
{
   public:
      /**
       * Constructs and initializes an HttpRequest object from a socket
       * @param socket the socket to read for initializing the object
       * @see Socket()
       */
      explicit HttpRequest(chaudiere::Socket& socket);
   
      /**
       * Copy constructor
       * @param copy the source of the copy
       */
      HttpRequest(const HttpRequest& copy) noexcept;
   
      /**
       * Move constructor
       * @param move the source of the move
       */
      HttpRequest(HttpRequest&& move) noexcept;

      /**
       * Destructor
       */
      virtual ~HttpRequest() noexcept;

      /**
       * Assignment operator
       * @param copy the source of the copy
       * @return reference to updated object
       */
      HttpRequest& operator=(const HttpRequest& copy) noexcept;
   
      /**
       * Move operator
       * @param move source of the move
       * @return reference to updated object
       */
      HttpRequest& operator=(HttpRequest&& move) noexcept;

      /**
       * Initializes HTTP request by reading from socket
       * @param socket the socket to read from
       * @see Socket()
       * @return boolean indicating whether reading from the socket succeeded
       */
      virtual bool streamFromSocket(chaudiere::Socket& socket) override;
   
      /**
       * Determines if the request object has been successfully initialized
       * @return boolean indicating whether the object was initialized
       */
      bool isInitialized() const noexcept;

      /**
       * Retrieves the request line for the request
       * @return the request line value
       */
      const std::string& getRequest() const noexcept;
   
      /**
       * Retrieves the HTTP method for the request
       * @return the method value
       */
      const std::string& getMethod() const noexcept;
   
      /**
       * Retrieves the path for the request
       * @return the path value
       */
      const std::string& getPath() const noexcept;

      /**
       * Determines if the specified key exists in the arguments
       * @param key the key whose existence is being tested
       * @return boolean indicating whether the specified key exists in the arguments
       */
      bool hasArgument(const std::string& key) const noexcept;
   
      /**
       * Retrieves the value associated with the specified argument key
       * @param key the key whose value is being retrieved
       * @throw InvalidKeyException
       * @return the value for the associated key
       */
      const std::string& getArgument(const std::string& key) const noexcept;
   
      /**
       * Retrieves the keys for all arguments given with the request
       * @param vector to be populated with argument keys
       */
      void getArgumentKeys(std::vector<std::string>& vecKeys) const noexcept;

      /**
       * Determines if the Accept header is present
       * @return boolean indicating if the header value is present
       */
      bool hasAccept() const noexcept;
   
      /**
       * Determines if the Accept-encoding header is present
       * @return boolean indicating if the header value is present
       */
      bool hasAcceptEncoding() const noexcept;
   
      /**
       * Determines if the Accept-language header is present
       * @return boolean indicating if the header value is present
       */
      bool hasAcceptLanguage() const noexcept;
   
      /**
       * Determines if the Connection header is present
       * @return boolean indicating if the header value is present
       */
      bool hasConnection() const noexcept;
   
      /**
       * Determines if the Do Not Track (dnt) header is present
       * @return boolean indicating if the header value is present
       */
      bool hasDNT() const noexcept;
   
      /**
       * Determines if the Host header is present
       * @return boolean indicating if the header value is present
       */
      bool hasHost() const noexcept;
   
      /**
       * Determines if the User-agent header is present
       * @return boolean indicating if the header value is present
       */
      bool hasUserAgent() const noexcept;

      /**
       * Retrieves the value associated with the Accept header
       * @return the specified HTTP header value
       */
      const std::string& getAccept() const;
   
      /**
       * Retrieves the value associated with the Accept-encoding header
       * @return the specified HTTP header value
       */
      const std::string& getAcceptEncoding() const;
   
      /**
       * Retrieves the value associated with the Accept-language header
       * @return the specified HTTP header value
       */
      const std::string& getAcceptLanguage() const;
   
      /**
       * Retrieves the value associated with the Connection header
       * @return the specified HTTP header value
       */
      const std::string& getConnection() const;
   
      /**
       * Retrieves the value associated with the Do Not Track (dnt) header
       * @return the specified HTTP header value
       */
      const std::string& getDNT() const;
   
      /**
       * Retrieves the value associated with the Host header
       * @return the specified HTTP header value
       */
      const std::string& getHost() const;
   
      /**
       * Retrieves the value associated with the User-agent header
       * @return the specified HTTP header value
       */
      const std::string& getUserAgent() const;

   
   protected:
      /**
       * Parse the body (if present in the request)
       */
      void parseBody() noexcept;



   private:
      std::string m_method;
      std::string m_path;
      chaudiere::KeyValuePairs m_arguments;
      bool m_initialized;

};

}

#endif


