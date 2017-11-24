#include "HttpConnection.h"

using namespace std;
using namespace misere;


HttpConnection* HttpConnection::open(const string& url) {
}

HttpConnection::HttpConnection(const HttpRequest& req) :
   request(req),
   response(NULL) {
}

HttpConnection::~HttpConnection() {
}

void HttpConnection::setRequestMethod(const string& method) {
}

void HttpConnection::setRequestProperty(const string& key,
                                        const string& value) {
}

int HttpConnection::getResponseCode() const {
}

void HttpConnection::disconnect() {
}

