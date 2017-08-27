// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "TestHttpServer.h"

using namespace misere;

//******************************************************************************

TestHttpServer::TestHttpServer() :
   TestSuite("TestHttpServer") {
}

//******************************************************************************

void TestHttpServer::runTests() {
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

void TestHttpServer::testConstructor() {
   //TEST_CASE("testConstructor");
   //TODO: implement testConstructor
}

//******************************************************************************

void TestHttpServer::testConstructorWithPort() {
   //TEST_CASE("testConstructorWithPort");
   //TODO: implement testConstructorWithPort
}

//******************************************************************************

void TestHttpServer::testConstructorWithConfigFile() {
   //TEST_CASE("testConstructorWithConfigFile");
   //TODO: implement testConstructorWithConfigFile
}

//******************************************************************************

void TestHttpServer::testGetSystemDateGMT() {
   //TEST_CASE("testGetSystemDateGMT");
   //TODO: implement testGetSystemDateGMT
}

//******************************************************************************

void TestHttpServer::testGetLocalDateTime() {
   //TEST_CASE("testGetLocalDateTime");
   //TODO: implement testGetLocalDateTime
}

//******************************************************************************

void TestHttpServer::testBuildHeader() {
   //TEST_CASE("testBuildHeader");
   //TODO: implement testBuildHeader
}

//******************************************************************************

void TestHttpServer::testIsDebug() {
   //TEST_CASE("testIsDebug");
   //TODO: implement testIsDebug
}

//******************************************************************************

void TestHttpServer::testAddPathHandler() {
   //TEST_CASE("testAddPathHandler");
   //TODO: implement testAddPathHandler
}

//******************************************************************************

void TestHttpServer::testRemovePathHandler() {
   //TEST_CASE("testRemovePathHandler");
   //TODO: implement testRemovePathHandler
}

//******************************************************************************

void TestHttpServer::testGetPathHandler() {
   //TEST_CASE("testGetPathHandler");
   //TODO: implement testGetPathHandler
}

//******************************************************************************

void TestHttpServer::testRunServer() {
   //TEST_CASE("testRunServer");
   //TODO: implement testRunServer
}

//******************************************************************************

void TestHttpServer::testLogRequest() {
   //TEST_CASE("testLogRequest");
   //TODO: implement testLogRequest
}

//******************************************************************************

void TestHttpServer::testLogRequestWithThreadId() {
   //TEST_CASE("testLogRequestWithThreadId");
   //TODO: implement testLogRequestWithThreadId
}

//******************************************************************************

void TestHttpServer::testGetConfigDataSource() {
   //TEST_CASE("testGetConfigDataSource");
   //TODO: implement testGetConfigDataSource
}

//******************************************************************************

void TestHttpServer::testGetSocketSendBufferSize() {
   //TEST_CASE("testGetSocketSendBufferSize");
   //TODO: implement testGetSocketSendBufferSize
}

//******************************************************************************

void TestHttpServer::testGetSocketReceiveBufferSize() {
   //TEST_CASE("testGetSocketReceiveBufferSize");
   //TODO: implement testGetSocketReceiveBufferSize
}

//******************************************************************************

void TestHttpServer::testGetServerId() {
   //TEST_CASE("testGetServerId");
   //TODO: implement testGetServerId
}

//******************************************************************************

void TestHttpServer::testPlatformPointerSizeBits() {
   //TEST_CASE("testPlatformPointerSizeBits");
   //TODO: implement testPlatformPointerSizeBits
}

//******************************************************************************

