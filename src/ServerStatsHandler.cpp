// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <stdio.h>

#include "HttpResponse.h"
#include "Logger.h"
#include "StdLogger.h"

#include "ServerStatsHandler.h"

using namespace std;
using namespace misere;
using namespace chaudiere;

//******************************************************************************
//******************************************************************************

ServerStatsHandler::ServerStatsHandler() {
   LOG_INSTANCE_CREATE("ServerStatsHandler")
}

//******************************************************************************

ServerStatsHandler::~ServerStatsHandler() {
   LOG_INSTANCE_DESTROY("ServerStatsHandler")
}

//******************************************************************************

std::string ServerStatsHandler::constructRow(const std::string& occurrenceType,
                                             const std::string& occurrenceName,
                                             long long occurrences) const {
   std::string row;
   char buffer[128];
   
   row += "<tr>";
   
   row += "<td>";
   row += occurrenceType;
   row += "</td>";

   row += "<td>";
   row += occurrenceName;
   row += "</td>";

   ::snprintf(buffer, 128, "%lld", occurrences);
   row += "<td align=\"right\">";
   row += std::string(buffer);
   row += "</td>";
   
   row += "</tr>";
   
   return row;
}

//******************************************************************************

void ServerStatsHandler::serviceRequest(const HttpRequest& request,
                                        HttpResponse& response) {
   string body = "<html><body>";
   
   Logger* logger = Logger::getLogger();
   
   if (logger) {
      Logger* pLoggerInstance = logger;
      StdLogger* stdLogger = dynamic_cast<StdLogger*>(pLoggerInstance);
      if (stdLogger) {
         unordered_map<string, unordered_map<string, long long> > mapOccurrenceTypes;
         stdLogger->populateOccurrences(mapOccurrenceTypes);
         
         if (!mapOccurrenceTypes.empty()) {
            
            body += "<table border=\"1\">";
            body += "<tr><th align=\"left\">Type</th><th align=\"left\">Name</th><th>Occurrences</th></tr>";
            
            long long totalForType = 0L;
            
            for (auto& pair: mapOccurrenceTypes) {
               const string& occurrenceType = pair.first;
               const unordered_map<string, long long>& mapOccurrences = pair.second;

               for (auto& innerPair: mapOccurrences) {
                  const string& occurrenceName = innerPair.first;
                  const long long numOccurrences = innerPair.second;
                  
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
   
   response.setBody(new ByteBuffer(body));
}

//******************************************************************************
//******************************************************************************

