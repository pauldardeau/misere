// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__ThreadingFactory__
#define __HttpServer__ThreadingFactory__

#include <string>


class Mutex;
class Thread;
class ConditionVariable;
class Runnable;
class ThreadPoolDispatcher;

/*!
 * ThreadingFactory is a factory for creating Thread, Mutex, and ThreadPoolDispatcher
 * instances based on the desired ThreadingPackage.
 */
class ThreadingFactory
{
public:
   enum class ThreadingPackage {
      CPP_11,
      PTHREADS,
      GCD_LIBDISPATCH
   };
   
   static ThreadingFactory* getThreadingFactory() noexcept;

   ThreadingFactory(ThreadingPackage threadingPackage) noexcept;
   ~ThreadingFactory() noexcept;
   
   // throws BasicException
   void setMutexType(ThreadingPackage threadingPackage);
   
   Mutex* createMutex();
   Mutex* createMutex(const std::string& mutexName);
   
   Thread* createThread() noexcept;
   Thread* createThread(Runnable* runnable) noexcept;
   
   ConditionVariable* createConditionVariable();
   
   ThreadPoolDispatcher* createThreadPoolDispatcher(int numberThreads) noexcept;
   
   // disallow copies
   ThreadingFactory(const ThreadingFactory&) = delete;
   ThreadingFactory(ThreadingFactory&&) = delete;
   ThreadingFactory& operator=(const ThreadingFactory&) = delete;
   ThreadingFactory& operator=(ThreadingFactory&&) = delete;

private:
   ThreadingPackage m_threadingPackage;
   ThreadingPackage m_packageMutexType;

   static ThreadingFactory* threadingFactoryInstance;

};

#endif /* defined(__HttpServer__ThreadingFactory__) */
