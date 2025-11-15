// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_TESTHTTPEXCEPTION_H
#define MISERE_TESTHTTPEXCEPTION_H

#include "TestSuite.h"


namespace misere {

class TestHttpException : public chaudiere::TestSuite {

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

}

#endif

