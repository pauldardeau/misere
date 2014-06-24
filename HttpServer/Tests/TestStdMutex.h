// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestStdMutex__
#define __HttpServer__TestStdMutex__

#include "TestSuite.h"


class TestStdMutex : public TestSuite
{
protected:
   void runTests();
   
   void testConstructor();
   void testConstructorWithName();
   
   void testLock();
   void testUnlock();
   void testHaveValidMutex();
   void testGetPlatformPrimitive();
   void testGetName();
   void testIsLocked();
   
public:
   TestStdMutex();
   
};

#endif /* defined(__HttpServer__TestStdMutex__) */
