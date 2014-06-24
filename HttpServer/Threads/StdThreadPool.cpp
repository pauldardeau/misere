// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "StdThreadPool.h"
#include "StdThread.h"
#include "Logger.h"

//******************************************************************************

StdThreadPool::StdThreadPool(int numWorkers) noexcept :
   ThreadPool(m_queue, numWorkers)
{
   Logger::logInstanceCreate("StdThreadPool");
}

//******************************************************************************

StdThreadPool::~StdThreadPool() noexcept
{
   Logger::logInstanceDestroy("StdThreadPool");
}

//******************************************************************************

Thread* StdThreadPool::createThreadWithRunnable(Runnable* runnable) noexcept
{
   return new StdThread(runnable);
}

//******************************************************************************

