// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "StdThreadManager.h"
#include "Logger.h"
#include "StdThread.h"

//******************************************************************************

StdThreadManager::StdThreadManager() noexcept
{
   Logger::logInstanceCreate("StdThreadManager");
}

//******************************************************************************

StdThreadManager::~StdThreadManager() noexcept
{
   Logger::logInstanceDestroy("StdThreadManager");
}

//******************************************************************************

int StdThreadManager::getCurrentThreadId() const noexcept
{
   const std::thread::id threadId = std::this_thread::get_id();
   const auto numThreads = m_vecThreadIds.size();
   for (int i = 0; i < numThreads; ++i) {
      if (m_vecThreadIds[i] == threadId) {
         return i;
      }
   }
   
   return -1;
}

//******************************************************************************

void StdThreadManager::deleteThreadHandleAtIndex(int threadIndex) noexcept
{
   m_vecThreadIds.erase(m_vecThreadIds.begin() + threadIndex);
}

//******************************************************************************

void StdThreadManager::addThread(Thread* thread) noexcept
{
   StdThread* stdThread = dynamic_cast<StdThread*>(thread);
   
   if (stdThread) {
      m_vecThreadIds.push_back(stdThread->getStdThreadId());
   }
}

//******************************************************************************

