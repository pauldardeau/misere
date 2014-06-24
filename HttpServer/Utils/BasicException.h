// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef C10KServer_BasicException_h
#define C10KServer_BasicException_h

#include <string>
#include <exception>


class BasicException : public std::exception
{
public:
   BasicException( const char* pszWhat ) noexcept :
      m_what( pszWhat )
   {
   }
    
   BasicException( const std::string& strWhat ) noexcept :
      m_what( strWhat )
   {
   }
    
   BasicException( const BasicException& copy ) noexcept :
      m_what( copy.m_what )
   {
   }

   BasicException( BasicException&& move ) noexcept :
      m_what( std::move(move.m_what) )
   {
   }
    
   ~BasicException() noexcept {}
    
   BasicException& operator=( const BasicException& copy ) noexcept
   {
      if (this == &copy) {
         return *this;
      }
        
      m_what = copy.m_what;
        
      return *this;
   }
    
   BasicException& operator=( BasicException&& move ) noexcept
   {
      if (this == &move) {
         return *this;
      }
        
      m_what = std::move(move.m_what);
        
      return *this;
   }
   
   virtual const char* what() const noexcept
   {
      return m_what.c_str();
   }
    
   virtual const std::string& whatString() const noexcept
   {
      return m_what;
   }
    
private:
   std::string m_what;
};


#endif
