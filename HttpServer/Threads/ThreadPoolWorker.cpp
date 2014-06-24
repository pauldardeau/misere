// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <exception>

#include "ThreadPoolWorker.h"
#include "ThreadPoolQueue.h"
#include "ThreadManager.h"
#include "Thread.h"
#include "BasicException.h"
#include "Logger.h"

//******************************************************************************

ThreadPoolWorker::ThreadPoolWorker(ThreadPoolQueue& queue, int workerId) noexcept :
   m_poolQueue(queue),
   m_workerId(workerId)
{
   Logger::logInstanceCreate("ThreadPoolWorker");
}

//******************************************************************************

ThreadPoolWorker::~ThreadPoolWorker() noexcept
{
   Logger::logInstanceDestroy("ThreadPoolWorker");
}

//******************************************************************************

void ThreadPoolWorker::run() noexcept
{
   bool isTimeToQuit = false;
   ThreadManager* pThreadManager = ThreadManager::getInstance();
   Thread* workerThread = nullptr;
   
   if (pThreadManager) {
      workerThread = pThreadManager->getCurrentThread();
      
      if (!workerThread) {
         Logger::error("no worker thread available in ThreadPoolWorker::run");
         return;
      }
   } else {
      Logger::warning("no thread manager available");
      return;
   }

   while (!isTimeToQuit) {
      if (Logger::isLogging(Logger::LogLevel::Debug)) {
         char message[128];
         std::snprintf(message, 128, "poolQueue taking request on thread %d", m_workerId);
         Logger::debug(std::string(message));
      }

      Runnable* runnable = m_poolQueue.takeRequest();
      if (!runnable) {
         if (!m_poolQueue.isRunning()) {
            isTimeToQuit = true;
            continue;
         }
      } else {
	      // has our thread been notified to shut down?
	      if (!workerThread->isAlive()) {
		      // put the request back on the front of the queue
				isTimeToQuit = true;
				continue;
	      } else {
            pThreadManager->startPoolWorker(workerThread);

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

            pThreadManager->stopPoolWorker(workerThread);

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


