// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_TESTHTTPCLIENT_H
#define MISERE_TESTHTTPCLIENT_H

#include "TestSuite.h"


namespace misere {

class TestHttpClient : public poivre::TestSuite {

protected:
   void runTests();

   void testConstructor();
   void testPost();
   void testSendReceive();
   void testSendReceiveWithPost();
   void testBuildHeader();
   void testBuildHttpRequest();

public:
   TestHttpClient();

};

}

#endif

