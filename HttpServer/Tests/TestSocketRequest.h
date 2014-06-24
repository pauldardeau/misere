// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestSocketRequest__
#define __HttpServer__TestSocketRequest__

#include "TestSuite.h"


class TestSocketRequest : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testRun();
   void testGetSocketFD();
   void testGetSocket();
   void testRequestComplete();

public:
   TestSocketRequest();
   

};

#endif /* defined(__HttpServer__TestSocketRequest__) */
