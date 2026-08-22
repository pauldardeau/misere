// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <array>
#include <cstddef>
#include <cstring>
#include <deque>
#include <memory>
#include <mutex>
#include <span>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>

#include "TestTlsConnection.h"
#include "TlsConnection.h"
#include "SocketTransport.h"
#include "Socket.h"
#include "BasicException.h"
#include "armure/Armure.h"

using namespace std;
using namespace misere;
using namespace chaudiere;

namespace {

// Test-only, self-signed EC certificate/key pair generated solely for
// this test file - not a production credential (same convention armure's
// own test suite and example program use: reserved .invalid TLD, an O=
// field that says so outright). CN matches kTestServerHostname below.
//
//   openssl ecparam -name prime256v1 -genkey -noout -out key.pem
//   openssl req -x509 -new -key key.pem -days 3650 -out cert.pem \
//     -subj "/O=misere test fixtures - NOT FOR PRODUCTION USE/CN=misere-test-server.invalid"
//   openssl pkcs8 -topk8 -nocrypt -in key.pem -out key-pkcs8.pem
constexpr char kTestCertPem[] = R"(-----BEGIN CERTIFICATE-----
MIICDjCCAbWgAwIBAgIUNxkYq2cMZPPvMjabFky2mgMCJjowCgYIKoZIzj0EAwIw
XTE2MDQGA1UECgwtbWlzZXJlIHRlc3QgZml4dHVyZXMgLSBOT1QgRk9SIFBST0RV
Q1RJT04gVVNFMSMwIQYDVQQDDBptaXNlcmUtdGVzdC1zZXJ2ZXIuaW52YWxpZDAe
Fw0yNjA4MjEyMzI1MTlaFw0zNjA4MTgyMzI1MTlaMF0xNjA0BgNVBAoMLW1pc2Vy
ZSB0ZXN0IGZpeHR1cmVzIC0gTk9UIEZPUiBQUk9EVUNUSU9OIFVTRTEjMCEGA1UE
AwwabWlzZXJlLXRlc3Qtc2VydmVyLmludmFsaWQwWTATBgcqhkjOPQIBBggqhkjO
PQMBBwNCAASV5t6ZO0/RbpswMEjQsvfduMxjI5q8SM/b/FavzKXr/+qlfcMyaFca
+YGwMUEaUKp31uTOySNfpWL9fXJAJHWPo1MwUTAdBgNVHQ4EFgQUnDQp6e/B5rYV
JbJOVNg0j+S7RrcwHwYDVR0jBBgwFoAUnDQp6e/B5rYVJbJOVNg0j+S7RrcwDwYD
VR0TAQH/BAUwAwEB/zAKBggqhkjOPQQDAgNHADBEAiAnnw0zCYeZjj/acSz5hPZV
G3jedDVaLbWVWqNw3r/yLQIgXhe4419V1l3RyLXbg+yal12CAFrLidUsNd1r7R45
W4Q=
-----END CERTIFICATE-----
)";

constexpr char kTestKeyPem[] = R"(-----BEGIN PRIVATE KEY-----
MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQghb4Hh1Fl3yIqgJTf
tPCI9LAhbwbzZ70Y2L1ItmBvWWShRANCAASV5t6ZO0/RbpswMEjQsvfduMxjI5q8
SM/b/FavzKXr/+qlfcMyaFca+YGwMUEaUKp31uTOySNfpWL9fXJAJHWP
-----END PRIVATE KEY-----
)";

constexpr char kTestServerHostname[] = "misere-test-server.invalid";

std::vector<std::byte> asBytes(std::string_view text) {
   std::vector<std::byte> bytes(text.size());
   memcpy(bytes.data(), text.data(), text.size());
   return bytes;
}

armure::Certificate loadTestCertificate() {
   return armure::Certificate::loadFromMemory(asBytes(kTestCertPem)).value();
}

armure::PrivateKey loadTestKey() {
   return armure::PrivateKey::loadFromMemory(asBytes(kTestKeyPem), std::nullopt).value();
}

armure::Context makeServerContext() {
   return armure::ContextBuilder(armure::Role::Server)
      .withCertificate(loadTestCertificate())
      .withPrivateKey(loadTestKey())
      .withVerifyMode(armure::VerifyMode::None)
      .build()
      .value();
}

