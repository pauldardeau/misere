// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cstdio>
#include <cstring>

#include "ServerSocket.h"
#include "Socket.h"
#include "BasicException.h"
#include "Logger.h"


static const int BACKLOG = 50;

typedef socklen_t SOCKLEN_T;


//******************************************************************************

ServerSocket::ServerSocket(int port) :
   m_port(port),
   m_serverSocket(-1)
{
   Logger::logInstanceCreate("ServerSocket");

   if (!create()) {
      throw BasicException("unable to create server socket");
   }

   if (!listen()) {
      close();
      throw BasicException("unable to listen on server socket");
   }
}

//******************************************************************************

ServerSocket::~ServerSocket() noexcept
{
   Logger::logInstanceDestroy("ServerSocket");

   close();
}

//******************************************************************************

bool ServerSocket::create() noexcept
{
   m_serverSocket = ::socket(AF_INET, SOCK_STREAM, 0);

   if (m_serverSocket < 0) {
      return false;
   }

   int val_to_set = 1;

   ::setsockopt(m_serverSocket,
                SOL_SOCKET,
                SO_REUSEADDR,
                (char *) &val_to_set,
                sizeof(val_to_set));

   ::memset((void *) &m_serverAddr, 0, sizeof(m_serverAddr));

   m_serverAddr.sin_family = AF_INET;
   m_serverAddr.sin_port = htons(m_port);
   m_serverAddr.sin_addr.s_addr = INADDR_ANY;

   const int rc = ::bind(m_serverSocket,
                         (struct sockaddr*) &m_serverAddr,
                         sizeof(m_serverAddr));

   if (rc < 0) {
      return false;
   } else {
      return true;
   }
}

//******************************************************************************

bool ServerSocket::listen() noexcept
{
   if (::listen(m_serverSocket, BACKLOG) != 0) {
      return false;
   } else {
      return true;
   }
}

//******************************************************************************

Socket* ServerSocket::accept() noexcept
{
   struct sockaddr_in clientAddr;
   SOCKLEN_T namelen = sizeof(clientAddr);

   const int connectionSocket = ::accept(m_serverSocket,
                                         (struct sockaddr*) &clientAddr,
                                         &namelen);

   if (connectionSocket < 0) {
      return nullptr;
   } else {
      return new Socket(connectionSocket);
   }
}

//******************************************************************************

void ServerSocket::close() noexcept
{
   m_serverSocket = -1;
}

//******************************************************************************




