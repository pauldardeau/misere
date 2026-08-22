// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <csignal>
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

#ifdef SIGPIPE
   // A write to a socket whose peer has already closed its end raises
   // SIGPIPE, which by default terminates the process outright rather
   // than surfacing as an ordinary write failure - chaudiere::Socket's
   // write()/send() have no MSG_NOSIGNAL/SO_NOSIGPIPE protection of
   // their own (see Socket.cpp). Ignoring it here, once, before any
   // socket or thread exists, is the standard fix for any process doing
   // raw socket writes; it covers plain HTTP and TLS writes alike, since
   // both ultimately go through chaudiere::Socket (see SocketConnection/
   // SocketTransport) regardless of which ByteConnection the HTTP layer
   // is handed. Signal disposition is process-wide and inherited by
   // every thread HttpServer creates afterward (its own thread pool
   // included), so setting it this early, this once, is sufficient -
   // no per-connection or per-thread handling needed.
   //
   // Guarded on SIGPIPE being defined (POSIX) rather than e.g. #ifdef
   // _WIN32, so this is simply a no-op - not a special case to
   // maintain - on a platform without it, where a failed write already
   // just returns an error rather than raising anything.
   ::signal(SIGPIPE, SIG_IGN);
#endif

   std::string configFilePath;

   if (argc > 1) {
      configFilePath = argv[1];
   } else {
      const char* configPath = std::getenv(ENV_VAR_CFG_PATH.c_str());

      if (nullptr != configPath) {
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

