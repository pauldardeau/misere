// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef THREADPOOLWORKER_H
#define THREADPOOLWORKER_H

#include "Runnable.h"

class ThreadPoolQueue;


/*!
 * ThreadPoolWorker provides the logic of pulling work off of a ThreadPoolQueue
 * and processing and then repeating. It's meant to be used within a ThreadPool.
 */
class ThreadPoolWorker : public Runnable
{
   public:
      ThreadPoolWorker(ThreadPoolQueue& queue, int workerId) noexcept;
      ~ThreadPoolWorker() noexcept;

      void run() noexcept override;

   private:
      ThreadPoolQueue& m_poolQueue;
      int m_workerId;
};

#endif

