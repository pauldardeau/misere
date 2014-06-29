// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__ServerStatusHandler__
#define __HttpServer__ServerStatusHandler__

#include "AbstractHandler.h"

class HttpRequest;
class HttpResponse;

/*!
 *
 */
class ServerStatusHandler : public AbstractHandler
{
public:
   ServerStatusHandler() noexcept;
   virtual ~ServerStatusHandler() noexcept;
   
   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response) noexcept override;
   
};


#endif /* defined(__HttpServer__ServerStatusHandler__) */
