// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "PthreadsThreadManager.h"

//******************************************************************************

PthreadsThreadManager::PthreadsThreadManager() noexcept
{
   Logger::logInstanceCreate("PthreadsThreadManager");
}

//******************************************************************************

PthreadsThreadManager::~PthreadsThreadManager() noexcept
{
   Logger::logInstanceDestroy("PthreadsThreadManager");
}

//******************************************************************************

int PthreadsThreadManager::threadIndexForThreadHandle(pthread_t threadHandle) const noexcept
{
   const auto numThreads = m_vecThreadHandles.size();
   
   for (int i = 0; i < numThreads; ++i) {
      if (::pthread_equal(threadHandle, m_vecThreadHandles[i])) {
         return i;
      }
   }
   
   return -1;
}

//******************************************************************************

int PthreadsThreadManager::getCurrentThreadId() const noexcept
{
   return threadIndexForThreadHandle(pthread_self());
}

//******************************************************************************

void PthreadsThreadManager::addThread(Thread* thread) noexcept
{
   PthreadsThread* pthreadsThread = dynamic_cast<PthreadsThread*>(thread);
   
   if (pthreadsThread) {
      m_vecThreadHandles.push_back(pthreadsThread->getHandle());
   }
}

//******************************************************************************

void PthreadsThreadManager::deleteThreadHandleAtIndex(int threadIndex) noexcept
{
   m_vecThreadHandles.erase(m_vecThreadHandles.begin() + threadIndex);
}

//******************************************************************************

PthreadsThread* PthreadsThreadManager::pthreadsThreadForThread(Thread* thread)
{
   return dynamic_cast<PthreadsThread*>(thread);
}

//******************************************************************************

