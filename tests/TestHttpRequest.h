// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_TESTHTTPREQUEST_H
#define MISERE_TESTHTTPREQUEST_H

#include "TestSuite.h"


namespace misere {

class TestHttpRequest : public poivre::TestSuite {

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
   
}

#endif

