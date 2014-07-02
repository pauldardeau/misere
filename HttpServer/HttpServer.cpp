// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <string>
#include <exception>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/time.h>

// http
#include "HttpServer.h"
#include "HTTP.h"
#include "HttpRequest.h"
#include "HttpHandler.h"
#include "RequestHandler.h"
#include "HttpSocketServiceHandler.h"

// sockets
#include "ServerSocket.h"
#include "Socket.h"

// utils
#include "BasicException.h"
#include "IniReader.h"
#include "KeyValuePairs.h"
#include "DynamicLibrary.h"
#include "StrUtils.h"
#include "Logger.h"
#include "SystemInfo.h"

// threading
#include "Mutex.h"
#include "Runnable.h"
#include "Thread.h"
#include "ThreadPoolDispatcher.h"
#include "ThreadingFactory.h"

// kernel events
#include "EpollServer.h"
#include "KqueueServer.h"

// built-in handlers
#include "EchoHandler.h"
#include "GMTDateTimeHandler.h"
#include "ServerDateTimeHandler.h"
#include "ServerObjectsDebugging.h"
#include "ServerStatsHandler.h"
#include "ServerStatusHandler.h"


static const std::string SERVER_NAME             = "Misere";
static const std::string SERVER_VERSION          = "0.1";

static const std::string ENV_VAR_CFG_PATH        = "MISERE_HOME";
static const std::string CFG_FILE_NAME           = "misere.ini";

static const std::string CFG_TRUE_SETTING_VALUES = "yes|true|1";

static const std::string EMPTY = "";
static const std::string SPACE = " ";
static const std::string EOL   = "\n";


// default settings
static const int CFG_DEFAULT_SEND_BUFFER_SIZE     = 8192;
static const int CFG_DEFAULT_RECEIVE_BUFFER_SIZE  = 8192;

static const int CFG_DEFAULT_PORT_NUMBER          = 9000;

static const int CFG_DEFAULT_THREAD_POOL_SIZE     = 4;


// configuration sections
static const std::string CFG_SECTION_SERVER                 = "server";
static const std::string CFG_SECTION_LOGGING                = "logging";
static const std::string CFG_SECTION_HANDLERS               = "handlers";

// logging config values
static const std::string CFG_LOGFILE_ACCESS                 = "access_log";
static const std::string CFG_LOGFILE_ERROR                  = "error_log";

// server config values
static const std::string CFG_SERVER_PORT                    = "port";
static const std::string CFG_SERVER_THREADING               = "threading";
static const std::string CFG_SERVER_THREAD_POOL_SIZE        = "thread_pool_size";
static const std::string CFG_SERVER_LOG_LEVEL               = "log_level";
static const std::string CFG_SERVER_SEND_BUFFER_SIZE        = "socket_send_buffer_size";
static const std::string CFG_SERVER_RECEIVE_BUFFER_SIZE     = "socket_receive_buffer_size";
static const std::string CFG_SERVER_ALLOW_BUILTIN_HANDLERS  = "allow_builtin_handlers";
static const std::string CFG_SERVER_STRING                  = "server_string";
static const std::string CFG_SERVER_SOCKETS                 = "sockets";

// socket options
static const std::string CFG_SOCKETS_SOCKET_SERVER          = "socket_server";
static const std::string CFG_SOCKETS_KERNEL_EVENTS          = "kernel_events";


// threading options
static const std::string CFG_THREADING_PTHREADS             = "pthreads";
static const std::string CFG_THREADING_CPP11                = "c++11";
static const std::string CFG_THREADING_GCD_LIBDISPATCH      = "gcd_libdispatch";
static const std::string CFG_THREADING_NONE                 = "none";

// logging level options
static const std::string CFG_LOGGING_CRITICAL               = "critical";
static const std::string CFG_LOGGING_ERROR                  = "error";
static const std::string CFG_LOGGING_WARNING                = "warning";
static const std::string CFG_LOGGING_INFO                   = "info";
static const std::string CFG_LOGGING_DEBUG                  = "debug";
static const std::string CFG_LOGGING_VERBOSE                = "verbose";


// module config values
static const std::string MODULE_DLL_NAME = "dll";
static const std::string APP_PREFIX = "app:";

static const auto APP_PREFIX_LEN = APP_PREFIX.length();

