// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef HttpServer_ServerDateTimeHandler_h
#define HttpServer_ServerDateTimeHandler_h

#include "AbstractHandler.h"

class HttpRequest;
class HttpResponse;

/*!
 *
 */
class ServerDateTimeHandler : public AbstractHandler
{
public:
   ServerDateTimeHandler() noexcept;
   virtual ~ServerDateTimeHandler() noexcept;
   
   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response) noexcept override;
   
};


#endif
