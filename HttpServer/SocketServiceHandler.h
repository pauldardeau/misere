// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef C10KServer_SocketServiceHandler_h
#define C10KServer_SocketServiceHandler_h

#include <string>

class SocketRequest;

/*!
 *
 */
class SocketServiceHandler
{
public:
    virtual ~SocketServiceHandler() {}
    virtual void serviceSocket(std::shared_ptr<SocketRequest> socketRequest) = 0;
    virtual const std::string& getName() const = 0;
};


#endif
