// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "HttpResponse.h"
#include "Logger.h"
#include "StdLogger.h"

#include "ServerObjectsDebugging.h"

using namespace misere;
using namespace chaudiere;

//******************************************************************************
//******************************************************************************

ServerObjectsDebugging::ServerObjectsDebugging() noexcept
{
   Logger::logInstanceCreate("ServerObjectsDebugging");
}

//******************************************************************************

ServerObjectsDebugging::~ServerObjectsDebugging() noexcept
{
   Logger::logInstanceDestroy("ServerObjectsDebugging");
}

//******************************************************************************

std::string ServerObjectsDebugging::constructRow(const std::string& className,
                                                 long long created,
                                                 long long destroyed,
                                                 long long alive) const noexcept
{
   std::string row;
   char buffer[128];
   
   bool isAlive = false;
   if (alive > 0L) {
      isAlive = true;
   }
   
   row += "<tr>";
   row += "<td>";
   if (isAlive) {
      row += "<b>";
   }
   row += className;
   if (isAlive) {
      row += "</b>";
   }
   row += "</td>";
   
   std::snprintf(buffer, 128, "%lld", created);
   row += "<td align=\"right\">";
   if (isAlive) {
      row += "<b>";
   }
   row += std::string(buffer);
   if (isAlive) {
      row += "</b>";
   }
   row += "</td>";
   
   std::snprintf(buffer, 128, "%lld", destroyed);
   row += "<td align=\"right\">";
   if (isAlive) {
      row += "<b>";
   }
   row += std::string(buffer);
   if (isAlive) {
      row += "</b>";
   }
   row += "</td>";
   
   std::snprintf(buffer, 128, "%lld", alive);
   row += "<td align=\"right\">";
   if (isAlive) {
      row += "<b>";
   }
   row += std::string(buffer);
   if (isAlive) {
      row += "</b>";
   }
   row += "</td>";
   
   row += "</tr>";
   
   return row;
}

//******************************************************************************

void ServerObjectsDebugging::serviceRequest(const HttpRequest& request,
                                            HttpResponse& response) noexcept
{
   std::string body = "<html><body>";
   
   Logger* logger = Logger::getLogger();
   
   if (logger) {
      Logger* pLoggerInstance = logger.get();
      StdLogger* stdLogger = dynamic_cast<StdLogger*>(pLoggerInstance);
      if (stdLogger) {
         std::unordered_map<std::string, LifecycleStats> mapClassStats;
         stdLogger->populateClassLifecycleStats(mapClassStats);
         
         if (!mapClassStats.empty()) {
            
            body += "<table border=\"1\">";
            body += "<tr><th align=\"left\">Class</th><th>Created</th><th>Destroyed</th><th>Alive</th></tr>";
            
            long long totalCreated = 0L;
            long long totalDestroyed = 0L;
            long long totalAlive = 0L;
         
            for (const auto kv : mapClassStats) {
               const std::string& className = kv.first;
               const LifecycleStats& stats = kv.second;
               const long long created = stats.m_instancesCreated;
               const long long destroyed = stats.m_instancesDestroyed;
               const long long alive = created - destroyed;
               
               totalCreated += created;
               totalDestroyed += destroyed;
               totalAlive += alive;
               
               body += constructRow(className, created, destroyed, alive);
            }
            
            body += constructRow("TOTAL", totalCreated, totalDestroyed, totalAlive);

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

