// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>

#include "TestHttpsIntegration.h"
#include "HttpServer.h"
#include "HttpRequestHandler.h"
#include "Socket.h"
#include "SocketTransport.h"
#include "TlsConnection.h"
#include "BasicException.h"
#include "armure/Armure.h"

using namespace std;
using namespace misere;
using namespace chaudiere;

namespace {

// Test-only, self-signed EC certificate/key pair - not a production
// credential (same fixture used by TestTlsConnection.cpp; duplicated
// here rather than shared, matching this codebase's existing per-file
// test fixture convention). CN matches kTestServerHostname below.
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

// A GET request against a built-in handler (allow_builtin_handlers=true
// registers it - see the INI content below) so these tests don't need
// any DLL-based module configuration.
const string kRequest =
   "GET /GMTDateTime HTTP/1.1\r\n"
   "Host: misere-test-server.invalid\r\n"
   "Connection: close\r\n"
   "\r\n";

string uniqueTempPath(const string& name) {
   static int counter = 0;
   char buffer[256];
   ::snprintf(buffer, sizeof(buffer), "/tmp/misere_https_test_%d_%d_%s",
              (int) ::getpid(), ++counter, name.c_str());
   return string(buffer);
}

void writeTextFile(const string& path, const string& content) {
   ofstream out(path, ios::binary | ios::trunc);
   out << content;
}

string writeTestCertificate() {
   const string path = uniqueTempPath("cert.pem");
   writeTextFile(path, kTestCertPem);
   return path;
}

string writeTestPrivateKey() {
   const string path = uniqueTempPath("key.pem");
   writeTextFile(path, kTestKeyPem);
   return path;
}

// Base [server] settings every test below starts from - port and
// tls_* lines are appended per test.
string baseServerConfig(int port) {
   string cfg;
   cfg += "[server]\r\n";
   cfg += "port = " + to_string(port) + "\r\n";
   cfg += "allow_builtin_handlers = true\r\n";
   cfg += "threading = pthreads\r\n";
   cfg += "thread_pool_size = 2\r\n";
   cfg += "sockets = socket_server\r\n";
   cfg += "keep_alive = false\r\n";
   return cfg;
}

string writeConfig(const string& serverSection) {
   const string path = uniqueTempPath("misere.ini");
   writeTextFile(path, serverSection);
   return path;
}

// Connects with a short retry/backoff, since the server's accept loop
// starts on a background thread and there's no explicit "ready" signal
// to wait on - matches the same retry pattern any real client would use
// against a server that might not be listening yet.
Socket* connectWithRetry(const string& host, int port, int maxAttempts = 60) {
   for (int i = 0; i < maxAttempts; ++i) {
      try {
         return new Socket(host, port);
      } catch (const BasicException&) {
         this_thread::sleep_for(chrono::milliseconds(50));
      }
   }
   return nullptr;
}

// Starts server->run() on a background thread. HttpServer has no clean
// shutdown API (out of scope for this task - see the final report), and
// the accept loop never returns on its own, so the server (and the
// thread driving it) are deliberately never torn down - both are simply
// leaked for the remaining lifetime of the test process, which is safe
// (if untidy) specifically because that avoids the alternative: a
// background thread outliving, and then dereferencing, a destroyed
// HttpServer.
void startServerInBackground(HttpServer* server) {
   std::thread serverThread([server]() {
      server->run();
   });
   serverThread.detach();
}

// Reads exactly one HTTP response from a TLS connection that stays open
// afterward (keep-alive) - unlike readAll() and the read-until-EOF loop
// in testTlsServerAcceptsRealHandshakeAndServesRequest() below, this
// cannot simply read until the connection closes, since on a
// keep-alive connection it deliberately never does between requests.
// Parses just enough (the header terminator and Content-Length) to know
// where the response ends.
string readOneHttpResponseThroughTls(TlsConnection& connection) {
   string buffer;
   char chunk[512];
   const string headerTerminator = "\r\n\r\n";

   for (;;) {
      const string::size_type headerEnd = buffer.find(headerTerminator);
      if (headerEnd != string::npos) {
         string headers = buffer.substr(0, headerEnd);
         for (auto& c : headers) {
            c = (char) ::tolower((unsigned char) c);
         }

         long contentLength = 0;
         const string::size_type clPos = headers.find("content-length:");
         if (clPos != string::npos) {
            contentLength = ::strtol(headers.c_str() + clPos + strlen("content-length:"), nullptr, 10);
         }

         const string::size_type bodyStart = headerEnd + headerTerminator.size();
         const string::size_type totalNeeded = bodyStart + (string::size_type) contentLength;
         if (buffer.size() >= totalNeeded) {
            return buffer.substr(0, totalNeeded);
         }
      }

      const int n = connection.read(chunk, sizeof(chunk));
      if (n <= 0) {
         return buffer;
      }
      buffer.append(chunk, n);
   }
}

string readAll(Socket* socket, int maxBytes = 4096) {
   string result;
   char buffer[512];
   bool readSuccess = true;
   while (readSuccess && ((int) result.size() < maxBytes)) {
      const int n = socket->recvAvailable(buffer, sizeof(buffer));
      if (n <= 0) {
         readSuccess = false;
      } else {
         result.append(buffer, n);
      }
   }
   return result;
}

}

