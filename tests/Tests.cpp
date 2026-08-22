// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <csignal>

#include "Tests.h"

#include "TestHTTP.h"
#include "TestHttpClient.h"
#include "TestHttpException.h"
#include "TestHttpRequest.h"
#include "TestHttpResponse.h"
#include "TestHttpServer.h"
#include "TestHttpTransaction.h"
#include "TestSocketConnection.h"
#include "TestSocketTransport.h"
#include "TestTlsConnection.h"
#include "TestUrl.h"

using namespace misere;

void Tests::run() {
   TestHTTP testHTTP;
   testHTTP.run();

   TestHttpClient testHttpClient;
   testHttpClient.run();

   TestHttpException testHttpException;
   testHttpException.run();

   TestHttpRequest testHttpRequest;
   testHttpRequest.run();

   TestHttpResponse testHttpResponse;
   testHttpResponse.run();

   TestHttpServer testHttpServer;
   testHttpServer.run();

   TestHttpTransaction testHttpTransaction;
   testHttpTransaction.run();

   TestSocketConnection testSocketConnection;
   testSocketConnection.run();

   TestSocketTransport testSocketTransport;
   testSocketTransport.run();

   TestTlsConnection testTlsConnection;
   testTlsConnection.run();

   TestUrl testUrl;
   testUrl.run();
}

int main(int argc, char* argv[]) {
#ifdef SIGPIPE
   // test_misere is a separate executable from misere_cli (src/main.cpp)
   // - process-wide signal disposition doesn't cross that boundary, so
   // this binary needs its own copy of the same fix rather than
   // inheriting main.cpp's. See main.cpp's own comment for the full
   // rationale (chaudiere::Socket's writes have no SIGPIPE protection of
   // their own); TestTlsConnection's close()-then-close() sequence
   // (closing one side's socket, then writing a close_notify from the
   // other) is what originally surfaced this here.
   ::signal(SIGPIPE, SIG_IGN);
#endif

   Tests tests;
   tests.run();
}

