// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestKqueueServer__
#define __HttpServer__TestKqueueServer__

#include "TestSuite.h"


class TestKqueueServer : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testInit();
   void testRun();
   void testNotifySocketComplete();

public:
   TestKqueueServer();
   

};

#endif /* defined(__HttpServer__TestKqueueServer__) */