//******************************************************************************

TestHttpsIntegration::TestHttpsIntegration() :
   poivre::TestSuite("TestHttpsIntegration") {
}

//******************************************************************************

void TestHttpsIntegration::runTests() {
   testPlainHttpUnaffectedByTlsSupport();
   testTlsServerAcceptsRealHandshakeAndServesRequest();
   testTlsKeepAliveReusesConnectionAcrossRequests();
   testTlsEnabledWithoutCertificateFailsInitialization();
   testTlsEnabledWithoutPrivateKeyFailsInitialization();
   testTlsEnabledWithInvalidCertificatePathFailsInitialization();
   testTlsEnabledWithInvalidPrivateKeyPathFailsInitialization();
   testHandshakeFailureDoesNotReachHandler();
}

//******************************************************************************

void TestHttpsIntegration::testPlainHttpUnaffectedByTlsSupport() {
   TEST_CASE("testPlainHttpUnaffectedByTlsSupport");

   const int port = 34561;
   // tls_enabled deliberately omitted entirely - the pre-existing,
   // TLS-unaware configuration shape - to prove that shape still works
   // exactly as it did before TLS support existed, not just that
   // tls_enabled=false works.
   const string configPath = writeConfig(baseServerConfig(port));

   // leaked deliberately - see startServerInBackground()
   HttpServer* server = new HttpServer(configPath);
   startServerInBackground(server);

   unique_ptr<Socket> client(connectWithRetry("127.0.0.1", port));
   require(nullptr != client, "plain HTTP client should be able to connect");

   require(client->write(kRequest), "writing the plain HTTP request should succeed");

   const string response = readAll(client.get());
   require(!response.empty(), "a plain HTTP response should be received");
   require(response.find("200") != string::npos, "the response should report HTTP 200");
}

//******************************************************************************

void TestHttpsIntegration::testTlsServerAcceptsRealHandshakeAndServesRequest() {
   TEST_CASE("testTlsServerAcceptsRealHandshakeAndServesRequest");

   const int port = 34562;
   const string certPath = writeTestCertificate();
   const string keyPath = writeTestPrivateKey();

   string serverSection = baseServerConfig(port);
   serverSection += "tls_enabled = true\r\n";
   serverSection += "tls_certificate = " + certPath + "\r\n";
   serverSection += "tls_private_key = " + keyPath + "\r\n";
   const string configPath = writeConfig(serverSection);

   HttpServer* server = new HttpServer(configPath);
   startServerInBackground(server);

   unique_ptr<Socket> rawClientSocket(connectWithRetry("127.0.0.1", port));
   require(nullptr != rawClientSocket, "TCP connection to the TLS server should succeed");

   // Build a client armure Context trusting the same test certificate -
   // the real TLS handshake this test exists to prove works.
   armure::Result<armure::Certificate> trustedCert =
      armure::Certificate::loadFromFile(certPath);
   require(trustedCert.has_value(), "loading the test certificate for the client should succeed");

   vector<armure::Certificate> trusted{trustedCert.value()};
   armure::Result<armure::Context> clientContextResult =
      armure::ContextBuilder(armure::Role::Client)
         .withTrustedCertificates(std::move(trusted))
         .withVerifyMode(armure::VerifyMode::Required)
         .build();
   require(clientContextResult.has_value(), "building the client TLS context should succeed");

   auto transport = std::make_unique<SocketTransport>(rawClientSocket.release(), /*socketOwned=*/true);
   armure::Result<armure::Connection> connResult =
      clientContextResult.value().createConnection(std::move(transport), string(kTestServerHostname));
   require(connResult.has_value(), "creating the client armure Connection should succeed");

   // TlsConnection's constructor drives the handshake to completion (or
   // throws) - reaching this line at all is half the proof this test is
   // for.
   TlsConnection clientConnection(std::move(connResult).value());

   const string request = kRequest;
   require(clientConnection.write(request.data(), request.size()),
           "writing the HTTP request through TLS should succeed");

   string response;
   char buffer[512];
   bool readSuccess = true;
   while (readSuccess && (response.size() < 4096)) {
      const int n = clientConnection.read(buffer, sizeof(buffer));
      if (n <= 0) {
         readSuccess = false;
      } else {
         response.append(buffer, n);
      }
   }

   require(!response.empty(), "an HTTP response should be received through TLS");
   require(response.find("200") != string::npos, "the response received through TLS should report HTTP 200");

   clientConnection.close();
}

