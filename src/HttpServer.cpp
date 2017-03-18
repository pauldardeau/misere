// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <string>
#include <exception>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// http
#include "HttpServer.h"
#include "HTTP.h"
#include "HttpRequest.h"
#include "HttpHandler.h"
#include "HttpRequestHandler.h"
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
#include "PthreadsThreadingFactory.h"

// kernel events
#include "KernelEventServer.h"
#include "EpollServer.h"
#include "KqueueServer.h"

// built-in handlers
#include "EchoHandler.h"
#include "GMTDateTimeHandler.h"
#include "ServerDateTimeHandler.h"
#include "ServerObjectsDebugging.h"
#include "ServerStatsHandler.h"
#include "ServerStatusHandler.h"


using namespace std;


static const std::string SERVER_NAME             = "Misere";
static const std::string SERVER_VERSION          = "0.1";
static const std::string CFG_TRUE_SETTING_VALUES = "yes|true|1";

static const std::string EMPTY = "";
static const std::string SPACE = " ";
static const std::string EOL   = "\n";
static const std::string COLON = ":";

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

// mime types
static const std::string MIME_APPLICATION_JSON  = "application/json";
static const std::string MIME_APPLICATION_XML   = "application/xml";
static const std::string MIME_TEXT_HTML         = "text/html";
static const std::string MIME_TEXT_PLAIN        = "text/plain";

// module config values
static const std::string MODULE_DLL_NAME = "dll";
static const std::string APP_PREFIX = "app:";

static const std::size_t APP_PREFIX_LEN = APP_PREFIX.length();

