// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <string>

#include "HTTP.h"

using namespace misere;


// protocol
const std::string HTTP::HTTP_PROTOCOL1_0    = "HTTP/1.0";
const std::string HTTP::HTTP_PROTOCOL1_1    = "HTTP/1.1";

// methods
const std::string HTTP::HTTP_METHOD_CONNECT = "CONNECT";
const std::string HTTP::HTTP_METHOD_DELETE  = "DELETE";
const std::string HTTP::HTTP_METHOD_GET     = "GET";
const std::string HTTP::HTTP_METHOD_HEAD    = "HEAD";
const std::string HTTP::HTTP_METHOD_OPTIONS = "OPTIONS";
const std::string HTTP::HTTP_METHOD_POST    = "POST";
const std::string HTTP::HTTP_METHOD_PUT     = "PUT";
const std::string HTTP::HTTP_METHOD_TRACE   = "TRACE";


// general header fields
const std::string HTTP::HTTP_CACHE_CONTROL        = "Cache-Control";
const std::string HTTP::HTTP_CONNECTION           = "Connection";
const std::string HTTP::HTTP_DATE                 = "Date";
const std::string HTTP::HTTP_PRAGMA               = "Pragma";
const std::string HTTP::HTTP_TRAILER              = "Trailer";
const std::string HTTP::HTTP_TRANSFER_ENCODING    = "Transfer-Encoding";
const std::string HTTP::HTTP_UPGRADE              = "Upgrade";
const std::string HTTP::HTTP_VIA                  = "Via";
const std::string HTTP::HTTP_WARNING              = "Warning";

// request header fields
const std::string HTTP::HTTP_ACCEPT               = "Accept";
const std::string HTTP::HTTP_ACCEPT_CHARSET       = "Accept-Charset";
const std::string HTTP::HTTP_ACCEPT_ENCODING      = "Accept-Encoding";
const std::string HTTP::HTTP_ACCEPT_LANGUAGE      = "Accept-Language";
const std::string HTTP::HTTP_AUTHORIZATION        = "Authorization";
const std::string HTTP::HTTP_EXPECT               = "Expect";
const std::string HTTP::HTTP_FROM                 = "From";
const std::string HTTP::HTTP_HOST                 = "Host";
const std::string HTTP::HTTP_IF_MATCH             = "If-Match";
const std::string HTTP::HTTP_IF_MODIFIED_SINCE    = "If-Modified-Since";
const std::string HTTP::HTTP_IF_NONE_MATCH        = "If-None-Match";
const std::string HTTP::HTTP_IF_RANGE             = "If-Range";
const std::string HTTP::HTTP_IF_UNMODIFIED_SINCE  = "If-Unmodified-Since";
const std::string HTTP::HTTP_MAX_FORWARDS         = "Max-Forwards";
const std::string HTTP::HTTP_PROXY_AUTHORIZATION  = "Proxy-Authorization";
const std::string HTTP::HTTP_RANGE                = "Range";
const std::string HTTP::HTTP_REFERER              = "Referer";
const std::string HTTP::HTTP_TE                   = "TE";
const std::string HTTP::HTTP_USER_AGENT           = "User-Agent";


// response header fields
const std::string HTTP::HTTP_ACCEPT_RANGES        = "Accept-Ranges";
const std::string HTTP::HTTP_AGE                  = "Age";
const std::string HTTP::HTTP_ETAG                 = "ETag";
const std::string HTTP::HTTP_LOCATION             = "Location";
const std::string HTTP::HTTP_PROXY_AUTHENTICATE   = "Proxy-Authenticate";
const std::string HTTP::HTTP_RETRY_AFTER          = "Retry-After";
const std::string HTTP::HTTP_SERVER               = "Server";
const std::string HTTP::HTTP_VARY                 = "Vary";
const std::string HTTP::HTTP_WWW_AUTHENTICATE     = "WWW-Authenticate";


