// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cstdio>

#include "ThreadPool.h"
#include "ThreadPoolQueue.h"

//******************************************************************************

ThreadPool::ThreadPool(ThreadPoolQueue& queue, int numWorkers) noexcept :
   m_queue(queue),
   m_workerCount(numWorkers),
   m_isRunning(false)
{
   for (int i = 0; i < numWorkers; ++i) {
      m_vecWorkers.push_back(new ThreadPoolWorker(m_queue, i));
   }
}

//******************************************************************************

ThreadPool::~ThreadPool() noexcept
{
   if (m_isRunning) {
      stop();
   }
}

//******************************************************************************

bool ThreadPool::start() noexcept
{
   for (int i = 0; i < m_workerCount; ++i) {
      Thread* thread = createThreadWithRunnable(m_vecWorkers[i]);
      thread->setPoolWorkerStatus(true);
      thread->setWorkerId(std::to_string(i));
      thread->start();
      m_vecThreads.push_back(thread);
   }
   
   m_isRunning = true;
   
   return true;
}

//******************************************************************************

bool ThreadPool::stop() noexcept
{
   m_queue.shutDown();
   m_vecThreads.erase(m_vecThreads.begin(), m_vecThreads.end());
   m_isRunning = false;
   return true;
}

//******************************************************************************

bool ThreadPool::addRequest(Runnable* runnableRequest) noexcept
{
   if (!m_isRunning) {
      return false;
   }
   
   m_queue.addRequest(runnableRequest);
   
   return true;
}

//******************************************************************************

int ThreadPool::getNumberWorkers() const noexcept
{
	return m_workerCount;
}

//******************************************************************************

void ThreadPool::addWorkers(int numberNewWorkers) noexcept
{
	adjustNumberWorkers(numberNewWorkers);
}

//******************************************************************************

void ThreadPool::removeWorkers(int numberWorkersToRemove) noexcept
{
	adjustNumberWorkers(-numberWorkersToRemove);
}

//******************************************************************************

void ThreadPool::adjustNumberWorkers(int numberToAddOrDelete) noexcept
{
   if (numberToAddOrDelete > 0) {   // adding?
      
		const int newNumberWorkers = m_workerCount + numberToAddOrDelete;
		
	   for (int i = m_workerCount; i < newNumberWorkers; ++i) {
	      m_vecWorkers.push_back(new ThreadPoolWorker(m_queue, i));
	   }
      
	   if (m_isRunning) {
		   for (int i = m_workerCount; i < newNumberWorkers; ++i) {
            Thread* thread = createThreadWithRunnable(m_vecWorkers[i]);
		      thread->setPoolWorkerStatus(true);
            thread->setWorkerId(std::to_string(i));
		      thread->start();
		      m_vecThreads.push_back(thread);
		   }
	   }
      
		m_workerCount += newNumberWorkers;
   } else if (numberToAddOrDelete < 0) {  // removing?
      if (m_isRunning) {
	      // tell thread to shut down
      }
   }
}

//******************************************************************************
