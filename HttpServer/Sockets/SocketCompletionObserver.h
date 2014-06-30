// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef SOCKETCOMPLETIONOBSERVER_H
#define SOCKETCOMPLETIONOBSERVER_H

#include <memory>

class Socket;

/*!
 *
 */
class SocketCompletionObserver
{
public:
   virtual ~SocketCompletionObserver() noexcept {}
   virtual void notifySocketComplete(std::shared_ptr<Socket> socket) noexcept = 0;

};

#endif
