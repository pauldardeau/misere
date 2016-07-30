// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "TestHttpTransaction.h"
#include "HttpTransaction.h"
#include "Socket.h"

using namespace misere;

//******************************************************************************

TestHttpTransaction::TestHttpTransaction() :
   TestSuite("TestHttpTransaction") {
}

//******************************************************************************

void TestHttpTransaction::runTests() {
   testConstructor();
   testCopyConstructor();
   testMoveConstructor();
   testAssignmentCopy();
   testAssignmentMove();
   testStreamFromSocket();
   testGetRawHeader();
   testGetBody();
   testSetBody();
   testHasHeaderValue();
   testGetHeaderValue();
   testGetHeaderValues();
   testSetHeaderValue();
   testGetProtocol();
}

//******************************************************************************

void TestHttpTransaction::testConstructor() {
   TEST_CASE("testConstructor");
}

//******************************************************************************

void TestHttpTransaction::testCopyConstructor() {
   TEST_CASE("testCopyConstructor");
}

//******************************************************************************

void TestHttpTransaction::testMoveConstructor() {
   TEST_CASE("testMoveConstructor");
}

//******************************************************************************

void TestHttpTransaction::testAssignmentCopy() {
   TEST_CASE("testAssignmentCopy");
}

//******************************************************************************

void TestHttpTransaction::testAssignmentMove() {
   TEST_CASE("testAssignmentMove");
}

//******************************************************************************

void TestHttpTransaction::testStreamFromSocket() {
   TEST_CASE("testStreamFromSocket");
}

//******************************************************************************

void TestHttpTransaction::testGetRawHeader() {
   TEST_CASE("testGetRawHeader");
}

//******************************************************************************

void TestHttpTransaction::testGetBody() {
   TEST_CASE("testGetBody");
}

//******************************************************************************

void TestHttpTransaction::testSetBody() {
   TEST_CASE("testSetBody");
}

//******************************************************************************

void TestHttpTransaction::testHasHeaderValue() {
   TEST_CASE("testHasHeaderValue");
}

//******************************************************************************

void TestHttpTransaction::testGetHeaderValue() {
   TEST_CASE("testGetHeaderValue");
}

//******************************************************************************

void TestHttpTransaction::testGetHeaderValues() {
   TEST_CASE("testGetHeaderValues");
}

//******************************************************************************

void TestHttpTransaction::testSetHeaderValue() {
   TEST_CASE("testSetHeaderValue");
}

//******************************************************************************

void TestHttpTransaction::testGetProtocol() {
   TEST_CASE("testGetProtocol");
}

//******************************************************************************

