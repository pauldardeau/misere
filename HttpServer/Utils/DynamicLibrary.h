// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef DYNAMICLIBRARY_H
#define DYNAMICLIBRARY_H

#include <string>

/*!
 * DynamicLibrary is a wrapper around dlopen, dlsym, dlclose.
 */
class DynamicLibrary
{
   public:
      DynamicLibrary() noexcept;
      // throws BasicException
      DynamicLibrary(const std::string& libraryName);
      ~DynamicLibrary() noexcept;
      
      void* resolve(const std::string& functionName) noexcept;
      bool open(const std::string& libraryName) noexcept;
      void close() noexcept;
   
      // copies not allowed
      DynamicLibrary(const DynamicLibrary&) = delete;
      DynamicLibrary(DynamicLibrary&&) = delete;
      DynamicLibrary& operator=(const DynamicLibrary&) = delete;
      DynamicLibrary& operator=(DynamicLibrary&&) = delete;

   private:
      typedef void* HDLL;
      HDLL m_hDll;
};

#endif