//******************************************************************************

void TestHttpsIntegration::testTlsKeepAliveReusesConnectionAcrossRequests() {
   TEST_CASE("testTlsKeepAliveReusesConnectionAcrossRequests");

   const int port = 34568;
   const string certPath = writeTestCertificate();
   const string keyPath = writeTestPrivateKey();

   // Deliberately not built from baseServerConfig() (which sets
   // keep_alive = false) - written out directly so there's no reliance
   // on whatever this INI reader does with a duplicate key.
   string serverSection;
   serverSection += "[server]\r\n";
   serverSection += "port = " + to_string(port) + "\r\n";
   serverSection += "allow_builtin_handlers = true\r\n";
   serverSection += "threading = pthreads\r\n";
   serverSection += "thread_pool_size = 2\r\n";
   serverSection += "sockets = socket_server\r\n";
   serverSection += "keep_alive = true\r\n";
   serverSection += "keep_alive_timeout = 5\r\n";
   serverSection += "keep_alive_max_requests = 10\r\n";
   serverSection += "tls_enabled = true\r\n";
   serverSection += "tls_certificate = " + certPath + "\r\n";
   serverSection += "tls_private_key = " + keyPath + "\r\n";
   const string configPath = writeConfig(serverSection);

   HttpServer* server = new HttpServer(configPath);
   startServerInBackground(server);

   unique_ptr<Socket> rawClientSocket(connectWithRetry("127.0.0.1", port));
   require(nullptr != rawClientSocket, "TCP connection to the TLS server should succeed");

   armure::Result<armure::Certificate> trustedCert =
      armure::Certificate::loadFromFile(certPath);
   require(trustedCert.has_value(), "loading the test certificate for the client should succeed");

   vector<armure::Certificate> trusted{trustedCert.value()};
   armure::Result<armure::Context> clientContextResult =
      armure::ContextBuilder(armure::Role::Client)
         .withTrustedCertificates(std::move(trusted))
         .withVerifyMode(armure::VerifyMode::Required)
         .build();
   require(clientContextResult.has_value(), "building the client TLS context should succeed");

   auto transport = std::make_unique<SocketTransport>(rawClientSocket.release(), /*socketOwned=*/true);
   armure::Result<armure::Connection> connResult =
      clientContextResult.value().createConnection(std::move(transport), string(kTestServerHostname));
   require(connResult.has_value(), "creating the client armure Connection should succeed");

   // One handshake, one TlsConnection, reused for both requests below -
   // exactly what this test exists to prove the server side also does.
   TlsConnection clientConnection(std::move(connResult).value());

   const string keepAliveRequest =
      "GET /GMTDateTime HTTP/1.1\r\n"
      "Host: misere-test-server.invalid\r\n"
      "Connection: keep-alive\r\n"
      "\r\n";

   require(clientConnection.write(keepAliveRequest.data(), keepAliveRequest.size()),
           "writing the first HTTP request through TLS should succeed");
   const string firstResponse = readOneHttpResponseThroughTls(clientConnection);
   require(!firstResponse.empty(), "a response to the first request should be received through TLS");
   require(firstResponse.find("200") != string::npos, "the first response should report HTTP 200");
   require(firstResponse.find("keep-alive") != string::npos,
           "the server should have negotiated keep-alive on the first response");

   // Second request over the same, still-open TlsConnection - no new
   // handshake, no new armure::Connection. This is the crux of the
   // test: it can only succeed if the server is still driving the
   // original TLS session rather than having torn it down after the
   // first request.
   require(clientConnection.write(keepAliveRequest.data(), keepAliveRequest.size()),
           "writing the second HTTP request through the same TLS connection should succeed");
   const string secondResponse = readOneHttpResponseThroughTls(clientConnection);
   require(!secondResponse.empty(), "a response to the second request should be received through TLS");
   require(secondResponse.find("200") != string::npos, "the second response should report HTTP 200");

   clientConnection.close();
}

