// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cstdio>
#include <cstdlib>

#include "StdThreadPoolQueue.h"
#include "MutexLock.h"
#include "Logger.h"

//******************************************************************************

StdThreadPoolQueue::StdThreadPoolQueue() noexcept :
   m_isRunning(true)
{
   Logger::logInstanceCreate("StdThreadPoolQueue");
}

//******************************************************************************

StdThreadPoolQueue::~StdThreadPoolQueue() noexcept
{
   Logger::logInstanceDestroy("StdThreadPoolQueue");
}

//******************************************************************************

bool StdThreadPoolQueue::addRequest(Runnable* runnableRequest) noexcept
{
   if (!runnableRequest) {
      Logger::log(Logger::LogLevel::Warning, "StdThreadPoolQueue::addRequest rejecting NULL request");
      return false;
   }
   
   std::unique_lock<std::mutex> lock(m_mutexLock);
   
   if (!m_isRunning) {
      Logger::log(Logger::LogLevel::Warning, "StdThreadPoolQueue::addRequest rejecting request, queue is shutting down");
      return false;
   }
   
   Logger::log(Logger::LogLevel::Debug, "StdThreadPoolQueue::addRequest accepting request");
   
   // add new request to the queue
   m_queue.push_back(runnableRequest);
   
   // did we just transition from QUEUE_EMPTY to QUEUE_NOT_EMPTY?
   if (m_queue.size() == 1) {
      // signal QUEUE_NOT_EMPTY (wake up a worker thread)
      Logger::log(Logger::LogLevel::Debug, "signalling queue_not_empty");
      m_condQueueNotEmpty.notify_one();
   }
   
   return true;
}

//******************************************************************************

Runnable* StdThreadPoolQueue::takeRequest() noexcept
{
   std::unique_lock<std::mutex> lock(m_mutexLock);
   
   // is the queue shut down?
   if (!m_isRunning) {
      return nullptr;
   }
   
   // is the queue empty?
   while (m_queue.empty() && m_isRunning) {
      Logger::log(Logger::LogLevel::Debug, "queue is empty, waiting for QUEUE_NOT_EMPTY event");
      
      // empty queue -- wait for QUEUE_NOT_EMPTY event
      m_condQueueNotEmpty.wait(lock);
   }
   
   // take a request from the queue
   Runnable* request = m_queue.front();
   m_queue.pop_front();
   
   // did we empty the queue?
   if (0 == m_queue.size()) {
      Logger::log(Logger::LogLevel::Debug, "--- signalling queue_empty_event");
      // signal that queue is now empty
      m_condQueueEmpty.notify_one();
   }
   
   return request;
}

//******************************************************************************

void StdThreadPoolQueue::shutDown() noexcept
{
   m_isRunning = false;
   
   if (!m_queue.empty()) {
      // wait for QUEUE_EMPTY event
      std::unique_lock<std::mutex> lock(m_mutexLock);
      m_condQueueEmpty.wait(lock);
   }
}

//******************************************************************************

bool StdThreadPoolQueue::isRunning() const noexcept
{
   return m_isRunning;
}

//******************************************************************************

bool StdThreadPoolQueue::isEmpty() noexcept
{
   std::unique_lock<std::mutex> lock(m_mutexLock);
   return(m_queue.empty());
}

//******************************************************************************

