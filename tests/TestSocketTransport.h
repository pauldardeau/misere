// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_TESTSOCKETTRANSPORT_H
#define MISERE_TESTSOCKETTRANSPORT_H

#include "TestSuite.h"

namespace misere {

class TestSocketTransport : public poivre::TestSuite {

protected:
   void runTests();

   void testRead();
   void testReadPartial();
   void testReadClosed();
   void testWrite();
   void testUnownedSocketNotDeleted();

public:
   TestSocketTransport();

};

}

#endif
