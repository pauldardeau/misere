// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "HttpResponse.h"
#include "HttpServer.h"
#include "Logger.h"
#include "StdLogger.h"

#include "ServerStatsHandler.h"

using namespace misere;
using namespace chaudiere;

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

std::string ServerStatsHandler::constructRow(const std::string& occurrenceType,
                                             const std::string& occurrenceName,
                                             long long occurrences) const noexcept
{
   std::string row;
   char buffer[128];
   
   row += "<tr>";
   
   row += "<td>";
   row += occurrenceType;
   row += "</td>";

   row += "<td>";
   row += occurrenceName;
   row += "</td>";

   std::snprintf(buffer, 128, "%lld", occurrences);
   row += "<td align=\"right\">";
   row += std::string(buffer);
   row += "</td>";
   
   row += "</tr>";
   
   return row;
}

//******************************************************************************

void ServerStatsHandler::serviceRequest(const HttpRequest& request,
                                        HttpResponse& response) noexcept
{
   std::string body = "<html><body>";
   
   Logger* logger = Logger::getLogger();
   
   if (logger) {
      Logger* pLoggerInstance = logger;
      StdLogger* stdLogger = dynamic_cast<StdLogger*>(pLoggerInstance);
      if (stdLogger) {
         std::map<std::string, std::map<std::string, long long>> mapOccurrenceTypes;
         stdLogger->populateOccurrences(mapOccurrenceTypes);
         
         if (!mapOccurrenceTypes.empty()) {
            
            body += "<table border=\"1\">";
            body += "<tr><th align=\"left\">Type</th><th align=\"left\">Name</th><th>Occurrences</th></tr>";
            
            long long totalForType = 0L;
            
            for (const auto kv : mapOccurrenceTypes) {
               const std::string& occurrenceType = kv.first;
               const std::map<std::string, long long>& mapOccurrences = kv.second;

               for (const auto kvOccurrence : mapOccurrences) {
                  const std::string& occurrenceName = kvOccurrence.first;
                  const long long numOccurrences = kvOccurrence.second;
                  
                  totalForType += numOccurrences;
                  
                  body += constructRow(occurrenceType, occurrenceName, numOccurrences);
               }
               
               body += constructRow(occurrenceType, "TOTAL", totalForType);
               
               totalForType = 0L;
            }
            
            body += "</table>";
         } else {
            body += "No stats available";
         }
      }
   }
   
   body += "</body></html>";
   
   response.setBody(body);
}

//******************************************************************************
//******************************************************************************

