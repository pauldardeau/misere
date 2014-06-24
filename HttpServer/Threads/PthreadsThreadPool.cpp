// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "PthreadsThreadPool.h"
#include "PthreadsThread.h"
#include "Logger.h"

//******************************************************************************

PthreadsThreadPool::PthreadsThreadPool(int numWorkers) noexcept :
   ThreadPool(m_queue, numWorkers)
{
   Logger::logInstanceCreate("PthreadsThreadPool");
}

//******************************************************************************

PthreadsThreadPool::~PthreadsThreadPool() noexcept
{
   Logger::logInstanceDestroy("PthreadsThreadPool");
}

//******************************************************************************

Thread* PthreadsThreadPool::createThreadWithRunnable(Runnable* runnable) noexcept
{
   return new PthreadsThread(runnable);
}

//******************************************************************************

