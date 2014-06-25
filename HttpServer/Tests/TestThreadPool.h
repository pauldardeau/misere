// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestThreadPool__
#define __HttpServer__TestThreadPool__

#include "TestSuite.h"


class TestThreadPool : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testStart();
   void testStop();
   void testAddRequest();
   void testGetNumberWorkers();
   void testAddWorkers();
   void testRemoveWorkers();

public:
   TestThreadPool();
   

};

#endif /* defined(__HttpServer__TestThreadPool__) */
