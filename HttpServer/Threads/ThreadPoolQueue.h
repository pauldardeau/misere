// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef HttpServer_ThreadPoolQueue_h
#define HttpServer_ThreadPoolQueue_h


class Runnable;


/*!
 * ThreadPoolQueue is an abstract base class for a queue being serviced
 * by a thread from a thread pool.
 */
class ThreadPoolQueue
{
public:
   ThreadPoolQueue() noexcept {}
   virtual ~ThreadPoolQueue() noexcept {}
   
   virtual bool addRequest(Runnable* runnableRequest) noexcept = 0;
   virtual Runnable* takeRequest() noexcept = 0;
   virtual void shutDown() noexcept = 0;
   virtual bool isRunning() const noexcept = 0;
   virtual bool isEmpty() noexcept = 0;
   
   // disallow copies
   ThreadPoolQueue(const ThreadPoolQueue&) = delete;
   ThreadPoolQueue(ThreadPoolQueue&&) = delete;
   ThreadPoolQueue& operator=(const ThreadPoolQueue&) = delete;
   ThreadPoolQueue& operator=(ThreadPoolQueue&&) = delete;
   
};


#endif
