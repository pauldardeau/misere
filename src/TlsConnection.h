// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_TLSCONNECTION_H
#define MISERE_TLSCONNECTION_H

#include <cstddef>

#include "ByteConnection.h"
#include "armure/Connection.h"

namespace chaudiere
{
   class Socket;
}

namespace misere
{

/**
 * TlsConnection is the ByteConnection implementation backed by an
 * armure::Connection - the HTTP layer sees only plaintext bytes;
 * armure performs the TLS encryption/decryption, driven over whatever
 * armure::Transport the Connection was created with (see
 * SocketTransport for the one that adapts a chaudiere::Socket).
 *
 * armure::Connection's read()/write()/handshake() can each report
 * ErrorCode::WantRead/WantWrite - a normal, expected retry signal for a
 * caller prepared to call again, not a failure. ByteConnection's own
 * contract has no such retry signal (it mirrors chaudiere::Socket's
 * always-blocking, always-definitive read()/write()), so TlsConnection
 * absorbs the retrying itself: every armure call below is wrapped in a
 * loop that retries immediately on WantRead/WantWrite and only returns
 * once armure reports something else. This does not busy-wait in
 * practice for a SocketTransport-backed Connection specifically, because
 * SocketTransport's own read()/write() block until they have a
 * definitive outcome (see SocketTransport.h) - so a retried armure call
 * either makes real progress or blocks again waiting for it, the same
 * blocking behavior every other misere ByteConnection implementation
 * already has. A Connection built over a genuinely non-blocking
 * Transport would make this loop spin - TlsConnection is not meant to be
 * used with one.
 */
class TlsConnection : public ByteConnection
{
   public:
      /**
       * Wraps an already-constructed armure::Connection. Drives its
       * handshake to completion (retrying internally on
       * ErrorCode::WantRead/WantWrite - see the class-level note above)
       * before returning, so that by the time a TlsConnection exists,
       * the HTTP layer can safely begin reading/writing plaintext.
       * @param connection an armure::Connection - handshake not required
       *        to have been started yet
       * @param socket the chaudiere::Socket the connection's underlying
       *        SocketTransport was constructed from, used only so
       *        close() can also close the underlying socket -
       *        armure::Connection has no way to reach back into its own
       *        Transport (see close()). Never owned or deleted by
       *        TlsConnection regardless of what is passed here; pass
       *        nullptr if there is no real socket (e.g. a Connection
       *        built over a test Transport).
       * @throw chaudiere::BasicException if the handshake fails
       */
      explicit TlsConnection(armure::Connection connection, chaudiere::Socket* socket=nullptr);

      /**
       * Destructor. Releases armure's TLS resources (see
       * armure::Connection's own destructor) and never touches socket.
       */
      virtual ~TlsConnection();

      virtual int read(char* buffer, int bufferSize);
      virtual bool write(const char* buffer, std::size_t length);

      /**
       * Sends a TLS close_notify (best-effort - see the .cpp for why
       * this can't report failure) and then, if a socket was supplied to
       * the constructor, closes it too - matching SocketConnection::
       * close()'s behavior of always closing the underlying socket
       * regardless of ownership.
       */
      virtual void close();

   private:
      // disallow copies
      TlsConnection(const TlsConnection&);
      TlsConnection& operator=(const TlsConnection&);

      armure::Connection m_connection;
      chaudiere::Socket* m_socket;
};

}

#endif
