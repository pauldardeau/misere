// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "PthreadsConditionVariable.h"
#include "PthreadsMutex.h"
#include "BasicException.h"
#include "Logger.h"

//******************************************************************************

PthreadsConditionVariable::PthreadsConditionVariable() :
   m_initialized(false)
{
   Logger::logInstanceCreate("PthreadsConditionVariable");

   if (0 != ::pthread_cond_init(&m_cond, nullptr)) {
      throw BasicException("unable to create condition variable (pthreads)");
   } else {
      m_initialized = true;
   }
}

//******************************************************************************

PthreadsConditionVariable::~PthreadsConditionVariable()
{
   Logger::logInstanceDestroy("PthreadsConditionVariable");
}

//******************************************************************************

void PthreadsConditionVariable::wait(std::shared_ptr<Mutex> mutex) noexcept
{
   if (m_initialized) {
      if (mutex) {
         std::shared_ptr<PthreadsMutex> pthreadsMutex =
            std::dynamic_pointer_cast<PthreadsMutex>(mutex);
         
         if (pthreadsMutex) {
            if (0 != ::pthread_cond_wait(&m_cond, &pthreadsMutex->getPlatformPrimitive())) {
               Logger::error("unable to wait on condition variable");
            }
         } else {
            Logger::error("mutex must be an instance of PthreadsMutex");
         }
      } else {
         Logger::error("no mutex given to wait on");
      }
   } else {
      Logger::error("unable to wait on condition variable that hasn't been initialized");
   }
}

//******************************************************************************

void PthreadsConditionVariable::notifyOne() noexcept
{
   if (m_initialized) {
      if (0 != ::pthread_cond_signal(&m_cond)) {
         Logger::error("unable to signal on condition variable");
      }
   }
}

//******************************************************************************

void PthreadsConditionVariable::notifyAll() noexcept
{
   if (m_initialized) {
      if (0 != ::pthread_cond_broadcast(&m_cond)) {
         Logger::error("unable to broadcast on condition variable");
      }
   }
}

//******************************************************************************

