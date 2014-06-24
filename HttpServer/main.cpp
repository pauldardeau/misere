// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <memory>

#include "HttpServer.h"
#include "BasicException.h"
#include "Logger.h"
#include "StdLogger.h"
//#include "Tests.h"


int main(int argc, char* argv[])
{
   //Tests tests;
   //tests.run();
   
   std::shared_ptr<StdLogger> logger(new StdLogger(Logger::LogLevel::Warning));
   logger->setLogInstanceLifecycles(true);
   Logger::setLogger(logger);
   
   try
   {
      HttpServer server("/Users/paul/cppserver/misere.ini");
      server.run();
      return 0;
   }
   catch (const BasicException& be)
   {
      Logger::critical("exception running HttpServer: " + be.whatString());
      return 1;
   }
   catch (const std::exception& e)
   {
      Logger::critical("exception running HttpServer: " + std::string(e.what()));
      return 1;
   }
   catch (...)
   {
      Logger::critical("unknown exception running HttpServer");
      return 1;
   }
}