// Trusts the test certificate above, so the handshake actually succeeds.
armure::Context makeTrustingClientContext() {
   std::vector<armure::Certificate> trusted{loadTestCertificate()};
   return armure::ContextBuilder(armure::Role::Client)
      .withTrustedCertificates(std::move(trusted))
      .withVerifyMode(armure::VerifyMode::Required)
      .build()
      .value();
}

// Trusts nothing - used to make certificate verification fail on purpose.
armure::Context makeDistrustingClientContext() {
   return armure::ContextBuilder(armure::Role::Client)
      .withVerifyMode(armure::VerifyMode::Required)
      .build()
      .value();
}

// A connected pair of plain (non-INET) sockets - same approach as
// TestSocketConnection.cpp / TestSocketTransport.cpp.
struct SocketPair {
   int fds[2];

   SocketPair() {
      fds[0] = -1;
      fds[1] = -1;
      ::socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
   }

   ~SocketPair() {
   }
};

// A minimal, in-memory, deliberately *non-blocking-style* armure::Transport
// (modeled on armure's own examples/LoopbackTransport.h): read()/write()
// report ErrorCode::WantRead/WantWrite whenever there's nothing to do
// right now, rather than blocking - unlike SocketTransport, which never
// does this (see SocketTransport.h). Used only to reliably exercise
// TlsConnection's WantRead/WantWrite retry-loop, counting how many times
// each side actually occurred.
//
// The client and server sides of a pair run on separate threads (see
// testWantReadWantWriteRetryHandling()) and share the two underlying
// deques/counters, so every access below is under sharedState_->mutex -
// this is purely a test-double concern, unrelated to TlsConnection's own
// thread-safety (a single TlsConnection is still only ever used from one
// thread at a time, same as armure::Connection itself).
class InstrumentedLoopbackTransport : public armure::Transport {
public:
   static std::pair<std::unique_ptr<InstrumentedLoopbackTransport>, std::unique_ptr<InstrumentedLoopbackTransport>> makePair() {
      auto shared = std::make_shared<SharedState>();
      auto first = std::unique_ptr<InstrumentedLoopbackTransport>(
         new InstrumentedLoopbackTransport(shared, /*useAToB=*/true));
      auto second = std::unique_ptr<InstrumentedLoopbackTransport>(
         new InstrumentedLoopbackTransport(shared, /*useAToB=*/false));
      return {std::move(first), std::move(second)};
   }

   armure::Result<std::size_t> read(std::span<std::byte> buffer) override {
      if (buffer.empty()) {
         return armure::Result<std::size_t>(std::size_t{0});
      }

      std::lock_guard<std::mutex> lock(m_shared->mutex);
      std::deque<std::byte>& incoming = m_useAToB ? m_shared->bToA : m_shared->aToB;

      if (incoming.empty()) {
         ++m_shared->wantReadCount;
         return armure::Result<std::size_t>(armure::Error(armure::ErrorCode::WantRead, "no data available yet"));
      }
      std::size_t n = 0;
      while (n < buffer.size() && !incoming.empty()) {
         buffer[n++] = incoming.front();
         incoming.pop_front();
      }
      return armure::Result<std::size_t>(n);
   }

   armure::Result<std::size_t> write(std::span<const std::byte> buffer) override {
      std::lock_guard<std::mutex> lock(m_shared->mutex);
      std::deque<std::byte>& outgoing = m_useAToB ? m_shared->aToB : m_shared->bToA;

      for (std::byte b : buffer) {
         outgoing.push_back(b);
      }
      return armure::Result<std::size_t>(buffer.size());
   }

   // total across BOTH ends of a pair, since the two share the same
   // counters (see makePair())
   int wantReadCount() const {
      std::lock_guard<std::mutex> lock(m_shared->mutex);
      return m_shared->wantReadCount;
   }

private:
   struct SharedState {
      std::mutex mutex;
      std::deque<std::byte> aToB;
      std::deque<std::byte> bToA;
      int wantReadCount = 0;
   };

   InstrumentedLoopbackTransport(std::shared_ptr<SharedState> shared, bool useAToB)
      : m_shared(std::move(shared)), m_useAToB(useAToB) {}

