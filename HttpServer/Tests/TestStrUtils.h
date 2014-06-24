// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestStrUtils__
#define __HttpServer__TestStrUtils__

#include "TestSuite.h"


class TestStrUtils : public TestSuite
{
protected:
   void runTests();
   
   void testStrip();
   void testStripWithChar();
   
   void testStartsWith();
   void testEndsWith();
   void testContainsString();
   
   void testToUpperCase();
   void testToLowerCase();
   
   void testReplaceAll();
   
   void testStripInPlace();
   void testStripTrailing();
   void testStripLeading();
   void testTrimLeadingSpaces();

public:
   TestStrUtils();
   
};

#endif /* defined(__HttpServer__TestStrUtils__) */
