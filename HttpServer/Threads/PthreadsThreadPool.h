// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "ThreadPool.h"
#include "PthreadsThreadPoolQueue.h"

class Runnable;
class Thread;


/*!
 * PthreadsThreadPool provides a configurable pool of threads and dispatches requests
 * to be run by the pool.
 */
class PthreadsThreadPool : public ThreadPool
{
   public:
      PthreadsThreadPool(int numWorkers) noexcept;
      ~PthreadsThreadPool() noexcept;

      virtual Thread* createThreadWithRunnable(Runnable* runnable) noexcept override;

      PthreadsThreadPool(const PthreadsThreadPool&) = delete;
      PthreadsThreadPool(PthreadsThreadPool&&) = delete;
      PthreadsThreadPool& operator=(const PthreadsThreadPool&) = delete;
      PthreadsThreadPool& operator=(PthreadsThreadPool&&) = delete;
		

   private:
      PthreadsThreadPoolQueue m_queue;
   
};

#endif

