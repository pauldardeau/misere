// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <time.h>

#include "GMTDateTimeHandler.h"
#include "HttpResponse.h"
#include "Logger.h"

using namespace misere;
using namespace chaudiere;

//******************************************************************************
//******************************************************************************

GMTDateTimeHandler::GMTDateTimeHandler() noexcept
{
   Logger::logInstanceCreate("GMTDateTimeHandler");
}

//******************************************************************************

GMTDateTimeHandler::~GMTDateTimeHandler() noexcept
{
   Logger::logInstanceDestroy("GMTDateTimeHandler");
}

//******************************************************************************

void GMTDateTimeHandler::serviceRequest(const HttpRequest& request,
                                        HttpResponse& response) noexcept
{
   std::string body = "<html><body>";
   
   time_t currentTime = time(nullptr);
   
   if (currentTime == (time_t)-1) {
      body += "Unavailable";
   } else {
      time_t currentGMT;
      ::time(&currentGMT);
      
      struct tm* timeptr = ::gmtime(&currentGMT);
      
      char dateBuffer[128];
      
      std::snprintf(dateBuffer, 128, "%d-%.2d-%.2d %.2d:%.2d:%.2d",
                1900 + timeptr->tm_year,
                timeptr->tm_mon,
                timeptr->tm_mday,
                timeptr->tm_hour,
                timeptr->tm_min,
                timeptr->tm_sec);

      body += dateBuffer;
   }
   
   body += "</body></html>";
   
   response.setBody(body);
}

//******************************************************************************
//******************************************************************************

