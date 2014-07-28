// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_HTTPHANDLER_H
#define MISERE_HTTPHANDLER_H

#include <string>

#include "KeyValuePairs.h"


namespace misere
{
   class HttpRequest;
   class HttpResponse;

/**
 * HttpHandler is the interface that all handlers for HTTP requests must implement.
 */
class HttpHandler
{
   public:
      /**
       * Destructor
       */
      virtual ~HttpHandler() {}

      /**
       * Initialize the handler with the indicated path and arguments.
       * @param path the path associated with this handler.
       * @param kvpArguments the configuration key-value arguments.
       * @see KeyValuePairs()
       * @return boolean indicating if initialization of handler succeeded
       */
      virtual bool init(const std::string& path, const chaudiere::KeyValuePairs& kvpArguments) noexcept = 0;
   
      /**
       * The destroy method is called as part of cleanup operations when the server
       * is being terminated.
       */
      virtual void destroy() noexcept {}
   
      /**
       * The serviceRequest method is called to satisfy an HTTP request on the
       * path associated with this handler.
       * @param request the HTTP request
       * @param response the HTTP response
       * @see HttpRequest()
       * @see HttpResponse()
       */
      virtual void serviceRequest(const HttpRequest& request, HttpResponse& response) noexcept = 0;
   
      /**
       * The isAvailable method is called by the server prior to handing off a request
       * for processing to determine if the handler is currently available.
       * @return boolean indicating whether the handler is currently available for handing requests.
       */
      virtual bool isAvailable() const noexcept = 0;
};

}

#endif


