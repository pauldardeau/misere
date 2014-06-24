// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "TestPthreadsThreadPool.h"
#include "PthreadsThreadPool.h"

//******************************************************************************

TestPthreadsThreadPool::TestPthreadsThreadPool() :
   TestSuite("TestPthreadsThreadPool")
{
}

//******************************************************************************

void TestPthreadsThreadPool::runTests()
{
   testConstructor();
   testStart();
   testStop();
   testAddRequest();
   testGetNumberWorkers();
   testAddWorkers();
   testRemoveWorkers();
}

//******************************************************************************

void TestPthreadsThreadPool::testConstructor()
{
   TEST_CASE("testConstructor");
}

//******************************************************************************

void TestPthreadsThreadPool::testStart()
{
   TEST_CASE("testStart");
   
}

//******************************************************************************

void TestPthreadsThreadPool::testStop()
{
   TEST_CASE("testStop");
   
}

//******************************************************************************

void TestPthreadsThreadPool::testAddRequest()
{
   TEST_CASE("testAddRequest");
   
}

//******************************************************************************

void TestPthreadsThreadPool::testGetNumberWorkers()
{
   TEST_CASE("testGetNumberWorkers");
   
}

//******************************************************************************

void TestPthreadsThreadPool::testAddWorkers()
{
   TEST_CASE("testAddWorkers");
   
}

//******************************************************************************

void TestPthreadsThreadPool::testRemoveWorkers()
{
   TEST_CASE("testRemoveWorkers");
   
}

//******************************************************************************
