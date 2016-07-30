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
   TEST_CASE("testConstructor");
}

//******************************************************************************

void TestHttpResponse::testConstructorWithSocket() {
   TEST_CASE("testConstructorWithSocket");
}

//******************************************************************************

void TestHttpResponse::testCopyConstructor() {
   TEST_CASE("testCopyConstructor");
}

//******************************************************************************

void TestHttpResponse::testMoveConstructor() {
   TEST_CASE("testMoveConstructor");
}

//******************************************************************************

void TestHttpResponse::testAssignmentCopy() {
   TEST_CASE("testAssignmentCopy");
}

//******************************************************************************

void TestHttpResponse::testAssignmentMove() {
   TEST_CASE("testAssignmentMove");
}

//******************************************************************************

void TestHttpResponse::testStreamFromSocket() {
   TEST_CASE("testStreamFromSocket");
}

//******************************************************************************

void TestHttpResponse::testGetStatusLine() {
   TEST_CASE("testGetStatusLine");
}

//******************************************************************************

void TestHttpResponse::testGetStatusCode() {
   TEST_CASE("testGetStatusCode");
}

//******************************************************************************

void TestHttpResponse::testSetStatusCode() {
   TEST_CASE("testSetStatusCode");
}

//******************************************************************************

void TestHttpResponse::testGetReasonPhrase() {
   TEST_CASE("testGetReasonPhrase");
}

//******************************************************************************

