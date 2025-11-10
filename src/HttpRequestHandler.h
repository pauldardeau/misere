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
   HttpRequestHandler(HttpServer& server, chaudiere::SocketRequest* socketRequest);

   /**
    * Constructs a HttpRequestHandler using a Socket
    * @param server the HttpServer that is being run
    * @param socket the Socket for handling the request
    * @param HttpServer()
    * @param Socket()
    */
   HttpRequestHandler(HttpServer& server, chaudiere::Socket* socket);

   /**
    * Destructor
    */
   ~HttpRequestHandler();

   /**
    * Process the HTTP request from beginning to end
    */
   void run();


private:
   // disallow copies
   HttpRequestHandler(const HttpRequestHandler&);
   HttpRequestHandler& operator=(const HttpRequestHandler&);

};

}

#endif

