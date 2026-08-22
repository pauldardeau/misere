// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "TestHttpTransaction.h"
#include "HttpTransaction.h"
#include "SocketConnection.h"
#include "MockSocket.h"

using namespace std;
using namespace misere;
using namespace chaudiere;

static const string EOL = "\r\n";

namespace {
// streamFromConnection() is protected on HttpTransaction (it's meant to be
// invoked by derived classes like HttpRequest/HttpResponse from their own
// constructors) -- re-expose it publicly so this test can exercise the
// base class's parsing logic directly.
class TestableHttpTransaction : public HttpTransaction {
public:
   TestableHttpTransaction(ByteConnection* connection, bool connectionOwned) :
      HttpTransaction(connection, connectionOwned) {
   }

   using HttpTransaction::streamFromConnection;
};

// A minimal ByteConnection that records whether it has been destroyed and
// whether close() was called on it, so close()/ownership behavior can be
// verified precisely rather than just "didn't crash".
class TrackingConnection : public ByteConnection {
public:
   TrackingConnection(bool* deletedFlag, bool* closedFlag) :
      m_deleted(deletedFlag),
      m_closed(closedFlag) {
      *m_deleted = false;
      *m_closed = false;
   }

   ~TrackingConnection() override {
      *m_deleted = true;
   }

   int read(char*, int) override { return -1; }
   bool write(const char*, std::size_t) override { return false; }
   void close() override { *m_closed = true; }

private:
   bool* m_deleted;
   bool* m_closed;
};
}

//******************************************************************************

TestHttpTransaction::TestHttpTransaction() :
   poivre::TestSuite("TestHttpTransaction") {
}

//******************************************************************************

void TestHttpTransaction::runTests() {
   testConstructor();
   testCopyConstructor();
   testMoveConstructor();
   testAssignmentCopy();
   testAssignmentMove();
   testStreamFromSocket();
   testStreamFromSocketWithBody();
   testStreamFromSocketSequentialRequests();
   testGetRawHeader();
   testGetBody();
   testSetBody();
   testHasHeaderValue();
   testGetHeaderValue();
   testGetHeaderValues();
   testSetHeaderValue();
   testGetProtocol();
   testCloseDoesNotDeleteUnownedConnection();
   testCloseDeletesOwnedConnection();
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

   MockSocket mock_socket(req);
   SocketConnection connection(&mock_socket, false);
   TestableHttpTransaction txn(&connection, false);
   require(txn.streamFromConnection(), "streamFromConnection");

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

void TestHttpTransaction::testStreamFromSocketWithBody() {
   TEST_CASE("testStreamFromSocketWithBody");

   // large enough that the header terminator and the full body can't both
   // arrive in the same 8192-byte read - this exercises the body-reading
   // loop's own read() calls, not just the leftover bytes captured while
   // scanning for the end of headers
   string bodyText;
   bodyText.reserve(9000);
   while (bodyText.size() < 9000) {
      bodyText += "0123456789";
   }

   const string req = "POST /submit HTTP/1.1" + EOL +
      "Host: www.acme.com" + EOL +
      "Content-Length: " + std::to_string(bodyText.size()) + EOL +
      "Connection: close" + EOL + EOL +
      bodyText;

   MockSocket mock_socket(req);
   SocketConnection connection(&mock_socket, false);
   TestableHttpTransaction txn(&connection, false);

   require(txn.streamFromConnection(), "streamFromConnection with body");

   const chaudiere::ByteBuffer* body = txn.getBody();
   require(nullptr != body, "body should be present");
   require((int) bodyText.size() == body->size(), "body size should match Content-Length");
   requireStringEquals(bodyText, string(body->const_data(), body->size()), "body content should match what was sent");
}

//*****************************************************************************

void TestHttpTransaction::testStreamFromSocketSequentialRequests() {
   TEST_CASE("testStreamFromSocketSequentialRequests");

   const string req1 = "GET /first HTTP/1.1" + EOL +
      "Host: www.acme.com" + EOL + "Connection: keep-alive" + EOL + EOL;
   const string req2 = "GET /second HTTP/1.1" + EOL +
      "Host: www.acme.com" + EOL + "Connection: keep-alive" + EOL + EOL;

   MockSocket mock_socket(req1);
   // borrowed, not owned - the same connection is reused across both
   // requests below, matching how HttpRequestHandler reuses one
   // connection across the requests on a persistent connection
   SocketConnection connection(&mock_socket, false);

   TestableHttpTransaction txn1(&connection, false);
   require(txn1.streamFromConnection(), "first request should parse");
   requireStringEquals(string("GET /first HTTP/1.1"), txn1.getFirstHeaderLine(), "first request line");

   // simulate the second request arriving only after the first was fully
   // consumed
   mock_socket.setNextPayload(req2);

   TestableHttpTransaction txn2(&connection, false);
   require(txn2.streamFromConnection(), "second request on the same connection should parse");
   requireStringEquals(string("GET /second HTTP/1.1"), txn2.getFirstHeaderLine(), "second request line");
}

//*****************************************************************************

void TestHttpTransaction::testCloseDoesNotDeleteUnownedConnection() {
   TEST_CASE("testCloseDoesNotDeleteUnownedConnection");

   bool deleted = false;
   bool closed = false;
   TrackingConnection connection(&deleted, &closed);
   TestableHttpTransaction txn(&connection, false);   // connectionOwned = false

   txn.close();

   requireFalse(deleted, "close() must not delete a connection the transaction does not own");
   require(closed, "close() should still close the underlying connection even when not owned");
}

//*****************************************************************************

void TestHttpTransaction::testCloseDeletesOwnedConnection() {
   TEST_CASE("testCloseDeletesOwnedConnection");

   bool deleted = false;
   bool closed = false;
   TrackingConnection* connection = new TrackingConnection(&deleted, &closed);
   TestableHttpTransaction txn(connection, true);   // connectionOwned = true

   txn.close();

   require(closed, "close() should close a connection the transaction owns");
   require(deleted, "close() should delete a connection the transaction owns");
   requireFalse(txn.isConnectionOwned(), "isConnectionOwned should be false after close()");
}

//*****************************************************************************

