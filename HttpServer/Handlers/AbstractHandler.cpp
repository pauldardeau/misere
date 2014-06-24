// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "AbstractHandler.h"
#include "Logger.h"

//******************************************************************************

bool AbstractHandler::init(const std::string& path,
                           const KeyValuePairs& kvpArguments) noexcept
{
   return true;
}

//******************************************************************************

void AbstractHandler::serviceRequest(const HttpRequest& request,
                                     HttpResponse& response) noexcept
{
   Logger::debug("serviceRequest called on AbstractHandler. did you forget to override serviceRequest?");
}

//******************************************************************************

bool AbstractHandler::isAvailable() const noexcept
{
   return true;
}

//******************************************************************************