// entity header fields
const std::string HTTP::HTTP_ALLOW                = "Allow";
const std::string HTTP::HTTP_CONTENT_ENCODING     = "Content-Encoding";
const std::string HTTP::HTTP_CONTENT_LANGUAGE     = "Content-Language";
const std::string HTTP::HTTP_CONTENT_LENGTH       = "Content-Length";
const std::string HTTP::HTTP_CONTENT_LOCATION     = "Content-Location";
const std::string HTTP::HTTP_CONTENT_MD5          = "Content-MD5";
const std::string HTTP::HTTP_CONTENT_RANGE        = "Content-Range";
const std::string HTTP::HTTP_CONTENT_TYPE         = "Content-Type";
const std::string HTTP::HTTP_EXPIRES              = "Expires";
const std::string HTTP::HTTP_LAST_MODIFIED        = "Last-Modified";


// Informational 1xx
const std::string HTTP::HTTP_RESP_INFO_CONTINUE                        = "100 Continue";
const std::string HTTP::HTTP_RESP_INFO_SWITCH_PROTOCOLS                = "101 Switching Protocols";


// Successful 2xx
const std::string HTTP::HTTP_RESP_SUCCESS_OK                           = "200 OK";
const std::string HTTP::HTTP_RESP_SUCCESS_CREATED                      = "201 Created";
const std::string HTTP::HTTP_RESP_SUCCESS_ACCEPTED                     = "202 Accepted";
const std::string HTTP::HTTP_RESP_SUCCESS_NO_CONTENT                   = "204 No Content";
const std::string HTTP::HTTP_RESP_SUCCESS_RESET_CONTENT                = "205 Reset Content";
const std::string HTTP::HTTP_RESP_SUCCESS_PARTIAL_CONTENT              = "206 Partial Content";


// Redirection 3xx
const std::string HTTP::HTTP_RESP_REDIRECT_MULTIPLE_CHOICES            = "300 Multiple Choices";
const std::string HTTP::HTTP_RESP_REDIRECT_MOVED_PERMANENTLY           = "301 Moved Permanently";
const std::string HTTP::HTTP_RESP_REDIRECT_FOUND                       = "302 Found";
const std::string HTTP::HTTP_RESP_REDIRECT_SEE_OTHER                   = "303 See Other";
const std::string HTTP::HTTP_RESP_REDIRECT_NOT_MODIFIED                = "304 Not Modified";
const std::string HTTP::HTTP_RESP_REDIRECT_USE_PROXY                   = "305 Use Proxy";
const std::string HTTP::HTTP_RESP_REDIRECT_TEMPORARY_REDIRECT          = "307 Temporary Redirect";


// Client Errors 4xx
const std::string HTTP::HTTP_RESP_CLIENT_ERR_BAD_REQUEST               = "400 Bad Request";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_UNAUTHORIZED              = "401 Unauthorized";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_FORBIDDEN                 = "403 Forbidden";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_NOT_FOUND                 = "404 Not Found";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_METHOD_NOT_ALLOWED        = "405 Method Not Allowed";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_NOT_ACCEPTABLE            = "406 Not Acceptable";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_PROXY_AUTH_REQUIRED       = "407 Proxy Authentication Required";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_REQUEST_TIMEOUT           = "408 Request Timeout";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_CONFLICT                  = "409 Conflict";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_GONE                      = "410 Gone";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_LENGTH_REQUIRED           = "411 Length Required";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_PRECONDITION_FAILED       = "412 Precondition Failed";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_REQUEST_TOO_LARGE         = "413 Request Entity Too Large";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_REQUEST_URI_TOO_LONG      = "414 Request-URI Too Long";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_REQUEST_UNSUPPORTED_MEDIA = "415 Unsupported Media Type";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_REQUESTED_RANGE           = "416 Requested Range Not Satisfiable";
const std::string HTTP::HTTP_RESP_CLIENT_ERR_EXPECTATION_FAILED        = "417 Expectation Failed";


