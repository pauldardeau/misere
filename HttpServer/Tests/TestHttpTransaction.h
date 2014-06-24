// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestHttpTransaction__
#define __HttpServer__TestHttpTransaction__

#include "TestSuite.h"


class TestHttpTransaction : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testCopyConstructor();
   void testMoveConstructor();
   void testAssignmentCopy();
   void testAssignmentMove();
   void testStreamFromSocket();
   void testGetRawHeader();
   void testGetBody();
   void testSetBody();
   void testHasHeaderValue();
   void testGetHeaderValue();
   void testGetHeaderValues();
   void testSetHeaderValue();
   void testGetProtocol();

public:
   TestHttpTransaction();
   

};

#endif /* defined(__HttpServer__TestHttpTransaction__) */
