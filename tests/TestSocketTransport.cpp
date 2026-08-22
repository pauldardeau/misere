// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <array>
#include <cstddef>
#include <cstring>
#include <span>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>

#include "TestSocketTransport.h"
#include "SocketTransport.h"
#include "Socket.h"

using namespace std;
using namespace misere;
using namespace chaudiere;

namespace {

// A connected pair of plain (non-INET) sockets, so SocketTransport can be
// exercised against a real chaudiere::Socket (real recv()/send() calls)
// without needing a listening TCP server - same approach as
// TestSocketConnection.cpp and chaudiere's own TestSocket::testReadMsg().
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
   }
};

}

//******************************************************************************

TestSocketTransport::TestSocketTransport() :
   poivre::TestSuite("TestSocketTransport") {
}

//******************************************************************************

void TestSocketTransport::runTests() {
   testRead();
   testReadPartial();
   testReadClosed();
   testWrite();
   testWriteAfterPeerClosed();
   testUnownedSocketNotDeleted();
}

//******************************************************************************

void TestSocketTransport::testRead() {
   TEST_CASE("testRead");

   SocketPair pair;
   Socket* socket = new Socket(pair.fds[0]);
   SocketTransport transport(socket, true);

   const char payload[] = "hello from peer";
   require(::write(pair.fds[1], payload, sizeof(payload) - 1) == (ssize_t)(sizeof(payload) - 1),
           "writing to the peer descriptor should succeed");

   std::array<std::byte, 64> buffer{};
   armure::Result<std::size_t> result = transport.read(std::span<std::byte>(buffer));

   require(result.has_value(), "read should succeed");
   require(result.value() == sizeof(payload) - 1, "read should return the number of bytes the peer wrote");
   requireStringEquals(string(payload),
                        string(reinterpret_cast<const char*>(buffer.data()), result.value()),
                        "read bytes should match what the peer wrote");
}

//******************************************************************************

void TestSocketTransport::testReadPartial() {
   TEST_CASE("testReadPartial");

   SocketPair pair;
   Socket* socket = new Socket(pair.fds[0]);
   SocketTransport transport(socket, true);

   const char payload[] = "short";
   require(::write(pair.fds[1], payload, sizeof(payload) - 1) == (ssize_t)(sizeof(payload) - 1),
           "writing to the peer descriptor should succeed");

   std::array<std::byte, 64> buffer{};
   armure::Result<std::size_t> result = transport.read(std::span<std::byte>(buffer));

   require(result.has_value(), "read should succeed");
   require(result.value() == sizeof(payload) - 1,
           "read should return exactly the bytes available, not block for a full buffer");
}

//******************************************************************************

void TestSocketTransport::testReadClosed() {
   TEST_CASE("testReadClosed");

   SocketPair pair;
   Socket* socket = new Socket(pair.fds[0]);
   SocketTransport transport(socket, true);

   ::close(pair.fds[1]);
   pair.fds[1] = -1; // already closed - don't let ~SocketPair() close it again

   std::array<std::byte, 64> buffer{};
   armure::Result<std::size_t> result = transport.read(std::span<std::byte>(buffer));

   requireFalse(result.has_value(), "read should fail once the peer has closed its end");
   require(armure::ErrorCode::Closed == result.error().code(),
           "a closed peer should map to ErrorCode::Closed");
}

//******************************************************************************

void TestSocketTransport::testWrite() {
   TEST_CASE("testWrite");

   SocketPair pair;
   Socket* socket = new Socket(pair.fds[0]);
   SocketTransport transport(socket, true);

   const std::string payload = "hello from transport";
   std::vector<std::byte> bytes(payload.size());
   memcpy(bytes.data(), payload.data(), payload.size());

   armure::Result<std::size_t> result = transport.write(std::span<const std::byte>(bytes));
   require(result.has_value(), "write should succeed");
   require(result.value() == payload.size(), "write should report the full length written");

   char buffer[64];
   memset(buffer, 0, sizeof(buffer));
   const ssize_t bytesRead = ::read(pair.fds[1], buffer, sizeof(buffer));

   require(bytesRead == (ssize_t) payload.size(), "peer should receive the number of bytes written");
   requireStringEquals(payload, string(buffer, bytesRead), "peer should receive exactly what was written");
}

//******************************************************************************

void TestSocketTransport::testWriteAfterPeerClosed() {
   TEST_CASE("testWriteAfterPeerClosed");

   // Demonstrates, for the TLS write path specifically (SocketTransport
   // is what TlsConnection's underlying armure::Connection writes
   // through), the scenario that motivated installing
   // signal(SIGPIPE, SIG_IGN) at process startup (see src/main.cpp):
   // writing to a socket whose peer has already closed must fail
   // cleanly - a TransportError Result - rather than raising SIGPIPE and
   // killing the process. This test only exercises that safely because
   // test_misere's own main() (tests/Tests.cpp) ignores SIGPIPE too.
   SocketPair pair;
   Socket* socket = new Socket(pair.fds[0]);
   SocketTransport transport(socket, true);

   ::close(pair.fds[1]);
   pair.fds[1] = -1; // already closed - don't let ~SocketPair() close it again

   const std::string payload = "written after the peer is gone";
   std::vector<std::byte> bytes(payload.size());
   memcpy(bytes.data(), payload.data(), payload.size());

   armure::Result<std::size_t> result = transport.write(std::span<const std::byte>(bytes));
   requireFalse(result.has_value(), "write should fail (not crash the process) once the peer has closed");
   require(armure::ErrorCode::TransportError == result.error().code(),
           "a write failure should map to ErrorCode::TransportError");
}

//******************************************************************************

void TestSocketTransport::testUnownedSocketNotDeleted() {
   TEST_CASE("testUnownedSocketNotDeleted");

   SocketPair pair;
   Socket socket(pair.fds[0]);

   {
      // socketOwned=false: destroying this SocketTransport must not
      // delete (or close) the underlying socket - the relationship a
      // TlsConnection's SocketTransport needs when the socket is
      // borrowed rather than owned.
      SocketTransport transport(&socket, false);
   }

   require(socket.isConnected(), "socket should still be usable after an unowned SocketTransport is destroyed");

   const char payload[] = "still alive";
   require(::write(pair.fds[1], payload, sizeof(payload) - 1) == (ssize_t)(sizeof(payload) - 1),
           "writing to the peer descriptor should succeed");

   char buffer[64];
   memset(buffer, 0, sizeof(buffer));
   const int bytesRead = socket.recvAvailable(buffer, sizeof(buffer));
   require(bytesRead == (int)(sizeof(payload) - 1), "socket should still be able to read after the wrapper is destroyed");
}

//******************************************************************************
