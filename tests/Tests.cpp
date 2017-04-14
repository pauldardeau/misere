// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "Tests.h"

#include "TestHTTP.h"
#include "TestHttpClient.h"
#include "TestHttpException.h"
#include "TestHttpRequest.h"
#include "TestHttpResponse.h"
#include "TestHttpServer.h"
#include "TestHttpTransaction.h"

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
}

int main(int argc, char* argv[]) {
   Tests tests;
   tests.run();
}

