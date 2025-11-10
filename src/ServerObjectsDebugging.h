// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_SERVEROBJECTSDEBUGGING_H
#define MISERE_SERVEROBJECTSDEBUGGING_H

#include <string>

#include "AbstractHandler.h"

namespace misere
{
   class HttpRequest;
   class HttpResponse;

/**
 *
 */
class ServerObjectsDebugging : public AbstractHandler
{
public:
   ServerObjectsDebugging();
   virtual ~ServerObjectsDebugging();

   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response);

   std::string constructRow(const std::string& className,
                            long long created,
                            long long destroyed,
                            long long alive) const;

};

}

#endif
