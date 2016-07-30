// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "TestHttpException.h"
#include "HttpException.h"

using namespace misere;
using namespace chaudiere;

//******************************************************************************

TestHttpException::TestHttpException() :
   TestSuite("TestHttpException") {
}

//******************************************************************************

void TestHttpException::runTests() {
   testConstructor();
   testCopyConstructor();
   testMoveConstructor();
   testAssignmentCopy();
   testAssignmentMove();
   testGetStatusCode();
}

//******************************************************************************

void TestHttpException::testConstructor() {
   TEST_CASE("testConstructor");
   HttpException httpException(500,"Server Error");
}

//******************************************************************************

void TestHttpException::testCopyConstructor() {
   TEST_CASE("testCopyConstructor");

   const int statusCode = 500;
   std::string reason = "Server Error";
   
   HttpException httpException(statusCode, reason);
   HttpException copyException(httpException);
   require(statusCode == copyException.getStatusCode(), "status code should match");
   requireStringEquals(reason, copyException.whatString(), "reason should match");
}

//******************************************************************************

void TestHttpException::testMoveConstructor() {
   TEST_CASE("testMoveConstructor");
}

//******************************************************************************

void TestHttpException::testAssignmentCopy() {
   TEST_CASE("testAssignmentCopy");
   
   const int statusCode = 500;
   std::string reason = "Server Error";
   
   HttpException httpException(statusCode, reason);
   HttpException copyException(404, "Not Found");
   copyException = httpException;
   require(statusCode == copyException.getStatusCode(), "status code should match");
   requireStringEquals(reason, copyException.whatString(), "reason should match");
}

//******************************************************************************

void TestHttpException::testAssignmentMove() {
   TEST_CASE("testAssignmentMove");
}

//******************************************************************************

void TestHttpException::testGetStatusCode() {
   TEST_CASE("testGetStatusCode");
   
   const int statusCode = 500;
   std::string reason = "Server Error";
   
   HttpException httpException(statusCode, reason);
   require(statusCode == httpException.getStatusCode(), "status code should match");
}

//******************************************************************************

