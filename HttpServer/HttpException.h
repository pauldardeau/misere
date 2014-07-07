// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__HttpException__
#define __HttpServer__HttpException__

#include <string>

#include "BasicException.h"

/**
 * HttpException is an exception that contains an HTTP status code.
 */
class HttpException : public BasicException
{
private:
   int m_statusCode;
   
   
public:
   /**
    * Constructs and HttpException using the given status code and reason
    * @param statusCode the HTTP status code
    * @param reasonPhrase textual reason for exception
    */
   HttpException(int statusCode,
                 const std::string& reasonPhrase) noexcept;
   
   /**
    * Copy constructor
    * @param copy the source of the copy
    */
   HttpException(const HttpException& copy) noexcept;
   
   /**
    * Move constructor
    * @param move the source of the move
    */
   HttpException(HttpException&& move) noexcept;
   
   /**
    * Destructor
    */
   ~HttpException() noexcept;
   
   /**
    * Copy operator
    * @param copy the source of the copy
    * @return reference to the updated object
    */
   HttpException& operator=(const HttpException& copy) noexcept;
   
   /**
    * Move operator
    * @param move the source of the move
    * @return reference to the target of the move
    */
   HttpException& operator=(HttpException&& move) noexcept;
   
   /**
    * Retrieves the HTTP status code
    * @return the HTTP status code
    */
   int getStatusCode() const noexcept;
   
};


#endif /* defined(__HttpServer__HttpException__) */
