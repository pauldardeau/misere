// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_ABSTRACTHANDLER_H
#define MISERE_ABSTRACTHANDLER_H

#include "HttpHandler.h"
#include "KeyValuePairs.h"

namespace misere
{

class AbstractHandler : public HttpHandler
{
public:
   virtual bool init(const std::string& path,
                     const chaudiere::KeyValuePairs& kvpArguments);
   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response);
   virtual bool isAvailable() const;

};

}

#endif

