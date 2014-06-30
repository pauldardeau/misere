// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__RequestHandler__
#define __HttpServer__RequestHandler__


#include "Runnable.h"

class HttpServer;
class Socket;
class SocketRequest;

/*!
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
   RequestHandler(HttpServer& server, std::shared_ptr<SocketRequest> socketRequest) noexcept;
   RequestHandler(HttpServer& server, std::shared_ptr<Socket> socket) noexcept;
   ~RequestHandler() noexcept;
   
   void run() override;
   void setThreadPooling(bool isThreadPooling) noexcept;
   
   
   // disallow copies
   RequestHandler(const RequestHandler&) = delete;
   RequestHandler(RequestHandler&&) = delete;
   RequestHandler& operator=(const RequestHandler&) = delete;
   RequestHandler& operator=(RequestHandler&&) = delete;
   
};

#endif /* defined(__HttpServer__RequestHandler__) */
