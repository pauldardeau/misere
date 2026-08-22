// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_SOCKETTRANSPORT_H
#define MISERE_SOCKETTRANSPORT_H

#include <cstddef>
#include <span>

#include "armure/Transport.h"

namespace chaudiere
{
   class Socket;
}

namespace misere
{

/**
 * SocketTransport adapts an existing chaudiere::Socket to armure's
 * Transport interface, so an armure::Connection can perform TLS
 * handshaking and encryption/decryption over an ordinary chaudiere
 * socket. Translation only - no TLS logic and no HTTP logic live here.
 *
 * chaudiere::Socket has no non-blocking mode (see Socket.h): every
 * read/write blocks until it has a definite outcome - data, an orderly
 * close, or a hard error. Consequently read()/write() below never
 * themselves report armure::ErrorCode::WantRead/WantWrite - those exist
 * in Transport's contract specifically for non-blocking transports,
 * which this one is not. An armure::Connection built on this Transport
 * may still report WantRead/WantWrite from its own read()/write()/
 * handshake() for reasons internal to the TLS protocol - a caller
 * driving such a Connection (see TlsConnection) still needs to handle
 * that, just not expect this Transport itself to be the source.
 */
class SocketTransport : public armure::Transport
{
   public:
      /**
       * Constructs a SocketTransport wrapping an existing socket
       * @param socket the socket to read from / write to
       * @param socketOwned whether this SocketTransport should close
       *        and delete the socket when it is destroyed
       */
      explicit SocketTransport(chaudiere::Socket* socket, bool socketOwned=true);

      /**
       * Destructor. Deletes the underlying socket only if socketOwned
       * was true.
       */
      virtual ~SocketTransport();

      virtual armure::Result<std::size_t> read(std::span<std::byte> buffer);
      virtual armure::Result<std::size_t> write(std::span<const std::byte> buffer);

   private:
      // disallow copies
      SocketTransport(const SocketTransport&);
      SocketTransport& operator=(const SocketTransport&);

      chaudiere::Socket* m_socket;
      bool m_socketOwned;
};

}

#endif
