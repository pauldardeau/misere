// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_TESTHTTPRESPONSE_H
#define MISERE_TESTHTTPRESPONSE_H

#include "TestSuite.h"


namespace misere
{

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

}

#endif
