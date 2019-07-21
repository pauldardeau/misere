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
   m_httpServer(httpServer) {
   LOG_INSTANCE_CREATE("HttpSocketServiceHandler")
}

//******************************************************************************

HttpSocketServiceHandler::~HttpSocketServiceHandler() {
   LOG_INSTANCE_DESTROY("HttpSocketServiceHandler")
}

//******************************************************************************

void HttpSocketServiceHandler::serviceSocket(SocketRequest* socketRequest) {
   m_httpServer.serviceSocket(socketRequest);
}

//******************************************************************************

const std::string& HttpSocketServiceHandler::getName() const {
   return HANDLER_NAME;
}

//******************************************************************************

