// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "TestRequestHandler.h"
#include "RequestHandler.h"

//******************************************************************************

TestRequestHandler::TestRequestHandler() :
   TestSuite("TestRequestHandler")
{
}

//******************************************************************************

void TestRequestHandler::runTests()
{
   testConstructor();
   testRun();
   testSetThreadPooling();
}

//******************************************************************************

void TestRequestHandler::testConstructor()
{
   TEST_CASE("testConstructor");
   
}

//******************************************************************************

void TestRequestHandler::testRun()
{
   TEST_CASE("testRun");
   
}

//******************************************************************************

void TestRequestHandler::testSetThreadPooling()
{
   TEST_CASE("testSetThreadPooling");
   
}

//******************************************************************************

