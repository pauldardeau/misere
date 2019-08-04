// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <stdio.h>

#include "HttpRequest.h"
#include "Socket.h"
#include "BasicException.h"
#include "StrUtils.h"
#include "StringTokenizer.h"
#include "Logger.h"
#include "HTTP.h"

using namespace std;

static const string ENC_DOUBLE_QUOTE  = "%22";   // "
static const string ENC_SINGLE_QUOTE  = "%27";   // '
static const string ENC_AMPERSAND     = "%26";   // &
static const string ENC_PERCENT       = "%25";   // %
static const string ENC_ATSIGN        = "%40";   // @
static const string ENC_DOLLAR        = "%24";   // $
static const string ENC_POUND         = "%23";   // #
static const string ENC_EXCLAMATION   = "%21";   // !
static const string ENC_TILDE         = "%7E";   // ~
static const string ENC_CARET         = "%5E";   // ^
static const string ENC_OPEN_PAREN    = "%28";   // (
static const string ENC_CLOSE_PAREN   = "%29";   // )
static const string ENC_PLUS          = "%2B";   // +
static const string ENC_BACKTICK      = "%60";   // `
static const string ENC_EQUAL         = "%3D";   // =
static const string ENC_OPEN_BRACE    = "%7B";   // {
static const string ENC_CLOSE_BRACE   = "%7D";   // }
static const string ENC_VERT_BAR      = "%7C";   // |
static const string ENC_OPEN_BRACKET  = "%5B";   // [
static const string ENC_CLOSE_BRACKET = "%5D";   // ]
static const string ENC_BACKSLASH     = "%5C";   //
static const string ENC_COLON         = "%3A";   // :
static const string ENC_SEMICOLON     = "%3B";   // ;
static const string ENC_LESS_THAN     = "%3C";   // <
static const string ENC_GREATER_THAN  = "%3E";   // >
static const string ENC_QUESTION      = "%3F";   // ?
static const string ENC_COMMA         = "%2C";   // ,
static const string ENC_SLASH         = "%2F";   // /

static const string DOUBLE_QUOTE  = "\"";
static const string SINGLE_QUOTE  = "'";
static const string AMPERSAND     = "&";
static const string PERCENT       = "%";
static const string ATSIGN        = "@";
static const string DOLLAR        = "$";
static const string POUND         = "#";
static const string EXCLAMATION   = "!";
static const string TILDE         = "~";
static const string CARET         = "^";
static const string OPEN_PAREN    = "(";
static const string CLOSE_PAREN   = ")";
static const string PLUS          = "+";
static const string BACKTICK      = "`";
static const string EQUAL         = "=";   // =
static const string OPEN_BRACE    = "{";   // {
static const string CLOSE_BRACE   = "}";   // }
static const string VERT_BAR      = "|";   // |
static const string OPEN_BRACKET  = "[";   // [
static const string CLOSE_BRACKET = "]";   // ]
static const string BACKSLASH     = "\\";   //
static const string COLON         = ":";   // :
static const string SEMICOLON     = ";";   // ;
static const string LESS_THAN     = "<";   // <
static const string GREATER_THAN  = ">";   // >
static const string QUESTION      = "?";   // ?
static const string COMMA         = ",";   // ,
static const string SLASH         = "/";   // /
static const string SPACE         = " ";

static const string EOL = "\r\n";

using namespace misere;
using namespace chaudiere;

//******************************************************************************

HttpRequest* HttpRequest::create(const Url& url) {
   return new HttpRequest(url);
}

//******************************************************************************

HttpRequest* HttpRequest::create(const string& urlText) {
   Url url(urlText);
   return new HttpRequest(url);
}

//******************************************************************************

HttpRequest::HttpRequest(const Url& url) :
   m_initialized(false),
   m_url(url) {

   LOG_INSTANCE_CREATE("HttpRequest")

   int port = url.port();
   if (port == 0) {
      port = 80;
   }

   m_path = url.path();

   Socket* s = new Socket(url.host(), port);
   if (s->isOpen()) {
      setSocket(s, true);
   } else {
      delete s;
      //TODO: throw exception
   }
}

//******************************************************************************

HttpRequest::HttpRequest(Socket* socket, bool socketOwned) :
   HttpTransaction(socket, socketOwned),
   m_initialized(false) {

   LOG_INSTANCE_CREATE("HttpRequest")
   m_initialized = streamFromSocket();
}

//******************************************************************************

