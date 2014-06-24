// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestSocket__
#define __HttpServer__TestSocket__

#include "TestSuite.h"


class TestSocket : public TestSuite
{
protected:
   void runTests();
   
   void testConstructorWithAddress();
   void testConstructorWithFD();
   void testConstructorWithCompletionObserver();
   
   void testSend();
   void testWriteWithBuffer();
   void testWriteWithString();
   
   void testReceive();
   void testRead();
   
   void testClose();
   void testIsOpen();
   void testIsConnected();
   void testCloseConnection();
   void testGetFileDescriptor();
   void testRequestComplete();
   
   void testSetUserIndex();
   void testGetUserIndex();
   
   void testSetTcpNoDelay();
   void testGetTcpNoDelay();
   
   void testSetSendBufferSize();
   void testGetSendBufferSize();
   
   void testSetReceiveBufferSize();
   void testGetReceiveBufferSize();
   
   void testSetKeepAlive();
   void testGetKeepAlive();
   
   void testReadLine();
   
   void testGetPeerIPAddress();
   
   void testGetPort();
   
   void testSetIncludeMessageSize();

public:
   TestSocket();
   

};

#endif /* defined(__HttpServer__TestSocket__) */
