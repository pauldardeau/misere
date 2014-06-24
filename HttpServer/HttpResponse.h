// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <string>

#include "HttpTransaction.h"


class Socket;


/*!
 * HttpRequest is used by an HTTP client for parsing an HTTP response.
 */
class HttpResponse : public HttpTransaction
{
   public:
      HttpResponse() noexcept;
      HttpResponse(Socket& socket);
      HttpResponse(const HttpResponse& copy) noexcept;
      HttpResponse(HttpResponse&& move) noexcept;
      ~HttpResponse() noexcept;

      HttpResponse& operator=(const HttpResponse& copy) noexcept;
      HttpResponse& operator=(HttpResponse&& move) noexcept;

      virtual bool streamFromSocket(Socket& socket) override;

      const std::string& getStatusLine() const noexcept;
      int getStatusCode() const noexcept;
      void setStatusCode(int statusCode) noexcept;
      const std::string& getReasonPhrase() const noexcept;


   private:
      std::string m_statusCode;
      std::string m_reasonPhrase;
      int m_statusCodeAsInteger;

};

#endif


