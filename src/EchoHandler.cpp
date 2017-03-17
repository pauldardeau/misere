// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "EchoHandler.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Logger.h"

using namespace std;
using namespace misere;
using namespace chaudiere;

//******************************************************************************
//******************************************************************************

EchoHandler::EchoHandler() {
   Logger::logInstanceCreate("EchoHandler");
}

//******************************************************************************

EchoHandler::~EchoHandler() {
   Logger::logInstanceDestroy("EchoHandler");
}

//******************************************************************************

void EchoHandler::serviceRequest(const HttpRequest& request,
                                 HttpResponse& response) {
   string body = "<html><body>";
   
   body += request.getRequestMethod();
   body += " ";
   body += request.getRequestPath();
   body += " ";
   body += request.getProtocol();
   body += "<br/>";

   vector<string> headerKeys;
   request.getHeaderKeys(headerKeys);
   
   if (!headerKeys.empty()) {
      vector<string>::const_iterator it = headerKeys.begin();
      const vector<string>::const_iterator itEnd = headerKeys.end();
      for (; it != itEnd; it++) {
         const std::string& headerKey = *it;
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
   
   const string& requestBody = request.getBody();
   
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

