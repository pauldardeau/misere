// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__HttpException__
#define __HttpServer__HttpException__

#include <string>

#include "BasicException.h"

/*!
 * HttpException is an exception that contains an HTTP status code.
 */
class HttpException : public BasicException
{
private:
   int m_statusCode;
   
   
public:
   HttpException(int statusCode,
                 const std::string& reasonPhrase) noexcept;
   
   HttpException(const HttpException& copy) noexcept;
   HttpException(HttpException&& move) noexcept;
   ~HttpException() noexcept;
   
   HttpException& operator=(const HttpException& copy) noexcept;
   HttpException& operator=(HttpException&& move) noexcept;
   
   int getStatusCode() const noexcept;
   
};


#endif /* defined(__HttpServer__HttpException__) */
