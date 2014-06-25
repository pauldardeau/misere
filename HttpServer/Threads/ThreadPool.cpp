// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cstdio>

#include "ThreadPool.h"
#include "ThreadPoolQueue.h"
#include "Logger.h"

//******************************************************************************

ThreadPool::ThreadPool(int numberWorkers) noexcept :
   m_threadingFactory(ThreadingFactory::getThreadingFactory()),
   m_queue(m_threadingFactory),
   m_workerCount(numberWorkers),
   m_workersCreated(0),
   m_isRunning(false)
{
   Logger::logInstanceCreate("ThreadPool");
}

//******************************************************************************

ThreadPool::ThreadPool(ThreadingFactory* threadingFactory, int numberWorkers) noexcept :
   m_threadingFactory(threadingFactory),
   m_queue(m_threadingFactory),
   m_workerCount(numberWorkers),
   m_workersCreated(0),
   m_isRunning(false)
{
   Logger::logInstanceCreate("ThreadPool");
}

//******************************************************************************

ThreadPool::~ThreadPool() noexcept
{
   Logger::logInstanceDestroy("ThreadPool");

   if (m_isRunning) {
      stop();
   }
   
   for (ThreadPoolWorker* worker : m_listWorkers) {
      delete worker;
   }
   
   m_listWorkers.clear();
}

//******************************************************************************

bool ThreadPool::start() noexcept
{
   for (int i = 0; i < m_workerCount; ++i) {
      ++m_workersCreated;
      ThreadPoolWorker* worker =
         new ThreadPoolWorker(m_threadingFactory, m_queue, m_workersCreated);
      worker->start();
      m_listWorkers.push_back(worker);
   }

   m_isRunning = true;
   
   return true;
}

//******************************************************************************

bool ThreadPool::stop() noexcept
{
   m_queue.shutDown();
   
   for (ThreadPoolWorker* worker : m_listWorkers) {
      worker->stop();
   }
   
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

Thread* ThreadPool::createThreadWithRunnable(Runnable* runnable) noexcept
{
   return m_threadingFactory->createThread(runnable);
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
         ++m_workersCreated;
         ++m_workerCount;
         ThreadPoolWorker* worker =
            new ThreadPoolWorker(m_threadingFactory, m_queue, m_workersCreated);
         
         if (m_isRunning) {
            worker->start();
         }
         
	      m_listWorkers.push_back(worker);
	   }
   } else if (numberToAddOrDelete < 0) {  // removing?
      if (m_isRunning) {
	      // tell thread to shut down
      }
   }
}

//******************************************************************************
