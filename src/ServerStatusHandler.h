// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_SERVERSTATUSHANDLER_H
#define MISERE_SERVERSTATUSHANDLER_H

#include "AbstractHandler.h"

namespace misere
{
   class HttpRequest;
   class HttpResponse;

/**
 *
 */
class ServerStatusHandler : public AbstractHandler
{
public:
   ServerStatusHandler();
   virtual ~ServerStatusHandler();

   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response);

};

}

#endif
