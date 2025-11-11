// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_TESTHTTP_H
#define MISERE_TESTHTTP_H

#include "TestSuite.h"


namespace misere {

class TestHTTP : public poivre::TestSuite {

protected:
   void runTests();

public:
   TestHTTP();

};

}

#endif

