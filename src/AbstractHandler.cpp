// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "AbstractHandler.h"
#include "Logger.h"

using namespace misere;
using namespace chaudiere;

//******************************************************************************

bool AbstractHandler::init(const std::string& path,
                           const KeyValuePairs& kvpArguments) {
   return true;
}

//******************************************************************************

void AbstractHandler::serviceRequest(const HttpRequest& request,
                                     HttpResponse& response) {
   Logger::debug("serviceRequest called on AbstractHandler. did you forget to override serviceRequest?");
}

//******************************************************************************

bool AbstractHandler::isAvailable() const {
   return true;
}

//******************************************************************************