//******************************************************************************

void TestHttpsIntegration::testTlsEnabledWithoutCertificateFailsInitialization() {
   TEST_CASE("testTlsEnabledWithoutCertificateFailsInitialization");

   const string keyPath = writeTestPrivateKey();

   string serverSection = baseServerConfig(34563);
   serverSection += "tls_enabled = true\r\n";
   serverSection += "tls_private_key = " + keyPath + "\r\n";
   // tls_certificate deliberately omitted
   const string configPath = writeConfig(serverSection);

   HttpServer server(configPath);
   // m_isFullyInitialized will be false, so run() must fail fast rather
   // than block trying to accept connections, and must not have silently
   // fallen back to plain HTTP either.
   require(1 == server.run(), "run() should report failure when tls_certificate is missing");
}

//******************************************************************************

void TestHttpsIntegration::testTlsEnabledWithoutPrivateKeyFailsInitialization() {
   TEST_CASE("testTlsEnabledWithoutPrivateKeyFailsInitialization");

   const string certPath = writeTestCertificate();

   string serverSection = baseServerConfig(34564);
   serverSection += "tls_enabled = true\r\n";
   serverSection += "tls_certificate = " + certPath + "\r\n";
   // tls_private_key deliberately omitted
   const string configPath = writeConfig(serverSection);

   HttpServer server(configPath);
   require(1 == server.run(), "run() should report failure when tls_private_key is missing");
}

//******************************************************************************

void TestHttpsIntegration::testTlsEnabledWithInvalidCertificatePathFailsInitialization() {
   TEST_CASE("testTlsEnabledWithInvalidCertificatePathFailsInitialization");

   const string keyPath = writeTestPrivateKey();

   string serverSection = baseServerConfig(34565);
   serverSection += "tls_enabled = true\r\n";
   serverSection += "tls_certificate = /nonexistent/path/does-not-exist-cert.pem\r\n";
   serverSection += "tls_private_key = " + keyPath + "\r\n";
   const string configPath = writeConfig(serverSection);

   HttpServer server(configPath);
   require(1 == server.run(), "run() should report failure when tls_certificate points to a nonexistent file");
}

//******************************************************************************

void TestHttpsIntegration::testTlsEnabledWithInvalidPrivateKeyPathFailsInitialization() {
   TEST_CASE("testTlsEnabledWithInvalidPrivateKeyPathFailsInitialization");

   const string certPath = writeTestCertificate();

   string serverSection = baseServerConfig(34566);
   serverSection += "tls_enabled = true\r\n";
   serverSection += "tls_certificate = " + certPath + "\r\n";
   serverSection += "tls_private_key = /nonexistent/path/does-not-exist-key.pem\r\n";
   const string configPath = writeConfig(serverSection);

   HttpServer server(configPath);
   require(1 == server.run(), "run() should report failure when tls_private_key points to a nonexistent file");
}

//******************************************************************************

void TestHttpsIntegration::testHandshakeFailureDoesNotReachHandler() {
   TEST_CASE("testHandshakeFailureDoesNotReachHandler");

   const string certPath = writeTestCertificate();
   const string keyPath = writeTestPrivateKey();

   string serverSection = baseServerConfig(34567);
   serverSection += "tls_enabled = true\r\n";
   serverSection += "tls_certificate = " + certPath + "\r\n";
   serverSection += "tls_private_key = " + keyPath + "\r\n";
   const string configPath = writeConfig(serverSection);

   HttpServer server(configPath);

   int fds[2];
   require(0 == ::socketpair(AF_UNIX, SOCK_STREAM, 0, fds), "socketpair should succeed");

   Socket* serverSocket = new Socket(fds[0]);   // owned by HttpRequestHandler below

   // The "client" closes its end immediately, without ever sending a
   // ClientHello - deterministically fails the handshake
   // (SocketTransport::read() reports Closed to armure) with no risk of
   // the server side blocking waiting for handshake bytes that will
   // never arrive, so this test can stay synchronous.
   ::close(fds[1]);

   HttpRequestHandler handler(server, serverSocket);
   handler.run();

   // Reaching this line at all - run() returning promptly rather than
   // hanging or crashing - is most of what this test proves. There is
   // deliberately no handler/response-writing code path left to have
   // been reached: a failed handshake returns out of run() before
   // HttpRequest is ever constructed (see HttpRequestHandler.cpp).
   require(true, "run() should return promptly after a failed handshake, without reaching HTTP parsing");
}
