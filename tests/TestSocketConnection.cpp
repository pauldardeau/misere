// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <unistd.h>
#include <sys/socket.h>
#include <cstring>

#include "TestSocketConnection.h"
#include "SocketConnection.h"
#include "Socket.h"

using namespace std;
using namespace misere;
using namespace chaudiere;

namespace {

// A connected pair of plain (non-INET) sockets, so SocketConnection can be
// exercised against a real chaudiere::Socket (real recv()/send() calls)
// without needing a listening TCP server - mirrors the same approach
// chaudiere's own TestSocket::testReadMsg() uses.
struct SocketPair {
   int fds[2];

   SocketPair() {
      fds[0] = -1;
      fds[1] = -1;
      ::socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
   }

   ~SocketPair() {
      if (fds[1] > -1) {
         ::close(fds[1]);
      }
      // fds[0] is handed to a chaudiere::Socket in every test below, which
      // owns and closes it.
   }
};

}

//******************************************************************************

TestSocketConnection::TestSocketConnection() :
   poivre::TestSuite("TestSocketConnection") {
}

//******************************************************************************

void TestSocketConnection::runTests() {
   testRead();
   testReadPartial();
   testWrite();
   testClose();
   testUnownedSocketNotDeleted();
}

//******************************************************************************

void TestSocketConnection::testRead() {
   TEST_CASE("testRead");

   SocketPair pair;
   Socket* socket = new Socket(pair.fds[0]);
   SocketConnection connection(socket, true);

   const char payload[] = "hello from peer";
   require(::write(pair.fds[1], payload, sizeof(payload) - 1) == (ssize_t)(sizeof(payload) - 1),
           "writing to the peer descriptor should succeed");

   char buffer[64];
   memset(buffer, 0, sizeof(buffer));
   const int bytesRead = connection.read(buffer, sizeof(buffer));

   require(bytesRead == (int)(sizeof(payload) - 1), "read should return the number of bytes the peer wrote");
   requireStringEquals(string(payload), string(buffer, bytesRead), "read bytes should match what the peer wrote");
}

//******************************************************************************

void TestSocketConnection::testReadPartial() {
   TEST_CASE("testReadPartial");

   // read() must return as soon as *some* data is available rather than
   // blocking until the buffer is full - the same recvAvailable()-style
   // contract the header-parsing loop in HttpTransaction depends on.
   SocketPair pair;
   Socket* socket = new Socket(pair.fds[0]);
   SocketConnection connection(socket, true);

   const char payload[] = "short";
   require(::write(pair.fds[1], payload, sizeof(payload) - 1) == (ssize_t)(sizeof(payload) - 1),
           "writing to the peer descriptor should succeed");

   char buffer[64];
   memset(buffer, 0, sizeof(buffer));
   const int bytesRead = connection.read(buffer, sizeof(buffer));

   require(bytesRead == (int)(sizeof(payload) - 1),
           "read should return exactly the bytes available, not block for a full buffer");
}

//******************************************************************************

void TestSocketConnection::testWrite() {
   TEST_CASE("testWrite");

   SocketPair pair;
   Socket* socket = new Socket(pair.fds[0]);
   SocketConnection connection(socket, true);

   const string payload = "hello from connection";
   require(connection.write(payload.data(), payload.size()), "write should succeed");

   char buffer[64];
   memset(buffer, 0, sizeof(buffer));
   const ssize_t bytesRead = ::read(pair.fds[1], buffer, sizeof(buffer));

   require(bytesRead == (ssize_t) payload.size(), "peer should receive the number of bytes written");
   requireStringEquals(payload, string(buffer, bytesRead), "peer should receive exactly what was written");
}

//******************************************************************************

void TestSocketConnection::testClose() {
   TEST_CASE("testClose");

   SocketPair pair;
   Socket* socket = new Socket(pair.fds[0]);
   SocketConnection connection(socket, true);

   connection.close();

   // the peer should now observe EOF (a 0-byte read) since this side was
   // closed
   char buffer[16];
   const ssize_t bytesRead = ::read(pair.fds[1], buffer, sizeof(buffer));
   require(bytesRead == 0, "peer should observe EOF after the connection is closed");
}

//******************************************************************************

void TestSocketConnection::testUnownedSocketNotDeleted() {
   TEST_CASE("testUnownedSocketNotDeleted");

   SocketPair pair;
   Socket socket(pair.fds[0]);

   {
      // socketOwned=false: destroying this SocketConnection must not
      // delete (or close) the underlying socket - the same borrowed
      // relationship HttpRequestHandler relies on across a keep-alive
      // connection's multiple requests.
      SocketConnection connection(&socket, false);
   }

   // if the socket had been deleted/closed above, this would either crash
   // or the descriptor would be gone
   require(socket.isConnected(), "socket should still be usable after an unowned SocketConnection is destroyed");

   const char payload[] = "still alive";
   require(::write(pair.fds[1], payload, sizeof(payload) - 1) == (ssize_t)(sizeof(payload) - 1),
           "writing to the peer descriptor should succeed");

   char buffer[64];
   memset(buffer, 0, sizeof(buffer));
   const int bytesRead = socket.recvAvailable(buffer, sizeof(buffer));
   require(bytesRead == (int)(sizeof(payload) - 1), "socket should still be able to read after the wrapper is destroyed");
}

//******************************************************************************
