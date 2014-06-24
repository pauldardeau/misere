// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__MockSocket__
#define __HttpServer__MockSocket__

#include <string>

#include "Socket.h"


class MockSocket : public Socket
{
public:
   MockSocket(const std::string& payload);
};

#endif /* defined(__HttpServer__MockSocket__) */
