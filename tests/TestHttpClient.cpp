// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "TestHttpClient.h"
#include "HttpClient.h"

using namespace misere;

//******************************************************************************

TestHttpClient::TestHttpClient() :
   TestSuite("TestHttpClient") {
}

//******************************************************************************

void TestHttpClient::runTests() {
   testConstructor();
   testPost();
   testSendReceive();
   testSendReceiveWithPost();
   testBuildHeader();
   testBuildHttpRequest();
}

//******************************************************************************

void TestHttpClient::testConstructor() {
   TEST_CASE("testConstructor");
}

//******************************************************************************

void TestHttpClient::testPost() {
   TEST_CASE("testPost");
}

//******************************************************************************

void TestHttpClient::testSendReceive() {
   TEST_CASE("testSendReceive");
}

//******************************************************************************

void TestHttpClient::testSendReceiveWithPost() {
   TEST_CASE("testSendReceiveWithPost");
}

//******************************************************************************

void TestHttpClient::testBuildHeader() {
   TEST_CASE("testBuildHeader");
}

//******************************************************************************

void TestHttpClient::testBuildHttpRequest() {
   TEST_CASE("testBuildHttpRequest");
}

//******************************************************************************

