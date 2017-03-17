// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <time.h>

#include "HttpResponse.h"
#include "Logger.h"

#include "ServerDateTimeHandler.h"

using namespace misere;
using namespace chaudiere;

//******************************************************************************
//******************************************************************************

ServerDateTimeHandler::ServerDateTimeHandler() {
   Logger::logInstanceCreate("ServerDateTimeHandler");
}

//******************************************************************************

ServerDateTimeHandler::~ServerDateTimeHandler() {
   Logger::logInstanceDestroy("ServerDateTimeHandler");
}

//******************************************************************************

void ServerDateTimeHandler::serviceRequest(const HttpRequest& request,
                                           HttpResponse& response) {
   std::string body = "<html><body>";
   
   time_t currentTime = time(NULL);
   
   if (currentTime == (time_t)-1) {
      body += "Unavailable";
   } else {
      char currentDateTime[80];
      const struct tm* nowstruct = localtime(&currentTime);
      if (strftime(currentDateTime, 80, "%Y-%m-%d %H:%M:%S", nowstruct)) {
         body += currentDateTime;
      } else {
         body += "Unavailable";
      }
   }
   
   body += "</body></html>";
   
   response.setBody(body);
}

//******************************************************************************
//******************************************************************************

