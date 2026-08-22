// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_BYTECONNECTION_H
#define MISERE_BYTECONNECTION_H

#include <cstddef>

namespace misere
{

/**
 * ByteConnection is the minimal bidirectional byte-I/O interface used by
 * the HTTP layer (HttpTransaction and its subclasses) to read and write
 * bytes, independent of what actually carries them - a chaudiere::Socket
 * today (see SocketConnection), potentially a TLS-backed connection
 * later. Only the operations the HTTP layer genuinely needs are exposed
 * here; connection setup (TCP options, buffer sizes, etc.) is
 * deliberately left to whatever constructs a concrete implementation
 * rather than being part of this interface.
 */
class ByteConnection
{
   public:
      virtual ~ByteConnection() {}

      /**
       * Reads whatever data is currently available, up to the size of
       * the buffer. Blocks until at least one byte has arrived (or the
       * connection closes/errors) - never busy-waits and never blocks
       * waiting for the buffer to fill completely.
       * @param buffer the buffer to receive the read bytes
       * @param bufferSize the size of the buffer (the maximum to read)
       * @return the number of bytes read, or a non-positive value on
       *         orderly close/error
       */
      virtual int read(char* buffer, int bufferSize) = 0;

      /**
       * Writes the specified buffer in its entirety.
       * @param buffer the buffer to write from
       * @param length the number of bytes to write
       * @return boolean indicating whether the write succeeded
       */
      virtual bool write(const char* buffer, std::size_t length) = 0;

      /**
       * Closes the connection.
       */
      virtual void close() = 0;
};

}

#endif
