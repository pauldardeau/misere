// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestIniReader__
#define __HttpServer__TestIniReader__

#include "TestSuite.h"


class TestIniReader : public TestSuite
{
private:
   std::string m_filePath;

protected:
   void runTests();
   void setupSuite();
   void tearDownSuite();
   
   void testConstructor();
   void testReadSection();
   void testGetSectionKeyValue();
   void testHasSection();
   
public:
   TestIniReader();

};

#endif /* defined(__HttpServer__TestIniReader__) */
