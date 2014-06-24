// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "ThreadingFactory.h"
#include "PthreadsMutex.h"
#include "StdMutex.h"
#include "PthreadsThread.h"
#include "StdThread.h"
#include "PthreadsThreadPool.h"
#include "StdThreadPool.h"
#include "ThreadPoolDispatch.h"
#include "BasicException.h"
#include "Logger.h"
#include "ThreadManager.h"
#include "StdThreadManager.h"
#include "PthreadsThreadManager.h"


//******************************************************************************

ThreadingFactory::ThreadingFactory(ThreadingPackage threadingPackage) noexcept :
   m_threadingPackage(threadingPackage),
   m_packageMutexType(threadingPackage)
{
   Logger::logInstanceCreate("ThreadingFactory");

   if (m_threadingPackage == ThreadingPackage::CPP_11) {
      ThreadManager::setInstance(new StdThreadManager());
   } else if (m_threadingPackage == ThreadingPackage::PTHREADS) {
      ThreadManager::setInstance(new PthreadsThreadManager());
   } else if (m_threadingPackage == ThreadingPackage::GCD_LIBDISPATCH) {
      ThreadManager::setInstance(nullptr);  // no threading manager needed (using GCD)
   } else {
      ThreadManager::setInstance(nullptr);
   }
}

//******************************************************************************

ThreadingFactory::~ThreadingFactory() noexcept
{
   Logger::logInstanceDestroy("ThreadingFactory");
}

//******************************************************************************

void ThreadingFactory::setMutexType(ThreadingPackage threadingPackage)
{
   if (m_threadingPackage == ThreadingPackage::GCD_LIBDISPATCH) {
      if (threadingPackage != ThreadingPackage::GCD_LIBDISPATCH) {
         m_packageMutexType = threadingPackage;
      } else {
         throw BasicException("Invalid mutex type. must be PTHREADS or CPP_11");
      }
   } else {
      // ignore it
      Logger::warning("Ignoring setMutexType for non GCD_LIBDISPATCH");
   }
}

//******************************************************************************

Mutex* ThreadingFactory::createMutex()
{
   if (m_packageMutexType == ThreadingPackage::CPP_11) {
      return new StdMutex();
   } else if (m_packageMutexType == ThreadingPackage::PTHREADS) {
      return new PthreadsMutex();
   } else if (m_packageMutexType == ThreadingPackage::GCD_LIBDISPATCH) {
      Logger::error("invalid package type for mutexes. see setMutexType");
      throw BasicException("Invalid threading package for mutexes");
   }
   
   return nullptr;
}

//******************************************************************************

Mutex* ThreadingFactory::createMutex(const std::string& mutexName)
{
   if (m_packageMutexType == ThreadingPackage::CPP_11) {
      return new StdMutex(mutexName);
   } else if (m_packageMutexType == ThreadingPackage::PTHREADS) {
      return new PthreadsMutex(mutexName);
   } else if (m_packageMutexType == ThreadingPackage::GCD_LIBDISPATCH) {
      Logger::error("invalid package type for mutexes. see setMutexType");
      throw BasicException("Invalid threading package for mutexes");
   }
   
   return nullptr;
}

//******************************************************************************

Thread* ThreadingFactory::createThread() noexcept
{
   return createThread(nullptr);
}

//******************************************************************************

Thread* ThreadingFactory::createThread(Runnable* runnable) noexcept
{
   if (m_threadingPackage == ThreadingPackage::CPP_11) {
      if (runnable != nullptr) {
         return new StdThread(runnable);
      } else {
         return new StdThread();
      }
   } else if (m_threadingPackage == ThreadingPackage::PTHREADS) {
      if (runnable != nullptr) {
         return new PthreadsThread(runnable);
      } else {
         return new PthreadsThread();
      }
   } else if (m_threadingPackage == ThreadingPackage::GCD_LIBDISPATCH) {
      Logger::error("createThread should not be called on GCD_LIBDISPATCH");
   }
   
   return nullptr;
}

//******************************************************************************

ThreadPoolDispatcher* ThreadingFactory::createThreadPoolDispatcher(int numberThreads) noexcept
{
   ThreadPoolDispatcher* threadPoolDispatcher = nullptr;
   
   if (m_threadingPackage == ThreadingPackage::CPP_11) {
      threadPoolDispatcher = new StdThreadPool(numberThreads);
   } else if (m_threadingPackage == ThreadingPackage::PTHREADS) {
      threadPoolDispatcher = new PthreadsThreadPool(numberThreads);
   } else if (m_threadingPackage == ThreadingPackage::GCD_LIBDISPATCH) {
      threadPoolDispatcher = new ThreadPoolDispatch();  // don't specify number threads
   }
   
   return threadPoolDispatcher;
}

//******************************************************************************

