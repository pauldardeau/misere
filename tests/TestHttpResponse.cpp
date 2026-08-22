// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "TestHttpResponse.h"
#include "HttpResponse.h"
#include "SocketConnection.h"
#include "MockSocket.h"

using namespace std;
using namespace misere;

static const string EOL = "\r\n";

//******************************************************************************

TestHttpResponse::TestHttpResponse() :
   poivre::TestSuite("TestHttpResponse") {
}

//******************************************************************************

void TestHttpResponse::runTests() {
   testConstructor();
   testConstructorWithSocket();
   testTwoResponsesInSingleRead();
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
   TEST_CASE("testConstructorWithSocket");

   const string bodyText = "hello client";
   const string resp = "HTTP/1.1 200 OK" + EOL +
      "Content-Type: text/plain" + EOL +
      "Content-Length: " + std::to_string(bodyText.size()) + EOL +
      EOL +
      bodyText;

   MockSocket mock_socket(resp);
   // HttpResponse's single-argument connection constructor always takes
   // ownership of the connection (mirroring HttpTransaction's default),
   // so the connection itself must be heap-allocated here - it does not
   // own mock_socket, though (socketOwned=false), since that one is
   // stack-allocated.
   SocketConnection* connection = new SocketConnection(&mock_socket, false);
   HttpResponse response(connection);

   require(200 == response.getStatusCode(), "status code should be 200");

   const chaudiere::ByteBuffer* body = response.getBody();
   require(nullptr != body, "body should be present");
   requireStringEquals(bodyText, string(body->const_data(), body->size()), "body content should match");
}

//******************************************************************************

void TestHttpResponse::testTwoResponsesInSingleRead() {
   TEST_CASE("testTwoResponsesInSingleRead");

   // statuses kept below 400 - HttpResponse's constructor throws
   // HttpException for 4xx/5xx status lines, which isn't what this test
   // is exercising
   const string body1 = "first";
   const string body2 = "second-response-body";

   const string resp1 = "HTTP/1.1 200 OK" + EOL +
      "Content-Length: " + std::to_string(body1.size()) + EOL + EOL + body1;
   const string resp2 = "HTTP/1.1 201 Created" + EOL +
      "Content-Length: " + std::to_string(body2.size()) + EOL + EOL + body2;

   // both responses primed as a single payload, delivered in one
   // underlying read - same over-read scenario as the request side
   MockSocket mock_socket(resp1 + resp2);

   SocketConnection* connection1 = new SocketConnection(&mock_socket, false);
   HttpResponse response1(connection1);
   require(200 == response1.getStatusCode(), "first response status code");
   const chaudiere::ByteBuffer* b1 = response1.getBody();
   require(nullptr != b1, "first response body should be present");
   requireStringEquals(body1, string(b1->const_data(), b1->size()), "first response body content");

   const string leftover = response1.takeUnconsumedBytes();
   requireFalse(leftover.empty(), "bytes belonging to the second response should have been retained");

   SocketConnection* connection2 = new SocketConnection(&mock_socket, false);
   HttpResponse response2(connection2, leftover);
   require(201 == response2.getStatusCode(), "second response status code");
   const chaudiere::ByteBuffer* b2 = response2.getBody();
   require(nullptr != b2, "second response body should be present");
   requireStringEquals(body2, string(b2->const_data(), b2->size()), "second response body content");
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

