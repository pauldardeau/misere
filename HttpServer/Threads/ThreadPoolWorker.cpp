// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <exception>

#include "ThreadPoolWorker.h"
#include "ThreadPoolQueue.h"
#include "ThreadingFactory.h"
#include "ThreadManager.h"
#include "Thread.h"
#include "BasicException.h"
#include "Logger.h"

//******************************************************************************

ThreadPoolWorker::ThreadPoolWorker(ThreadingFactory* threadingFactory,
                                   ThreadPoolQueue& queue,
                                   int workerId) noexcept :
   m_threadingFactory(threadingFactory),
   m_workerThread(nullptr),
   m_poolQueue(queue),
   m_workerId(workerId),
   m_isRunning(false)
{
   Logger::logInstanceCreate("ThreadPoolWorker");
}

//******************************************************************************

ThreadPoolWorker::~ThreadPoolWorker() noexcept
{
   Logger::logInstanceDestroy("ThreadPoolWorker");
   
   if (m_workerThread) {
      delete m_workerThread;
   }
}

//******************************************************************************

void ThreadPoolWorker::start() noexcept
{
   if (!m_workerThread) {
      m_workerThread = m_threadingFactory->createThread(this);
      m_workerThread->setPoolWorkerStatus(true);
      m_workerThread->setWorkerId(std::to_string(m_workerId));
   }
   
   m_isRunning = true;
   m_workerThread->start();
}

//******************************************************************************

void ThreadPoolWorker::stop() noexcept
{
   m_isRunning = false;
}

//******************************************************************************

void ThreadPoolWorker::run() noexcept
{
   ThreadManager* pThreadManager = ThreadManager::getInstance();
   
   while (m_isRunning) {
      if (Logger::isLogging(Logger::LogLevel::Debug)) {
         char message[128];
         std::snprintf(message, 128, "poolQueue taking request on thread %d", m_workerId);
         Logger::debug(std::string(message));
      }

      Runnable* runnable = m_poolQueue.takeRequest();
      if (runnable) {
	      // has our thread been notified to shut down?
	      if (!m_workerThread->isAlive()) {
		      // put the request back on the front of the queue
				m_isRunning = false;
				continue;
	      } else {
            pThreadManager->startPoolWorker(m_workerThread);

            try
            {
               runnable->run();
            }
            catch (const BasicException& be)
            {
               Logger::error("run method of runnable threw exception: " + be.whatString());
            }
            catch (const std::exception& e)
            {
               Logger::error("run method of runnable threw exception: " + std::string(e.what()));
            }
            catch (...)
            {
               Logger::error("run method of runnable threw exception");
            }

            pThreadManager->stopPoolWorker(m_workerThread);

            if (Logger::isLogging(Logger::LogLevel::Debug)) {
               char message[128];
               std::snprintf(message, 128, "ending processing request on thread %d", m_workerId);
               Logger::debug(std::string(message));
            }
            
            if (runnable->isAutoDelete()) {
               try
               {
                  delete runnable;
               }
               catch( ... )
               {
               }
            }
            
         }
      }
   }
}

//******************************************************************************


