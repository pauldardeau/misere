// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_TESTHTTPSINTEGRATION_H
#define MISERE_TESTHTTPSINTEGRATION_H

#include "TestSuite.h"

namespace misere {

class TestHttpsIntegration : public poivre::TestSuite {

protected:
   void runTests();

   void testPlainHttpUnaffectedByTlsSupport();
   void testTlsServerAcceptsRealHandshakeAndServesRequest();
   void testTlsKeepAliveReusesConnectionAcrossRequests();
   void testTlsEnabledWithoutCertificateFailsInitialization();
   void testTlsEnabledWithoutPrivateKeyFailsInitialization();
   void testTlsEnabledWithInvalidCertificatePathFailsInitialization();
   void testTlsEnabledWithInvalidPrivateKeyPathFailsInitialization();
   void testHandshakeFailureDoesNotReachHandler();

public:
   TestHttpsIntegration();

};

}

#endif
