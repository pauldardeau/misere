// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cstdio>
#include <cstdlib>

#include "ThreadPoolQueue.h"
#include "ThreadingFactory.h"
#include "ConditionVariable.h"
#include "MutexLock.h"
#include "Logger.h"
#include "BasicException.h"

//******************************************************************************

ThreadPoolQueue::ThreadPoolQueue(ThreadingFactory* threadingFactory) noexcept :
   m_threadingFactory(threadingFactory),
   m_mutex(nullptr),
   m_condQueueEmpty(nullptr),
   m_condQueueNotEmpty(nullptr),
   m_isInitialized(false),
   m_isRunning(false)
{
   Logger::logInstanceCreate("ThreadPoolQueue");

   if (threadingFactory) {
      try {
         m_mutex = threadingFactory->createMutex();
         m_condQueueEmpty = threadingFactory->createConditionVariable();
         m_condQueueNotEmpty = threadingFactory->createConditionVariable();
         
         if (m_mutex && m_condQueueEmpty && m_condQueueNotEmpty) {
            m_isInitialized = true;
            m_isRunning = true;
         }
      }
      catch (const BasicException& be)
      {
         Logger::error("exception setting up thread pool queue: " + be.whatString());
      }
      catch (const std::exception& e)
      {
         Logger::error("exception setting up thread pool queue: " + std::string(e.what()));
      }
      catch (...)
      {
         Logger::error("unknown exception setting up thread pool queue");
      }
   } else {
      Logger::error("unable to start thread pool queue - no threading factory provided");
   }
}

//******************************************************************************

ThreadPoolQueue::~ThreadPoolQueue() noexcept
{
   Logger::logInstanceDestroy("ThreadPoolQueue");
   
   m_isRunning = false;
   
   if (m_isInitialized) {
      if (m_mutex) {
         delete m_mutex;
      }
   
      if (m_condQueueEmpty) {
         delete m_condQueueEmpty;
      }
   
      if (m_condQueueNotEmpty) {
         delete m_condQueueNotEmpty;
      }
   }
}

//******************************************************************************

bool ThreadPoolQueue::addRequest(Runnable* runnableRequest) noexcept
{
   if (!m_isInitialized) {
      Logger::log(Logger::LogLevel::Warning, "ThreadPoolQueue::addRequest queue not initialized");
      return false;
   }

   if (!runnableRequest) {
      Logger::log(Logger::LogLevel::Warning, "ThreadPoolQueue::addRequest rejecting NULL request");
      return false;
   }
   
   MutexLock lock(*m_mutex);
   
   if (!m_isRunning) {
      Logger::log(Logger::LogLevel::Warning, "ThreadPoolQueue::addRequest rejecting request, queue is shutting down");
      return false;
   }
   
   Logger::log(Logger::LogLevel::Debug, "ThreadPoolQueue::addRequest accepting request");
   
   // add new request to the queue
   m_queue.push_back(runnableRequest);
   
   // did we just transition from QUEUE_EMPTY to QUEUE_NOT_EMPTY?
   if (!m_queue.empty()) {
      // signal QUEUE_NOT_EMPTY (wake up a worker thread)
      Logger::log(Logger::LogLevel::Debug, "signalling queue_not_empty");
      m_condQueueNotEmpty->notifyAll();
   }
   
   return true;
}

//******************************************************************************

Runnable* ThreadPoolQueue::takeRequest() noexcept
{
   if (!m_isInitialized) {
      Logger::log(Logger::LogLevel::Warning, "ThreadPoolQueue::takeRequest queue not initialized");
      return nullptr;
   }

   MutexLock lock(*m_mutex);
   
   // is the queue shut down?
   if (!m_isRunning) {
      return nullptr;
   }
   
   // is the queue empty?
   while (m_queue.empty() && m_isRunning) {
      Logger::log(Logger::LogLevel::Debug, "queue is empty, waiting for QUEUE_NOT_EMPTY event");
      
      // empty queue -- wait for QUEUE_NOT_EMPTY event
      m_condQueueNotEmpty->wait(m_mutex);
   }
   
   // take a request from the queue
   Runnable* request = m_queue.front();
   m_queue.pop_front();
   
   // did we empty the queue?
   if (m_queue.empty()) {
      Logger::log(Logger::LogLevel::Debug, "--- signalling queue_empty_event");
      // signal that queue is now empty
      m_condQueueEmpty->notifyOne();
   }
   
   return request;
}

//******************************************************************************

void ThreadPoolQueue::shutDown() noexcept
{
   if (m_isInitialized && m_isRunning) {
      MutexLock lock(*m_mutex);
      m_isRunning = false;
   
      if (!m_queue.empty()) {
         // wait for QUEUE_EMPTY event
         m_condQueueEmpty->wait(m_mutex);
      }
   }
}

//******************************************************************************

bool ThreadPoolQueue::isRunning() const noexcept
{
   return m_isRunning;
}

//******************************************************************************

bool ThreadPoolQueue::isEmpty() const noexcept
{
   MutexLock lock(*m_mutex);
   return(m_queue.empty());
}

//******************************************************************************

bool ThreadPoolQueue::isInitialized() const noexcept
{
   return m_isInitialized;
}

//******************************************************************************
