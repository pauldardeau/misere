// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "HttpSocketServiceHandler.h"
#include "Socket.h"
#include "HttpServer.h"
#include "Logger.h"

static const std::string HANDLER_NAME = "HttpSocketServiceHandler";

using namespace misere;
using namespace chaudiere;

//******************************************************************************

HttpSocketServiceHandler::HttpSocketServiceHandler(HttpServer& httpServer) :
   m_httpServer(httpServer)
{
   Logger::logInstanceCreate("HttpSocketServiceHandler");
}

//******************************************************************************

HttpSocketServiceHandler::~HttpSocketServiceHandler()
{
   Logger::logInstanceDestroy("HttpSocketServiceHandler");
}

//******************************************************************************

void HttpSocketServiceHandler::serviceSocket(SocketRequest* socketRequest)
{
   m_httpServer.serviceSocket(socketRequest);
}

//******************************************************************************

const std::string& HttpSocketServiceHandler::getName() const
{
   return HANDLER_NAME;
}

//******************************************************************************

