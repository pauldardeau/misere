// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestServerSocket__
#define __HttpServer__TestServerSocket__

#include "TestSuite.h"


class TestServerSocket : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testAccept();
   void testClose();

public:
   TestServerSocket();
   

};

#endif /* defined(__HttpServer__TestServerSocket__) */