static const char* LOG_WEEKDAY_NAME[7] = {
   "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char* LOG_MONTH_NAME[12] = {
   "Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

using namespace misere;
using namespace chaudiere;

typedef HttpHandler* (*PFN_CREATE_HANDLER)();


//******************************************************************************
//******************************************************************************

HttpServer::HttpServer(const std::string& configFilePath) :
   m_serverSocket(NULL),
   m_threadPool(NULL),
   m_threadingFactory(NULL),
   m_configFilePath(configFilePath),
   m_isDone(false),
   m_isThreaded(true),
   m_isUsingKernelEventServer(false),
   m_isFullyInitialized(false),
   m_allowBuiltInHandlers(false),
   m_requireAllHandlersForStartup(false),
   m_compressionEnabled(true),
   m_threadPoolSize(CFG_DEFAULT_THREAD_POOL_SIZE),
   m_serverPort(CFG_DEFAULT_PORT_NUMBER),
   m_minimumCompressionSize(1000) {

   Logger::logInstanceCreate("HttpServer");
   init(CFG_DEFAULT_PORT_NUMBER);
}

//******************************************************************************

SectionedConfigDataSource* HttpServer::getConfigDataSource() {
   return new IniReader(m_configFilePath);
}

//******************************************************************************

int HttpServer::getSocketSendBufferSize() const {
   return m_socketSendBufferSize;
}

//******************************************************************************

int HttpServer::getSocketReceiveBufferSize() const {
   return m_socketReceiveBufferSize;
}

//******************************************************************************

const std::string& HttpServer::getServerId() const {
   return m_serverString;
}

//******************************************************************************

bool HttpServer::hasTrueValue(const KeyValuePairs& kvp,
                              const std::string& setting) const {
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
                            const std::string& setting) const {
   int value = -1;
   
   if (kvp.hasKey(setting)) {
      const std::string& valueAsString = kvp.getValue(setting);
      const int intValue = atoi(valueAsString.c_str());
      
      if (intValue > 0) {
         value = intValue;
      }
   }
   
   return value;
}

//******************************************************************************

void HttpServer::replaceVariables(const KeyValuePairs& kvp,
                                  std::string& s) const {
   if (!s.empty()) {
      vector<string> keys;
      kvp.getKeys(keys);
      vector<string>::const_iterator it = keys.begin();
      const vector<string>::const_iterator itEnd = keys.end();
      
      for (; it != itEnd; it++) {
         const string& key = *it;
         if (StrUtils::containsString(s, key)) {
            StrUtils::replaceAll(s, key, kvp.getValue(key));
         }
      }
   }
}

//******************************************************************************

bool HttpServer::init(int port) {
   const bool isLoggingDebug = Logger::isLogging(Debug);
   m_serverPort = port;
	
   SectionedConfigDataSource* configDataSource = NULL;
   bool haveDataSource = false;
   
   try {
      configDataSource = getConfigDataSource();
      haveDataSource = true;
   } catch (const BasicException& be) {
      Logger::error("exception retrieving config data: " + be.whatString());
   } catch (const std::exception& e) {
      Logger::error("exception retrieving config data: " + std::string(e.what()));
   } catch (...) {
      Logger::error("exception retrieving config data");
   }
   
   if (!configDataSource || !haveDataSource) {
      Logger::error("unable to retrieve config data");
      if (configDataSource != NULL) {
         delete configDataSource;
      }
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
          configDataSource->readSection(CFG_SECTION_LOGGING,
                                        kvpLoggingSettings)) {
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
          configDataSource->readSection(CFG_SECTION_SERVER,
                                        kvpServerSettings)) {
         
         if (kvpServerSettings.hasKey(CFG_SERVER_PORT)) {
            const int portNumber =
               getIntValue(kvpServerSettings, CFG_SERVER_PORT);

            if (portNumber > 0) {
               port = portNumber;
               m_serverPort = portNumber;
               
               if (isLoggingDebug) {
                  char msg[128];
                  ::snprintf(msg, 128, "port number=%d", port);
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
            const std::string& sockets =
               kvpServerSettings.getValue(CFG_SERVER_SOCKETS);
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
               Logger* logger = Logger::getLogger();
               
               if (logger != NULL) {
                  if (m_logLevel == CFG_LOGGING_CRITICAL) {
                     logger->setLogLevel(Critical);
                  } else if (m_logLevel == CFG_LOGGING_ERROR) {
                     logger->setLogLevel(Error);
                  } else if (m_logLevel == CFG_LOGGING_WARNING) {
                     logger->setLogLevel(Warning);
                  } else if (m_logLevel == CFG_LOGGING_INFO) {
                     logger->setLogLevel(Info);
                  } else if (m_logLevel == CFG_LOGGING_DEBUG) {
                     logger->setLogLevel(Debug);
                  } else if (m_logLevel == CFG_LOGGING_VERBOSE) {
                     logger->setLogLevel(Verbose);
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

               const std::string::size_type posDollar =
                  serverString.find("$");
               if (posDollar != std::string::npos) {
                  KeyValuePairs kvpVars;
                  kvpVars.addPair("$PRODUCT_NAME", SERVER_NAME);
                  kvpVars.addPair("$PRODUCT_VERSION", SERVER_VERSION);
                  kvpVars.addPair("$CFG_SOCKETS", m_sockets);
                  kvpVars.addPair("$CFG_THREADING", m_threading);
                  
                  const std::string::size_type posDollarOS =
                     serverString.find("$OS_");
                  
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

         std::vector<std::string>::const_iterator it = vecKeys.begin();
         const std::vector<std::string>::const_iterator itEnd = vecKeys.end();

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
                  HttpHandler* pHandler = NULL;
                  
                  if (isLoggingDebug) {
                     Logger::debug("trying to load dynamic library='" +
                                   dllName +
                                   "'");
                  }
                  
                  DynamicLibrary* dll = new DynamicLibrary(dllName);

                  // load the dll
                  try {
                     void* pfn = dll->resolve("CreateHandler");
                     if (pfn == NULL) {
                        Logger::error("unable to find module library entry point");
                     } else {
                        if (isLoggingDebug) {
                           Logger::debug("dynamic library loaded");
                        }
                     }

                     PFN_CREATE_HANDLER pfnCreateHandler = (PFN_CREATE_HANDLER) pfn;
                     pHandler = (*pfnCreateHandler)();
                  } catch (const std::exception& e) {
                     Logger::error(std::string("exception caught trying to load module library ") +
                                   dllName);
                     Logger::error(std::string(e.what()));
                  } catch (...) {
                     Logger::error(std::string("unable to load module library ") +
                                   dllName);
                  }

                  // continue loading application specific parameters for the module
                  std::vector<std::string> vecModuleKeys;
                  kvpModule.getKeys(vecModuleKeys);

                  std::vector<std::string>::const_iterator itMod =
                     vecModuleKeys.begin();
                  const std::vector<std::string>::const_iterator itModEnd =
                     vecModuleKeys.end();

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
                     } else {
                        m_mapPathLibraries[path] = dll;
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
   } catch (const BasicException& be) {
      Logger::critical("exception initializing server: " + be.whatString());
      return false;
   } catch (const std::exception& e) {
      Logger::critical("exception initializing server: " +
                       std::string(e.what()));
      return false;
   } catch (...) {
      Logger::critical("unknown exception initializing server");
      return false;
   }

   if (!m_isUsingKernelEventServer) {
      try {
         if (isLoggingDebug) {
            char msg[128];
            ::snprintf(msg, 128,
                       "creating server socket on port=%d",
                       port);
            Logger::debug(std::string(msg));
         }
      
         m_serverSocket = new ServerSocket(port);
      } catch (...) {
         std::string exception = "unable to open server socket port '";
         exception += StrUtils::toString(port);
         exception += "'";
         Logger::critical(exception);
         return false;
      }
   }

   std::string concurrencyModel = EMPTY;

   if (m_isThreaded) {
      bool isUsingLibDispatch = false;
      m_threadingFactory = new PthreadsThreadingFactory;
      ThreadingFactory::setThreadingFactory(m_threadingFactory);
      m_threadPool =
         m_threadingFactory->createThreadPoolDispatcher(m_threadPoolSize,
                                                        "thread_pool");
         
      m_threadPool->start();

      concurrencyModel = "multithreaded - ";
      concurrencyModel += m_threading;
      
      if (!isUsingLibDispatch) {
         char numberThreads[128];
         ::snprintf(numberThreads, 128, " [%d threads]",
                    m_threadPoolSize);
         concurrencyModel += numberThreads;
      }
   } else {
      concurrencyModel = "serial";
      m_threadPoolSize = 1;   // not a pool, we have 1 processing thread
   }

   m_concurrencyModel = concurrencyModel;

   std::string startupMsg = SERVER_NAME;
   startupMsg += " ";
   startupMsg += SERVER_VERSION;
   startupMsg += " listening on port ";
   startupMsg += StrUtils::toString(port);
   startupMsg += " (request concurrency: ";
   startupMsg += concurrencyModel;
   startupMsg += ")";
   startupMsg += " (sockets: ";
   startupMsg += m_sockets;
   startupMsg += ")";

   ::printf("%s\n", startupMsg.c_str());

   m_isFullyInitialized = true;
   
   return true;
}

//******************************************************************************

HttpServer::~HttpServer() {
   Logger::logInstanceDestroy("HttpServer");

   if (m_serverSocket) {
      m_serverSocket->close();
      delete m_serverSocket;
   }

   if (m_threadPool) {
      m_threadPool->stop();
      delete m_threadPool;
   }

   if (m_threadingFactory != NULL) {
      delete m_threadingFactory;
   }

   map<string,HttpHandler*>::iterator it =
      m_mapPathHandlers.begin();
   const map<string,HttpHandler*>::const_iterator itEnd =
      m_mapPathHandlers.end();
   for (; it != itEnd; it++) {
      delete it->second;
   }

   m_mapPathHandlers.erase(m_mapPathHandlers.begin(),
                           m_mapPathHandlers.end());
}

//******************************************************************************

std::string HttpServer::getSystemDateGMT() const {
   time_t currentGMT;
   ::time(&currentGMT);
   
   struct tm* timeptr = ::gmtime(&currentGMT);
   char dateBuffer[128];
   
   ::snprintf(dateBuffer, 128,
              "%.3s, %02d %.3s %d %.2d:%.2d:%.2d GMT",
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

std::string HttpServer::getLocalDateTime() const {
   time_t currentTime;
   ::time(&currentTime);
   
   struct tm* timeptr = ::localtime(&currentTime);
   char dateBuffer[128];
   
   ::snprintf(dateBuffer, 128,
              "%d-%02d-%02d %.2d:%.2d:%.2d",
              1900 + timeptr->tm_year,
              timeptr->tm_mon + 1,
              timeptr->tm_mday,
              timeptr->tm_hour,
              timeptr->tm_min,
              timeptr->tm_sec);
   
   return std::string(dateBuffer);
}

//******************************************************************************

bool HttpServer::compressResponse(const std::string& mimeType) const {
   //TODO: make this configurable through config file
   return (mimeType == MIME_TEXT_HTML) ||
          (mimeType == MIME_TEXT_PLAIN) ||
          (mimeType == MIME_APPLICATION_JSON) ||
          (mimeType == MIME_APPLICATION_XML);
}

//******************************************************************************

bool HttpServer::compressionEnabled() const {
   return m_compressionEnabled;
}

//******************************************************************************

int HttpServer::minimumCompressionSize() const {
   return m_minimumCompressionSize;
}

//******************************************************************************

bool HttpServer::addPathHandler(const std::string& path,
                                HttpHandler* pHandler) {
   bool isSuccess = false;

   if (!path.empty() && (NULL != pHandler)) {
      m_mapPathHandlers[path] = pHandler;
      isSuccess = true;
   }

   return isSuccess;
}

//******************************************************************************

bool HttpServer::removePathHandler(const std::string& path) {
   bool isSuccess = false;
   map<string,HttpHandler*>::iterator it =
      m_mapPathHandlers.find(path);
   
   if (it != m_mapPathHandlers.end()) {
      delete (*it).second;
      m_mapPathHandlers.erase(it);
      isSuccess = true;
   }

   return isSuccess;
}

//******************************************************************************

HttpHandler* HttpServer::getPathHandler(const std::string& path) {
   map<string,HttpHandler*>::iterator it = m_mapPathHandlers.find(path);
   if (it != m_mapPathHandlers.end()) {
      return (*it).second;
   }

   return NULL;
}

//******************************************************************************

std::string HttpServer::buildHeader(const std::string& responseCode,
                                    const chaudiere::KeyValuePairs& headers) const {
   string sb;
   if (!responseCode.empty()) {
      sb += HTTP::HTTP_PROTOCOL1_1;
      sb += SPACE;
      sb += responseCode;
      sb += EOL;
   }

   vector<string> keys;
   headers.getKeys(keys);
   const vector<string>::const_iterator itEnd = keys.end();
   vector<string>::const_iterator it = keys.begin();

   for ( ; it != itEnd; ++it) {
      const string& headerKey = *it;
      sb += headerKey;  // header key
      
      if (!StrUtils::endsWith(headerKey, COLON)) {
         sb += COLON;
      }
      
      sb += SPACE;

      sb += headers.getValue(headerKey);  // header value
      sb += EOL;
   }

   sb += EOL;

   return sb;
}

//******************************************************************************

bool HttpServer::addBuiltInHandlers() {
   return addPathHandler("/Echo", new EchoHandler()) &&
          addPathHandler("/GMTDateTime", new GMTDateTimeHandler()) &&
          addPathHandler("/ServerDateTime", new ServerDateTimeHandler()) &&
          addPathHandler("/ServerObjectsDebugging", new ServerObjectsDebugging()) &&
          addPathHandler("/ServerStats", new ServerStatsHandler()) &&
          addPathHandler("/ServerStatus", new ServerStatusHandler());
}

//******************************************************************************

int HttpServer::platformPointerSizeBits() const {
   return sizeof(void*) * 8;
}

//******************************************************************************

void HttpServer::serviceSocket(SocketRequest* socketRequest) {
   if (NULL != m_threadPool) {
      // Hand off the request to the thread pool for asynchronous processing
      HttpRequestHandler* requestHandler =
         new HttpRequestHandler(*this, socketRequest);
      requestHandler->setThreadPooling(true);
      m_threadPool->addRequest(requestHandler);
   } else {
      // no thread pool available -- process it synchronously
      HttpRequestHandler requestHandler(*this,socketRequest);
      requestHandler.run();
   }
}

//******************************************************************************

int HttpServer::runSocketServer() {
   int rc = 0;
   
   if (!m_serverSocket) {
      Logger::critical("runSocketServer called with null serverSocket");
      return 1;
   }
   
   while (!m_isDone) {
      Socket* socket = m_serverSocket->accept();

      if (NULL == socket) {
         continue;
      }

      if (Logger::isLogging(Debug)) {
         Logger::debug("*****************************************");
         Logger::debug("client connected");
      }

      try {
         if (m_isThreaded && (NULL != m_threadPool)) {
            HttpRequestHandler* handler =
               new HttpRequestHandler(*this, socket);
            handler->setThreadPooling(true);

            // give it to the thread pool
            m_threadPool->addRequest(handler);
         } else {
            HttpRequestHandler handler(*this, socket);
            handler.run();
         }
      } catch (const BasicException& be) {
         rc = 1;
         Logger::error("HttpServer runServer exception caught: " +
                       be.whatString());
      } catch (const std::exception& e) {
         rc = 1;
         Logger::error(std::string("HttpServer runServer exception caught: ") +
                       std::string(e.what()));
      } catch (...) {
         rc = 1;
         Logger::error("HttpServer runServer unknown exception caught");
      }
   }
   
   return rc;
}

//******************************************************************************

int HttpServer::runKernelEventServer() {
   const int MAX_CON = 1200;
   int rc = 0;
   
   if (m_threadingFactory != NULL) {
      Mutex* mutexFD = m_threadingFactory->createMutex("fdMutex");
      Mutex* mutexHWMConnections =
         m_threadingFactory->createMutex("hwmConnectionsMutex");
      KernelEventServer* kernelEventServer = NULL;
      
      if (KqueueServer::isSupportedPlatform()) {
         kernelEventServer =
            new KqueueServer(*mutexFD, *mutexHWMConnections);
      } else if (EpollServer::isSupportedPlatform()) {
         kernelEventServer =
            new EpollServer(*mutexFD, *mutexHWMConnections);
      } else {
         Logger::critical("no kernel event server available for platform");
         rc = 1;
      }
      
      if (kernelEventServer != NULL) {
         try {
            SocketServiceHandler* serviceHandler =
               new HttpSocketServiceHandler(*this);

            if (kernelEventServer->init(serviceHandler,
                                        m_serverPort,
                                        MAX_CON)) {
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
         delete kernelEventServer;
      }
   } else {
      Logger::critical("no threading factory configured");
      rc = 1;
   }
   
   return rc;
}

//******************************************************************************

int HttpServer::run() {
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
                            const std::string& responseCode) {
   logRequest(clientIPAddress,
              requestLine,
              responseCode,
              EMPTY);
}

//******************************************************************************

void HttpServer::logRequest(const std::string& clientIPAddress,
                            const std::string& requestLine,
                            const std::string& responseCode,
                            const std::string& workerThreadId) {
   const std::string localDateTime = getLocalDateTime();

   if (!workerThreadId.empty()) {
      ::printf("[%s] [thread=%s] %s, %s, %s\n",
               localDateTime.c_str(),
               workerThreadId.c_str(),
               clientIPAddress.c_str(),
               requestLine.c_str(),
               responseCode.c_str());
   } else {
      ::printf("[%s] %s, %s, %s\n",
               localDateTime.c_str(),
               clientIPAddress.c_str(),
               requestLine.c_str(),
               responseCode.c_str());
   }
}

//******************************************************************************

