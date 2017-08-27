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
   //TEST_CASE("testConstructor");
   //TODO: implement testConstructor
}

//******************************************************************************

void TestHttpClient::testPost() {
   //TEST_CASE("testPost");
   //TODO: implement testPost
}

//******************************************************************************

void TestHttpClient::testSendReceive() {
   //TEST_CASE("testSendReceive");
   //TODO: implement testSendReceive
}

//******************************************************************************

void TestHttpClient::testSendReceiveWithPost() {
   //TEST_CASE("testSendReceiveWithPost");
   //TODO: implement testSendReceiveWithPost
}

//******************************************************************************

void TestHttpClient::testBuildHeader() {
   //TEST_CASE("testBuildHeader");
   //TODO: implement testBuildHeader
}

//******************************************************************************

void TestHttpClient::testBuildHttpRequest() {
   //TEST_CASE("testBuildHttpRequest");
   //TODO: implement testBuildHttpRequest
}

//******************************************************************************

