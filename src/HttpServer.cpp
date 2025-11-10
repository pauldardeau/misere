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
#include "AutoPointer.h"

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


static const string SERVER_NAME             = "Misere";
static const string SERVER_VERSION          = "0.1";
static const string CFG_TRUE_SETTING_VALUES = "yes|true|1";

static const string EMPTY = "";
static const string SPACE = " ";
static const string EOL   = "\n";
static const string COLON = ":";

// default settings
static const int CFG_DEFAULT_SEND_BUFFER_SIZE     = 8192;
static const int CFG_DEFAULT_RECEIVE_BUFFER_SIZE  = 8192;
static const int CFG_DEFAULT_PORT_NUMBER          = 9000;
static const int CFG_DEFAULT_THREAD_POOL_SIZE     = 4;

// configuration sections
static const string CFG_SECTION_SERVER                 = "server";
static const string CFG_SECTION_LOGGING                = "logging";
static const string CFG_SECTION_HANDLERS               = "handlers";

// logging config values
static const string CFG_LOGFILE_ACCESS                 = "access_log";
static const string CFG_LOGFILE_ERROR                  = "error_log";

// server config values
static const string CFG_SERVER_PORT                    = "port";
static const string CFG_SERVER_THREADING               = "threading";
static const string CFG_SERVER_THREAD_POOL_SIZE        = "thread_pool_size";
static const string CFG_SERVER_LOG_LEVEL               = "log_level";
static const string CFG_SERVER_SEND_BUFFER_SIZE        = "socket_send_buffer_size";
static const string CFG_SERVER_RECEIVE_BUFFER_SIZE     = "socket_receive_buffer_size";
static const string CFG_SERVER_ALLOW_BUILTIN_HANDLERS  = "allow_builtin_handlers";
static const string CFG_SERVER_STRING                  = "server_string";
static const string CFG_SERVER_SOCKETS                 = "sockets";

// socket options
static const string CFG_SOCKETS_SOCKET_SERVER          = "socket_server";
static const string CFG_SOCKETS_KERNEL_EVENTS          = "kernel_events";

// threading options
static const string CFG_THREADING_PTHREADS             = "pthreads";
static const string CFG_THREADING_CPP11                = "c++11";
static const string CFG_THREADING_GCD_LIBDISPATCH      = "gcd_libdispatch";
static const string CFG_THREADING_NONE                 = "none";

// logging level options
static const string CFG_LOGGING_CRITICAL               = "critical";
static const string CFG_LOGGING_ERROR                  = "error";
static const string CFG_LOGGING_WARNING                = "warning";
static const string CFG_LOGGING_INFO                   = "info";
static const string CFG_LOGGING_DEBUG                  = "debug";
static const string CFG_LOGGING_VERBOSE                = "verbose";

// mime types
static const string MIME_APPLICATION_JSON  = "application/json";
static const string MIME_APPLICATION_XML   = "application/xml";
static const string MIME_TEXT_HTML         = "text/html";
static const string MIME_TEXT_PLAIN        = "text/plain";

// module config values
static const string MODULE_DLL_NAME = "dll";
static const string APP_PREFIX = "app:";

static const size_t APP_PREFIX_LEN = APP_PREFIX.length();

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
   m_compressionEnabled(true),
   m_usingConfigFile(false),
   m_threadPoolSize(CFG_DEFAULT_THREAD_POOL_SIZE),
   m_serverPort(CFG_DEFAULT_PORT_NUMBER),
   m_socketSendBufferSize(CFG_DEFAULT_SEND_BUFFER_SIZE),
   m_socketReceiveBufferSize(CFG_DEFAULT_RECEIVE_BUFFER_SIZE),
   m_minimumCompressionSize(1000) {
   LOG_INSTANCE_CREATE("HttpServer")
   init(CFG_DEFAULT_PORT_NUMBER);
}

//******************************************************************************

