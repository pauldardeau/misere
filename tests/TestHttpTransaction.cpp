// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "TestHttpTransaction.h"
#include "HttpTransaction.h"
#include "MockSocket.h"

using namespace std;
using namespace misere;
using namespace poivre;

static const string EOL = "\r\n";

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
   //TEST_CASE("testConstructor");
   //TODO: implement testConstructor
}

//******************************************************************************

void TestHttpTransaction::testCopyConstructor() {
   //TEST_CASE("testCopyConstructor");
   //TODO: implement testCopyConstructor
}

//******************************************************************************

void TestHttpTransaction::testMoveConstructor() {
   //TEST_CASE("testMoveConstructor");
   //TODO: implement testMoveConstructor
}

//******************************************************************************

void TestHttpTransaction::testAssignmentCopy() {
   //TEST_CASE("testAssignmentCopy");
   //TODO: implement testAssignmentCopy
}

//******************************************************************************

void TestHttpTransaction::testAssignmentMove() {
   //TEST_CASE("testAssignmentMove");
   //TODO: implement testAssignmentMove
}

//******************************************************************************

void TestHttpTransaction::testGetRawHeader() {
   //TEST_CASE("testGetRawHeader");
   //TODO: implement testGetRawHeader
}

//******************************************************************************

void TestHttpTransaction::testGetBody() {
   //TEST_CASE("testGetBody");
   //TODO: implement testGetBody
}

//******************************************************************************

void TestHttpTransaction::testSetBody() {
   //TEST_CASE("testSetBody");
   //TODO: implement testSetBody
}

//******************************************************************************

void TestHttpTransaction::testHasHeaderValue() {
   //TEST_CASE("testHasHeaderValue");
   //TODO: implement testHasHeaderValue
}

//******************************************************************************

void TestHttpTransaction::testGetHeaderValue() {
   //TEST_CASE("testGetHeaderValue");
   //TODO: implement testGetHeaderValue
}

//******************************************************************************

void TestHttpTransaction::testGetHeaderValues() {
   //TEST_CASE("testGetHeaderValues");
   //TODO: implement testGetHeaderValues
}

//******************************************************************************

void TestHttpTransaction::testSetHeaderValue() {
   //TEST_CASE("testSetHeaderValue");
   //TODO: implement testSetHeaderValue
}

//******************************************************************************

void TestHttpTransaction::testGetProtocol() {
   //TEST_CASE("testGetProtocol");
   //TODO: implement testGetProtocol
}

//******************************************************************************

void TestHttpTransaction::testStreamFromSocket() {
   TEST_CASE("testStreamFromSocket");

   const string verb = "GET";
   const string resource = "/doc/test.html";
   const string protocol = "HTTP/1.1";
   const string request_line = verb + " " + resource + " " + protocol;
   const string host = "www.acme.com";
   const string accept = "image/gif, image/jpeg, */*";
   const string accept_language = "en-us";
   const string accept_encoding = "gzip, deflate";
   const string user_agent = "Mozilla";

   const string key_host = "Host";
   const string key_accept = "Accept";
   const string key_accept_language = "Accept-Language";
   const string key_accept_encoding = "Accept-Encoding";
   const string key_user_agent = "User-Agent";

   const string req = request_line + EOL +
   key_host + ": " + host + EOL +
   key_accept + ": " + accept + EOL +
   key_accept_language + ": " + accept_language + EOL +
   key_accept_encoding + ": " + accept_encoding + EOL +
   key_user_agent + ": " + user_agent + EOL + EOL;

   HttpTransaction txn;
   MockSocket mock_socket(req);
   //require(txn.streamFromSocket(), "streamFromSocket");

   // request line
   requireStringEquals(verb, txn.getRequestMethod(), "http verb");
   requireStringEquals(resource, txn.getRequestPath(), "request path");
   requireStringEquals(protocol, txn.getProtocol(), "protocol");
   requireStringEquals(request_line, txn.getFirstHeaderLine(), "request line");

   // host
   require(txn.hasHeaderValue(key_host), "host header exists");
   requireStringEquals(host, txn.getHeaderValue(key_host), "host");

   // accept
   require(txn.hasHeaderValue(key_accept), "accept header exists");
   requireStringEquals(accept, txn.getHeaderValue(key_accept), "accept");

   // accept language
   require(txn.hasHeaderValue(key_accept_language), "accept language exists");
   requireStringEquals(accept_language, txn.getHeaderValue(key_accept_language), "accept language");

   // accept encoding
   require(txn.hasHeaderValue(key_accept_encoding), "accept encoding exists");
   requireStringEquals(accept_encoding, txn.getHeaderValue(key_accept_encoding), "accept encoding");

   // user agent
   require(txn.hasHeaderValue(key_user_agent), "user agent exists");
   requireStringEquals(user_agent, txn.getHeaderValue(key_user_agent), "user agent");
}

//*****************************************************************************

