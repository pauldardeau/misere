// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_HTTPEXCEPTION_H
#define MISERE_HTTPEXCEPTION_H

#include <string>

#include "BasicException.h"

namespace misere
{

/**
 * HttpException is an exception that contains an HTTP status code.
 */
class HttpException : public chaudiere::BasicException
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
                 const std::string& reasonPhrase);

   /**
    * Copy constructor
    * @param copy the source of the copy
    */
   HttpException(const HttpException& copy);

   /**
    * Destructor
    */
   ~HttpException() throw ();

   /**
    * Copy operator
    * @param copy the source of the copy
    * @return reference to the updated object
    */
   HttpException& operator=(const HttpException& copy);

   /**
    * Retrieves the HTTP status code
    * @return the HTTP status code
    */
   int getStatusCode() const;

};

}

#endif
