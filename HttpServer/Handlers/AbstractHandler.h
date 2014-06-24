// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__AbstractHandler__
#define __HttpServer__AbstractHandler__

#include "HttpHandler.h"


class AbstractHandler : public HttpHandler
{
public:
   virtual bool init(const std::string& path,
                     const KeyValuePairs& kvpArguments) noexcept override;
   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response) noexcept override;
   virtual bool isAvailable() const noexcept override;

};

#endif /* defined(__HttpServer__AbstractHandler__) */

