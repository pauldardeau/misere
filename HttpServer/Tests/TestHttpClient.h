// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestHttpClient__
#define __HttpServer__TestHttpClient__

#include "TestSuite.h"


class TestHttpClient : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testPost();
   void testSendReceive();
   void testSendReceiveWithPost();
   void testBuildHeader();
   void testBuildHttpRequest();

public:
   TestHttpClient();
   

};

#endif /* defined(__HttpServer__TestHttpClient__) */