HttpRequest::HttpRequest(const HttpRequest& copy) :
   HttpTransaction(copy),
   m_method(copy.m_method),
   m_path(copy.m_path),
   m_arguments(copy.m_arguments),
   m_initialized(copy.m_initialized) {
   LOG_INSTANCE_CREATE("HttpRequest")
}

//******************************************************************************

HttpRequest::~HttpRequest() {
   LOG_INSTANCE_DESTROY("HttpRequest")
}

//******************************************************************************

HttpRequest& HttpRequest::operator=(const HttpRequest& copy) {
   if (this == &copy) {
      return *this;
   }

   HttpTransaction::operator=(copy);
   m_method = copy.m_method;
   m_path = copy.m_path;
   m_arguments = copy.m_arguments;

   return *this;
}

//******************************************************************************

bool HttpRequest::streamFromSocket() {
   bool streamSuccess = false;
  
   if (HttpTransaction::streamFromSocket()) {
      const string& firstLine = getFirstHeaderLine(); 
      StringTokenizer st(firstLine, " ");
      vector<string> reqLineValues;
      if (st.countTokens() != 3) {
         //throw BasicException("unable to parse headers");
         return false;
      }

      reqLineValues.push_back(st.nextToken());
      reqLineValues.push_back(st.nextToken());
      reqLineValues.push_back(st.nextToken());

      m_method = reqLineValues[0];
      m_path = reqLineValues[1];
      setProtocol(reqLineValues[2]);

      setRequestLineValues(reqLineValues);

      streamSuccess = true;
   } else {
      throw BasicException("unable to parse headers");
   }
   
   return streamSuccess;
}

//******************************************************************************

bool HttpRequest::isInitialized() const {
   return m_initialized;
}

//******************************************************************************

const string& HttpRequest::getRequest() const {
   return getFirstHeaderLine();
}

//******************************************************************************

const string& HttpRequest::getMethod() const {
   return m_method;
}

//******************************************************************************

const string& HttpRequest::getPath() const {
   return m_path;
}

//******************************************************************************

bool HttpRequest::hasArgument(const string& key) const {
   return m_arguments.hasKey(key);
}

//******************************************************************************

const string& HttpRequest::getArgument(const string& key) const {
   return m_arguments.getValue(key);
}

//******************************************************************************

void HttpRequest::getArgumentKeys(vector<string>& vecKeys) const {
   m_arguments.getKeys(vecKeys);
}

//******************************************************************************
/*
void HttpRequest::parseBody() {
   const string& body = getBody();
   
   if (!body.empty() && StrUtils::containsString(body, AMPERSAND)) {
      StringTokenizer st1(body, AMPERSAND);

      while (st1.hasMoreTokens()) {
         string pair(st1.nextToken());

         StrUtils::replaceAll(pair, PLUS, SPACE);
         StrUtils::replaceAll(pair, ENC_DOUBLE_QUOTE, DOUBLE_QUOTE);
         StrUtils::replaceAll(pair, ENC_SINGLE_QUOTE, SINGLE_QUOTE);
         StrUtils::replaceAll(pair, ENC_AMPERSAND, AMPERSAND);
         StrUtils::replaceAll(pair, ENC_PERCENT, PERCENT);
         StrUtils::replaceAll(pair, ENC_ATSIGN, ATSIGN);
         StrUtils::replaceAll(pair, ENC_DOLLAR, DOLLAR);
         StrUtils::replaceAll(pair, ENC_POUND, POUND);
         StrUtils::replaceAll(pair, ENC_EXCLAMATION, EXCLAMATION);
         StrUtils::replaceAll(pair, ENC_TILDE, TILDE);
         StrUtils::replaceAll(pair, ENC_CARET, CARET);
         StrUtils::replaceAll(pair, ENC_OPEN_PAREN, OPEN_PAREN);
         StrUtils::replaceAll(pair, ENC_CLOSE_PAREN, CLOSE_PAREN);
         StrUtils::replaceAll(pair, ENC_PLUS, PLUS);
         StrUtils::replaceAll(pair, ENC_BACKTICK, BACKTICK);
         StrUtils::replaceAll(pair, ENC_OPEN_BRACE, OPEN_BRACE);
         StrUtils::replaceAll(pair, ENC_CLOSE_BRACE, CLOSE_BRACE);
         StrUtils::replaceAll(pair, ENC_VERT_BAR, VERT_BAR);
         StrUtils::replaceAll(pair, ENC_OPEN_BRACKET, OPEN_BRACKET);
         StrUtils::replaceAll(pair, ENC_CLOSE_BRACKET, CLOSE_BRACKET);
         StrUtils::replaceAll(pair, ENC_BACKSLASH, BACKSLASH);
         StrUtils::replaceAll(pair, ENC_COLON, COLON);
         StrUtils::replaceAll(pair, ENC_SEMICOLON, SEMICOLON);
         StrUtils::replaceAll(pair, ENC_LESS_THAN, LESS_THAN);
         StrUtils::replaceAll(pair, ENC_GREATER_THAN, GREATER_THAN);
         StrUtils::replaceAll(pair, ENC_QUESTION, QUESTION);
         StrUtils::replaceAll(pair, ENC_COMMA, COMMA);
         StrUtils::replaceAll(pair, ENC_SLASH, SLASH);

         const string::size_type posEqual = pair.find('=');
         if (posEqual != string::npos) {
            const string& key = pair.substr(0, posEqual);
            string value(pair.substr(posEqual + 1));
            StrUtils::replaceAll(value, ENC_EQUAL, EQUAL);
            m_arguments.addPair(key, value);
         }
      }
   }
}
*/
//******************************************************************************

