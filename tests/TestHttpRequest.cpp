// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <string>

#include "TestHttpRequest.h"
#include "HttpRequest.h"
#include "SocketConnection.h"
#include "MockSocket.h"
#include "ByteBuffer.h"

// Modeled after tests from:
// http://subversion.assembla.com/svn/opencats/trunk/cats-0.9.2/lib/simpletest/test/http_test.php

static const std::string GET_PATH = "http://a.valid.host/here.html";
static const std::string POST_PATH = "/here.html";

static const std::string DEFAULT_GET =
   "GET " + GET_PATH  + " HTTP/1.0\r\n" +
   "Host: my-proxy:8080\r\n" +
   "Connection: close\r\n" +
   "\r\n";

static const std::string DEFAULT_POST =
   "POST " + POST_PATH + " HTTP/1.0\r\n" +
   "Host: a.valid.host\r\n" +
   "Connection: close\r\n" +
   "\r\n";

static const std::string GET_WITH_PORT =
   "GET /here.html HTTP/1.0\r\n"
   "Host: a.valid.host:81\r\n"
   "Connection: close\r\n"
   "\r\n";

static const std::string GET_WITH_PARAMETERS =
   "GET /here.html?a=1&b=2 HTTP/1.0\r\n"
   "Host: a.valid.host\r\n"
   "Connection: close\r\n"
   "\r\n";

using namespace misere;

//******************************************************************************

TestHttpRequest::TestHttpRequest() :
   poivre::TestSuite("TestHttpRequest") {
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
   testTwoRequestsInSingleRead();
   testRequestWithBodyFollowedByNextRequest();
}

//******************************************************************************

void TestHttpRequest::testConstructor() {
   TEST_CASE("testConstructor");
   MockSocket socket(DEFAULT_GET);
   SocketConnection connection(&socket, false);
   HttpRequest request(&connection, false);
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
   SocketConnection connection(&socket, false);
   HttpRequest request(&connection, false);
}

//******************************************************************************

void TestHttpRequest::testGetRequest() {
   TEST_CASE("testGetRequest");

   MockSocket socket(DEFAULT_GET);
   SocketConnection connection(&socket, false);
   HttpRequest request(&connection, false);
}

//******************************************************************************

void TestHttpRequest::testGetMethod() {
   TEST_CASE("testGetMethod");

   MockSocket socketGet(DEFAULT_GET);
   SocketConnection connectionGet(&socketGet, false);
   HttpRequest requestGet(&connectionGet, false);
   requireStringEquals("GET", requestGet.getMethod(), "method is GET");

   MockSocket socketPost(DEFAULT_POST);
   SocketConnection connectionPost(&socketPost, false);
   HttpRequest requestPost(&connectionPost, false);
   requireStringEquals("POST", requestPost.getMethod(), "method is POST");
}

//******************************************************************************

void TestHttpRequest::testGetPath() {
   TEST_CASE("testGetPath");

   MockSocket socketGet(DEFAULT_GET);
   SocketConnection connectionGet(&socketGet, false);
   HttpRequest requestGet(&connectionGet, false);
   requireStringEquals(GET_PATH, requestGet.getPath(), "path should be GET path");

   MockSocket socketPost(DEFAULT_POST);
   SocketConnection connectionPost(&socketPost, false);
   HttpRequest requestPost(&connectionPost, false);
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

void TestHttpRequest::testTwoRequestsInSingleRead() {
   TEST_CASE("testTwoRequestsInSingleRead");

   const std::string req1 = "GET /first HTTP/1.1\r\n"
                             "Host: host\r\n"
                             "Connection: keep-alive\r\n\r\n";
   const std::string req2 = "GET /second HTTP/1.1\r\n"
                             "Host: host\r\n"
                             "Connection: keep-alive\r\n\r\n";

   // both requests primed as a single payload - MockSocket serves the
   // whole thing on the first read, so streamFromConnection() must find
   // request 1's terminator, leave request 2's bytes in unconsumed
   // storage, and never touch the underlying socket for request 2 at all
   MockSocket socket(req1 + req2);
   SocketConnection connection(&socket, false);

   HttpRequest request1(&connection, false);
   requireStringEquals(std::string("/first"), request1.getPath(), "first request path");

   const std::string leftover = request1.takeUnconsumedBytes();
   requireFalse(leftover.empty(), "bytes belonging to the second request should have been retained");

   HttpRequest request2(&connection, false, leftover);
   requireStringEquals(std::string("/second"), request2.getPath(), "second request path");
   require(request2.takeUnconsumedBytes().empty(), "nothing should remain after the second request is fully consumed");
}

//******************************************************************************

void TestHttpRequest::testRequestWithBodyFollowedByNextRequest() {
   TEST_CASE("testRequestWithBodyFollowedByNextRequest");

   const std::string bodyText = "abc=123";
   const std::string req1 = "POST /submit HTTP/1.1\r\n"
                             "Host: host\r\n"
                             "Content-Length: " + std::to_string(bodyText.size()) + "\r\n"
                             "Connection: keep-alive\r\n\r\n" + bodyText;
   const std::string req2 = "GET /second HTTP/1.1\r\n"
                             "Host: host\r\n"
                             "Connection: keep-alive\r\n\r\n";

   // request 2's bytes arrive in the same underlying read that completes
   // request 1's body
   MockSocket socket(req1 + req2);
   SocketConnection connection(&socket, false);

   HttpRequest request1(&connection, false);
   requireStringEquals(std::string("/submit"), request1.getPath(), "first request path");

   const chaudiere::ByteBuffer* body = request1.getBody();
   require(nullptr != body, "first request body should be present");
   requireStringEquals(bodyText, std::string(body->const_data(), body->size()), "first request body content");

   const std::string leftover = request1.takeUnconsumedBytes();
   requireFalse(leftover.empty(), "bytes belonging to the second request should have been retained");

   HttpRequest request2(&connection, false, leftover);
   requireStringEquals(std::string("/second"), request2.getPath(), "second request path");
}

//******************************************************************************