static const char* LOG_WEEKDAY_NAME[7] =
{
   "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char* LOG_MONTH_NAME[12] =
{
   "Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

typedef HttpHandler* (*PFN_CREATE_HANDLER)();


//******************************************************************************
//******************************************************************************

HttpServer::HttpServer() :
   m_kernelEventServer(nullptr),
   m_serverSocket(nullptr),
   m_threadPool(nullptr),
   m_threadingFactory(nullptr),
   m_isDone(false),
   m_isThreaded(true),
   m_isUsingKernelEventServer(false),
   m_isFullyInitialized(false),
   m_allowBuiltInHandlers(false),
   m_requireAllHandlersForStartup(false),
   m_threadPoolSize(CFG_DEFAULT_THREAD_POOL_SIZE),
   m_serverPort(0)
{
   Logger::logInstanceCreate("HttpServer");

   init(CFG_DEFAULT_PORT_NUMBER);
}

//******************************************************************************

HttpServer::HttpServer(int port) :
   m_kernelEventServer(nullptr),
   m_serverSocket(nullptr),
   m_threadPool(nullptr),
   m_threadingFactory(nullptr),
   m_isDone(false),
   m_isThreaded(true),
   m_isUsingKernelEventServer(false),
   m_isFullyInitialized(false),
   m_allowBuiltInHandlers(false),
   m_requireAllHandlersForStartup(false),
   m_threadPoolSize(CFG_DEFAULT_THREAD_POOL_SIZE),
   m_serverPort(0)
{
   Logger::logInstanceCreate("HttpServer");

   init(port);
}

//******************************************************************************

HttpServer::HttpServer(const std::string& configFilePath) :
   m_kernelEventServer(nullptr),
   m_serverSocket(nullptr),
   m_threadPool(nullptr),
   m_threadingFactory(nullptr),
   m_configFilePath(configFilePath),
   m_isDone(false),
   m_isThreaded(true),
   m_isUsingKernelEventServer(false),
   m_isFullyInitialized(false),
   m_allowBuiltInHandlers(false),
   m_requireAllHandlersForStartup(false),
   m_threadPoolSize(CFG_DEFAULT_THREAD_POOL_SIZE),
   m_serverPort(0)
{
   Logger::logInstanceCreate("HttpServer");

   init(CFG_DEFAULT_PORT_NUMBER);
}

//******************************************************************************

std::unique_ptr<SectionedConfigDataSource> HttpServer::getConfigDataSource() noexcept
{
   if (m_configFilePath.empty()) {
      const char* configFilePath = std::getenv(ENV_VAR_CFG_PATH.c_str());

      if (nullptr != configFilePath) {
         m_configFilePath = configFilePath;
         
         if (m_configFilePath[m_configFilePath.length()-1] != '/') {
            m_configFilePath += "/";
         }

         m_configFilePath += CFG_FILE_NAME;
      }
   }

   if (m_configFilePath.empty()) {
      m_configFilePath = CFG_FILE_NAME;
   }

   return std::unique_ptr<SectionedConfigDataSource>(new IniReader(m_configFilePath));
}

//******************************************************************************

int HttpServer::getSocketSendBufferSize() const noexcept
{
   return m_socketSendBufferSize;
}

//******************************************************************************

int HttpServer::getSocketReceiveBufferSize() const noexcept
{
   return m_socketReceiveBufferSize;
}

//******************************************************************************

const std::string& HttpServer::getServerId() const noexcept
{
   return m_serverString;
}

//******************************************************************************

bool HttpServer::hasTrueValue(const KeyValuePairs& kvp,
                              const std::string& setting) const noexcept
{
   bool hasTrueValue = false;
   
   if (kvp.hasKey(setting)) {
      const std::string& settingValue = kvp.getValue(setting);
      
      if (StrUtils::containsString(CFG_TRUE_SETTING_VALUES, settingValue)) {
         hasTrueValue = true;
      }
   }
   
   return hasTrueValue;
}

//******************************************************************************

int HttpServer::getIntValue(const KeyValuePairs& kvp,
                            const std::string& setting) const noexcept
{
   int value = -1;
   
   if (kvp.hasKey(setting)) {
      const std::string& valueAsString = kvp.getValue(setting);
      const int intValue = std::stoi(valueAsString);
      
      if (intValue > 0) {
         value = intValue;
      }
   }
   
   return value;
}

//******************************************************************************

void HttpServer::replaceVariables(const KeyValuePairs& kvp,
                                  std::string& s) const noexcept
{
   if (!s.empty()) {
      std::vector<std::string> keys;
      kvp.getKeys(keys);
      
      for (const std::string& key : keys) {
         if (StrUtils::containsString(s, key)) {
            StrUtils::replaceAll(s, key, kvp.getValue(key));
         }
      }
   }
}

//******************************************************************************

bool HttpServer::init(int port)
{
   const bool isLoggingDebug = Logger::isLogging(Logger::LogLevel::Debug);
   
	m_serverPort = port;
	
   std::unique_ptr<SectionedConfigDataSource> configDataSource =
      getConfigDataSource();
   
   if (!configDataSource) {
      // throw exception??
      return false;
   }

   // start out with our default settings
   m_socketSendBufferSize = CFG_DEFAULT_SEND_BUFFER_SIZE;
   m_socketReceiveBufferSize = CFG_DEFAULT_RECEIVE_BUFFER_SIZE;

   try {
      KeyValuePairs kvpServerSettings;
      KeyValuePairs kvpLoggingSettings;
      KeyValuePairs kvpHandlerSettings;


      // read and process "logging" section
      if (configDataSource->hasSection(CFG_SECTION_LOGGING) &&
          configDataSource->readSection(CFG_SECTION_LOGGING, kvpLoggingSettings)) {
         if (kvpLoggingSettings.hasKey(CFG_LOGFILE_ACCESS)) {
            const std::string& accessLog =
               kvpLoggingSettings.getValue(CFG_LOGFILE_ACCESS);
				m_accessLogFile = accessLog;
            Logger::info(std::string("access log=") + accessLog);
         }

         if (kvpLoggingSettings.hasKey(CFG_LOGFILE_ERROR)) {
            const std::string& errorLog =
               kvpLoggingSettings.getValue(CFG_LOGFILE_ERROR);
				m_errorLogFile = errorLog;
            Logger::info(std::string("error log=") + errorLog);
         }
      }


      // read and process "server" section
      if (configDataSource->hasSection(CFG_SECTION_SERVER) &&
          configDataSource->readSection(CFG_SECTION_SERVER, kvpServerSettings)) {
         
         if (kvpServerSettings.hasKey(CFG_SERVER_PORT)) {
            const int portNumber =
               getIntValue(kvpServerSettings, CFG_SERVER_PORT);

            if (portNumber > 0) {
               port = portNumber;
					m_serverPort = portNumber;
               
               if (isLoggingDebug) {
                  char msg[128];
                  std::snprintf(msg, 128, "port number=%d", port);
                  Logger::debug(std::string(msg));
               }
            }
         }

         // defaults
         m_isThreaded = true;
         m_threading = CFG_THREADING_PTHREADS;
         m_threadPoolSize = 4;
         
         if (kvpServerSettings.hasKey(CFG_SERVER_THREADING)) {
            const std::string& threading =
               kvpServerSettings.getValue(CFG_SERVER_THREADING);
            if (!threading.empty()) {
               if ((threading == CFG_THREADING_PTHREADS) ||
                   (threading == CFG_THREADING_CPP11) ||
                   (threading == CFG_THREADING_GCD_LIBDISPATCH)) {
                  m_threading = threading;
                  m_isThreaded = true;
               } else if (threading == CFG_THREADING_NONE) {
                  m_isThreaded = false;
               }
            }
         }
         
         if (kvpServerSettings.hasKey(CFG_SERVER_THREAD_POOL_SIZE)) {
            const int poolSize =
               getIntValue(kvpServerSettings, CFG_SERVER_THREAD_POOL_SIZE);

            if (poolSize > 0) {
               m_threadPoolSize = poolSize;
            }
         }
         
         // defaults
         m_sockets = CFG_SOCKETS_SOCKET_SERVER;
         
         if (kvpServerSettings.hasKey(CFG_SERVER_SOCKETS)) {
            const std::string& sockets = kvpServerSettings.getValue(CFG_SERVER_SOCKETS);
            if (sockets == CFG_SOCKETS_KERNEL_EVENTS) {
               m_isUsingKernelEventServer = true;
               m_sockets = CFG_SOCKETS_KERNEL_EVENTS;
            }
         }

         if (kvpServerSettings.hasKey(CFG_SERVER_LOG_LEVEL)) {
            m_logLevel =
               kvpServerSettings.getValue(CFG_SERVER_LOG_LEVEL);
            if (!m_logLevel.empty()) {
               StrUtils::toLowerCase(m_logLevel);
               Logger::info(std::string("log level: ") + m_logLevel);
               std::shared_ptr<Logger> logger = Logger::getLogger();
               
               if (logger != nullptr) {
                  if (m_logLevel == CFG_LOGGING_CRITICAL) {
                     logger->setLogLevel(Logger::LogLevel::Critical);
                  } else if (m_logLevel == CFG_LOGGING_ERROR) {
                     logger->setLogLevel(Logger::LogLevel::Error);
                  } else if (m_logLevel == CFG_LOGGING_WARNING) {
                     logger->setLogLevel(Logger::LogLevel::Warning);
                  } else if (m_logLevel == CFG_LOGGING_INFO) {
                     logger->setLogLevel(Logger::LogLevel::Info);
                  } else if (m_logLevel == CFG_LOGGING_DEBUG) {
                     logger->setLogLevel(Logger::LogLevel::Debug);
                  } else if (m_logLevel == CFG_LOGGING_VERBOSE) {
                     logger->setLogLevel(Logger::LogLevel::Verbose);
                  } else {
                     Logger::warning("unrecognized log level: '" + m_logLevel);
                  }
               }
            }
         }

         if (kvpServerSettings.hasKey(CFG_SERVER_SEND_BUFFER_SIZE)) {
            const int buffSize =
               getIntValue(kvpServerSettings, CFG_SERVER_SEND_BUFFER_SIZE);

            if (buffSize > 0) {
               m_socketSendBufferSize = buffSize;
            }
         }

         if (kvpServerSettings.hasKey(CFG_SERVER_RECEIVE_BUFFER_SIZE)) {
            const int buffSize =
               getIntValue(kvpServerSettings, CFG_SERVER_RECEIVE_BUFFER_SIZE);

            if (buffSize > 0) {
               m_socketReceiveBufferSize = buffSize;
            }
         }
         
         m_allowBuiltInHandlers = hasTrueValue(kvpServerSettings,
                                               CFG_SERVER_ALLOW_BUILTIN_HANDLERS);
         
         if (kvpServerSettings.hasKey(CFG_SERVER_STRING)) {
            const std::string& serverString =
               kvpServerSettings.getValue(CFG_SERVER_STRING);
            if (!serverString.empty()) {
               m_serverString = serverString;

               const auto posDollar = serverString.find("$");
               if (posDollar != std::string::npos) {
                  KeyValuePairs kvpVars;
                  kvpVars.addPair("$PRODUCT_NAME", SERVER_NAME);
                  kvpVars.addPair("$PRODUCT_VERSION", SERVER_VERSION);
                  kvpVars.addPair("$CFG_SOCKETS", m_sockets);
                  kvpVars.addPair("$CFG_THREADING", m_threading);
                  
                  const auto posDollarOS = serverString.find("$OS_");
                  
                  if (posDollarOS != std::string::npos) {
                     SystemInfo systemInfo;
                     if (systemInfo.retrievedSystemInfo()) {
                        kvpVars.addPair("$OS_SYSNAME", systemInfo.sysName());
                        kvpVars.addPair("$OS_NODENAME", systemInfo.nodeName());
                        kvpVars.addPair("$OS_RELEASE", systemInfo.release());
                        kvpVars.addPair("$OS_VERSION", systemInfo.version());
                        kvpVars.addPair("$OS_MACHINE", systemInfo.machine());
                     } else {
                        Logger::warning("unable to retrieve system information to populate server string");
                     }
                  }
                  
                  replaceVariables(kvpVars, m_serverString);
               }
               
               Logger::info("setting server string: '" + m_serverString + "'");
            }
         }
      }

		m_startupTime = getLocalDateTime();

      if (m_allowBuiltInHandlers) {
         Logger::debug("adding built-in handlers");
         addBuiltInHandlers();
      }
      
      if (isLoggingDebug) {
         Logger::debug("processing handlers");
      }

      // read and process "handlers" section
      KeyValuePairs kvpHandlers;
      if (configDataSource->hasSection(CFG_SECTION_HANDLERS) &&
          configDataSource->readSection(CFG_SECTION_HANDLERS, kvpHandlers)) {
         std::vector<std::string> vecKeys;
         kvpHandlers.getKeys(vecKeys);

         auto it = vecKeys.begin();
         const auto itEnd = vecKeys.end();

         for ( ; it != itEnd; ++it) {
            const std::string& path = (*it);
            const std::string& moduleSection = kvpHandlers.getValue(path);

            if (isLoggingDebug) {
               Logger::debug("path='" + path + "'");
            }
            
            if (moduleSection.empty()) {
               Logger::warning(std::string("nothing specified for path ") + path);
               Logger::warning("Not servicing this path");
               continue;
            }

            if (configDataSource->hasSection(moduleSection)) {
               KeyValuePairs kvpModule;
               if (configDataSource->readSection(moduleSection, kvpModule)) {
                  if (!kvpModule.hasKey(MODULE_DLL_NAME)) {
                     Logger::error(MODULE_DLL_NAME +
                                   std::string(" not specified for module ") +
                                   moduleSection);
                  }

                  const std::string& dllName = kvpModule.getValue(MODULE_DLL_NAME);
                  HttpHandler* pHandler = nullptr;
                  
                  if (isLoggingDebug) {
                     Logger::debug("trying to load dynamic library='" +
                                   dllName +
                                   "'");
                  }
                  
                  std::unique_ptr<DynamicLibrary> dll(new DynamicLibrary(dllName));

                  // load the dll
                  try {
                     void* pfn = dll->resolve("CreateHandler");
                     if (pfn == nullptr) {
                        dll->close();
                        Logger::error("unable to find module library entry point");
                     } else {
                        if (isLoggingDebug) {
                           Logger::debug("dynamic library loaded");
                        }
                     }

                     PFN_CREATE_HANDLER pfnCreateHandler = (PFN_CREATE_HANDLER) pfn;
                     pHandler = (*pfnCreateHandler)();
                  }
                  catch (const std::exception& e)
                  {
                     Logger::error(std::string("exception caught trying to load module library ") +
                                   dllName);
                     Logger::error(std::string(e.what()));
                  }
                  catch (...)
                  {
                     Logger::error(std::string("unable to load module library ") +
                                   dllName);
                  }

                  // continue loading application specific parameters for the module
                  std::vector<std::string> vecModuleKeys;
                  kvpModule.getKeys(vecModuleKeys);

                  auto itMod = vecModuleKeys.begin();
                  const auto itModEnd = vecModuleKeys.end();

                  KeyValuePairs kvpApp;

                  for ( ; itMod != itModEnd; ++itMod) {
                     const std::string& moduleKey = (*itMod);

                     // starts with app prefix?
                     if (StrUtils::startsWith(moduleKey, APP_PREFIX)) {
                        if (moduleKey.length() > APP_PREFIX_LEN) {
                           kvpApp.addPair(moduleKey.substr(APP_PREFIX_LEN),
                                          kvpModule.getValue(moduleKey));
                        }
                     }
                  }

                  if (isLoggingDebug) {
                     Logger::debug("initializing the handler");
                  }

                  // now initialize the servlet
                  if (pHandler->init(path, kvpApp)) {
                     if (isLoggingDebug) {
                        Logger::debug("initialization succeeded");
							}
                     
                     // register it
                     if (!addPathHandler(path, pHandler)) {
                        Logger::error(std::string("unable to register handler for path ") +
                                      path);
                        
                        if (m_requireAllHandlersForStartup) {
                           return false;
                        }
                     }
                  } else {
                     Logger::error(std::string("unable to initialize handler for path ") +
                                   path);
                     if (m_requireAllHandlersForStartup) {
                        return false;
                     }
                  }
               }
            } else {
               if (!moduleSection.empty()) {
                  Logger::error(std::string("no configuration for handler ") +
                                moduleSection);
               } else {
                  Logger::error(std::string("no configuration for handler ") +
                                path);
               }

               if (m_requireAllHandlersForStartup) {
                  return false;
               }
            }
         }

         // do we have any handlers?
         if (!m_allowBuiltInHandlers && m_mapPathHandlers.empty()) {
            Logger::critical("no handlers registered");
            return false;
         }
      }
   }
   catch (const BasicException& be)
   {
      Logger::critical("exception initializing server: " + be.whatString());
      return false;
   }
   catch (const std::exception& e)
   {
      Logger::critical("exception initializing server: " +
                       std::string(e.what()));
      return false;
   }
   catch (...)
   {
      Logger::critical("unknown exception initializing server");
      return false;
   }


   if (!m_isUsingKernelEventServer) {
      try {
         if (isLoggingDebug) {
            char msg[128];
            std::snprintf(msg, 128, "creating server socket on port=%d", port);
            Logger::debug(std::string(msg));
         }
      
         std::unique_ptr<ServerSocket> serverSocket(new ServerSocket(port));
         m_serverSocket = std::move(serverSocket);
      }
      catch (...)
      {
         std::string exception = "unable to open server socket port '";
         exception += std::to_string(port);
         exception += "'";
         throw BasicException(exception);
      }
   }


   std::string concurrencyModel = EMPTY;

   if (m_isThreaded) {
      ThreadingFactory::ThreadingPackage threadingPackage;
      bool isUsingLibDispatch = false;
      
      if (m_threading == CFG_THREADING_PTHREADS) {
         threadingPackage = ThreadingFactory::ThreadingPackage::PTHREADS;
      } else if (m_threading == CFG_THREADING_CPP11) {
         threadingPackage = ThreadingFactory::ThreadingPackage::CPP_11;
      } else if (m_threading == CFG_THREADING_GCD_LIBDISPATCH) {
         threadingPackage = ThreadingFactory::ThreadingPackage::GCD_LIBDISPATCH;
         isUsingLibDispatch = true;
      } else {
         threadingPackage = ThreadingFactory::ThreadingPackage::PTHREADS;
      }
      
      std::shared_ptr<ThreadingFactory>
         threadingFactory(new ThreadingFactory(threadingPackage));
      ThreadingFactory::setThreadingFactory(threadingFactory);
      m_threadingFactory = std::move(threadingFactory);
      
      if (m_isUsingKernelEventServer) {
         m_threadingFactory->setMutexType(ThreadingFactory::ThreadingPackage::PTHREADS);
      }
         
      m_threadPool = m_threadingFactory->createThreadPoolDispatcher(m_threadPoolSize);
         
      m_threadPool->start();

      concurrencyModel = "multithreaded - ";
      concurrencyModel += m_threading;
      
      if (!isUsingLibDispatch) {
         char numberThreads[128];
         std::snprintf(numberThreads, 128, " [%d threads]",
                       m_threadPoolSize);
         concurrencyModel += numberThreads;
      }
   } else {
      concurrencyModel = "serial";
		m_threadPoolSize = 1;   // not a pool, we have 1 processing thread
   }

	m_concurrencyModel = concurrencyModel;

   std::string portAsString = std::to_string(port);

   std::string startupMsg = SERVER_NAME;
   startupMsg += " ";
   startupMsg += SERVER_VERSION;
   startupMsg += " listening on port ";
   startupMsg += portAsString;
   startupMsg += " (request concurrency: ";
   startupMsg += concurrencyModel;
   startupMsg += ")";
   startupMsg += " (sockets: ";
   startupMsg += m_sockets;
   startupMsg += ")";

   std::printf("%s\n", startupMsg.c_str());

   m_isFullyInitialized = true;
   
   return true;
}

//******************************************************************************

HttpServer::~HttpServer() noexcept
{
   Logger::logInstanceDestroy("HttpServer");

   if (m_serverSocket) {
      m_serverSocket->close();
   }

   if (m_threadPool) {
      m_threadPool->stop();
   }

   m_mapPathHandlers.erase(m_mapPathHandlers.begin(), m_mapPathHandlers.end());
}

//******************************************************************************

std::string HttpServer::getSystemDateGMT() const noexcept
{
   time_t currentGMT;
   ::time(&currentGMT);
   
   struct tm* timeptr = ::gmtime(&currentGMT);
   char dateBuffer[128];
   
   std::snprintf(dateBuffer, 128, "%.3s, %02d %.3s %d %.2d:%.2d:%.2d GMT",
             LOG_WEEKDAY_NAME[timeptr->tm_wday],
             timeptr->tm_mday,
             LOG_MONTH_NAME[timeptr->tm_mon],
             1900 + timeptr->tm_year,
             timeptr->tm_hour,
             timeptr->tm_min,
             timeptr->tm_sec);
   
   return std::string(dateBuffer);
}

//******************************************************************************

std::string HttpServer::getLocalDateTime() const noexcept
{
   time_t currentTime;
   ::time(&currentTime);
   
   struct tm* timeptr = ::localtime(&currentTime);
   char dateBuffer[128];
   
   std::snprintf(dateBuffer, 128, "%d-%02d-%02d %.2d:%.2d:%.2d",
             1900 + timeptr->tm_year,
             timeptr->tm_mon + 1,
             timeptr->tm_mday,
             timeptr->tm_hour,
             timeptr->tm_min,
             timeptr->tm_sec);
   
   return std::string(dateBuffer);
}

//******************************************************************************

bool HttpServer::addPathHandler(const std::string& path,
                                HttpHandler* pHandler) noexcept
{
   bool isSuccess = false;

   if (!path.empty() && (nullptr != pHandler)) {
      std::unique_ptr<HttpHandler> handler(pHandler);
      m_mapPathHandlers[path] = std::move(handler);
      isSuccess = true;
   }

   return isSuccess;
}

//******************************************************************************

bool HttpServer::removePathHandler(const std::string& path) noexcept
{
   bool isSuccess = false;
   auto it = m_mapPathHandlers.find(path);
   
   if (it != m_mapPathHandlers.end()) {
      m_mapPathHandlers.erase(it);
      isSuccess = true;
   }

   return isSuccess;
}

//******************************************************************************

HttpHandler* HttpServer::getPathHandler(const std::string& path) noexcept
{
   auto it = m_mapPathHandlers.find(path);

   if (it != m_mapPathHandlers.end()) {
      return (*it).second.get();
   }

   return nullptr;
}

//******************************************************************************

std::string HttpServer::buildHeader(const std::string& responseCode,
                                    const std::unordered_map<std::string,
                                    std::string>& mapHeaders) const noexcept
{
   auto it = mapHeaders.begin();
   const auto itEnd = mapHeaders.end();

   std::string sb;

   if (!responseCode.empty()) {
      sb += HTTP::HTTP_PROTOCOL1_0;
      sb += SPACE;
      sb += responseCode;
      sb += EOL;
   }

   for ( ; it != itEnd; ++it) {
      sb += (*it).first;  // header key
      sb += SPACE;

      sb += (*it).second;  // header value
      sb += EOL;
   }

   sb += EOL;

   return sb;
}

//******************************************************************************

bool HttpServer::addBuiltInHandlers() noexcept
{
   return addPathHandler("/Echo", new EchoHandler()) &&
          addPathHandler("/GMTDateTime", new GMTDateTimeHandler()) &&
          addPathHandler("/ServerDateTime", new ServerDateTimeHandler()) &&
          addPathHandler("/ServerObjectsDebugging", new ServerObjectsDebugging()) &&
          addPathHandler("/ServerStats", new ServerStatsHandler(*this)) &&
          addPathHandler("/ServerStatus", new ServerStatusHandler());
}

//******************************************************************************

int HttpServer::platformPointerSizeBits() const noexcept
{
   return sizeof(void*) * 8;
}

//******************************************************************************

void HttpServer::serviceSocket(std::shared_ptr<SocketRequest> socketRequest)
{
   if (nullptr != m_threadPool) {
      // Hand off the request to the thread pool for asynchronous processing
      std::shared_ptr<RequestHandler> requestHandler(new RequestHandler(*this, socketRequest));
      requestHandler->setThreadPooling(true);
      m_threadPool->addRequest(requestHandler);
   } else {
      // no thread pool available -- process it synchronously
      RequestHandler requestHandler(*this,socketRequest);
      requestHandler.run();
   }
}

//******************************************************************************

int HttpServer::runSocketServer() noexcept
{
   int rc = 0;
   
   if (!m_serverSocket) {
      Logger::critical("runSocketServer called with null serverSocket");
      return 1;
   }
   
   while (!m_isDone) {
      
      std::shared_ptr<Socket> socket(m_serverSocket->accept());

      if (nullptr == socket) {
         continue;
      }

      if (Logger::isLogging(Logger::LogLevel::Debug)) {
         Logger::debug("*****************************************");
         Logger::debug("client connected");
      }

      try {
         
         if (m_isThreaded && (nullptr != m_threadPool)) {
            std::shared_ptr<RequestHandler> handler(new RequestHandler(*this, socket));

            handler->setThreadPooling(true);

            // give it to the thread pool
            m_threadPool->addRequest(handler);
         } else {
            RequestHandler handler(*this, socket);
            handler.run();
         }
      }
      catch (const BasicException& be)
      {
         rc = 1;
         Logger::error("HttpServer runServer exception caught: " +
                       be.whatString());
      }
      catch (const std::exception& e)
      {
         rc = 1;
         Logger::error(std::string("HttpServer runServer exception caught: ") +
                       std::string(e.what()));
      }
      catch (...)
      {
         rc = 1;
         Logger::error("HttpServer runServer unknown exception caught");
      }
   }
   
   return rc;
}

//******************************************************************************

int HttpServer::runKernelEventServer() noexcept
{
   const int MAX_CON = 1200;
   
   int rc = 0;
   
   if (m_threadingFactory != nullptr) {
      std::shared_ptr<Mutex> mutexFD(m_threadingFactory->createMutex("fdMutex"));
      std::shared_ptr<Mutex> mutexHWMConnections(m_threadingFactory->createMutex("hwmConnectionsMutex"));
      std::unique_ptr<KernelEventServer> kernelEventServer = nullptr;
      
      if (KqueueServer::isSupportedPlatform()) {
         kernelEventServer =
            std::unique_ptr<KernelEventServer>(new KqueueServer(*mutexFD, *mutexHWMConnections));
      } else if (EpollServer::isSupportedPlatform()) {
         kernelEventServer =
            std::unique_ptr<KernelEventServer>(new EpollServer(*mutexFD, *mutexHWMConnections));
      } else {
         Logger::critical("no kernel event server available for platform");
         rc = 1;
      }
      
      if (kernelEventServer != nullptr) {
         try {
            std::shared_ptr<SocketServiceHandler> serviceHandler(new HttpSocketServiceHandler(*this));

            if (kernelEventServer->init(serviceHandler, m_serverPort, MAX_CON))
            {
               kernelEventServer->run();
            } else {
               rc = 1;
            }
         } catch (const BasicException& be) {
            Logger::critical("exception running kernel event server: " +
                             be.whatString());
         } catch (const std::exception& e) {
            Logger::critical("exception running kernel event server: " +
                             std::string(e.what()));
         } catch (...) {
            Logger::critical("unidentified exception running kernel event server");
         }
      }
   } else {
      Logger::critical("no threading factory configured");
      rc = 1;
   }
   
   return rc;
}

//******************************************************************************

int HttpServer::run() noexcept
{
   if (!m_isFullyInitialized) {
      Logger::critical("server not initialized");
      return 1;
   } else {
      if (m_isUsingKernelEventServer) {
         return runKernelEventServer();
      } else {
         return runSocketServer();
      }
   }
}

//******************************************************************************

void HttpServer::logRequest(const std::string& clientIPAddress,
                            const std::string& requestLine,
                            const std::string& responseCode) noexcept
{
   logRequest(clientIPAddress,
              requestLine,
              responseCode,
              EMPTY);
}

//******************************************************************************

void HttpServer::logRequest(const std::string& clientIPAddress,
                            const std::string& requestLine,
                            const std::string& responseCode,
                            const std::string& workerThreadId) noexcept
{
   const std::string localDateTime = getLocalDateTime();

   if (!workerThreadId.empty()) {
      std::printf("[%s] [thread=%s] %s, %s, %s\n", localDateTime.c_str(),
                                              workerThreadId.c_str(),
                                              clientIPAddress.c_str(),
                                              requestLine.c_str(),
                                              responseCode.c_str());
   } else {
      std::printf("[%s] %s, %s, %s\n", localDateTime.c_str(),
                                  clientIPAddress.c_str(),
                                  requestLine.c_str(),
                                  responseCode.c_str());
   }
}

//******************************************************************************

