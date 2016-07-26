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
   ServerStatsHandler() noexcept;
   virtual ~ServerStatsHandler() noexcept;
   
   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response) noexcept override;
   
   std::string constructRow(const std::string& occurrenceType,
                            const std::string& occurrenceName,
                            long long occurrenceCount) const noexcept;

};

}

#endif
