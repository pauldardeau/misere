// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestMutex__
#define __HttpServer__TestMutex__

#include "TestSuite.h"


class TestPthreadsMutex : public TestSuite
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
   TestPthreadsMutex();
   
};


#endif /* defined(__HttpServer__TestMutex__) */
