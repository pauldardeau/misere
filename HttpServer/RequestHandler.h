// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__RequestHandler__
#define __HttpServer__RequestHandler__


#include "Runnable.h"

class HttpServer;
class Socket;
class SocketRequest;

/**
 * RequestHandler is the interface that must be implemented by all 'modules'
 * that are used by HttpServer.
 */
class RequestHandler : public Runnable
{
private:
   HttpServer& m_server;
   std::shared_ptr<Socket> m_socket;
   std::shared_ptr<SocketRequest> m_socketRequest;
   bool m_isThreadPooling;
   
public:
   /**
    * Constructs a RequestHandler using a SocketRequest for use by a KernelEventServer
    * @param server the HttpServer that is being run
    * @param socketRequest the SocketRequest for processing using KernelEventServer
    * @see HttpServer()
    * @see SocketRequest()
    */
   RequestHandler(HttpServer& server, std::shared_ptr<SocketRequest> socketRequest) noexcept;
   
   /**
    * Constructs a RequestHandler using a Socket
    * @param server the HttpServer that is being run
    * @param socket the Socket for handling the request
    * @param HttpServer()
    * @param Socket()
    */
   RequestHandler(HttpServer& server, std::shared_ptr<Socket> socket) noexcept;
   
   /**
    * Destructor
    */
   ~RequestHandler() noexcept;
   
   /**
    * Process the HTTP request from beginning to end
    */
   void run() override;
   
   /**
    * Sets boolean indicating whether request is being run on thread pool
    * @param isThreadPooling boolean indicating if request is being run on thread pool
    */
   void setThreadPooling(bool isThreadPooling) noexcept;
   
   
   // disallow copies
   RequestHandler(const RequestHandler&) = delete;
   RequestHandler(RequestHandler&&) = delete;
   RequestHandler& operator=(const RequestHandler&) = delete;
   RequestHandler& operator=(RequestHandler&&) = delete;
   
};

#endif /* defined(__HttpServer__RequestHandler__) */
