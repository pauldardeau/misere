// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__RequestHandler__
#define __HttpServer__RequestHandler__


#include "Runnable.h"
#include "RequestHandler.h"

class HttpServer;
class Socket;
class SocketRequest;

/**
 * HttpRequestHandler is the interface that must be implemented by all
 * HttpServer 'modules'.
 */
class HttpRequestHandler : public RequestHandler
{
private:
   HttpServer& m_server;

   
public:
   /**
    * Constructs a HttpRequestHandler using a SocketRequest for use by a KernelEventServer
    * @param server the HttpServer that is being run
    * @param socketRequest the SocketRequest for processing using KernelEventServer
    * @see HttpServer()
    * @see SocketRequest()
    */
   HttpRequestHandler(HttpServer& server, std::shared_ptr<SocketRequest> socketRequest) noexcept;
   
   /**
    * Constructs a HttpRequestHandler using a Socket
    * @param server the HttpServer that is being run
    * @param socket the Socket for handling the request
    * @param HttpServer()
    * @param Socket()
    */
   HttpRequestHandler(HttpServer& server, std::shared_ptr<Socket> socket) noexcept;
   
   /**
    * Destructor
    */
   ~HttpRequestHandler() noexcept;
   
   /**
    * Process the HTTP request from beginning to end
    */
   void run() override;
   
   
   // disallow copies
   HttpRequestHandler(const HttpRequestHandler&) = delete;
   HttpRequestHandler(HttpRequestHandler&&) = delete;
   HttpRequestHandler& operator=(const HttpRequestHandler&) = delete;
   HttpRequestHandler& operator=(HttpRequestHandler&&) = delete;
   
};

#endif
