// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_SERVERDATETIMEHANDLER_H
#define MISERE_SERVERDATETIMEHANDLER_H

#include "AbstractHandler.h"

namespace misere
{
   class HttpRequest;
   class HttpResponse;

/**
 *
 */
class ServerDateTimeHandler : public AbstractHandler
{
public:
   ServerDateTimeHandler();
   virtual ~ServerDateTimeHandler();
   
   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response);
   
};

}

#endif
