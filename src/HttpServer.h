// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_HTTPSERVER_H
#define MISERE_HTTPSERVER_H

#include <string>
#include <map>

#include "HttpHandler.h"
#include "KeyValuePairs.h"
#include "ServerSocket.h"
#include "SocketRequest.h"
#include "DynamicLibrary.h"
#include "ThreadPoolDispatcher.h"
#include "SectionedConfigDataSource.h"
#include "ThreadingFactory.h"


namespace misere {

/**
 * HttpServer is an HTTP server meant to be used for servicing application
 * HTTP requests. It is not meant to be a general purpose web server (no
 * provisions for serving files).
 */
class HttpServer {
   public:
      /**
       * Constructs an HttpServer with the file name/path for a configuration file
       * @param configFilePath the file name/path for configuration settings
       */
      explicit HttpServer(const std::string& configFilePath);
   
      /**
       * Destructor
       */
      ~HttpServer();
   
      /**
       * Retrieves the current time in Greenwich Mean Time (GMT)
       * @return current time in GMT
       */
      std::string getSystemDateGMT() const;
   
      /**
       * Retrieves the current time for server in local time
       * @return current time as local server time
       */
      std::string getLocalDateTime() const;

      /**
       * Constructs HTTP response headers using the specified response code and
       * collection of header key/value pairs
       * @param responseCode the HTTP response code
       * @param headers collection of HTTP header key/value pairs
       * @return HTTP headers formatted as a string
       */
      std::string buildHeader(const std::string& responseCode,
                              const chaudiere::KeyValuePairs& headers) const;

      /**
       * Registers an HttpHandler for the specified path
       * @param path the path to associate with the specified handler
       * @param handler the handler to invoke when a request arrives for the specified path
       * @see HttpHandler()
       * @return boolean indicating if the handler was successfully registered
       */
      bool addPathHandler(const std::string& path, HttpHandler* handler);
   
      /**
       * Removes the handler for the specified path
       * @param path the path whose associated handler should be removed (deregistered)
       * @return boolean indicating if a path was deregistered for the path
       */
      bool removePathHandler(const std::string& path);
   
      /**
       * Retrieves the handler associated with the specified path
       * @param path the path whose handler is desired
       * @return the handler associated with the path, or null if there is none
       */
      HttpHandler* getPathHandler(const std::string& path);

      /**
       * Runs the built-in socket server
       * @return exit code for the HTTP server process
       */
      int runSocketServer();
   
      /**
       * Runs a kernel event server (e.g., kqueue or epoll)
       * @return exit code for the HTTP server process
       */
      int runKernelEventServer();
   
      /**
       * Runs the HTTP server using either the built-in socket server or a kernel event server
       * @return exit code for the HTTP server process
       */
      int run();

      /**
       * Logs a HTTP request
       * @param clientIPAddress the IP address of the system that generated the HTTP request
       * @param requestLine the first line of the HTTP request
       * @param responseCode the HTTP response code
       */
      void logRequest(const std::string& clientIPAddress,
                      const std::string& requestLine,
                      const std::string& responseCode);

      /**
       * Logs a HTTP request that was processed by a thread pool worker
       * @param clientIPAddress the IP address of the system that generated the HTTP request
       * @param requestLine the first line of the HTTP request
       * @param responseCode the HTTP response code
       * @param threadWorkerId the identifier of the thread pool worker that processed the request
       */
      void logRequest(const std::string& clientIPAddress,
                      const std::string& requestLine,
                      const std::string& responseCode,
                      const std::string& threadWorkerId);
   
      /**
       * Retrieves the configuration data source of configuration settings
       * @see SectionedConfigDataSource()
       * @return the configuration data source
       */
      chaudiere::SectionedConfigDataSource* getConfigDataSource();
   
      /**
       * Retrieves the size of the socket send buffer
       * @return size of the socket send buffers
       */
      int getSocketSendBufferSize() const;
   
