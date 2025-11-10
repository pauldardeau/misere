// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_GMTDATETIMEHANDLER_H
#define MISERE_GMTDATETIMEHANDLER_H

#include "AbstractHandler.h"

namespace misere
{
   class HttpRequest;
   class HttpResponse;

/**
 *
 */
class GMTDateTimeHandler : public AbstractHandler
{
public:
   GMTDateTimeHandler();
   virtual ~GMTDateTimeHandler();

   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response);

};

}

#endif
