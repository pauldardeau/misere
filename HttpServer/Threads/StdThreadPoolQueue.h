// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__StdThreadPoolQueue__
#define __HttpServer__StdThreadPoolQueue__

#include <deque>
#include <condition_variable>
#include <mutex>

#include "Runnable.h"
#include "ThreadPoolQueue.h"

/*!
 * StdThreadPoolQueue is a ThreadPoolQueue implemented using C++ 11.
 */
class StdThreadPoolQueue : public ThreadPoolQueue
{
public:
   StdThreadPoolQueue() noexcept;
   ~StdThreadPoolQueue() noexcept;
   
   // ThreadPoolQueue
   virtual bool addRequest(Runnable* runnableRequest) noexcept override;
   virtual Runnable* takeRequest() noexcept override;
   virtual void shutDown() noexcept override;
   virtual bool isRunning() const noexcept override;
   virtual bool isEmpty() noexcept override;
   
   // disallow copies
   StdThreadPoolQueue(const StdThreadPoolQueue&) = delete;
   StdThreadPoolQueue(StdThreadPoolQueue&&) = delete;
   StdThreadPoolQueue& operator=(const StdThreadPoolQueue&) = delete;
   StdThreadPoolQueue& operator=(StdThreadPoolQueue&&) = delete;
   
   
private:
   std::deque<Runnable*> m_queue;
   bool m_isRunning;
   std::mutex m_mutexLock;
   std::condition_variable m_condQueueEmpty;
   std::condition_variable m_condQueueNotEmpty;
};

#endif /* defined(__HttpServer__StdThreadPoolQueue__) */
