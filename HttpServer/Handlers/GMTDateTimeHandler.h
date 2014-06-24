// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__GMTDateTimeHandler__
#define __HttpServer__GMTDateTimeHandler__

#include "AbstractHandler.h"

class HttpRequest;
class HttpResponse;

/*!
 *
 */
class GMTDateTimeHandler : public AbstractHandler
{
public:
   GMTDateTimeHandler() noexcept;
   virtual ~GMTDateTimeHandler() noexcept;
   
   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response) noexcept override;
   
};


#endif
