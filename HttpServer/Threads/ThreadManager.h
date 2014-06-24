// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef HttpServer_ThreadManager_h
#define HttpServer_ThreadManager_h

#include <vector>

#include "ThreadCompletionObserver.h"

class Thread;
class ThreadInfo;

/*!
 * ThreadManager is an abstract base class for providing housekeeping
 * of threads.
 */
class ThreadManager : public ThreadCompletionObserver
{
public:
   static ThreadManager* getInstance() noexcept;
   static void setInstance(ThreadManager* threadManager) noexcept;
   static Thread* getCurrentThread() noexcept;

   ThreadManager() noexcept;
   virtual ~ThreadManager() noexcept;
   
   virtual int getCurrentThreadId() const noexcept = 0;
   virtual void addThread(Thread* thread) noexcept = 0;
   virtual void deleteThreadHandleAtIndex(int threadIndex) noexcept = 0;


   // ThreadCompletionObserver
   virtual void notifyThreadComplete(Thread* thread) noexcept override;

   
   bool registerThread(Thread* thread) noexcept;
   int getNumberThreads() const noexcept;
   
   void startPoolWorker(Thread* thread) noexcept;
   void stopPoolWorker(Thread* thread) noexcept;
   
   double getPercentBusy() const noexcept;
   
   // disallow copies
   ThreadManager(const ThreadManager&) = delete;
   ThreadManager(ThreadManager&&) = delete;
   ThreadManager& operator=(const ThreadManager&) = delete;
   ThreadManager& operator=(ThreadManager&&) = delete;
   
   
protected:
   double _getPercentBusy() const noexcept;
   int indexForThread(Thread* thread) const noexcept;
   
   
private:
   static ThreadManager* s_pInstance;
   
   std::vector<Thread*> m_vecThreads;
   std::vector<ThreadInfo*> m_vecThreadInfos;
   int m_threadCount;
   int m_busyThreadCount;
};


#endif
