// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestHttpException__
#define __HttpServer__TestHttpException__

#include "TestSuite.h"


class TestHttpException : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testCopyConstructor();
   void testMoveConstructor();
   void testAssignmentCopy();
   void testAssignmentMove();
   void testGetStatusCode();

public:
   TestHttpException();
   

};

#endif /* defined(__HttpServer__TestHttpException__) */