bool HttpRequest::hasAccept() const {
   return hasHeaderValue(HTTP::HTTP_ACCEPT);
}

//******************************************************************************

bool HttpRequest::hasAcceptEncoding() const {
   return hasHeaderValue(HTTP::HTTP_ACCEPT_ENCODING);
}

//******************************************************************************

bool HttpRequest::hasAcceptLanguage() const {
   return hasHeaderValue(HTTP::HTTP_ACCEPT_LANGUAGE);
}

//******************************************************************************

bool HttpRequest::hasConnection() const {
   return hasHeaderValue(HTTP::HTTP_CONNECTION);
}

//******************************************************************************

bool HttpRequest::hasDNT() const {
   return hasHeaderValue("dnt");
}

//******************************************************************************

bool HttpRequest::hasHost() const {
   return hasHeaderValue(HTTP::HTTP_HOST);
}

//******************************************************************************

bool HttpRequest::hasUserAgent() const {
   return hasHeaderValue(HTTP::HTTP_USER_AGENT);
}

//******************************************************************************

const string& HttpRequest::getAccept() const {
   return getHeaderValue(HTTP::HTTP_ACCEPT);
}

//******************************************************************************

const string& HttpRequest::getAcceptEncoding() const {
   return getHeaderValue(HTTP::HTTP_ACCEPT_ENCODING);
}

//******************************************************************************

const string& HttpRequest::getAcceptLanguage() const {
   return getHeaderValue(HTTP::HTTP_ACCEPT_LANGUAGE);
}

//******************************************************************************

const string& HttpRequest::getConnection() const {
   return getHeaderValue(HTTP::HTTP_CONNECTION);
}

//******************************************************************************

const string& HttpRequest::getDNT() const {
   return getHeaderValue("dnt");
}

//******************************************************************************

const string& HttpRequest::getHost() const {
   return getHeaderValue(HTTP::HTTP_HOST);
}

//******************************************************************************

const string& HttpRequest::getUserAgent() const {
   return getHeaderValue(HTTP::HTTP_USER_AGENT);
}

//******************************************************************************

void HttpRequest::setMethod(const string& method) {
   m_method = method;
}

//******************************************************************************

void HttpRequest::setHeaderValue(const string& key,
                                 const string& value) {
   m_arguments.addPair(key, value);
}

//******************************************************************************

HttpResponse* HttpRequest::getResponse() {
   return new HttpResponse(takeSocket());
}

//******************************************************************************

bool HttpRequest::write(chaudiere::Socket* s) {
   return write(s, -1L);
}

//******************************************************************************

bool HttpRequest::write(chaudiere::Socket* s, long bodyLength) {
   bool success = false;
   if (s != nullptr) {
      const string& method = getMethod();
      const string& path = getPath();
      if (method.length() == 0) {
         return false;
      }

      if (path.length() == 0) {
         return false;
      }

      if (bodyLength >= 0) {
         setHeaderValue(HTTP::HTTP_CONTENT_LENGTH,
                        StrUtils::toString(bodyLength));
      }

      string headers;
      headers += method;
      headers += " ";
      headers += path;
      headers += " ";
      headers += "HTTP/1.1";
      headers += EOL;

      vector<string> headerKeys;
      m_arguments.getKeys(headerKeys);

      for (const string& headerKey : headerKeys) {
         headers += headerKey;
         headers += ": ";
         headers += m_arguments.getValue(headerKey);
         headers += EOL;
      }
      
      headers += EOL;

      success = s->write(headers);
   }

   return success;
}

//******************************************************************************
