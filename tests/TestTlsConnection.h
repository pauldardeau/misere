// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_TESTTLSCONNECTION_H
#define MISERE_TESTTLSCONNECTION_H

#include "TestSuite.h"

namespace misere {

class TestTlsConnection : public poivre::TestSuite {

protected:
   void runTests();

   void testHandshakeIOAndClose();
   void testWantReadWantWriteRetryHandling();
   void testHandshakeFailureThrows();

public:
   TestTlsConnection();

};

}

#endif
