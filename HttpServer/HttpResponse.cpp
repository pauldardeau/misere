// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cstdio>
#include <cstdlib>

#include "HttpResponse.h"
#include "HTTP.h"
#include "Socket.h"
#include "BasicException.h"
#include "HttpException.h"
#include "Logger.h"

static const std::string TEXT_HTML = "text/html";

//******************************************************************************

HttpResponse::HttpResponse() noexcept :
   m_statusCodeAsInteger(200)
{
   Logger::logInstanceCreate("HttpResponse");
   
   setContentType(TEXT_HTML);
}

//******************************************************************************

HttpResponse::HttpResponse(const HttpResponse& copy) noexcept :
   HttpTransaction(copy),
   m_statusCode(copy.m_statusCode),
   m_reasonPhrase(copy.m_reasonPhrase),
   m_statusCodeAsInteger(copy.m_statusCodeAsInteger)
{
   Logger::logInstanceCreate("HttpResponse");
}

//******************************************************************************

HttpResponse::HttpResponse(HttpResponse&& move) noexcept :
   HttpTransaction(move),
   m_statusCode(std::move(move.m_statusCode)),
   m_reasonPhrase(std::move(move.m_reasonPhrase)),
   m_statusCodeAsInteger(move.m_statusCodeAsInteger)
{
   Logger::logInstanceCreate("HttpResponse");
}

//******************************************************************************

HttpResponse::HttpResponse(Socket& socket)
{
   Logger::logInstanceCreate("HttpResponse");

   streamFromSocket(socket);
}

//******************************************************************************

HttpResponse::~HttpResponse() noexcept
{
   Logger::logInstanceDestroy("HttpResponse");
}

//******************************************************************************

HttpResponse& HttpResponse::operator=(const HttpResponse& copy) noexcept
{
   if (this == &copy) {
      return *this;
   }

   HttpTransaction::operator=(copy);
   m_statusCode = copy.m_statusCode;
   m_reasonPhrase = copy.m_reasonPhrase;
   m_statusCodeAsInteger = copy.m_statusCodeAsInteger;

   return *this;
}

//******************************************************************************

HttpResponse& HttpResponse::operator=(HttpResponse&& move) noexcept
{
   if (this == &move) {
      return *this;
   }
   
   HttpTransaction::operator=(move);
   m_statusCode = std::move(move.m_statusCode);
   m_reasonPhrase = std::move(move.m_reasonPhrase);
   m_statusCodeAsInteger = move.m_statusCodeAsInteger;
   
   return *this;
}

//******************************************************************************

bool HttpResponse::streamFromSocket(Socket& socket)
{
   if (Logger::isLogging(Logger::LogLevel::Debug)) {
      Logger::debug("******** start of HttpResponse::streamFromSocket");
   }
   
   bool streamSuccess = false;

   if (HttpTransaction::streamFromSocket(socket)) {

      const std::vector<std::string>& vecRequestLineValues = getRequestLineValues();

      if (3 == vecRequestLineValues.size()) {
         setProtocol(vecRequestLineValues[0]);
         m_statusCode = vecRequestLineValues[1];
         m_reasonPhrase = vecRequestLineValues[2];

         m_statusCodeAsInteger = std::stoi(m_statusCode);

         if (0 == m_statusCodeAsInteger) {
            throw BasicException("unable to parse status code");
         } else if (m_statusCodeAsInteger >= 500) {
            std::string reasonPhrase;

            switch (m_statusCodeAsInteger)
            {
               case 500:
                  reasonPhrase = "Internal Server Error";
                  break;

               case 501:
                  reasonPhrase = "Not Implemented";
                  break;

               case 502:
                  reasonPhrase = "Bad Gateway";
                  break;

               case 503:
                  reasonPhrase = "Service Unavailable";
                  break;

               case 504:
                  reasonPhrase = "Gateway Timeout";
                  break;

               case 505:
                  reasonPhrase = "HTTP Version not supported";
                  break;
            }

            throw HttpException(m_statusCodeAsInteger, reasonPhrase);

         } else if ((m_statusCodeAsInteger >= 400) && (m_statusCodeAsInteger < 500)) {
            std::string reasonPhrase;

            switch(m_statusCodeAsInteger)
            {
               case 400:
                  reasonPhrase = "Bad Request";
                  break;

               case 401:
                  reasonPhrase = "Unauthorized";
                  break;

               case 402:
                  reasonPhrase = "Payment Required";
                  break;

               case 403:
                  reasonPhrase = "Forbidden";
                  break;

               case 404:
                  reasonPhrase = "Not Found";
                  break;

               case 405:
                  reasonPhrase = "Method Not Allowed";
                  break;

               case 406:
                  reasonPhrase = "Not Acceptable";
                  break;

               case 407:
                  reasonPhrase = "Proxy Authentication Required";
                  break;

               case 408:
                  reasonPhrase = "Request Timeout";
                  break;

               case 409:
                  reasonPhrase = "Conflict";
                  break;

               case 410:
                  reasonPhrase = "Gone";
                  break;

               case 411:
                  reasonPhrase = "Length Required";
                  break;

               case 412:
                  reasonPhrase = "Precondition Failed";
                  break;

               case 413:
                  reasonPhrase = "Request Entity Too Large";
                  break;

               case 414:
                  reasonPhrase = "Request-URI Too Large";
                  break;

               case 415:
                  reasonPhrase = "Unsupported Media Type";
                  break;

               case 416:
                  reasonPhrase = "Request range not satisfiable";
                  break;

               case 417:
                  reasonPhrase = "Expectation Failed";
                  break;
            }

            throw HttpException(m_statusCodeAsInteger, reasonPhrase);
         }
      }
      
      streamSuccess = true;
   } else {
      throw BasicException("unable to parse headers");
   }
   
   return streamSuccess;
}

//******************************************************************************

const std::string& HttpResponse::getStatusLine() const noexcept
{
   return getRequestLine();
}

//******************************************************************************

int HttpResponse::getStatusCode() const noexcept
{
   return m_statusCodeAsInteger;
}

//******************************************************************************

void HttpResponse::setStatusCode(int statusCode) noexcept
{
   m_statusCodeAsInteger = statusCode;
}

//******************************************************************************

const std::string& HttpResponse::getReasonPhrase() const noexcept
{
   return m_reasonPhrase;
}

//******************************************************************************

bool HttpResponse::hasContentEncoding() const noexcept
{
   return hasHeaderValue(HTTP::HTTP_CONTENT_ENCODING);
}

//******************************************************************************

bool HttpResponse::hasContentType() const noexcept
{
   return hasHeaderValue(HTTP::HTTP_CONTENT_TYPE);
}

//******************************************************************************

const std::string& HttpResponse::getContentEncoding() const noexcept
{
   return getHeaderValue(HTTP::HTTP_CONTENT_ENCODING);
}

//******************************************************************************

const std::string& HttpResponse::getContentType() const noexcept
{
   return getHeaderValue(HTTP::HTTP_CONTENT_TYPE);
}

//******************************************************************************

void HttpResponse::setContentEncoding(const std::string& contentEncoding) noexcept
{
   setHeaderValue(HTTP::HTTP_CONTENT_ENCODING, contentEncoding);
}

//******************************************************************************

void HttpResponse::setContentType(const std::string& contentType) noexcept
{
   setHeaderValue(HTTP::HTTP_CONTENT_TYPE, contentType);
}

//******************************************************************************

