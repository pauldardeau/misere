// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__EchoHandler__
#define __HttpServer__EchoHandler__

#include "AbstractHandler.h"

class HttpRequest;
class HttpResponse;

/*!
 *
 */
class EchoHandler : public AbstractHandler
{
public:
   EchoHandler() noexcept;
   virtual ~EchoHandler() noexcept;
   
   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response) noexcept override;
   
};

#endif /* defined(__HttpServer__EchoHandler__) */
