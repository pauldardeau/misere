// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_TESTHTTPTRANSACTION_H
#define MISERE_TESTHTTPTRANSACTION_H

#include "TestSuite.h"

namespace misere {

class TestHttpTransaction : public TestSuite {

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

}

#endif

