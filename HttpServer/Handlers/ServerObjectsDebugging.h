// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__ServerObjectsDebugging__
#define __HttpServer__ServerObjectsDebugging__

#include <string>

#include "AbstractHandler.h"

class HttpRequest;
class HttpResponse;


/*!
 *
 */
class ServerObjectsDebugging : public AbstractHandler
{
public:
   ServerObjectsDebugging() noexcept;
   virtual ~ServerObjectsDebugging() noexcept;
   
   virtual void serviceRequest(const HttpRequest& request,
                               HttpResponse& response) noexcept override;
   
   std::string constructRow(const std::string& className,
                            long long created,
                            long long destroyed,
                            long long alive) const noexcept;
   
};


#endif /* defined(__HttpServer__ServerObjectsDebugging__) */
