#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <string>

#include "HttpRequest.h"
#include "HttpResponse.h"

namespace misere {

class HttpConnection {

private:
   HttpRequest request;
   HttpResponse* response;

   HttpConnection();
   HttpConnection(const HttpConnection& copy);
   HttpConnection& operator=(const HttpConnection& copy);

public:
   static HttpConnection* open(const std::string& url);

   HttpConnection(const HttpRequest& request);
   ~HttpConnection();
   void setRequestMethod(const std::string& method);
   void setRequestProperty(const std::string& key,
                           const std::string& value);
   int getResponseCode() const;
   void disconnect();
};

}

#endif

