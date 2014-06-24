// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestHttpResponse__
#define __HttpServer__TestHttpResponse__

#include "TestSuite.h"


class TestHttpResponse : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testConstructorWithSocket();
   void testCopyConstructor();
   void testMoveConstructor();
   void testAssignmentCopy();
   void testAssignmentMove();
   void testStreamFromSocket();
   void testGetStatusLine();
   void testGetStatusCode();
   void testSetStatusCode();
   void testGetReasonPhrase();

public:
   TestHttpResponse();
   
};

#endif /* defined(__HttpServer__TestHttpResponse__) */
