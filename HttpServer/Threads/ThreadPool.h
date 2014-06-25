// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__ThreadPool__
#define __HttpServer__ThreadPool__

#include <list>

#include "Thread.h"
#include "ThreadPoolQueue.h"
#include "ThreadPoolWorker.h"
#include "ThreadPoolDispatcher.h"
#include "ThreadingFactory.h"

class Runnable;


/*!
 * ThreadPool is an abstract base class for thread pools.
 */
class ThreadPool : public ThreadPoolDispatcher
{
public:
   ThreadPool(int numberWorkers) noexcept;
   ThreadPool(ThreadingFactory* threadingFactory, int numberWorkers) noexcept;

   ~ThreadPool() noexcept;
   
   // ThreadPoolDispatcher
   virtual bool start() noexcept override;
   virtual bool stop() noexcept override;
   virtual bool addRequest(Runnable* runnableRequest) noexcept override;
   
   virtual Thread* createThreadWithRunnable(Runnable* runnable) noexcept;
   
   int getNumberWorkers() const noexcept;
   void addWorkers(int numberNewWorkers) noexcept;
   void removeWorkers(int numberWorkersToRemove) noexcept;
   
   ThreadPool(const ThreadPool&) = delete;
   ThreadPool(ThreadPool&&) = delete;
   ThreadPool& operator=(const ThreadPool&) = delete;
   ThreadPool& operator=(ThreadPool&&) = delete;
   
protected:
   void adjustNumberWorkers(int numberToAddOrDelete) noexcept;
   
private:
   ThreadingFactory* m_threadingFactory;
   std::list<ThreadPoolWorker*> m_listWorkers;
   ThreadPoolQueue m_queue;
   int m_workerCount;
   int m_workersCreated;
   bool m_isRunning;
};

#endif /* defined(__HttpServer__ThreadPool__) */
