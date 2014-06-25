// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef C10KServer_Runnable_h
#define C10KServer_Runnable_h

#include <string>

#include "RunCompletionObserver.h"
#include "Logger.h"


/*!
 * Runnable is an abstract base class that's conceptually very similar to
 * Java's Runnable interface for classes that can be used in background
 * threads.
 */
class Runnable
{
private:
   RunCompletionObserver* m_pCompletionObserver;
   std::string m_runByThreadWorkerId;
   bool m_autoDelete;
   int m_runByThreadId;
   
public:
   Runnable() noexcept :
      m_pCompletionObserver(nullptr),
      m_autoDelete(false),
      m_runByThreadId(0)
   {
   }
   
   Runnable(bool isAutoDelete) :
      m_pCompletionObserver(nullptr),
      m_autoDelete(isAutoDelete),
      m_runByThreadId(0)
   {
   }
   
   
   // disallow copies
   Runnable(const Runnable&) noexcept = delete;
   Runnable(Runnable&&) noexcept = delete;
   Runnable& operator=(const Runnable&) noexcept = delete;
   Runnable& operator=(Runnable&&) noexcept = delete;
   

   virtual ~Runnable() noexcept {}
    

   virtual void run() = 0;
   
   virtual void setRunByThreadId(int runByThreadId) noexcept
   {
      m_runByThreadId = runByThreadId;
   }
   
   virtual int getRunByThreadId() const noexcept
   {
      return m_runByThreadId;
   }
   
   virtual void setRunByThreadWorkerId(const std::string& runByThreadWorkerId) noexcept
   {
      m_runByThreadWorkerId = runByThreadWorkerId;
   }
   
   virtual const std::string& getRunByThreadWorkerId() const noexcept
   {
      return m_runByThreadWorkerId;
   }
   
   virtual void autoDelete()
   {
      if (Logger::isLogging(Logger::LogLevel::Debug)) {
         Logger::debug("turning on autoDelete on Runnable");
      }
      
      m_autoDelete = true;
   }
   
   virtual bool isAutoDelete() const
   {
      return m_autoDelete;
   }
   
   void setCompletionObserver(RunCompletionObserver* pCompletionObserver) noexcept
   {
       m_pCompletionObserver = pCompletionObserver;
   }
   
   /*!
    * This should only be called AFTER the run method has completed
    */
   virtual void notifyOnCompletion() noexcept
   {
      if (m_pCompletionObserver) {
         m_pCompletionObserver->notifyRunComplete(this);
      }
   }
};


#endif
