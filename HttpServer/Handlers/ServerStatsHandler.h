// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__ServerStatsHandler__
#define __HttpServer__ServerStatsHandler__

#include "AbstractHandler.h"

class HttpRequest;
class HttpResponse;
class HttpServer;

/*!
 *
 */
class ServerStatsHandler : public AbstractHandler
{
private:
   const HttpServer& m_server;
   
public:
   ServerStatsHandler(const HttpServer& server) noexcept;
   virtual ~ServerStatsHandler() noexcept;
   
   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response) noexcept override;
   
   std::string constructRow(const std::string& occurrenceType,
                            const std::string& occurrenceName,
                            long long occurrenceCount) const noexcept;

};

#endif /* defined(__HttpServer__ServerStatsHandler__) */
