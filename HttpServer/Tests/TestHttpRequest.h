// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestHttpRequest__
#define __HttpServer__TestHttpRequest__

#include "TestSuite.h"


class TestHttpRequest : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testCopyConstructor();
   void testMoveConstructor();
   void testAssignmentCopy();
   void testAssignmentMove();
   void testStreamFromSocket();
   void testGetRequest();
   void testGetMethod();
   void testGetPath();
   void testGetRequestLine();
   void testHasArgument();
   void testGetArgument();
   void testGetArgumentKeys();

public:
   TestHttpRequest();
   

};

#endif /* defined(__HttpServer__TestHttpRequest__) */