// Server Errors 5xx
const std::string HTTP::HTTP_RESP_SERV_ERR_INTERNAL_ERROR              = "500 Internal Server Error";
const std::string HTTP::HTTP_RESP_SERV_ERR_NOT_IMPLEMENTED             = "501 Not Implemented";
const std::string HTTP::HTTP_RESP_SERV_ERR_BAD_GATEWAY                 = "502 Bad Gateway";
const std::string HTTP::HTTP_RESP_SERV_ERR_SERVICE_UNAVAILABLE         = "503 Service Unavailable";
const std::string HTTP::HTTP_RESP_SERV_ERR_GATEWAY_TIMEOUT             = "504 Gateway Timeout";
const std::string HTTP::HTTP_RESP_SERV_ERR_HTTP_VERSION_UNSUPPORTED    = "505 HTTP Version Not Supported";


//******************************************************************************

std::string HTTP::responseLineForStatusCode(int statusCode) {
   switch (statusCode) {
      case 100: return HTTP_RESP_INFO_CONTINUE;
      case 101: return HTTP_RESP_INFO_SWITCH_PROTOCOLS;

      case 200: return HTTP_RESP_SUCCESS_OK;
      case 201: return HTTP_RESP_SUCCESS_CREATED;
      case 202: return HTTP_RESP_SUCCESS_ACCEPTED;
      case 204: return HTTP_RESP_SUCCESS_NO_CONTENT;
      case 205: return HTTP_RESP_SUCCESS_RESET_CONTENT;
      case 206: return HTTP_RESP_SUCCESS_PARTIAL_CONTENT;

      case 300: return HTTP_RESP_REDIRECT_MULTIPLE_CHOICES;
      case 301: return HTTP_RESP_REDIRECT_MOVED_PERMANENTLY;
      case 302: return HTTP_RESP_REDIRECT_FOUND;
      case 303: return HTTP_RESP_REDIRECT_SEE_OTHER;
      case 304: return HTTP_RESP_REDIRECT_NOT_MODIFIED;
      case 305: return HTTP_RESP_REDIRECT_USE_PROXY;
      case 307: return HTTP_RESP_REDIRECT_TEMPORARY_REDIRECT;

      case 400: return HTTP_RESP_CLIENT_ERR_BAD_REQUEST;
      case 401: return HTTP_RESP_CLIENT_ERR_UNAUTHORIZED;
      case 403: return HTTP_RESP_CLIENT_ERR_FORBIDDEN;
      case 404: return HTTP_RESP_CLIENT_ERR_NOT_FOUND;
      case 405: return HTTP_RESP_CLIENT_ERR_METHOD_NOT_ALLOWED;
      case 406: return HTTP_RESP_CLIENT_ERR_NOT_ACCEPTABLE;
      case 407: return HTTP_RESP_CLIENT_ERR_PROXY_AUTH_REQUIRED;
      case 408: return HTTP_RESP_CLIENT_ERR_REQUEST_TIMEOUT;
      case 409: return HTTP_RESP_CLIENT_ERR_CONFLICT;
      case 410: return HTTP_RESP_CLIENT_ERR_GONE;
      case 411: return HTTP_RESP_CLIENT_ERR_LENGTH_REQUIRED;
      case 412: return HTTP_RESP_CLIENT_ERR_PRECONDITION_FAILED;
      case 413: return HTTP_RESP_CLIENT_ERR_REQUEST_TOO_LARGE;
      case 414: return HTTP_RESP_CLIENT_ERR_REQUEST_URI_TOO_LONG;
      case 415: return HTTP_RESP_CLIENT_ERR_REQUEST_UNSUPPORTED_MEDIA;
      case 416: return HTTP_RESP_CLIENT_ERR_REQUESTED_RANGE;
      case 417: return HTTP_RESP_CLIENT_ERR_EXPECTATION_FAILED;

      case 500: return HTTP_RESP_SERV_ERR_INTERNAL_ERROR;
      case 501: return HTTP_RESP_SERV_ERR_NOT_IMPLEMENTED;
      case 502: return HTTP_RESP_SERV_ERR_BAD_GATEWAY;
      case 503: return HTTP_RESP_SERV_ERR_SERVICE_UNAVAILABLE;
      case 504: return HTTP_RESP_SERV_ERR_GATEWAY_TIMEOUT;
      case 505: return HTTP_RESP_SERV_ERR_HTTP_VERSION_UNSUPPORTED;

      default: return std::to_string(statusCode);
   }
}

//******************************************************************************
//******************************************************************************


