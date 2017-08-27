// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "TestHttpResponse.h"
#include "HttpResponse.h"
#include "Socket.h"

using namespace misere;

//******************************************************************************

TestHttpResponse::TestHttpResponse() :
   TestSuite("TestHttpResponse") {
}

//******************************************************************************

void TestHttpResponse::runTests() {
   testConstructor();
   testConstructorWithSocket();
   testCopyConstructor();
   testMoveConstructor();
   testAssignmentCopy();
   testAssignmentMove();
   testStreamFromSocket();
   testGetStatusLine();
   testGetStatusCode();
   testSetStatusCode();
   testGetReasonPhrase();
}

//******************************************************************************

void TestHttpResponse::testConstructor() {
   //TEST_CASE("testConstructor");
   //TODO: implement testConstructor
}

//******************************************************************************

void TestHttpResponse::testConstructorWithSocket() {
   //TEST_CASE("testConstructorWithSocket");
   //TODO: implement testConstructorWithSocket
}

//******************************************************************************

void TestHttpResponse::testCopyConstructor() {
   //TEST_CASE("testCopyConstructor");
   //TODO: implement testCopyConstructor
}

//******************************************************************************

void TestHttpResponse::testMoveConstructor() {
   //TEST_CASE("testMoveConstructor");
   //TODO: implement testMoveConstructor
}

//******************************************************************************

void TestHttpResponse::testAssignmentCopy() {
   //TEST_CASE("testAssignmentCopy");
   //TODO: implement testAssignmentCopy
}

//******************************************************************************

void TestHttpResponse::testAssignmentMove() {
   //TEST_CASE("testAssignmentMove");
   //TODO: implement testAssignmentMove
}

//******************************************************************************

void TestHttpResponse::testStreamFromSocket() {
   //TEST_CASE("testStreamFromSocket");
   //TODO: implement testStreamFromSocket
}

//******************************************************************************

void TestHttpResponse::testGetStatusLine() {
   //TEST_CASE("testGetStatusLine");
   //TODO: implement testGetStatusLine
}

//******************************************************************************

void TestHttpResponse::testGetStatusCode() {
   //TEST_CASE("testGetStatusCode");
   //TODO: implement testGetStatusCode
}

//******************************************************************************

void TestHttpResponse::testSetStatusCode() {
   //TEST_CASE("testSetStatusCode");
   //TODO: implement testSetStatusCode
}

//******************************************************************************

void TestHttpResponse::testGetReasonPhrase() {
   //TEST_CASE("testGetReasonPhrase");
   //TODO: implement testGetReasonPhrase
}

//******************************************************************************

