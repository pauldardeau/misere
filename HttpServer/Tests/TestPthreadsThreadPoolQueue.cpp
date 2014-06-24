// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "TestPthreadsThreadPoolQueue.h"
#include "PthreadsThreadPoolQueue.h"

//******************************************************************************

TestPthreadsThreadPoolQueue::TestPthreadsThreadPoolQueue() :
   TestSuite("TestPthreadsThreadPoolQueue")
{
}

//******************************************************************************

void TestPthreadsThreadPoolQueue::runTests()
{
   testConstructor();
   testAddRequest();
   testTakeRequest();
   testShutDown();
   testIsRunning();
   testIsEmpty();
}

//******************************************************************************

void TestPthreadsThreadPoolQueue::testConstructor()
{
   TEST_CASE("testConstructor");
}

//******************************************************************************

void TestPthreadsThreadPoolQueue::testAddRequest()
{
   TEST_CASE("testAddRequest");
}

//******************************************************************************

void TestPthreadsThreadPoolQueue::testTakeRequest()
{
   TEST_CASE("testTakeRequest");
}

//******************************************************************************

void TestPthreadsThreadPoolQueue::testShutDown()
{
   TEST_CASE("testShutDown");
}

//******************************************************************************

void TestPthreadsThreadPoolQueue::testIsRunning()
{
   TEST_CASE("testIsRunning");
}

//******************************************************************************

void TestPthreadsThreadPoolQueue::testIsEmpty()
{
   TEST_CASE("testIsEmpty");
}

//******************************************************************************

