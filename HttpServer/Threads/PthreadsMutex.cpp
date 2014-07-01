// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "PthreadsMutex.h"
#include "BasicException.h"
#include "Logger.h"

//******************************************************************************

PthreadsMutex::PthreadsMutex() :
   m_haveValidMutex(false),
   m_isLocked(false)
{
   Logger::logInstanceCreate("PthreadsMutex");

   if (0 == ::pthread_mutex_init(&m_mutex, nullptr)) {
      m_haveValidMutex = true;
      //printf("have valid mutex\n");
   } else {
      throw BasicException("unable to create pthreads mutex");
   }
}

//******************************************************************************

PthreadsMutex::PthreadsMutex(const std::string& mutexName) :
   m_mutexName(mutexName),
   m_haveValidMutex(false),
   m_isLocked(false)
{
   Logger::logInstanceCreate("PthreadsMutex");

   if (0 == ::pthread_mutex_init(&m_mutex, nullptr)) {
      m_haveValidMutex = true;
      //printf("have valid mutex\n");
   } else {
      throw BasicException("unable to create pthreads mutex");
   }
}

//******************************************************************************

PthreadsMutex::~PthreadsMutex() noexcept
{
   Logger::logInstanceDestroy("PthreadsMutex");

   if (m_haveValidMutex) {
      if (m_isLocked) {
         unlock();
      }
      ::pthread_mutex_destroy(&m_mutex);
   }
}

//******************************************************************************

bool PthreadsMutex::unlock() noexcept
{
   if (m_haveValidMutex) {
      const int rc = ::pthread_mutex_unlock(&m_mutex);
      if (0 == rc) {
         m_isLocked = false;
      } else {
         Logger::error("unable to unlock PthreadsMutex");
      }
      
      return !m_isLocked;
   } else {
      if (!m_haveValidMutex) {
         Logger::error("can't unlock PthreadsMutex, don't have valid mutex");
      }
      
      if (!m_isLocked) {
         Logger::error("can't unlock PthreadsMutex, not locked");
      }
      
      return false;
   }
}

//******************************************************************************

bool PthreadsMutex::lock() noexcept
{
   if (m_haveValidMutex) {
      const int rc = ::pthread_mutex_lock(&m_mutex);
      if (0 == rc) {
         m_isLocked = true;
      } else {
         Logger::error("unable to lock PthreadsMutex");
      }
      
      return m_isLocked;
   } else {
      if (!m_haveValidMutex) {
         Logger::error("can't lock PthreadsMutex, don't have a valid Pthreads mutex");
      }
      
      if (m_isLocked) {
         Logger::error("can't lock PthreadsMutex, already locked");
      }
      
      return false;
   }
}

//******************************************************************************

bool PthreadsMutex::isLocked() const noexcept
{
   return m_isLocked;
}

//******************************************************************************

bool PthreadsMutex::haveValidMutex() const noexcept
{
   return m_haveValidMutex;
}

//******************************************************************************

const std::string& PthreadsMutex::getName() const noexcept
{
   return m_mutexName;
}

//******************************************************************************

