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
#include "TestIniReader.h"
#include "TestInvalidKeyException.h"
#include "TestKeyValuePairs.h"
#include "TestKqueueServer.h"
#include "TestPthreadsMutex.h"
#include "TestStdMutex.h"
#include "TestRequestHandler.h"
#include "TestServerSocket.h"
#include "TestSocket.h"
#include "TestSocketRequest.h"
#include "TestStringTokenizer.h"
#include "TestStrUtils.h"
#include "TestThread.h"
#include "TestThreadManager.h"
#include "TestThreadPool.h"
#include "TestThreadPoolQueue.h"
#include "TestThreadPoolWorker.h"


void Tests::run()
{
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
   
   TestIniReader testIniReader;
   testIniReader.run();
   
   TestInvalidKeyException testIKE;
   testIKE.run();
   
   TestKeyValuePairs testKVP;
   testKVP.run();
   
   TestKqueueServer testKqueueServer;
   testKqueueServer.run();
   
   TestPthreadsMutex testPthreadsMutex;
   testPthreadsMutex.run();

   TestStdMutex testStdMutex;
   testStdMutex.run();

   TestRequestHandler testRequestHandler;
   testRequestHandler.run();
   
   TestServerSocket testServerSocket;
   testServerSocket.run();
   
   TestSocket testSocket;
   testSocket.run();
   
   TestSocketRequest testSocketRequest;
   testSocketRequest.run();
   
   TestStringTokenizer testStringTokenizer;
   testStringTokenizer.run();
   
   TestStrUtils testStrUtils;
   testStrUtils.run();
   
   TestThread testThread;
   testThread.run();
   
   TestThreadManager testThreadManager;
   testThreadManager.run();

   TestThreadPool testThreadPool;
   testThreadPool.run();

   TestThreadPoolQueue testThreadPoolQueue;
   testThreadPoolQueue.run();

   TestThreadPoolWorker testThreadPoolWorker;
   testThreadPoolWorker.run();
}
