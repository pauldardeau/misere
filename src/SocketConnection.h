// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_SOCKETCONNECTION_H
#define MISERE_SOCKETCONNECTION_H

#include <cstddef>

#include "ByteConnection.h"

namespace chaudiere
{
   class Socket;
}

namespace misere
{

/**
 * SocketConnection is the ByteConnection implementation backed by an
 * existing chaudiere::Socket - currently the only ByteConnection
 * implementation. It owns the underlying Socket only when told to
 * (mirroring the ownership convention HttpTransaction already used for
 * chaudiere::Socket* directly), so the same Socket can be wrapped by a
 * SocketConnection more than once - e.g. once per request on a
 * persistent connection - without any one of those wrappers deleting it.
 */
class SocketConnection : public ByteConnection
{
   public:
      /**
       * Constructs a SocketConnection wrapping an existing socket
       * @param socket the socket to read from / write to
       * @param socketOwned whether this SocketConnection should close
       *        and delete the socket when it is destroyed
       */
      explicit SocketConnection(chaudiere::Socket* socket, bool socketOwned=true);

      /**
       * Destructor. Deletes the underlying socket only if socketOwned
       * was true.
       */
      virtual ~SocketConnection();

      virtual int read(char* buffer, int bufferSize);
      virtual bool write(const char* buffer, std::size_t length);
      virtual void close();

   private:
      // disallow copies
      SocketConnection(const SocketConnection&);
      SocketConnection& operator=(const SocketConnection&);

      chaudiere::Socket* m_socket;
      bool m_socketOwned;
};

}

#endif