      /**
       * Retrieves the size of the socket receive buffer
       * @return size of the socket receive buffers
       */
      int getSocketReceiveBufferSize() const;
   
      /**
       * Retrieves the identifier for the server
       * @return server identifier
       */
      const std::string& getServerId() const;
   
      /**
       * Retrieves the size in bytes of a generic (void*) pointer
       * @return platform pointer size
       */
      int platformPointerSizeBits() const;

      /**
       * Service a request for a socket when using a kernel event server
       * @param socketRequest the SocketRequest to process
       * @see SocketRequest()
       */
      void serviceSocket(chaudiere::SocketRequest* socketRequest);

      /**
       * Convenience method to retrieve a setting and convert it to a boolean
       * @param kvp the collection of key/value pair settings
       * @param setting the name of the setting whose value is to be retrieved and converted
       * @see KeyValuePairs()
       * @return boolean value (or false if value cannot be retrieved or converted)
       */
      bool hasTrueValue(const chaudiere::KeyValuePairs& kvp,
                        const std::string& setting) const;
   
      /**
       * Convenience method to retrieve a setting and convert it to an integer
       * @param kvp the collection of key/value pair settings
       * @param setting the name of the setting whose value is to be retrieved and converted
       * @see KeyValuePairs()
       * @return integer value (or -1 if value cannot be retrieved or converted)
       */
      int getIntValue(const chaudiere::KeyValuePairs& kvp,
                      const std::string& setting) const;
   
      /**
       * Convenience method to replace all occurrences of keys in collection with their values
       * @param kvp the collection of key/value pairs for replacement
       * @param s the string to search and replace all variables in
       */
      void replaceVariables(const chaudiere::KeyValuePairs& kvp,
                            std::string& s) const;
   
      /**
       * Determines if compression is turned on for the specified mime type
       * @param mimeType the mime type to check whether to compress
       * @return boolean indicating whether the specified mime type is to be compressed
       */
      bool compressResponse(const std::string& mimeType) const;
   
      /**
       * Determines if gzip compression is enabled for the server
       * @return boolean indicating if gzip compression is enabled
       */
      bool compressionEnabled() const;
   
      /**
       * Retrieves the minimum size of the response payload to be compressed
       * @return minimum size of response payload (in bytes) to be compressed
       */
      int minimumCompressionSize() const;

   
   protected:
      /**
       * Initializes the HTTP server on the specified port by default by reading and
       * applying configuration values read from configuration data source
       * @param port the default port (may be overridden by configuration values)
       * @return boolean indicating whether initialiation was successful
       */
      virtual bool init(int port);
   
      /**
       * Adds built-in handlers
       * @return boolean indicating whether the built-in handlers were successfully added
       */
      virtual bool addBuiltInHandlers();


   private:
      chaudiere::ServerSocket* m_serverSocket;
      chaudiere::ThreadPoolDispatcher* m_threadPool;
      chaudiere::ThreadingFactory* m_threadingFactory;
      chaudiere::KeyValuePairs m_properties;
      std::map<std::string, HttpHandler*> m_mapPathHandlers;
      std::map<std::string, chaudiere::DynamicLibrary*> m_mapPathLibraries;
      std::string m_accessLogFile;
      std::string m_errorLogFile;
      std::string m_logLevel;
      std::string m_concurrencyModel;
      std::string m_configFilePath;
      std::string m_startupTime;
      std::string m_serverString;
      std::string m_threading;
      std::string m_sockets;
      bool m_isDone;
      bool m_isThreaded;
      bool m_isUsingKernelEventServer;
      bool m_isFullyInitialized;
      bool m_allowBuiltInHandlers;
      bool m_requireAllHandlersForStartup;
      bool m_compressionEnabled;
      int m_threadPoolSize;
      int m_serverPort;
      int m_socketSendBufferSize;
      int m_socketReceiveBufferSize;
      int m_minimumCompressionSize;

      // copies not allowed
      HttpServer(const HttpServer&);
      HttpServer& operator=(const HttpServer&);

};

}

#endif

