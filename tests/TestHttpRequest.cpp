// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "TestHttpRequest.h"
#include "HttpRequest.h"
#include "MockSocket.h"

// Modeled after tests from:
// http://subversion.assembla.com/svn/opencats/trunk/cats-0.9.2/lib/simpletest/test/http_test.php

static const std::string GET_PATH = "http://a.valid.host/here.html";
static const std::string POST_PATH = "/here.html";

static const std::string DEFAULT_GET =
   "GET " + GET_PATH  + " HTTP/1.0\r\n" +
   "Host: my-proxy:8080\r\n" +
   "Connection: close\r\n";

static const std::string DEFAULT_POST =
   "POST " + POST_PATH + " HTTP/1.0\r\n" +
   "Host: a.valid.host\r\n" +
   "Connection: close\r\n";

static const std::string GET_WITH_PORT =
   "GET /here.html HTTP/1.0\r\n"
   "Host: a.valid.host:81\r\n"
   "Connection: close\r\n";

static const std::string GET_WITH_PARAMETERS =
   "GET /here.html?a=1&b=2 HTTP/1.0\r\n"
   "Host: a.valid.host\r\n"
   "Connection: close\r\n";

using namespace misere;

//******************************************************************************

TestHttpRequest::TestHttpRequest() :
   TestSuite("TestHttpRequest") {
}

//******************************************************************************

void TestHttpRequest::runTests() {
   testConstructor();
   testCopyConstructor();
   testMoveConstructor();
   testAssignmentCopy();
   testAssignmentMove();
   testStreamFromSocket();
   testGetRequest();
   testGetMethod();
   testGetPath();
   testGetRequestLine();
   testHasArgument();
   testGetArgument();
   testGetArgumentKeys();
}

//******************************************************************************

void TestHttpRequest::testConstructor() {
   TEST_CASE("testConstructor");
   MockSocket socket(DEFAULT_GET);
   HttpRequest request(socket);
}

//******************************************************************************

void TestHttpRequest::testCopyConstructor() {
   //TEST_CASE("testCopyConstructor");
   //TODO: implement testCopyConstructor
}

//******************************************************************************

void TestHttpRequest::testMoveConstructor() {
   //TEST_CASE("testMoveConstructor");
   //TODO: implement testMoveConstructor
}

//******************************************************************************

void TestHttpRequest::testAssignmentCopy() {
   //TEST_CASE("testAssignmentCopy");
   //TODO: implement testAssignmentCopy
}

//******************************************************************************

void TestHttpRequest::testAssignmentMove() {
   //TEST_CASE("testAssignmentMove");
   //TODO: implement testAssignmentMove
}

//******************************************************************************

void TestHttpRequest::testStreamFromSocket() {
   TEST_CASE("testStreamFromSocket");
   
   MockSocket socket(DEFAULT_GET);
   HttpRequest request(socket);
}

//******************************************************************************

void TestHttpRequest::testGetRequest() {
   TEST_CASE("testGetRequest");
   
   MockSocket socket(DEFAULT_GET);
   HttpRequest request(socket);
}

//******************************************************************************

void TestHttpRequest::testGetMethod() {
   TEST_CASE("testGetMethod");

   MockSocket socketGet(DEFAULT_GET);
   HttpRequest requestGet(socketGet);
   requireStringEquals("GET", requestGet.getMethod(), "method is GET");

   MockSocket socketPost(DEFAULT_POST);
   HttpRequest requestPost(socketPost);
   requireStringEquals("POST", requestPost.getMethod(), "method is POST");
}

//******************************************************************************

void TestHttpRequest::testGetPath() {
   TEST_CASE("testGetPath");

   MockSocket socketGet(DEFAULT_GET);
   HttpRequest requestGet(socketGet);
   requireStringEquals(GET_PATH, requestGet.getPath(), "path should be GET path");
   
   MockSocket socketPost(DEFAULT_POST);
   HttpRequest requestPost(socketPost);
   requireStringEquals(POST_PATH, requestPost.getPath(), "path should be POST path");
}

//******************************************************************************

void TestHttpRequest::testGetRequestLine() {
   //TEST_CASE("testGetRequestLine");
   //TODO: implement testGetRequestLine
}

//******************************************************************************

void TestHttpRequest::testHasArgument() {
   //TEST_CASE("testHasArgument");
   //TODO: implement testHasArgument
}

//******************************************************************************

void TestHttpRequest::testGetArgument() {
   //TEST_CASE("testGetArgument");
   //TODO: implement testGetArgument
}

//******************************************************************************

void TestHttpRequest::testGetArgumentKeys() {
   //TEST_CASE("testGetArgumentKeys");
   //TODO: implement testGetArgumentKeys
}

//******************************************************************************

