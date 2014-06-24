// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__TestSuite__
#define __HttpServer__TestSuite__

#include <string>

#include "TestMacros.h"

class TestCase;


class TestSuite
{
private:
   std::string m_suiteName;
   int m_numTestsExecuted;
   int m_numFailures;

   
protected:
   virtual void setup();
   virtual void tearDown();
   
   virtual void suiteSetup();
   virtual void suiteTearDown();
   virtual void runTests() = 0;

   
public:
   TestSuite(const std::string& suiteName);
   TestSuite(const TestSuite& copy);
   
   virtual ~TestSuite();
   
   TestSuite& operator=(const TestSuite& copy);

   
   // file management
   std::string getTempFile() const;
   bool deleteFile(const std::string& filePath);
   
   void run();
   
   const std::string& getName() const;
   
   void require(bool expression, const std::string& testDesc);
   void requireFalse(bool expression, const std::string& testDesc);

   void requireStringEquals(const std::string& expected,
                            const std::string& actual,
                            const std::string& testDesc);
   void requireNonEmptyString(const std::string& actual, const std::string& testDesc);
   
   void startingTestCase(const TestCase& testCase);
   void endingTestCase(const TestCase& testCase);
   
};

#endif /* defined(__HttpServer__TestSuite__) */
