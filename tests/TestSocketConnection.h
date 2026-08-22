// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_TESTSOCKETCONNECTION_H
#define MISERE_TESTSOCKETCONNECTION_H

#include "TestSuite.h"

namespace misere {

class TestSocketConnection : public poivre::TestSuite {

protected:
   void runTests();

   void testRead();
   void testReadPartial();
   void testWrite();
   void testWriteAfterPeerClosed();
   void testClose();
   void testUnownedSocketNotDeleted();

public:
   TestSocketConnection();

};

}

#endif
