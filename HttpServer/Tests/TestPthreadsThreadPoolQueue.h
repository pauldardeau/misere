// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestThreadPoolQueue__
#define __HttpServer__TestThreadPoolQueue__

#include "TestSuite.h"


class TestPthreadsThreadPoolQueue : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testAddRequest();
   void testTakeRequest();
   void testShutDown();
   void testIsRunning();
   void testIsEmpty();

public:
   TestPthreadsThreadPoolQueue();
   

};

#endif /* defined(__HttpServer__TestThreadPoolQueue__) */
