// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestStringTokenizer__
#define __HttpServer__TestStringTokenizer__

#include "TestSuite.h"


class TestStringTokenizer : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testConstructorWithDelimiter();
   void testHasMoreTokens();
   void testNextToken();
   void testCountTokens();
   
public:
   TestStringTokenizer();
   

};

#endif /* defined(__HttpServer__TestStringTokenizer__) */
