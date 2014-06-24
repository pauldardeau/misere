// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cstdio>
#include <cstdlib>

#include "PthreadsThreadPoolQueue.h"
#include "MutexLock.h"
#include "Logger.h"

//******************************************************************************

PthreadsThreadPoolQueue::PthreadsThreadPoolQueue() noexcept :
   m_isRunning(true)
{
   Logger::logInstanceCreate("PthreadsThreadPoolQueue");

   ::pthread_cond_init(&m_condQueueEmpty, nullptr);
   ::pthread_cond_init(&m_condQueueNotEmpty, nullptr);
}

//******************************************************************************

PthreadsThreadPoolQueue::~PthreadsThreadPoolQueue() noexcept
{
   Logger::logInstanceDestroy("PthreadsThreadPoolQueue");
}

//******************************************************************************

bool PthreadsThreadPoolQueue::addRequest(Runnable* runnableRequest) noexcept
{
   if (!runnableRequest) {
      Logger::log(Logger::LogLevel::Warning, "PthreadsThreadPoolQueue::addRequest rejecting NULL request");
      return false;
   }

   MutexLock lock(m_lock);

   if (!m_isRunning) {
      Logger::log(Logger::LogLevel::Warning, "PthreadsThreadPoolQueue::addRequest rejecting request, queue is shutting down");
      return false;
   }

   Logger::log(Logger::LogLevel::Debug, "PthreadsThreadPoolQueue::addRequest accepting request");

   // add new request to the queue
   m_queue.push_back(runnableRequest);

   // did we just transition from QUEUE_EMPTY to QUEUE_NOT_EMPTY?
   if (m_queue.size() == 1) {
      // signal QUEUE_NOT_EMPTY (wake up a worker thread)
      Logger::log(Logger::LogLevel::Debug, "signalling queue_not_empty");
      pthread_cond_broadcast(&m_condQueueNotEmpty);
   }
   
   return true;
}

//******************************************************************************

Runnable* PthreadsThreadPoolQueue::takeRequest() noexcept
{
   MutexLock lock(m_lock);

   // is the queue shut down?
   if (!m_isRunning) {
      return nullptr;
   }

   // is the queue empty?
   while (m_queue.empty() && m_isRunning) {
      Logger::log(Logger::LogLevel::Debug, "queue is empty, waiting for QUEUE_NOT_EMPTY event");

      // empty queue -- wait for QUEUE_NOT_EMPTY event
      ::pthread_cond_wait(&m_condQueueNotEmpty,
                        &(m_lock.getPlatformPrimitive()));
   }

   // take a request from the queue
   Runnable* request = m_queue.front();
   m_queue.pop_front();

   // did we empty the queue?
   if (0 == m_queue.size()) {
      Logger::log(Logger::LogLevel::Debug, "--- signalling queue_empty_event");
      // signal that queue is now empty
      ::pthread_cond_signal(&m_condQueueEmpty);
   }

   return request;
}

//******************************************************************************

void PthreadsThreadPoolQueue::shutDown() noexcept
{
   MutexLock lock(m_lock);
   m_isRunning = false;

   if (!m_queue.empty()) {
      // wait for QUEUE_EMPTY event
      ::pthread_cond_wait(&m_condQueueEmpty,
                        &(m_lock.getPlatformPrimitive()));
   }
}

//******************************************************************************

bool PthreadsThreadPoolQueue::isRunning() const noexcept
{
   return m_isRunning;
}

//******************************************************************************

bool PthreadsThreadPoolQueue::isEmpty() noexcept
{
   MutexLock lock(m_lock);
   return(m_queue.empty());
}

//******************************************************************************

