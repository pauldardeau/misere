// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestThreadPoolWorker__
#define __HttpServer__TestThreadPoolWorker__

#include "TestSuite.h"


class TestThreadPoolWorker : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testRun();

public:
   TestThreadPoolWorker();
   

};

#endif /* defined(__HttpServer__TestThreadPoolWorker__) */
