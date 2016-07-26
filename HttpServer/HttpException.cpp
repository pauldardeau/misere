// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "HttpException.h"
#include "Logger.h"

using namespace misere;
using namespace chaudiere;

//******************************************************************************

HttpException::HttpException(int statusCode,
                             const std::string& reasonPhrase) noexcept :
   BasicException(reasonPhrase),
   m_statusCode(statusCode)
{
   Logger::logInstanceCreate("HttpException");
}

//******************************************************************************

HttpException::HttpException(const HttpException& copy) noexcept :
   BasicException(copy),
   m_statusCode(copy.m_statusCode)
{
   Logger::logInstanceCreate("HttpException");
}

//******************************************************************************

HttpException::~HttpException() noexcept
{
   Logger::logInstanceDestroy("HttpException");
}

//******************************************************************************

HttpException& HttpException::operator=(const HttpException& copy) noexcept
{
   if (this == &copy) {
      return *this;
   }
   
   BasicException::operator=(copy);
   m_statusCode = copy.m_statusCode;
   
   return *this;
}

//******************************************************************************

int HttpException::getStatusCode() const noexcept
{
   return m_statusCode;
}

//******************************************************************************

