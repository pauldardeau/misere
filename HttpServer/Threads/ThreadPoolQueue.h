// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef HttpServer_ThreadPoolQueue_h
#define HttpServer_ThreadPoolQueue_h

#include <deque>

class ConditionVariable;
class Mutex;
class Runnable;
class ThreadingFactory;


/*!
 * ThreadPoolQueue is an abstract base class for a queue being serviced
 * by a thread from a thread pool.
 */
class ThreadPoolQueue
{
public:
   ThreadPoolQueue(ThreadingFactory* threadingFactory) noexcept;
   virtual ~ThreadPoolQueue() noexcept;
   
   virtual bool addRequest(Runnable* runnableRequest) noexcept;
   virtual Runnable* takeRequest() noexcept;
   virtual void shutDown() noexcept;
   virtual bool isRunning() const noexcept;
   virtual bool isEmpty() const noexcept;
   virtual bool isInitialized() const noexcept;
   
   // disallow copies
   ThreadPoolQueue(const ThreadPoolQueue&) = delete;
   ThreadPoolQueue(ThreadPoolQueue&&) = delete;
   ThreadPoolQueue& operator=(const ThreadPoolQueue&) = delete;
   ThreadPoolQueue& operator=(ThreadPoolQueue&&) = delete;
   
private:
   ThreadingFactory* m_threadingFactory;
   std::deque<Runnable*> m_queue;
   Mutex* m_mutex;
   ConditionVariable* m_condQueueEmpty;
   ConditionVariable* m_condQueueNotEmpty;
   bool m_isInitialized;
   bool m_isRunning;
};


#endif
