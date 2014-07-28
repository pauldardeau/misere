// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_SERVERSTATSHANDLER_H
#define MISERE_SERVERSTATSHANDLER_H

#include "AbstractHandler.h"

namespace misere
{
   class HttpRequest;
   class HttpResponse;
   class HttpServer;

/**
 *
 */
class ServerStatsHandler : public AbstractHandler
{
private:
   const HttpServer& m_server;
   
public:
   explicit ServerStatsHandler(const HttpServer& server) noexcept;
   virtual ~ServerStatsHandler() noexcept;
   
   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response) noexcept override;
   
   std::string constructRow(const std::string& occurrenceType,
                            const std::string& occurrenceName,
                            long long occurrenceCount) const noexcept;

};

}

#endif
