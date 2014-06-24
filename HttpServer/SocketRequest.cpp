// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "SocketRequest.h"
#include "Socket.h"
#include "SocketServiceHandler.h"
#include "Logger.h"
#include "BasicException.h"

//******************************************************************************

SocketRequest::SocketRequest(Socket* socket,
                             SocketServiceHandler* handler) noexcept :
   Runnable(true),
   m_socket(socket),
   m_handler(handler)
{
   Logger::logInstanceCreate("SocketRequest");
}

//******************************************************************************

SocketRequest::~SocketRequest() noexcept
{
   Logger::logInstanceDestroy("SocketRequest");
   
   if (m_socket) {
      delete m_socket;
   }
}

//******************************************************************************

void SocketRequest::run()
{
   if (Logger::isLogging(Logger::LogLevel::Debug)) {
      char msg[128];
      std::snprintf(msg, 128, "request for socket fd=%d",
               m_socket->getFileDescriptor());
      Logger::debug(std::string(msg));
   }

   if (m_handler) {
      try
      {
         m_handler->serviceSocket(this);
      }
      catch (const BasicException& be)
      {
         Logger::error("exception in serviceSocket on handler: " + be.whatString());
      }
      catch (const std::exception& e)
      {
         Logger::error("exception in serviceSocket on handler: " + std::string(e.what()));
      }
      catch (...)
      {
         Logger::error("exception in serviceSocket on handler");
      }
   } else {
      Logger::error("no handler present in SocketRequest");
   }
    
   //m_socket->requestComplete();
}

//******************************************************************************

int SocketRequest::getSocketFD() const noexcept
{
   return m_socket->getFileDescriptor();
}

//******************************************************************************

Socket* SocketRequest::getSocket() noexcept
{
   return m_socket;
}

//******************************************************************************

void SocketRequest::requestComplete() noexcept
{
   if (nullptr != m_socket) {
      m_socket->requestComplete();
   }
}

//******************************************************************************
