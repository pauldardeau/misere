// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <vector>

#include "PthreadsThread.h"
#include "ThreadManager.h"


/*!
 * PthreadsThreadManager is a ThreadManager specifically for use with pthreads.
 */
class PthreadsThreadManager : public ThreadManager
{
   public:
      PthreadsThreadManager() noexcept;
      ~PthreadsThreadManager() noexcept;

      virtual int getCurrentThreadId() const noexcept override;
      virtual void addThread(Thread* thread) noexcept override;
      virtual void deleteThreadHandleAtIndex(int threadIndex) noexcept override;


      // disallow copies
      PthreadsThreadManager(const PthreadsThreadManager&) = delete;
      PthreadsThreadManager(PthreadsThreadManager&&) = delete;
      PthreadsThreadManager& operator=(const PthreadsThreadManager&) = delete;
      PthreadsThreadManager& operator=(PthreadsThreadManager&&) = delete;

   
   protected:
      PthreadsThread* pthreadsThreadForThread(Thread* thread);
      int threadIndexForThreadHandle(pthread_t threadHandle) const noexcept;


   private:
      std::vector<pthread_t> m_vecThreadHandles;
   
};


#endif


