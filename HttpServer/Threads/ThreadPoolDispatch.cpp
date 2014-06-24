// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <dispatch/dispatch.h>

#include "ThreadPoolDispatch.h"
#include "Runnable.h"
#include "Logger.h"

//******************************************************************************

ThreadPoolDispatch::ThreadPoolDispatch() noexcept :
   m_isRunning(false)
{
   Logger::logInstanceCreate("ThreadPoolDispatch");
}

//******************************************************************************

ThreadPoolDispatch::~ThreadPoolDispatch() noexcept
{
   Logger::logInstanceDestroy("ThreadPoolDispatch");
}

//******************************************************************************

bool ThreadPoolDispatch::start() noexcept
{
   m_isRunning = true;
   return true;
}

//******************************************************************************

bool ThreadPoolDispatch::stop() noexcept
{
   m_isRunning = false;
   return true;
}

//******************************************************************************

bool ThreadPoolDispatch::addRequest(Runnable* runnableRequest) noexcept
{
   if (!m_isRunning || !runnableRequest) {
      return false;
   }
   
   dispatch_queue_t queue =
      dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
   
   dispatch_async(queue, ^{
      //TODO: if a runnable is allowed to throw an exception, wrap the run
      // call in try/catch
      runnableRequest->run();
      runnableRequest->notifyOnCompletion();
   });
   
   return true;
}

//******************************************************************************

