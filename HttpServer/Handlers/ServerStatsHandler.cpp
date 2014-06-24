// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "HttpResponse.h"
#include "HttpServer.h"
#include "Logger.h"

#include "ServerStatsHandler.h"


//******************************************************************************
//******************************************************************************

ServerStatsHandler::ServerStatsHandler(const HttpServer& server) noexcept :
   m_server(server)
{
   Logger::logInstanceCreate("ServerStatsHandler");
}

//******************************************************************************

ServerStatsHandler::~ServerStatsHandler() noexcept
{
   Logger::logInstanceDestroy("ServerStatsHandler");
}

//******************************************************************************

void ServerStatsHandler::serviceRequest(const HttpRequest& request,
                                        HttpResponse& response) noexcept
{
   std::string body = "<html><body>";
   
   body += "Platform word size:&nbsp;";
   
   char platformBits[10];
   std::snprintf(platformBits, 10, "%d bit", m_server.platformPointerSizeBits());
   body += std::string(platformBits);
   
   body += "</body></html>";
   
   response.setBody(body);
}

//******************************************************************************
//******************************************************************************

