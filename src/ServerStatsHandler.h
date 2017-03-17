// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_SERVERSTATSHANDLER_H
#define MISERE_SERVERSTATSHANDLER_H

#include "AbstractHandler.h"

namespace misere {

class HttpRequest;
class HttpResponse;

/**
 *
 */
class ServerStatsHandler : public AbstractHandler {

public:
   ServerStatsHandler();
   virtual ~ServerStatsHandler();
   
   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response);
   
   std::string constructRow(const std::string& occurrenceType,
                            const std::string& occurrenceName,
                            long long occurrenceCount) const;

};

}

#endif
