// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef C10KServer_Runnable_h
#define C10KServer_Runnable_h


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
   bool m_autoDelete;
   
public:
   Runnable(Runnable&&) = delete;
   Runnable& operator=(Runnable&&) = delete;
   
   
   Runnable() noexcept :
      m_pCompletionObserver(nullptr),
      m_autoDelete(false)
   {
   }
   
   Runnable(bool isAutoDelete) :
      m_pCompletionObserver(nullptr),
      m_autoDelete(isAutoDelete)
   {
   }
   
   Runnable(const Runnable& copy) noexcept :
      m_pCompletionObserver(copy.m_pCompletionObserver),
      m_autoDelete(copy.m_autoDelete)
   {
   }

   virtual ~Runnable() noexcept {}
    
   Runnable& operator=(const Runnable& copy) noexcept
   {
      if (this == &copy) {
         return *this;
      }
        
      m_pCompletionObserver = copy.m_pCompletionObserver;
      m_autoDelete = copy.m_autoDelete;
        
      return *this;
   }

   virtual void run() = 0;
   
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
