// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <vector>
#include <map>

#include "HttpTransaction.h"
#include "KeyValuePairs.h"


class Socket;


/*!
 * HttpRequest is used by an HTTP server for parsing an HTTP request.
 */
class HttpRequest : public HttpTransaction
{
   public:
      HttpRequest(Socket& socket);
      HttpRequest(const HttpRequest& copy) noexcept;
      HttpRequest(HttpRequest&& move) noexcept;

      virtual ~HttpRequest() noexcept;


      HttpRequest& operator=(const HttpRequest& copy) noexcept;
      HttpRequest& operator=(HttpRequest&& move) noexcept;


      virtual bool streamFromSocket(Socket& socket) override;
   
      bool isInitialized() const noexcept;

      const std::string& getRequest() const noexcept;
      const std::string& getMethod() const noexcept;
      const std::string& getPath() const noexcept;

      const std::string& getRequestLine() const noexcept;

      bool hasArgument(const std::string& key) const noexcept;
      const std::string& getArgument(const std::string& key) const noexcept;
      void getArgumentKeys(std::vector<std::string>& vecKeys) const noexcept;

   protected:
      void parseBody() noexcept;



   private:
      std::string m_method;
      std::string m_path;
      KeyValuePairs m_arguments;
      bool m_initialized;

};

#endif