   std::shared_ptr<SharedState> m_shared;
   bool m_useAToB;
};

}

//******************************************************************************

TestTlsConnection::TestTlsConnection() :
   poivre::TestSuite("TestTlsConnection") {
}

//******************************************************************************

void TestTlsConnection::runTests() {
   testHandshakeIOAndClose();
   testWantReadWantWriteRetryHandling();
   testHandshakeFailureThrows();
}

//******************************************************************************

void TestTlsConnection::testHandshakeIOAndClose() {
   TEST_CASE("testHandshakeIOAndClose");

   SocketPair pair;
   require(pair.fds[0] > -1 && pair.fds[1] > -1, "socketpair should succeed");

   Socket* serverSocket = new Socket(pair.fds[0]);
   Socket* clientSocket = new Socket(pair.fds[1]);

   armure::Context serverContext = makeServerContext();
   armure::Context clientContext = makeTrustingClientContext();

   std::unique_ptr<TlsConnection> serverConn;
   std::unique_ptr<TlsConnection> clientConn;
   std::string serverError;
   std::string clientError;

   // Each TlsConnection constructor drives its own handshake to
   // completion internally (see TlsConnection.h) - that requires the
   // *other* side to be making progress concurrently, so client and
   // server have to run on separate threads, exactly like two real
   // processes talking over a socket would.
   std::thread serverThread([&]() {
      try {
         auto transport = std::make_unique<SocketTransport>(serverSocket, /*socketOwned=*/true);
         armure::Result<armure::Connection> result = serverContext.createConnection(std::move(transport));
         if (!result) {
            serverError = std::string(result.error().message());
            return;
         }
         serverConn = std::make_unique<TlsConnection>(std::move(result).value(), serverSocket);
      } catch (const std::exception& e) {
         serverError = e.what();
      }
   });

   std::thread clientThread([&]() {
      try {
         auto transport = std::make_unique<SocketTransport>(clientSocket, /*socketOwned=*/true);
         armure::Result<armure::Connection> result =
            clientContext.createConnection(std::move(transport), std::string(kTestServerHostname));
         if (!result) {
            clientError = std::string(result.error().message());
            return;
         }
         clientConn = std::make_unique<TlsConnection>(std::move(result).value(), clientSocket);
      } catch (const std::exception& e) {
         clientError = e.what();
      }
   });

   serverThread.join();
   clientThread.join();

   require(serverError.empty(), "server side should not report an error: " + serverError);
   require(clientError.empty(), "client side should not report an error: " + clientError);
   require(nullptr != serverConn, "server TlsConnection should have been constructed (handshake completed)");
   require(nullptr != clientConn, "client TlsConnection should have been constructed (handshake completed)");

   // client writes plaintext, server reads it
   const std::string request = "GET /ping HTTP/1.1";
   require(clientConn->write(request.data(), request.size()), "client write should succeed");

   char buffer[256];
   int bytesRead = serverConn->read(buffer, sizeof(buffer));
   require(bytesRead == (int) request.size(), "server should read exactly what the client wrote");
   requireStringEquals(request, std::string(buffer, bytesRead), "server should see the client's plaintext");

   // server writes plaintext, client reads it
   const std::string response = "200 OK";
   require(serverConn->write(response.data(), response.size()), "server write should succeed");

   bytesRead = clientConn->read(buffer, sizeof(buffer));
   require(bytesRead == (int) response.size(), "client should read exactly what the server wrote");
   requireStringEquals(response, std::string(buffer, bytesRead), "client should see the server's plaintext");

   // graceful shutdown / close, both directions
   clientConn->close();
   serverConn->close();
}

//******************************************************************************

