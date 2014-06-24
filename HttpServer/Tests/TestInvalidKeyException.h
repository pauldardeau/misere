// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestInvalidKeyException__
#define __HttpServer__TestInvalidKeyException__

#include "TestSuite.h"


class TestInvalidKeyException : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testCopyConstructor();
   void testMoveConstructor();
   
   void testAssignmentCopy();
   void testAssignmentMove();
   
   void testGetKey();
   
public:
   TestInvalidKeyException();
   

   
};

#endif /* defined(__HttpServer__TestInvalidKeyException__) */
