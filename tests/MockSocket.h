// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef MISERE_MOCKSOCKET_H
#define MISERE_MOCKSOCKET_H

#include <string>

#include "Socket.h"

namespace misere
{

class MockSocket : public chaudiere::Socket
{
public:
   MockSocket(const std::string& payload);
};

}

#endif
