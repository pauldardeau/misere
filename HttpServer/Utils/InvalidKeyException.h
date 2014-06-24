// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__InvalidKeyException__
#define __HttpServer__InvalidKeyException__

#include "BasicException.h"

/*!
 * InvalidKeyException is an exception that indicates that the specified
 * key is invalid or not found.
 */
class InvalidKeyException : public BasicException
{
public:
   InvalidKeyException(const std::string& key) noexcept;
   InvalidKeyException(const InvalidKeyException& copy) noexcept;
   InvalidKeyException(InvalidKeyException&& move) noexcept;
   
   virtual ~InvalidKeyException() noexcept;
   
   InvalidKeyException& operator=(const InvalidKeyException& copy) noexcept;
   InvalidKeyException& operator=(InvalidKeyException&& move) noexcept;

   const std::string& getKey() const noexcept;
   
private:
   std::string m_key;
};


#endif /* defined(__HttpServer__InvalidKeyException__) */
