// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__HttpSocketServiceHandler__
#define __HttpServer__HttpSocketServiceHandler__

#include <string>
#include <memory>

#include "SocketServiceHandler.h"

class SocketRequest;
class HttpServer;


/**
 * HttpSocketServiceHandler is a SocketServiceHandler built for use
 * with HttpServer.
 */
class HttpSocketServiceHandler : public SocketServiceHandler
{
public:
   /**
    * Constructs an HttpSocketServiceHandler with a reference to the HttpServer
    * @param httpServer the HttpServer being run
    * @see HttpServer()
    */
   HttpSocketServiceHandler(HttpServer& httpServer);
   
   /**
    * Destructor
    */
   ~HttpSocketServiceHandler();
   
   /**
    * Forwards the SocketRequest to the HttpServer for processing
    * @param socketRequest the SocketRequest to be processed
    * @see SocketRequest()
    */
   virtual void serviceSocket(std::shared_ptr<SocketRequest> socketRequest) override;
   
   /**
    * Retrieves the handler name. This is primarily an aid for debugging.
    * @return the handler name
    */
   virtual const std::string& getName() const;
   
private:
   HttpServer& m_httpServer;
};

#endif /* defined(__HttpServer__HttpSocketServiceHandler__) */

