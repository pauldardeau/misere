// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "HttpResponse.h"
#include "Logger.h"

#include "ServerDateTimeHandler.h"


//******************************************************************************
//******************************************************************************

ServerDateTimeHandler::ServerDateTimeHandler() noexcept
{
   Logger::logInstanceCreate("ServerDateTimeHandler");
}

//******************************************************************************

ServerDateTimeHandler::~ServerDateTimeHandler() noexcept
{
   Logger::logInstanceDestroy("ServerDateTimeHandler");
}

//******************************************************************************

void ServerDateTimeHandler::serviceRequest(const HttpRequest& request,
                                           HttpResponse& response) noexcept
{
   std::string body = "<html><body>";
   
   time_t currentTime = time(nullptr);
   
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

