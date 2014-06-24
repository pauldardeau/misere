// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef THREADPOOLQUEUE_H
#define THREADPOOLQUEUE_H

#include <deque>

#include <pthread.h>

#include "Runnable.h"
#include "PthreadsMutex.h"
#include "ThreadPoolQueue.h"

/*!
 * PthreadsThreadPoolQueue is a ThreadPoolQueue implemented with pthreads.
 */
class PthreadsThreadPoolQueue : public ThreadPoolQueue
{
   public:
      PthreadsThreadPoolQueue() noexcept;
      ~PthreadsThreadPoolQueue() noexcept;

      // ThreadPoolQueue
      virtual bool addRequest(Runnable* runnableRequest) noexcept override;
      virtual Runnable* takeRequest() noexcept override;
      virtual void shutDown() noexcept override;
      virtual bool isRunning() const noexcept override;
      virtual bool isEmpty() noexcept override;
   
      // disallow copies
      PthreadsThreadPoolQueue(const PthreadsThreadPoolQueue&) = delete;
      PthreadsThreadPoolQueue(PthreadsThreadPoolQueue&&) = delete;
      PthreadsThreadPoolQueue& operator=(const PthreadsThreadPoolQueue&) = delete;
      PthreadsThreadPoolQueue& operator=(PthreadsThreadPoolQueue&&) = delete;


   private:
      std::deque<Runnable*> m_queue;
      bool m_isRunning;
      PthreadsMutex m_lock;
      pthread_cond_t m_condQueueEmpty;
      pthread_cond_t m_condQueueNotEmpty;
};

#endif

