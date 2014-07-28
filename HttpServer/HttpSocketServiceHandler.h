// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_HTTPSOCKETSERVICEHANDLER_H
#define MISERE_HTTPSOCKETSERVICEHANDLER_H

#include <string>
#include <memory>

#include "SocketServiceHandler.h"
#include "SocketRequest.h"


namespace misere
{

class HttpServer;


/**
 * HttpSocketServiceHandler is a SocketServiceHandler built for use
 * with HttpServer.
 */
class HttpSocketServiceHandler : public chaudiere::SocketServiceHandler
{
public:
   /**
    * Constructs an HttpSocketServiceHandler with a reference to the HttpServer
    * @param httpServer the HttpServer being run
    * @see HttpServer()
    */
   explicit HttpSocketServiceHandler(HttpServer& httpServer);
   
   /**
    * Destructor
    */
   ~HttpSocketServiceHandler();
   
   /**
    * Forwards the SocketRequest to the HttpServer for processing
    * @param socketRequest the SocketRequest to be processed
    * @see SocketRequest()
    */
   virtual void serviceSocket(std::shared_ptr<chaudiere::SocketRequest> socketRequest) override;
   
   /**
    * Retrieves the handler name. This is primarily an aid for debugging.
    * @return the handler name
    */
   virtual const std::string& getName() const;
   
private:
   HttpServer& m_httpServer;
};

}

#endif