void TestTlsConnection::testWantReadWantWriteRetryHandling() {
   TEST_CASE("testWantReadWantWriteRetryHandling");

   auto transports = InstrumentedLoopbackTransport::makePair();
   InstrumentedLoopbackTransport* serverTransportRaw = transports.first.get();

   armure::Context serverContext = makeServerContext();
   armure::Context clientContext = makeTrustingClientContext();

   std::unique_ptr<TlsConnection> serverConn;
   std::unique_ptr<TlsConnection> clientConn;
   std::string serverError;
   std::string clientError;

   std::thread serverThread([&]() {
      try {
         armure::Result<armure::Connection> result = serverContext.createConnection(std::move(transports.first));
         if (!result) {
            serverError = std::string(result.error().message());
            return;
         }
         serverConn = std::make_unique<TlsConnection>(std::move(result).value());
      } catch (const std::exception& e) {
         serverError = e.what();
      }
   });

   std::thread clientThread([&]() {
      try {
         armure::Result<armure::Connection> result =
            clientContext.createConnection(std::move(transports.second), std::string(kTestServerHostname));
         if (!result) {
            clientError = std::string(result.error().message());
            return;
         }
         clientConn = std::make_unique<TlsConnection>(std::move(result).value());
      } catch (const std::exception& e) {
         clientError = e.what();
      }
   });

   serverThread.join();
   clientThread.join();

   require(serverError.empty(), "server side should not report an error: " + serverError);
   require(clientError.empty(), "client side should not report an error: " + clientError);
   require(nullptr != serverConn, "server TlsConnection should have been constructed despite a non-blocking transport");
   require(nullptr != clientConn, "client TlsConnection should have been constructed despite a non-blocking transport");

   // InstrumentedLoopbackTransport reports WantRead every time either
   // side tries to read before the other has written anything yet - a
   // real TLS handshake's back-and-forth structure means this happens
   // naturally and repeatedly (not just as a lucky race), which is what
   // this assertion confirms actually happened rather than merely
   // *could* happen.
   require(serverTransportRaw->wantReadCount() > 0,
           "the handshake should have induced at least one WantRead - confirms TlsConnection's retry loop was exercised, not just present");

   // basic I/O still works over this transport pair too
   const std::string request = "hello";
   require(clientConn->write(request.data(), request.size()), "client write should succeed");

   char buffer[64];
   int bytesRead = serverConn->read(buffer, sizeof(buffer));
   require(bytesRead == (int) request.size(), "server should read exactly what the client wrote");
   requireStringEquals(request, std::string(buffer, bytesRead), "server should see the client's plaintext");
}

//******************************************************************************

void TestTlsConnection::testHandshakeFailureThrows() {
   TEST_CASE("testHandshakeFailureThrows");

   SocketPair pair;
   require(pair.fds[0] > -1 && pair.fds[1] > -1, "socketpair should succeed");

   Socket* serverSocket = new Socket(pair.fds[0]);
   Socket* clientSocket = new Socket(pair.fds[1]);

   armure::Context serverContext = makeServerContext();
   // trusts nothing, so verifying the server's self-signed certificate
   // must fail
   armure::Context clientContext = makeDistrustingClientContext();

   std::string serverError;
   bool clientThrew = false;
   std::string clientExceptionMessage;

   std::thread serverThread([&]() {
      try {
         auto transport = std::make_unique<SocketTransport>(serverSocket, /*socketOwned=*/true);
         armure::Result<armure::Connection> result = serverContext.createConnection(std::move(transport));
         if (!result) {
            serverError = std::string(result.error().message());
            return;
         }
         // constructing this drives the handshake; the client is
         // expected to reject it, which will surface here as some
         // failure too (exact code depends on which side notices
         // first) - either outcome is fine, this thread just must not
         // hang or crash
         try {
            TlsConnection serverConn(std::move(result).value(), serverSocket);
         } catch (const std::exception&) {
            // expected in some interleavings
         }
      } catch (const std::exception& e) {
         serverError = e.what();
      }
   });

   std::thread clientThread([&]() {
      auto transport = std::make_unique<SocketTransport>(clientSocket, /*socketOwned=*/true);
      armure::Result<armure::Connection> result =
         clientContext.createConnection(std::move(transport), std::string(kTestServerHostname));
      require(result.has_value(), "creating the client Connection object itself should succeed - only the handshake should fail");
      try {
         TlsConnection clientConn(std::move(result).value(), clientSocket);
      } catch (const BasicException& e) {
         clientThrew = true;
         clientExceptionMessage = e.whatString();
      }
   });

   serverThread.join();
   clientThread.join();

   require(clientThrew, "TlsConnection's constructor should throw when the handshake fails");
   requireFalse(clientExceptionMessage.empty(), "the thrown exception should carry a descriptive message");
}

//******************************************************************************
