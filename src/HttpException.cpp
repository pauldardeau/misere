// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "HttpException.h"
#include "Logger.h"

using namespace misere;
using namespace chaudiere;

//******************************************************************************

HttpException::HttpException(int statusCode,
                             const std::string& reasonPhrase) :
   BasicException(reasonPhrase),
   m_statusCode(statusCode) {
   LOG_INSTANCE_CREATE("HttpException")
}

//******************************************************************************

HttpException::HttpException(const HttpException& copy) :
   BasicException(copy),
   m_statusCode(copy.m_statusCode) {
   LOG_INSTANCE_CREATE("HttpException")
}

//******************************************************************************

HttpException::~HttpException() throw () {
   LOG_INSTANCE_DESTROY("HttpException")
}

//******************************************************************************

HttpException& HttpException::operator=(const HttpException& copy) {
   if (this == &copy) {
      return *this;
   }
   
   BasicException::operator=(copy);
   m_statusCode = copy.m_statusCode;
   
   return *this;
}

//******************************************************************************

int HttpException::getStatusCode() const {
   return m_statusCode;
}

//******************************************************************************

