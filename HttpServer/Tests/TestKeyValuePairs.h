// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestKeyValuePairs__
#define __HttpServer__TestKeyValuePairs__

#include "TestSuite.h"


class TestKeyValuePairs : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testCopyConstructor();
   void testMoveConstructor();
   void testAssignmentCopy();
   void testAssignmentMove();
   void testGetKeys();
   void testHasKey();
   void testGetValue();
   void testAddPair();
   void testRemovePair();
   void testClear();
   void testSize();
   void testEmpty();
   
public:
   TestKeyValuePairs();
   

};

#endif /* defined(__HttpServer__TestKeyValuePairs__) */
