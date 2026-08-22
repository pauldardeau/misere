// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_MOCKSOCKET_H
#define MISERE_MOCKSOCKET_H

#include <string>

#include "Socket.h"

namespace misere {

class MockSocket : public chaudiere::Socket {
public:
   MockSocket(const std::string& payload);

   /**
    * Re-primes the socket with a further payload, simulating a second
    * request arriving on a persistent (keep-alive) connection only after
    * the first has been fully consumed.
    * @param payload the payload to serve on subsequent reads
    */
   void setNextPayload(const std::string& payload);
};

}

#endif
