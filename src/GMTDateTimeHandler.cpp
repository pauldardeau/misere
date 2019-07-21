// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <stdio.h>
#include <time.h>

#include "GMTDateTimeHandler.h"
#include "HttpResponse.h"
#include "Logger.h"

using namespace std;
using namespace misere;
using namespace chaudiere;

//******************************************************************************
//******************************************************************************

GMTDateTimeHandler::GMTDateTimeHandler() {
   LOG_INSTANCE_CREATE("GMTDateTimeHandler")
}

//******************************************************************************

GMTDateTimeHandler::~GMTDateTimeHandler() {
   LOG_INSTANCE_DESTROY("GMTDateTimeHandler")
}

//******************************************************************************

void GMTDateTimeHandler::serviceRequest(const HttpRequest& request,
                                        HttpResponse& response) {
   std::string body = "<html><body>";
   
   time_t currentTime = time(NULL);
   
   if (currentTime == (time_t)-1) {
      body += "Unavailable";
   } else {
      time_t currentGMT;
      ::time(&currentGMT);
      
      struct tm* timeptr = ::gmtime(&currentGMT);
      
      char dateBuffer[128];
      
      ::snprintf(dateBuffer, 128,
                 "%d-%.2d-%.2d %.2d:%.2d:%.2d",
                 1900 + timeptr->tm_year,
                 timeptr->tm_mon,
                 timeptr->tm_mday,
                 timeptr->tm_hour,
                 timeptr->tm_min,
                 timeptr->tm_sec);

      body += dateBuffer;
   }
   
   body += "</body></html>";
   
   response.setBody(new ByteBuffer(body));
}

//******************************************************************************
//******************************************************************************

