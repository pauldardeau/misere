// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "ByteBuffer.h"
#include "EchoHandler.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Logger.h"
#include "StrUtils.h"

using namespace std;
using namespace misere;
using namespace chaudiere;

//******************************************************************************
//******************************************************************************

EchoHandler::EchoHandler() {
   LOG_INSTANCE_CREATE("EchoHandler")
}

//******************************************************************************

EchoHandler::~EchoHandler() {
   LOG_INSTANCE_DESTROY("EchoHandler")
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
      for (const auto& headerKey : headerKeys) {
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

   const ByteBuffer* requestBody = request.getBody();
   if (requestBody != nullptr && !requestBody->empty()) {
      std::string bodyReply = "body bytes: ";
      bodyReply += StrUtils::toString((int)requestBody->size());
      body += bodyReply;
   } else {
      body += "*** no body in request ***<br/>";
   }

   body += "<br/>";
   body += "</body></html>";

   response.setBody(new ByteBuffer(body));
}

//******************************************************************************
//******************************************************************************

