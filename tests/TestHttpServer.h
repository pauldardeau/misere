// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_TESTHTTPSERVER_H
#define MISERE_TESTHTTPSERVER_H

#include "TestSuite.h"

namespace misere
{

class TestHttpServer : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testConstructorWithPort();
   void testConstructorWithConfigFile();
   void testGetSystemDateGMT();
   void testGetLocalDateTime();
   void testBuildHeader();
   void testIsDebug();
   void testAddPathHandler();
   void testRemovePathHandler();
   void testGetPathHandler();
   void testRunServer();
   void testLogRequest();
   void testLogRequestWithThreadId();
   void testGetConfigDataSource();
   void testGetSocketSendBufferSize();
   void testGetSocketReceiveBufferSize();
   void testGetServerId();
   void testPlatformPointerSizeBits();

public:
   TestHttpServer();

};

}

#endif
