// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef C10KServer_SocketRequest_h
#define C10KServer_SocketRequest_h


#include "Runnable.h"

class Socket;
class SocketServiceHandler;

/*!
 *
 */
class SocketRequest : public Runnable
{
private:
   Socket* m_socket;
   SocketServiceHandler* m_handler;
    
public:
   SocketRequest(Socket* socket, SocketServiceHandler* handler) noexcept;
   ~SocketRequest() noexcept;
   void run() override;
   int getSocketFD() const noexcept;
   Socket* getSocket() noexcept;
   void requestComplete() noexcept;
   
   // copies not allowed
   SocketRequest(const SocketRequest&) = delete;
   SocketRequest(SocketRequest&&) = delete;
   SocketRequest& operator=(const SocketRequest&) = delete;
   SocketRequest& operator=(SocketRequest&&) = delete;
};

#endif
