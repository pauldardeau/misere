// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <memory>


class Socket;

/*!
 * ServerSocket is very similar to Java's ServerSocket class. It provides
 * server-side handling of Socket calls.
 */
class ServerSocket
{
   public:
      static bool setReuseAddr(int socketFD) noexcept;
      static bool listen(int socketFD, int backlog) noexcept;
      static bool bind(int socketFD, int port) noexcept;
   
      // throws BasicException
      ServerSocket(int port);
      ~ServerSocket() noexcept;

      std::shared_ptr<Socket> accept() noexcept;

      void close() noexcept;

      // copying not allowed
      ServerSocket(const ServerSocket&) = delete;
      ServerSocket(const ServerSocket&&) = delete;
      ServerSocket& operator=(const ServerSocket&) = delete;
      ServerSocket& operator=(const ServerSocket&&) = delete;

   
   private:
      bool create() noexcept;
      bool listen() noexcept;

      int m_serverSocket;
      int m_port;
};


#endif


