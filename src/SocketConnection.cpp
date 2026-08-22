// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "SocketConnection.h"
#include "Socket.h"

using namespace misere;
using namespace chaudiere;

//******************************************************************************

SocketConnection::SocketConnection(Socket* socket, bool socketOwned) :
   m_socket(socket),
   m_socketOwned(socketOwned) {
}

//******************************************************************************

SocketConnection::~SocketConnection() {
   if (m_socketOwned && (m_socket != nullptr)) {
      delete m_socket;
   }
}

//******************************************************************************

int SocketConnection::read(char* buffer, int bufferSize) {
   return m_socket->recvAvailable(buffer, bufferSize);
}

//******************************************************************************

bool SocketConnection::write(const char* buffer, std::size_t length) {
   return m_socket->write(buffer, static_cast<unsigned long>(length));
}

//******************************************************************************

void SocketConnection::close() {
   m_socket->close();
}

//******************************************************************************
