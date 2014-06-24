// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "TestThreadManager.h"
#include "ThreadManager.h"

//******************************************************************************

TestThreadManager::TestThreadManager() :
   TestSuite("TestThreadManager")
{
}

//******************************************************************************

void TestThreadManager::runTests()
{
   testGetInstance();
   testKillInstance();
   testGetCurrentThread();
   testRegisterThread();
   testNotifyOnThreadDone();
   testGetNumberThreads();
   testStartPoolWorker();
   testStopPoolWorker();
   testGetPercentBusy();
}

//******************************************************************************

void TestThreadManager::testGetInstance()
{
   TEST_CASE("testGetInstance");
}

//******************************************************************************

void TestThreadManager::testKillInstance()
{
   TEST_CASE("testKillInstance");
}

//******************************************************************************

void TestThreadManager::testGetCurrentThread()
{
   TEST_CASE("testGetCurrentThread");
}

//******************************************************************************

void TestThreadManager::testRegisterThread()
{
   TEST_CASE("testRegisterThread");
}

//******************************************************************************

void TestThreadManager::testNotifyOnThreadDone()
{
   TEST_CASE("testNotifyOnThreadDone");
}

//******************************************************************************

void TestThreadManager::testGetNumberThreads()
{
   TEST_CASE("testGetNumberThreads");
}

//******************************************************************************

void TestThreadManager::testStartPoolWorker()
{
   TEST_CASE("testStartPoolWorker");
}

//******************************************************************************

void TestThreadManager::testStopPoolWorker()
{
   TEST_CASE("testStopPoolWorker");
}

//******************************************************************************

void TestThreadManager::testGetPercentBusy()
{
   TEST_CASE("testGetPercentBusy");
}

//******************************************************************************

