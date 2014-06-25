// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cstdio>
#include "StdLogger.h"
#include "PthreadsMutex.h"
#include "MutexLock.h"

const std::string StdLogger::prefixCritical = "Criticial:";
const std::string StdLogger::prefixError    = "Error:";
const std::string StdLogger::prefixWarning  = "Warning:";
const std::string StdLogger::prefixInfo     = "Info:";
const std::string StdLogger::prefixDebug    = "Debug:";
const std::string StdLogger::prefixVerbose  = "Verbose:";


//******************************************************************************

StdLogger::StdLogger() noexcept :
   m_logLevel(Logger::LogLevel::Debug),
   m_isLoggingInstanceLifecycles(false)
{
   m_lockLifecycleStats =
      std::unique_ptr<Mutex>(new PthreadsMutex("lockLifecycleStats"));
}

//******************************************************************************

StdLogger::StdLogger(LogLevel logLevel) noexcept :
   m_logLevel(logLevel),
   m_isLoggingInstanceLifecycles(false)
{
   m_lockLifecycleStats =
      std::unique_ptr<Mutex>(new PthreadsMutex("lockLifecycleStats"));
}

//******************************************************************************

StdLogger::~StdLogger()
{
}

//******************************************************************************

Logger::LogLevel StdLogger::getLogLevel() const noexcept
{
   return m_logLevel;
}

//******************************************************************************

void StdLogger::setLogLevel(LogLevel logLevel) noexcept
{
   m_logLevel = logLevel;
}

//******************************************************************************

void StdLogger::logMessage(LogLevel logLevel, const std::string& logMessage) noexcept
{
   if (isLogging(logLevel)) {
      std::printf("%s %s\n", logLevelPrefix(logLevel).c_str(), logMessage.c_str());
   }
}

//******************************************************************************

bool StdLogger::isLoggingLevel(LogLevel logLevel) const noexcept
{
   return (logLevel <= m_logLevel);
}

//******************************************************************************

const std::string& StdLogger::logLevelPrefix(LogLevel level) const noexcept
{
   switch (level) {
      case Logger::LogLevel::Critical:
         return prefixCritical;
      case Logger::LogLevel::Error:
         return prefixError;
      case Logger::LogLevel::Warning:
         return prefixWarning;
      case Logger::LogLevel::Info:
         return prefixInfo;
      case Logger::LogLevel::Verbose:
         return prefixVerbose;
      case Logger::LogLevel::Debug:
      default:
         return prefixDebug;
   }
}

//******************************************************************************

bool StdLogger::isLoggingInstanceLifecycles() const noexcept
{
   return m_isLoggingInstanceLifecycles;
}

//******************************************************************************

void StdLogger::setLogInstanceLifecycles(bool logInstanceLifecycles) noexcept
{
   m_isLoggingInstanceLifecycles = logInstanceLifecycles;
}

//******************************************************************************

void StdLogger::logInstanceCreate(const std::string& className) noexcept
{
   if (m_lockLifecycleStats) {
      MutexLock lock(*m_lockLifecycleStats);
      auto it = m_mapClassLifecycleStats.find(className);
      if (it != m_mapClassLifecycleStats.end()) {
         LifecycleStats& stats = it->second;
         ++stats.m_instancesCreated;
      } else {
         LifecycleStats stats;
         ++stats.m_instancesCreated;
         m_mapClassLifecycleStats[className] = stats;
      }
   }
}

//******************************************************************************

void StdLogger::logInstanceDestroy(const std::string& className) noexcept
{
   if (m_lockLifecycleStats) {
      MutexLock lock(*m_lockLifecycleStats);
      auto it = m_mapClassLifecycleStats.find(className);
      if (it != m_mapClassLifecycleStats.end()) {
         LifecycleStats& stats = it->second;
         ++stats.m_instancesDestroyed;
      } else {
         error("unable to find class lifecycle stats for destroy of " + className);
      }
   }
}

//******************************************************************************

void StdLogger::populateClassLifecycleStats(std::unordered_map<std::string, LifecycleStats>& mapClassLifecycleStats)
{
   MutexLock lock(*m_lockLifecycleStats);
   mapClassLifecycleStats = m_mapClassLifecycleStats;
}

//******************************************************************************
