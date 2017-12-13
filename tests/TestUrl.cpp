// Copyright Paul Dardeau, SwampBits LLC 2017
// BSD License

#include "TestUrl.h"
#include "Url.h"

static const std::string PROTOCOL = "http";
static const std::string HOST1 = "192.168.0.26";
static const int PORT = 8000;
static const std::string PORT_TEXT = "8000";
static const std::string PATH1 = "/documents/test.txt";
static const std::string FULL_TEXT = PROTOCOL + std::string("://") +
                                     HOST1 +
                                     std::string(":") +
                                     PORT_TEXT +
                                     PATH1;
static const std::string FULL_NO_PORT = PROTOCOL + std::string("://") +
                                        HOST1 +
                                        PATH1;
static const std::string HOST2 = "172.16.0.81";
static const std::string PATH2 = "/images/dog.png";
static const std::string FULL_TEXT2 = PROTOCOL + std::string("://") +
                                     HOST2 +
                                     std::string(":") +
                                     PORT_TEXT +
                                     PATH2;

using namespace std;
using namespace misere;

//******************************************************************************

TestUrl::TestUrl() :
   TestSuite("TestUrl") {
}

//******************************************************************************

void TestUrl::runTests() {
   testConstructor();
   testCopyConstructor();
   testAssignmentCopy();
   testGetFullText();
   testGetHost();
   testGetPath();
   testGetProtocol();
   testGetPort();
}

//******************************************************************************

void TestUrl::testConstructor() {
   TEST_CASE("testConstructor");
   Url url(FULL_TEXT);
}

//******************************************************************************

void TestUrl::testCopyConstructor() {
   TEST_CASE("testCopyConstructor");
   Url url(FULL_TEXT);
   Url urlCopy(url);
   require(url == urlCopy, "copy ctor should result in equality");
}

//******************************************************************************

void TestUrl::testAssignmentCopy() {
   TEST_CASE("testAssignmentCopy");
   Url url1(FULL_TEXT);
   Url url2(FULL_TEXT2);
   url2 = url1;
   require(url1 == url2, "assign operator should result in equality");
}

//******************************************************************************

void TestUrl::testGetProtocol() {
   TEST_CASE("testGetProtocol");
   Url url(FULL_TEXT);
   requireStringEquals(PROTOCOL, url.protocol(), "protocol should match"); 
}

//******************************************************************************

void TestUrl::testGetPath() {
   TEST_CASE("testGetPath");
   Url url(FULL_TEXT);
   requireStringEquals(PATH1, url.path(), "path should match");
}

//******************************************************************************

void TestUrl::testGetHost() {
   TEST_CASE("testGetHost");
   Url url(FULL_TEXT);
   requireStringEquals(HOST1, url.host(), "host should match");
}

//******************************************************************************

void TestUrl::testGetFullText() {
   TEST_CASE("testGetFullText");
   Url url(FULL_TEXT);
   requireStringEquals(FULL_TEXT, url.fullText(), "full text should match");
}

//******************************************************************************

void TestUrl::testGetPort() {
   TEST_CASE("testGetPort");
   Url url(FULL_TEXT);
   require(PORT == url.port(), "port should match");
}

