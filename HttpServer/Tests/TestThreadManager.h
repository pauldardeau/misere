// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestThreadManager__
#define __HttpServer__TestThreadManager__

#include "TestSuite.h"


class TestThreadManager : public TestSuite
{
protected:
   void runTests();
   
   void testGetInstance();
   void testKillInstance();
   void testGetCurrentThread();
   void testRegisterThread();
   void testNotifyOnThreadDone();
   void testGetNumberThreads();
   void testStartPoolWorker();
   void testStopPoolWorker();
   void testGetPercentBusy();

public:
   TestThreadManager();
   

};

#endif /* defined(__HttpServer__TestThreadManager__) */
