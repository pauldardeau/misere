// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__HttpSocketServiceHandler__
#define __HttpServer__HttpSocketServiceHandler__

#include <string>

#include "SocketServiceHandler.h"

class SocketRequest;
class HttpServer;


class HttpSocketServiceHandler : public SocketServiceHandler
{
public:
   HttpSocketServiceHandler(HttpServer* httpServer);
   ~HttpSocketServiceHandler();
   virtual void serviceSocket(SocketRequest* socketRequest);
   virtual const std::string& getName() const;
   
private:
   HttpServer* m_httpServer;
};

#endif /* defined(__HttpServer__HttpSocketServiceHandler__) */

