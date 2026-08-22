// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "SocketTransport.h"
#include "Socket.h"

using namespace misere;
using namespace chaudiere;

//******************************************************************************

SocketTransport::SocketTransport(Socket* socket, bool socketOwned) :
   m_socket(socket),
   m_socketOwned(socketOwned) {
}

//******************************************************************************

SocketTransport::~SocketTransport() {
   if (m_socketOwned && (m_socket != nullptr)) {
      delete m_socket;
   }
}

//******************************************************************************

armure::Result<std::size_t> SocketTransport::read(std::span<std::byte> buffer) {
   if (buffer.empty()) {
      return armure::Result<std::size_t>(std::size_t{0});
   }

   const int bytesRead = m_socket->recvAvailable(reinterpret_cast<char*>(buffer.data()),
                                                  static_cast<int>(buffer.size()));

   if (bytesRead > 0) {
      return armure::Result<std::size_t>(static_cast<std::size_t>(bytesRead));
   }

   // chaudiere::Socket::recvAvailable() collapses "the peer closed the
   // connection" and "recv() reported a hard error" into the same <= 0
   // result - Socket.h/.cpp expose no way to distinguish them (no
   // non-blocking mode, no errno accessor). Every other call site in
   // misere that reads from a Socket already treats <=0 the same way -
   // "no more data, stop" (see HttpTransaction::streamFromConnection()) -
   // so mapping it to Closed here is consistent with that existing
   // convention, not a guess specific to this adapter.
   return armure::Result<std::size_t>(armure::Error(
      armure::ErrorCode::Closed,
      "chaudiere::Socket::recvAvailable() returned no data - the connection is closed, or the read failed"));
}

//******************************************************************************

armure::Result<std::size_t> SocketTransport::write(std::span<const std::byte> buffer) {
   if (buffer.empty()) {
      return armure::Result<std::size_t>(std::size_t{0});
   }

   const bool success = m_socket->write(reinterpret_cast<const char*>(buffer.data()),
                                         static_cast<unsigned long>(buffer.size()));

   if (success) {
      // chaudiere::Socket::write() loops internally (via sendPayload())
      // until the entire buffer is sent or it fails outright - there is
      // no genuine short-write outcome it can report, so success always
      // means the full buffer was written.
      return armure::Result<std::size_t>(buffer.size());
   }

   return armure::Result<std::size_t>(
      armure::Error(armure::ErrorCode::TransportError, "chaudiere::Socket::write() failed"));
}

//******************************************************************************