HttpServer::HttpServer(int port) :
   m_serverSocket(nullptr),
   m_threadPool(nullptr),
   m_threadingFactory(nullptr),
   m_configFilePath(""),
   m_isDone(false),
   m_isThreaded(true),
   m_isUsingKernelEventServer(false),
   m_isFullyInitialized(false),
   m_allowBuiltInHandlers(false),
   m_requireAllHandlersForStartup(false),
   m_compressionEnabled(false),
   m_usingConfigFile(false),
   m_threadPoolSize(CFG_DEFAULT_THREAD_POOL_SIZE),
   m_serverPort(CFG_DEFAULT_PORT_NUMBER),
   m_socketSendBufferSize(CFG_DEFAULT_SEND_BUFFER_SIZE),
   m_socketReceiveBufferSize(CFG_DEFAULT_RECEIVE_BUFFER_SIZE),
   m_minimumCompressionSize(1000) {
   LOG_INSTANCE_CREATE("HttpServer")
   init(port);
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
      const string& settingValue = kvp.getValue(setting);
      
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
      const string& valueAsString = StrUtils::strip(kvp.getValue(setting));
      if (!valueAsString.empty()) {
         const int intValue = StrUtils::parseInt(valueAsString);
         if (intValue > 0) {
            value = intValue;
         }
      } else {
         LOG_WARNING(string("getIntValue has empty string for '") +
                     setting +
                     "'")
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
      
      for (const auto& key : keys) {
         if (StrUtils::containsString(s, key)) {
            StrUtils::replaceAll(s, key, kvp.getValue(key));
         }
      }
   }
}

//******************************************************************************

void HttpServer::outputStartupMessage() {
   string startupMsg = SERVER_NAME;
   startupMsg += " ";
   startupMsg += SERVER_VERSION;
   startupMsg += " listening on port ";
   startupMsg += StrUtils::toString(m_serverPort);
   startupMsg += " (request concurrency: ";
   startupMsg += m_concurrencyModel;
   startupMsg += ")";
   startupMsg += " (sockets: ";
   startupMsg += m_sockets;
   startupMsg += ")";
   ::printf("%s\n", startupMsg.c_str());
}

//******************************************************************************

bool HttpServer::init(int port) {
   m_serverPort = port;

   if (m_usingConfigFile) {
      AutoPointer<SectionedConfigDataSource*> configDataSource(nullptr);
      bool haveDataSource = false;
   
      try {
         configDataSource.assign(getConfigDataSource());
         haveDataSource = true;
      } catch (const BasicException& be) {
         LOG_ERROR("BasicException retrieving config data: " + be.whatString())
      } catch (const exception& e) {
         LOG_ERROR("exception retrieving config data: " + string(e.what()))
      } catch (...) {
         LOG_ERROR("unknown exception retrieving config data")
      }
   
      if (!configDataSource.haveObject() || !haveDataSource) {
         LOG_ERROR("unable to retrieve config data")
         return false;
      }

      try {
         KeyValuePairs kvpServerSettings;

         // read and process "logging" section
         setupLogFiles(*configDataSource.m_object);

         // read and process "server" section
         if (configDataSource->hasSection(CFG_SECTION_SERVER) &&
             configDataSource->readSection(CFG_SECTION_SERVER,
                                           kvpServerSettings)) {
            setupListeningPort(kvpServerSettings); 
            setupThreading(kvpServerSettings);
            setupSocketHandling(kvpServerSettings);
            setupLogLevel(kvpServerSettings);
            setupSocketBufferSizes(kvpServerSettings);
            m_allowBuiltInHandlers =
               hasTrueValue(kvpServerSettings,
                            CFG_SERVER_ALLOW_BUILTIN_HANDLERS);
            setupServerString(kvpServerSettings);
         } else {
            LOG_WARNING("HttpServer init no server section found")
         }

         // read and process "handlers" section
         if (!setupHandlers(configDataSource.m_object)) {
            return false;
         }
      } catch (const BasicException& be) {
         LOG_CRITICAL("BasicException initializing server: " + be.whatString())
         return false;
      } catch (const exception& e) {
         LOG_CRITICAL("exception initializing server: " +
                      string(e.what()))
         return false;
      } catch (...) {
         LOG_CRITICAL("unknown exception initializing server")
         return false;
      }
   }

   if (!setupServerSocket()) {
      return false;
   }

   setupConcurrency();
   m_startupTime = getLocalDateTime();
   m_isFullyInitialized = true;
   outputStartupMessage();

   return true;
}

