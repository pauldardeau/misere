// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__StdThreadManager__
#define __HttpServer__StdThreadManager__

#include <vector>
#include <thread>

#include "ThreadManager.h"

/*!
 * StdThreadManager is a ThreadManager specifically for use with C++11 threads.
 */
class StdThreadManager : public ThreadManager
{
public:
   StdThreadManager() noexcept;
   ~StdThreadManager() noexcept;
   
   virtual int getCurrentThreadId() const noexcept override;
   virtual void deleteThreadHandleAtIndex(int threadIndex) noexcept override;
   virtual void addThread(Thread* thread) noexcept override;

private:
   std::vector<std::thread::id> m_vecThreadIds;

};

#endif /* defined(__HttpServer__StdThreadManager__) */
