// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <span>
#include <string>
#include <utility>

#include "TlsConnection.h"
#include "Socket.h"
#include "BasicException.h"
#include "Logger.h"

using namespace misere;
using namespace chaudiere;

//******************************************************************************

namespace {
   bool isWantRetry(armure::ErrorCode code) {
      return (code == armure::ErrorCode::WantRead) || (code == armure::ErrorCode::WantWrite);
   }
}

//******************************************************************************

TlsConnection::TlsConnection(armure::Connection connection, Socket* socket) :
   m_connection(std::move(connection)),
   m_socket(socket) {

   for (;;) {
      armure::Result<void> result = m_connection.handshake();
      if (result) {
         return;
      }

      if (isWantRetry(result.error().code())) {
         continue;
      }

      throw BasicException(std::string("TLS handshake failed: ") + std::string(result.error().message()));
   }
}

//******************************************************************************

TlsConnection::~TlsConnection() {
}

//******************************************************************************

int TlsConnection::read(char* buffer, int bufferSize) {
   if (bufferSize <= 0) {
      return 0;
   }

   std::span<std::byte> span(reinterpret_cast<std::byte*>(buffer), static_cast<std::size_t>(bufferSize));

   for (;;) {
      armure::Result<std::size_t> result = m_connection.read(span);
      if (result) {
         return static_cast<int>(result.value());
      }

      const armure::ErrorCode code = result.error().code();
      if (isWantRetry(code)) {
         continue;
      }

      if (code != armure::ErrorCode::Closed) {
         LOG_ERROR(std::string("TlsConnection::read failed: ") + std::string(result.error().message()))
      }

      return -1;
   }
}

//******************************************************************************

bool TlsConnection::write(const char* buffer, std::size_t length) {
   if (length == 0) {
      return true;
   }

   std::size_t totalWritten = 0;

   while (totalWritten < length) {
      std::span<const std::byte> remaining(reinterpret_cast<const std::byte*>(buffer) + totalWritten,
                                            length - totalWritten);

      for (;;) {
         armure::Result<std::size_t> result = m_connection.write(remaining);
         if (result) {
            if (result.value() == 0) {
               // Not a documented outcome for a non-empty buffer (see
               // Connection::write()/Transport::write()'s own
               // contracts) - guards against looping forever if it
               // ever happened anyway.
               return false;
            }
            totalWritten += result.value();
            break;
         }

         const armure::ErrorCode code = result.error().code();
         if (isWantRetry(code)) {
            // retry with the identical `remaining` span (same pointer
            // and size) - required by Connection::write()'s documented
            // retry contract
            continue;
         }

         LOG_ERROR(std::string("TlsConnection::write failed: ") + std::string(result.error().message()))
         return false;
      }
   }

   return true;
}

//******************************************************************************

void TlsConnection::close() {
   for (;;) {
      armure::Result<void> result = m_connection.shutdown();
      if (result) {
         break;
      }

      const armure::ErrorCode code = result.error().code();
      if (isWantRetry(code)) {
         continue;
      }

      // close() returns void (matching ByteConnection::close()'s and
      // SocketConnection::close()'s own contracts - neither can report
      // failure either), so this is best-effort: log and still fall
      // through to closing the underlying socket below.
      LOG_ERROR(std::string("TlsConnection::close: TLS shutdown failed: ") + std::string(result.error().message()))
      break;
   }

   if (m_socket != nullptr) {
      m_socket->close();
   }
}

//******************************************************************************
