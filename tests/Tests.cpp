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
   // chaudiere::Socket's write()/send() have no SIGPIPE protection
   // (no MSG_NOSIGNAL, no SO_NOSIGPIPE) - writing to a socket whose peer
   // has already closed raises SIGPIPE, which by default terminates the
   // process outright rather than surfacing as an ordinary write
   // failure. TestTlsConnection's close()-then-close() sequence hits
   // exactly this (closing one side's socket, then writing a
   // close_notify from the other). Ignoring SIGPIPE here is the standard
   // fix for any process making raw socket writes - see this task's
   // final report for why misere's own server entry point likely needs
   // the same treatment, not just this test binary.
   ::signal(SIGPIPE, SIG_IGN);

   Tests tests;
   tests.run();
}

