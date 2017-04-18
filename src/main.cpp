// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cstdlib>
#include <memory>

#include "HttpServer.h"
#include "BasicException.h"
#include "Logger.h"
#include "StdLogger.h"
//#include "Tests.h"

static const std::string ENV_VAR_CFG_PATH        = "MISERE_HOME";
static const std::string CFG_FILE_NAME           = "misere.ini";

using namespace misere;
using namespace chaudiere;

int main(int argc, char* argv[]) {
   //Tests tests;
   //tests.run();
   
   std::string configFilePath;
   
   if (argc > 1) {
      configFilePath = argv[1];
   } else {
      const char* configPath = std::getenv(ENV_VAR_CFG_PATH.c_str());
      
      if (NULL != configPath) {
         configFilePath = configPath;
         
         if (configFilePath[configFilePath.length()-1] != '/') {
            configFilePath += "/";
         }
         
         configFilePath += CFG_FILE_NAME;
      }
   }

   StdLogger* logger = new StdLogger(Warning);
   logger->setLogInstanceLifecycles(true);
   Logger::setLogger(logger);

   if (configFilePath.empty()) {
      Logger::error("no config file provided");
      return 1;
   }
   
   try {
      HttpServer server(configFilePath);
      server.run();
      return 0;
   } catch (const BasicException& be) {
      Logger::critical("exception running HttpServer: " + be.whatString());
      return 1;
   } catch (const std::exception& e) {
      Logger::critical("exception running HttpServer: " + std::string(e.what()));
      return 1;
   } catch (...) {
      Logger::critical("unknown exception running HttpServer");
      return 1;
   }
}

