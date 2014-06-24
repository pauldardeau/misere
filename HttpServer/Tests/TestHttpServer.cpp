// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "TestHttpServer.h"

//******************************************************************************

TestHttpServer::TestHttpServer() :
   TestSuite("TestHttpServer")
{
}

//******************************************************************************

void TestHttpServer::runTests()
{
   testConstructor();
   testConstructorWithPort();
   testConstructorWithConfigFile();
   testGetSystemDateGMT();
   testGetLocalDateTime();
   testBuildHeader();
   testIsDebug();
   testAddPathHandler();
   testRemovePathHandler();
   testGetPathHandler();
   testRunServer();
   testLogRequest();
   testLogRequestWithThreadId();
   testGetConfigDataSource();
   testGetSocketSendBufferSize();
   testGetSocketReceiveBufferSize();
   testGetServerId();
   testPlatformPointerSizeBits();
}

//******************************************************************************

void TestHttpServer::testConstructor()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testConstructorWithPort()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testConstructorWithConfigFile()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testGetSystemDateGMT()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testGetLocalDateTime()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testBuildHeader()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testIsDebug()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testAddPathHandler()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testRemovePathHandler()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testGetPathHandler()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testRunServer()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testLogRequest()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testLogRequestWithThreadId()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testGetConfigDataSource()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testGetSocketSendBufferSize()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testGetSocketReceiveBufferSize()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testGetServerId()
{
   TEST_CASE("test");
}

//******************************************************************************

void TestHttpServer::testPlatformPointerSizeBits()
{
   TEST_CASE("test");
}

//******************************************************************************
