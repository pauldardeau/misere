#include "HttpConnection.h"

using namespace std;
using namespace misere;


HttpConnection* HttpConnection::open(const string& url) {
   //TODO: implement HttpConnection::open
   return nullptr;
}

HttpConnection::HttpConnection(const HttpRequest& req) :
   request(req),
   response(nullptr) {
}

HttpConnection::~HttpConnection() {
   //TODO: implement HttpConnection::~HttpConnection
}

void HttpConnection::setRequestMethod(const string& method) {
   //TODO: implement HttpConnection::setRequestMethod
}

void HttpConnection::setRequestProperty(const string& key,
                                        const string& value) {
   //TODO: implement HttpConnection::setRequestProperty
}

int HttpConnection::getResponseCode() const {
   //TODO: implement HttpConnection::getResponseCode
   return 0;
}

void HttpConnection::disconnect() {
   //TODO: implement HttpConnection::disconnect
}

