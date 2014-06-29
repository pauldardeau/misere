// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "EchoHandler.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Logger.h"


//******************************************************************************
//******************************************************************************

EchoHandler::EchoHandler() noexcept
{
   Logger::logInstanceCreate("EchoHandler");
}

//******************************************************************************

EchoHandler::~EchoHandler() noexcept
{
   Logger::logInstanceDestroy("EchoHandler");
}

//******************************************************************************

void EchoHandler::serviceRequest(const HttpRequest& request,
                                 HttpResponse& response) noexcept
{
   std::string body = "<html><body>";
   
   body += request.getRequestMethod();
   body += " ";
   body += request.getRequestPath();
   body += " ";
   body += request.getProtocol();
   body += "<br/>";

   std::vector<std::string> headerKeys;
   request.getHeaderKeys(headerKeys);
   
   if (!headerKeys.empty()) {
      for (const std::string& headerKey : headerKeys) {
         const std::string& headerValue = request.getHeaderValue(headerKey);
         body += headerKey;
         body += ": ";
         body += headerValue;
         body += "<br/>";
      }
   } else {
      body += "no headers found!";
   }
   
   body += "<br/>";
   
   const std::string& requestBody = request.getBody();
   
   if (!requestBody.empty()) {
      body += requestBody;
   } else {
      body += "*** no body in request ***<br/>";
   }

   body += "<br/>";
   
   body += "</body></html>";
   
   response.setBody(body);
}

//******************************************************************************
//******************************************************************************

