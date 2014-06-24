// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestRequestHandler__
#define __HttpServer__TestRequestHandler__

#include "TestSuite.h"


class TestRequestHandler : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testRun();
   void testSetThreadPooling();

public:
   TestRequestHandler();
   

};

#endif /* defined(__HttpServer__TestRequestHandler__) */
