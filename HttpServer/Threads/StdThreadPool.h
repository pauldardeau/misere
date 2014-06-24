// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__StdThreadPool__
#define __HttpServer__StdThreadPool__

#include "ThreadPool.h"
#include "StdThreadPoolQueue.h"

class Runnable;
class Thread;


/*!
 * StdThreadPool provides a configurable pool of threads and dispatches requests
 * to be run by the pool.
 */
class StdThreadPool : public ThreadPool
{
public:
   StdThreadPool(int numWorkers) noexcept;
   ~StdThreadPool() noexcept;
   
   virtual Thread* createThreadWithRunnable(Runnable* runnable) noexcept override;
   
   StdThreadPool(const StdThreadPool&) = delete;
   StdThreadPool(StdThreadPool&&) = delete;
   StdThreadPool& operator=(const StdThreadPool&) = delete;
   StdThreadPool& operator=(StdThreadPool&&) = delete;
   
   
private:
   StdThreadPoolQueue m_queue;
   
};


#endif /* defined(__HttpServer__StdThreadPool__) */
