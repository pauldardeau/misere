// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_ECHOHANDLER_H
#define MISERE_ECHOHANDLER_H

#include "AbstractHandler.h"

namespace misere
{
   class HttpRequest;
   class HttpResponse;

/**
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

}

#endif
