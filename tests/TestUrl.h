// Copyright Paul Dardeau, SwampBits LLC 2017
// BSD License

#ifndef MISERE_TESTURL_H
#define MISERE_TESTURL_H

#include "TestSuite.h"


namespace misere {

class TestUrl : public chaudiere::TestSuite {

protected:
   void runTests();

   void testConstructor();
   void testCopyConstructor();
   void testAssignmentCopy();
   void testGetFullText();
   void testGetHost();
   void testGetPort();
   void testGetPath();
   void testGetProtocol();

public:
   TestUrl();

};

}

#endif

