// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "ServerStatusHandler.h"
#include "HttpResponse.h"
#include "Logger.h"
#include "SystemStats.h"

using namespace misere;
using namespace chaudiere;

//******************************************************************************
//******************************************************************************

ServerStatusHandler::ServerStatusHandler() noexcept {
   Logger::logInstanceCreate("ServerStatusHandler");
}

//******************************************************************************

ServerStatusHandler::~ServerStatusHandler() noexcept {
   Logger::logInstanceDestroy("ServerStatusHandler");
}

//******************************************************************************

void ServerStatusHandler::serviceRequest(const HttpRequest& request,
                                         HttpResponse& response) noexcept {
   std::string body = "<html><body>";

   double oneMinuteLoad = 0.0;
   double fiveMinuteLoad = 0.0;
   double fifteenMinuteLoad = 0.0;
   
   if (SystemStats::getLoadAverages(oneMinuteLoad,
                                    fiveMinuteLoad,
                                    fifteenMinuteLoad)) {
      char buffer[128];
      snprintf(buffer, 128,
               "load averages: %f, %f, %f",
               oneMinuteLoad,
               fiveMinuteLoad,
               fifteenMinuteLoad);
      body += std::string(buffer);
   }
   
   body += "</body></html>";
   
   response.setBody(body);
}

//******************************************************************************
//******************************************************************************

