// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <string>
#include <unordered_map>

#include "HttpHandler.h"
#include "KernelEventServer.h"

class ServerSocket;
class SocketRequest;
class ThreadPoolDispatcher;
class SectionedConfigDataSource;
class ThreadingFactory;


/*!
 * HttpServer is an HTTP server meant to be used for servicing application
 * HTTP requests. It is not meant to be a general purpose web server (no
 * provisions for serving files).
 */
class HttpServer
{
   public:
      HttpServer();
      HttpServer(int port);
      HttpServer(const std::string& configFilePath);
      ~HttpServer() noexcept;
   
      // copies not allowed
      HttpServer(const HttpServer&) = delete;
      HttpServer(HttpServer&&) = delete;
      HttpServer& operator=(const HttpServer&) = delete;
      HttpServer& operator=(HttpServer&&) = delete;


      std::string getSystemDateGMT() const noexcept;
      std::string getLocalDateTime() const noexcept;

      std::string buildHeader(const std::string& responseCode,
                              const std::unordered_map<std::string, std::string>& mapHeaders) const noexcept;

      bool addPathHandler(const std::string& path, HttpHandler* pHandler) noexcept;
      bool removePathHandler(const std::string& path) noexcept;
      HttpHandler* getPathHandler(const std::string& path) noexcept;

      int runSocketServer() noexcept;
      int runKernelEventServer() noexcept;
      int run() noexcept;

      void logRequest(const std::string& clientIPAddress,
                      const std::string& requestLine,
                      const std::string& responseCode) noexcept;

      void logRequest(const std::string& clientIPAddress,
                      const std::string& requestLine,
                      const std::string& responseCode,
                      const std::string& threadWorkerId) noexcept;
                       
      std::unique_ptr<SectionedConfigDataSource> getConfigDataSource();
   
      int getSocketSendBufferSize() const noexcept;
      int getSocketReceiveBufferSize() const noexcept;
   
      const std::string& getServerId() const noexcept;
   
      int platformPointerSizeBits() const noexcept;

      void serviceSocket(std::shared_ptr<SocketRequest> socketRequest);

      bool hasTrueValue(const KeyValuePairs& kvp,
                        const std::string& setting) const noexcept;
      int getIntValue(const KeyValuePairs& kvp,
                      const std::string& setting) const noexcept;
   
      void replaceVariables(const KeyValuePairs& kvp, std::string& s) const noexcept;
   
      bool compressResponse(const std::string& mimeType) const noexcept;
      bool compressionEnabled() const noexcept;
      int minimumCompressionSize() const noexcept;

   
   protected:
      // throws BasicException
      virtual bool init(int port);
   
      virtual bool addBuiltInHandlers() noexcept;


   private:
      std::unique_ptr<KernelEventServer> m_kernelEventServer;
      std::unique_ptr<ServerSocket> m_serverSocket;
      std::shared_ptr<ThreadPoolDispatcher> m_threadPool;
      std::shared_ptr<ThreadingFactory> m_threadingFactory;
      std::unordered_map<std::string, std::string> m_mapProperties;
      std::unordered_map<std::string, std::unique_ptr<HttpHandler>> m_mapPathHandlers;
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

};


#endif


