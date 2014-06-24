// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cstdio>

#include "ThreadManager.h"
#include "PthreadsMutex.h"
#include "MutexLock.h"
#include "Logger.h"
#include "ThreadInfo.h"
#include "Thread.h"

ThreadManager* ThreadManager::s_pInstance = nullptr;

static PthreadsMutex lockThread;


//******************************************************************************

ThreadManager* ThreadManager::getInstance() noexcept
{
   return s_pInstance;
}

//******************************************************************************

void ThreadManager::setInstance(ThreadManager* threadManager) noexcept
{
   s_pInstance = threadManager;
}

//******************************************************************************

Thread* ThreadManager::getCurrentThread() noexcept
{
   Thread* thread = nullptr;
   
   if (s_pInstance) {
      MutexLock lock(lockThread);
   
      const int threadIndex = s_pInstance->getCurrentThreadId();
   
      if (threadIndex > -1) {
         thread = s_pInstance->m_vecThreads[threadIndex];
      } else {
         Logger::log(Logger::LogLevel::Error, "can't find thread object for current thread");
      }
   }
   
   return thread;
}

//******************************************************************************

bool ThreadManager::registerThread(Thread* thread) noexcept
{
   if (!thread->hasThreadId()) {
      Logger::error("unable to register thread, missing thread id");
      return false;
   }
      
   ThreadInfo* ti = new ThreadInfo(thread->getThreadId());
      
   MutexLock lock(lockThread);
      
   ++m_threadCount;
      
   if (Logger::isLogging(Logger::LogLevel::Debug)) {
      char msg[128];
      std::snprintf(msg, 128, "registering thread id '%s'", ti->threadId.c_str());
      Logger::log(Logger::LogLevel::Debug, std::string(msg));
   }
   
   addThread(thread);
   m_vecThreads.push_back(thread);
   m_vecThreadInfos.push_back(std::move(ti));
      
   thread->registerThreadCompletionObserver(this);
   
   return true;
}

//******************************************************************************

int ThreadManager::indexForThread(Thread* thread) const noexcept
{
   if (thread) {
      const auto numThreads = m_vecThreads.size();
      for (int i = 0; i < numThreads; ++i) {
         if (thread == m_vecThreads[i]) {
            return i;
         }
      }
   }
   
   return -1;
}

//******************************************************************************

void ThreadManager::notifyThreadComplete(Thread* thread) noexcept
{
   if (thread) {
      MutexLock lock(lockThread);
      
      const int threadIndex = indexForThread(thread);
      
      if (threadIndex > -1) {
         ThreadInfo* ti = m_vecThreadInfos[threadIndex];
         ti->isBusy = false;
         --m_busyThreadCount;
      }
   }
}

//******************************************************************************

int ThreadManager::getNumberThreads() const noexcept
{
   MutexLock lock(lockThread);
   return m_threadCount;
}

//******************************************************************************

void ThreadManager::startPoolWorker(Thread* thread) noexcept
{
   if (!thread) {
      Logger::log(Logger::LogLevel::Warning, "NULL Thread pointer passed to startPoolWorker");
      return;
   }
   
   MutexLock lock(lockThread);
      
   const int threadIndex = indexForThread(thread);
      
   if (threadIndex > -1) {
      ++m_busyThreadCount;
      ThreadInfo* ti = m_vecThreadInfos[threadIndex];
      ti->isBusy = true;
      const double percentBusy = _getPercentBusy();
      if (percentBusy >= 0.75) {
         char msg[128];
         std::snprintf(msg, 128, "*** above threshhold: %lf", percentBusy);
         Logger::log(Logger::LogLevel::Info, std::string(msg));
      }
   }
}

//******************************************************************************

void ThreadManager::stopPoolWorker(Thread* thread) noexcept
{
   if (thread) {
      MutexLock lock(lockThread);

      const int threadIndex = indexForThread(thread);
   
      if (threadIndex > -1) {
         --m_busyThreadCount;
         ThreadInfo* ti = m_vecThreadInfos[threadIndex];
         ti->isBusy = false;
      }
   }
}

//******************************************************************************

double ThreadManager::getPercentBusy() const noexcept
{
   MutexLock lock(lockThread);
   return _getPercentBusy();
}

//******************************************************************************

double ThreadManager::_getPercentBusy() const noexcept
{
   return (((double) m_busyThreadCount) / m_threadCount);
}

//******************************************************************************

ThreadManager::ThreadManager() noexcept :
   m_threadCount(0),
   m_busyThreadCount(0)
{
}

//******************************************************************************

ThreadManager::~ThreadManager() noexcept
{
}

//******************************************************************************


