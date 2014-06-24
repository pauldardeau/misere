// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef SOCKETCOMPLETIONOBSERVER_H
#define SOCKETCOMPLETIONOBSERVER_H

class Socket;

/*!
 *
 */
class SocketCompletionObserver
{
public:
    virtual ~SocketCompletionObserver() noexcept {}
    virtual void notifySocketComplete(Socket* pSocket) noexcept = 0;
};

#endif