//******************************************************************************

HttpServer::~HttpServer() {
   LOG_INSTANCE_DESTROY("HttpServer")

   if (m_serverSocket) {
      m_serverSocket->close();
   }

   if (m_threadPool) {
      m_threadPool->stop();
   }

   for (auto& pair : m_mapPathHandlers) {
      delete pair.second;
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
   
   return string(dateBuffer);
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
   
   return string(dateBuffer);
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

   if (!path.empty() && (nullptr != pHandler)) {
      m_mapPathHandlers[path] = pHandler;
      isSuccess = true;
   }

   return isSuccess;
}

//******************************************************************************

bool HttpServer::removePathHandler(const std::string& path) {
   bool isSuccess = false;
   unordered_map<string,HttpHandler*>::iterator it =
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
   unordered_map<string,HttpHandler*>::iterator it = m_mapPathHandlers.find(path);
   if (it != m_mapPathHandlers.end()) {
      return (*it).second;
   }

   return nullptr;
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

   for (const auto& headerKey : keys) {
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
   if (nullptr != m_threadPool) {
      // Hand off the request to the thread pool for asynchronous processing
      HttpRequestHandler* requestHandler =
         new HttpRequestHandler(*this, socketRequest);
      requestHandler->setThreadPooling(true);
      requestHandler->setAutoDelete();
      m_threadPool->addRequest(requestHandler);
   } else {
      // no thread pool available -- process it synchronously
      HttpRequestHandler requestHandler(*this, socketRequest);
      requestHandler.run();
      if (socketRequest->isAutoDelete()) {
         delete socketRequest;
      }
   }
}

//******************************************************************************

int HttpServer::runSocketServer() {
   int rc = 0;
   
   if (!m_serverSocket) {
      LOG_CRITICAL("runSocketServer called with null serverSocket")
      return 1;
   }
   
   while (!m_isDone) {
      Socket* socket = m_serverSocket->accept();

      if (nullptr == socket) {
         continue;
      }

      //if (Logger::isLogging(Debug)) {
         //LOG_DEBUG("*****************************************")
         //LOG_DEBUG("client connected")
      //}

      try {
         if (m_isThreaded && (nullptr != m_threadPool)) {
            HttpRequestHandler* handler =
               new HttpRequestHandler(*this, socket);
            handler->setThreadPooling(true);
	    handler->setAutoDelete();

            // give it to the thread pool
            bool added = m_threadPool->addRequest(handler);
            if (!added) {
               printf("HttpServer::runServer unable to add request\n");
            }
         } else {
            HttpRequestHandler handler(*this, socket);
            handler.run();
         }
      } catch (const BasicException& be) {
         rc = 1;
         LOG_ERROR("HttpServer runServer BasicException caught: " +
                   be.whatString())
      } catch (const exception& e) {
         rc = 1;
         LOG_ERROR(string("HttpServer runServer exception caught: ") +
                   string(e.what()))
      } catch (...) {
         rc = 1;
         LOG_ERROR("HttpServer runServer unknown exception caught")
      }
   }
   
   return rc;
}

//******************************************************************************

int HttpServer::runKernelEventServer() {
   const int MAX_CON = 1200;
   int rc = 0;
   
   if (m_threadingFactory != nullptr) {
      Mutex* mutexFD = m_threadingFactory->createMutex("fdMutex");
      Mutex* mutexHWMConnections =
         m_threadingFactory->createMutex("hwmConnectionsMutex");
      AutoPointer<KernelEventServer*> kernelEventServer(nullptr);
      
      if (KqueueServer::isSupportedPlatform()) {
         kernelEventServer.assign(
            new KqueueServer(*mutexFD, *mutexHWMConnections));
      } else if (EpollServer::isSupportedPlatform()) {
         kernelEventServer.assign(
            new EpollServer(*mutexFD, *mutexHWMConnections));
      } else {
         LOG_CRITICAL("no kernel event server available for platform")
         rc = 1;
      }
      
      if (kernelEventServer.haveObject()) {
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
            LOG_CRITICAL("exception running kernel event server: " +
                         be.whatString())
         } catch (const exception& e) {
            LOG_CRITICAL("exception running kernel event server: " +
                         string(e.what()))
         } catch (...) {
            LOG_CRITICAL("unidentified exception running kernel event server")
         }
      }
   } else {
      LOG_CRITICAL("no threading factory configured")
      rc = 1;
   }
   
   return rc;
}

//******************************************************************************

int HttpServer::run() {
   if (!m_isFullyInitialized) {
      LOG_DEBUG("HttpServer::run m_isFullyInitialized is false")
      LOG_CRITICAL("server not initialized")
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
   const string localDateTime = getLocalDateTime();

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

void HttpServer::setupLogFiles(const SectionedConfigDataSource& dataSource) {
   LOG_DEBUG("setupLogFiles")
   KeyValuePairs kvpLogFiles;
   if (dataSource.hasSection(CFG_SECTION_LOGGING) &&
       dataSource.readSection(CFG_SECTION_LOGGING,
                              kvpLogFiles)) {
      if (kvpLogFiles.hasKey(CFG_LOGFILE_ACCESS)) {
         const string& accessLog =
            kvpLogFiles.getValue(CFG_LOGFILE_ACCESS);
         m_accessLogFile = accessLog;
         LOG_INFO(string("access log=") + accessLog)
      }

      if (kvpLogFiles.hasKey(CFG_LOGFILE_ERROR)) {
         const string& errorLog =
            kvpLogFiles.getValue(CFG_LOGFILE_ERROR);
         m_errorLogFile = errorLog;
         LOG_INFO(string("error log=") + errorLog)
      }
   }
}

//******************************************************************************

void HttpServer::setupLogLevel(const KeyValuePairs& kvp) {
   LOG_DEBUG("setupLogLevel")
   if (kvp.hasKey(CFG_SERVER_LOG_LEVEL)) {
      m_logLevel = kvp.getValue(CFG_SERVER_LOG_LEVEL);
      if (!m_logLevel.empty()) {
         StrUtils::toLowerCase(m_logLevel);
         LOG_INFO(string("log level: ") + m_logLevel)
         Logger* logger = Logger::getLogger();

         if (logger != nullptr) {
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
               LOG_WARNING("unrecognized log level: '" + m_logLevel)
            }
         }
      }
   }
}

//******************************************************************************

void HttpServer::setupSocketBufferSizes(const chaudiere::KeyValuePairs& kvp) {
   //LOG_DEBUG("setupSocketBufferSizes")
   if (kvp.hasKey(CFG_SERVER_SEND_BUFFER_SIZE)) {
      const int buffSize =
         getIntValue(kvp, CFG_SERVER_SEND_BUFFER_SIZE);

      if (buffSize > 0) {
         m_socketSendBufferSize = buffSize;
      }
   }

   if (kvp.hasKey(CFG_SERVER_RECEIVE_BUFFER_SIZE)) {
      const int buffSize =
         getIntValue(kvp, CFG_SERVER_RECEIVE_BUFFER_SIZE);

      if (buffSize > 0) {
         m_socketReceiveBufferSize = buffSize;
      }
   }
}

//******************************************************************************

void HttpServer::setupServerString(const chaudiere::KeyValuePairs& kvp) {
   //LOG_DEBUG("setupServerString")
   if (kvp.hasKey(CFG_SERVER_STRING)) {
      const string& serverString =
         kvp.getValue(CFG_SERVER_STRING);
      if (!serverString.empty()) {
         m_serverString = serverString;

         const string::size_type posDollar =
            serverString.find("$");
         if (posDollar != string::npos) {
            KeyValuePairs kvpVars;
            kvpVars.addPair("$PRODUCT_NAME", SERVER_NAME);
            kvpVars.addPair("$PRODUCT_VERSION", SERVER_VERSION);
            kvpVars.addPair("$CFG_SOCKETS", m_sockets);
            kvpVars.addPair("$CFG_THREADING", m_threading);
                  
            const string::size_type posDollarOS =
               serverString.find("$OS_");
                  
            if (posDollarOS != string::npos) {
               SystemInfo systemInfo;
               if (systemInfo.retrievedSystemInfo()) {
                  kvpVars.addPair("$OS_SYSNAME", systemInfo.sysName());
                  kvpVars.addPair("$OS_NODENAME", systemInfo.nodeName());
                  kvpVars.addPair("$OS_RELEASE", systemInfo.release());
                  kvpVars.addPair("$OS_VERSION", systemInfo.version());
                  kvpVars.addPair("$OS_MACHINE", systemInfo.machine());
               } else {
                  LOG_WARNING("unable to retrieve system information to populate server string")
               }
            }
                  
            replaceVariables(kvpVars, m_serverString);
         }
               
         //LOG_INFO("setting server string: '" + m_serverString + "'")
      }
   }
}

//******************************************************************************

bool HttpServer::setupHandlers(const chaudiere::SectionedConfigDataSource* dataSource) {
   //LOG_DEBUG("setupHandlers")
   const bool isLoggingDebug = Logger::isLogging(Debug);

   if (m_allowBuiltInHandlers) {
      //LOG_DEBUG("adding built-in handlers")
      addBuiltInHandlers();
   }
      
   if (isLoggingDebug) {
      //LOG_DEBUG("processing handlers")
   }

   KeyValuePairs kvpHandlers;
   if (dataSource->hasSection(CFG_SECTION_HANDLERS) &&
       dataSource->readSection(CFG_SECTION_HANDLERS, kvpHandlers)) {

      vector<string> vecKeys;
      kvpHandlers.getKeys(vecKeys);

      for (const auto& path : vecKeys) {
         const string& moduleSection = kvpHandlers.getValue(path);

         if (isLoggingDebug) {
            LOG_DEBUG("path='" + path + "'")
         }
            
         if (moduleSection.empty()) {
            LOG_WARNING(string("nothing specified for path ") + path)
            LOG_WARNING("Not servicing this path")
            continue;
         }

         if (dataSource->hasSection(moduleSection)) {
            KeyValuePairs kvpModule;
            if (dataSource->readSection(moduleSection, kvpModule)) {
               if (!kvpModule.hasKey(MODULE_DLL_NAME)) {
                  LOG_ERROR(MODULE_DLL_NAME +
                            string(" not specified for module ") +
                            moduleSection)
               }

               const string& dllName = kvpModule.getValue(MODULE_DLL_NAME);
               HttpHandler* pHandler = nullptr;
                  
               if (isLoggingDebug) {
                  LOG_DEBUG("trying to load dynamic library='" +
                            dllName +
                            "'")
               }
                  
               DynamicLibrary* dll = new DynamicLibrary(dllName);

               // load the dll
               try {
                  void* pfn = dll->resolve("CreateHandler");
                  if (pfn == nullptr) {
                     LOG_ERROR("unable to find module library entry point")
                  } else {
                     if (isLoggingDebug) {
                        LOG_DEBUG("dynamic library loaded")
                     }
                  }

                  PFN_CREATE_HANDLER pfnCreateHandler = (PFN_CREATE_HANDLER) pfn;
                  pHandler = (*pfnCreateHandler)();
               } catch (const exception& e) {
                  LOG_ERROR(string("exception caught trying to load module library ") +
                            dllName)
                  LOG_ERROR(e.what())
               } catch (...) {
                  LOG_ERROR(string("unable to load module library ") +
                            dllName)
               }

               // continue loading application specific parameters for the module
               vector<string> vecModuleKeys;
               kvpModule.getKeys(vecModuleKeys);

               KeyValuePairs kvpApp;

               for (const auto& moduleKey : vecModuleKeys) {

                  // starts with app prefix?
                  if (StrUtils::startsWith(moduleKey, APP_PREFIX)) {
                     if (moduleKey.length() > APP_PREFIX_LEN) {
                        kvpApp.addPair(moduleKey.substr(APP_PREFIX_LEN),
                                       kvpModule.getValue(moduleKey));
                     }
                  }
               }

               if (isLoggingDebug) {
                  //LOG_DEBUG("initializing the handler")
               }

               // now initialize the servlet
               if (pHandler->init(path, kvpApp)) {
                  if (isLoggingDebug) {
                     //LOG_DEBUG("initialization succeeded")
                  }
                     
                  // register it
                  if (!addPathHandler(path, pHandler)) {
                     LOG_ERROR(string("unable to register handler for path ") +
                               path)
                     
                     if (m_requireAllHandlersForStartup) {
                        return false;
                     }
                  } else {
                     m_mapPathLibraries[path] = dll;
                  }
               } else {
                  LOG_ERROR(string("unable to initialize handler for path ") +
                            path)
                  if (m_requireAllHandlersForStartup) {
                     return false;
                  }
               }
            }
         } else {
            if (!moduleSection.empty()) {
               LOG_ERROR(string("no configuration for handler ") +
                         moduleSection)
            } else {
               LOG_ERROR(string("no configuration for handler ") +
                         path)
            }

            if (m_requireAllHandlersForStartup) {
               return false;
            }
         }
      }
   }

   // do we have any handlers?
   if (!m_allowBuiltInHandlers && m_mapPathHandlers.empty()) {
      LOG_CRITICAL("no handlers registered")
      return false;
   }

   return true;
}

//******************************************************************************

void HttpServer::setupThreading(const chaudiere::KeyValuePairs& kvp) {
   //LOG_DEBUG("setupThreading")
   m_isThreaded = true;
   m_threading = CFG_THREADING_PTHREADS;
   m_threadPoolSize = 4;

   if (kvp.hasKey(CFG_SERVER_THREADING)) {
      const string& threading =
         kvp.getValue(CFG_SERVER_THREADING);
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

   if (kvp.hasKey(CFG_SERVER_THREAD_POOL_SIZE)) {
      const int poolSize =
         getIntValue(kvp, CFG_SERVER_THREAD_POOL_SIZE);

      if (poolSize > 0) {
         m_threadPoolSize = poolSize;
      }
   }
}

//******************************************************************************

void HttpServer::setupSocketHandling(const chaudiere::KeyValuePairs& kvp) {
   //LOG_DEBUG("setupSocketHandling")
   m_sockets = CFG_SOCKETS_SOCKET_SERVER;
   if (kvp.hasKey(CFG_SERVER_SOCKETS)) {
      const string& sockets =
         kvp.getValue(CFG_SERVER_SOCKETS);
      if (sockets == CFG_SOCKETS_KERNEL_EVENTS) {
         m_isUsingKernelEventServer = true;
         m_sockets = CFG_SOCKETS_KERNEL_EVENTS;
      }
   }
}

//******************************************************************************

void HttpServer::setupListeningPort(const chaudiere::KeyValuePairs& kvp) {
   //LOG_DEBUG("setupListeningPort")
   if (kvp.hasKey(CFG_SERVER_PORT)) {
      const int portNumber =
         getIntValue(kvp, CFG_SERVER_PORT);
      if (portNumber > 0) {
         m_serverPort = portNumber;
         if (Logger::isLogging(LogLevel::Debug)) {
            char msg[128];
            ::snprintf(msg, 128, "port number=%d", m_serverPort);
            LOG_DEBUG(msg)
         }
      }
   }
}

//******************************************************************************

void HttpServer::setupConcurrency() {
   //LOG_DEBUG("setupConcurrency")
   string concurrencyModel = EMPTY;

   if (m_isThreaded) {
      bool isUsingLibDispatch = false;
      m_threadingFactory.reset(new PthreadsThreadingFactory);
      //ThreadingFactory::setThreadingFactory(m_threadingFactory);
      m_threadPool.reset(
         m_threadingFactory->createThreadPoolDispatcher(m_threadPoolSize,
                                                        "thread_pool"));

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
}

//******************************************************************************

bool HttpServer::setupServerSocket() {
   //LOG_DEBUG("setupServerSocket")
   if (!m_isUsingKernelEventServer) {
      try {
         if (Logger::isLogging(LogLevel::Debug)) {
            //char msg[128];
            //::snprintf(msg, 128,
            //           "creating server socket on port=%d",
            //           m_serverPort);
            //LOG_DEBUG(string(msg))
         }

         m_serverSocket = std::make_unique<ServerSocket>(m_serverPort);
      } catch (...) {
         string exception = "unable to open server socket port '";
         exception += StrUtils::toString(m_serverPort);
         exception += "'";
         LOG_CRITICAL(exception)
         return false;
      }
   }

   return true;
}

//******************************************************************************

