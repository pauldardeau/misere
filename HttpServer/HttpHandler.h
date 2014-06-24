// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include <string>

class HttpRequest;
class HttpResponse;
class KeyValuePairs;

/*!
 *
 */
class HttpHandler
{
   public:
      virtual ~HttpHandler() {}

      virtual bool init(const std::string& path, const KeyValuePairs& kvpArguments) noexcept = 0;
      virtual void destroy() noexcept {}
      virtual void serviceRequest(const HttpRequest& request, HttpResponse& response) noexcept = 0;
      virtual bool isAvailable() const noexcept = 0;
};


#endif


