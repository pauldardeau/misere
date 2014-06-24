// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef HTTP_H
#define HTTP_H

#include <string>

//****************************************************************************

/*!
 * HTTP provides constants for working with HTTP protocol.
 */
class HTTP
{
   public:
      // protocol
      static const std::string HTTP_PROTOCOL;

      // methods
      static const std::string HTTP_METHOD_CONNECT;
      static const std::string HTTP_METHOD_DELETE;
      static const std::string HTTP_METHOD_GET;
      static const std::string HTTP_METHOD_HEAD;
      static const std::string HTTP_METHOD_OPTIONS;
      static const std::string HTTP_METHOD_POST;
      static const std::string HTTP_METHOD_PUT;
      static const std::string HTTP_METHOD_TRACE;

      // general header fields
      static const std::string HTTP_CACHE_CONTROL;
      static const std::string HTTP_CONNECTION;
      static const std::string HTTP_DATE;
      static const std::string HTTP_PRAGMA;
      static const std::string HTTP_TRAILER;
      static const std::string HTTP_TRANSFER_ENCODING;
      static const std::string HTTP_UPGRADE;
      static const std::string HTTP_VIA;
      static const std::string HTTP_WARNING;

      // request header fields
      static const std::string HTTP_ACCEPT;
      static const std::string HTTP_ACCEPT_CHARSET;
      static const std::string HTTP_ACCEPT_ENCODING;
      static const std::string HTTP_ACCEPT_LANGUAGE;
      static const std::string HTTP_AUTHORIZATION;
      static const std::string HTTP_EXPECT;
      static const std::string HTTP_FROM;
      static const std::string HTTP_HOST;
      static const std::string HTTP_IF_MATCH;
      static const std::string HTTP_IF_MODIFIED_SINCE;
      static const std::string HTTP_IF_NONE_MATCH;
      static const std::string HTTP_IF_RANGE;
      static const std::string HTTP_IF_UNMODIFIED_SINCE;
      static const std::string HTTP_MAX_FORWARDS;
      static const std::string HTTP_PROXY_AUTHORIZATION;
      static const std::string HTTP_RANGE;
      static const std::string HTTP_REFERER;
      static const std::string HTTP_TE;
      static const std::string HTTP_USER_AGENT;


      // response header fields
      static const std::string HTTP_ACCEPT_RANGES;
      static const std::string HTTP_AGE;
      static const std::string HTTP_ETAG;
      static const std::string HTTP_LOCATION;
      static const std::string HTTP_PROXY_AUTHENTICATE;
      static const std::string HTTP_RETRY_AFTER;
      static const std::string HTTP_SERVER;
      static const std::string HTTP_VARY;
      static const std::string HTTP_WWW_AUTHENTICATE;


      // entity header fields
      static const std::string HTTP_ALLOW;
      static const std::string HTTP_CONTENT_ENCODING;
      static const std::string HTTP_CONTENT_LANGUAGE;
      static const std::string HTTP_CONTENT_LENGTH;
      static const std::string HTTP_CONTENT_LOCATION;
      static const std::string HTTP_CONTENT_MD5;
      static const std::string HTTP_CONTENT_RANGE;
      static const std::string HTTP_CONTENT_TYPE;
      static const std::string HTTP_EXPIRES;
      static const std::string HTTP_LAST_MODIFIED;


      // Informational 1xx
      static const std::string HTTP_RESP_INFO_CONTINUE;
      static const std::string HTTP_RESP_INFO_SWITCH_PROTOCOLS;
      
      
      // Successful 2xx
      static const std::string HTTP_RESP_SUCCESS_OK;
      static const std::string HTTP_RESP_SUCCESS_CREATED;
      static const std::string HTTP_RESP_SUCCESS_ACCEPTED;
      static const std::string HTTP_RESP_SUCCESS_NO_CONTENT;
      static const std::string HTTP_RESP_SUCCESS_RESET_CONTENT;
      static const std::string HTTP_RESP_SUCCESS_PARTIAL_CONTENT;
      
      
      // Redirection 3xx
      static const std::string HTTP_RESP_REDIRECT_MULTIPLE_CHOICES;
      static const std::string HTTP_RESP_REDIRECT_MOVED_PERMANENTLY;
      static const std::string HTTP_RESP_REDIRECT_FOUND;
      static const std::string HTTP_RESP_REDIRECT_SEE_OTHER;
      static const std::string HTTP_RESP_REDIRECT_NOT_MODIFIED;
      static const std::string HTTP_RESP_REDIRECT_USE_PROXY;
      static const std::string HTTP_RESP_REDIRECT_TEMPORARY_REDIRECT;
      
      
      // Client Errors 4xx
      static const std::string HTTP_RESP_CLIENT_ERR_BAD_REQUEST;
      static const std::string HTTP_RESP_CLIENT_ERR_UNAUTHORIZED;
      static const std::string HTTP_RESP_CLIENT_ERR_FORBIDDEN;
      static const std::string HTTP_RESP_CLIENT_ERR_NOT_FOUND;
      static const std::string HTTP_RESP_CLIENT_ERR_METHOD_NOT_ALLOWED;
      static const std::string HTTP_RESP_CLIENT_ERR_NOT_ACCEPTABLE;
      static const std::string HTTP_RESP_CLIENT_ERR_PROXY_AUTH_REQUIRED;
      static const std::string HTTP_RESP_CLIENT_ERR_REQUEST_TIMEOUT;
      static const std::string HTTP_RESP_CLIENT_ERR_CONFLICT;
      static const std::string HTTP_RESP_CLIENT_ERR_GONE;
      static const std::string HTTP_RESP_CLIENT_ERR_LENGTH_REQUIRED;
      static const std::string HTTP_RESP_CLIENT_ERR_PRECONDITION_FAILED;
      static const std::string HTTP_RESP_CLIENT_ERR_REQUEST_TOO_LARGE;
      static const std::string HTTP_RESP_CLIENT_ERR_REQUEST_URI_TOO_LONG;
      static const std::string HTTP_RESP_CLIENT_ERR_REQUEST_UNSUPPORTED_MEDIA;
      static const std::string HTTP_RESP_CLIENT_ERR_REQUESTED_RANGE;
      static const std::string HTTP_RESP_CLIENT_ERR_EXPECTATION_FAILED;
      
      
      // Server Errors 5xx
      static const std::string HTTP_RESP_SERV_ERR_INTERNAL_ERROR;
      static const std::string HTTP_RESP_SERV_ERR_NOT_IMPLEMENTED;
      static const std::string HTTP_RESP_SERV_ERR_BAD_GATEWAY;
      static const std::string HTTP_RESP_SERV_ERR_SERVICE_UNAVAILABLE;
      static const std::string HTTP_RESP_SERV_ERR_GATEWAY_TIMEOUT;
      static const std::string HTTP_RESP_SERV_ERR_HTTP_VERSION_UNSUPPORTED;

};


#endif


