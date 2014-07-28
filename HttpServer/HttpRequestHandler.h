// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_HTTPREQUESTHANDLER_H
#define MISERE_HTTPREQUESTHANDLER_H


#include "Runnable.h"
#include "RequestHandler.h"
#include "Socket.h"
#include "SocketRequest.h"


namespace misere
{
   class HttpServer;

/**
 * HttpRequestHandler is the interface that must be implemented by all
 * HttpServer 'modules'.
 */
class HttpRequestHandler : public chaudiere::RequestHandler
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
   HttpRequestHandler(HttpServer& server, std::shared_ptr<chaudiere::SocketRequest> socketRequest) noexcept;
   
   /**
    * Constructs a HttpRequestHandler using a Socket
    * @param server the HttpServer that is being run
    * @param socket the Socket for handling the request
    * @param HttpServer()
    * @param Socket()
    */
   HttpRequestHandler(HttpServer& server, std::shared_ptr<chaudiere::Socket> socket) noexcept;
   
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

}

#endif
