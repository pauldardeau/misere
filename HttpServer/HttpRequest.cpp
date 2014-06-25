// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cstdio>

#include "HttpRequest.h"
#include "Socket.h"
#include "BasicException.h"
#include "StrUtils.h"
#include "StringTokenizer.h"
#include "Logger.h"

static const std::string ENC_DOUBLE_QUOTE  = "%22";   // "
static const std::string ENC_SINGLE_QUOTE  = "%27";   // '
static const std::string ENC_AMPERSAND     = "%26";   // &
static const std::string ENC_PERCENT       = "%25";   // %
static const std::string ENC_ATSIGN        = "%40";   // @
static const std::string ENC_DOLLAR        = "%24";   // $
static const std::string ENC_POUND         = "%23";   // #
static const std::string ENC_EXCLAMATION   = "%21";   // !
static const std::string ENC_TILDE         = "%7E";   // ~
static const std::string ENC_CARET         = "%5E";   // ^
static const std::string ENC_OPEN_PAREN    = "%28";   // (
static const std::string ENC_CLOSE_PAREN   = "%29";   // )
static const std::string ENC_PLUS          = "%2B";   // +
static const std::string ENC_BACKTICK      = "%60";   // `
static const std::string ENC_EQUAL         = "%3D";   // =
static const std::string ENC_OPEN_BRACE    = "%7B";   // {
static const std::string ENC_CLOSE_BRACE   = "%7D";   // }
static const std::string ENC_VERT_BAR      = "%7C";   // |
static const std::string ENC_OPEN_BRACKET  = "%5B";   // [
static const std::string ENC_CLOSE_BRACKET = "%5D";   // ]
static const std::string ENC_BACKSLASH     = "%5C";   //
static const std::string ENC_COLON         = "%3A";   // :
static const std::string ENC_SEMICOLON     = "%3B";   // ;
static const std::string ENC_LESS_THAN     = "%3C";   // <
static const std::string ENC_GREATER_THAN  = "%3E";   // >
static const std::string ENC_QUESTION      = "%3F";   // ?
static const std::string ENC_COMMA         = "%2C";   // ,
static const std::string ENC_SLASH         = "%2F";   // /

static const std::string DOUBLE_QUOTE  = "\"";
static const std::string SINGLE_QUOTE  = "'";
static const std::string AMPERSAND     = "&";
static const std::string PERCENT       = "%";
static const std::string ATSIGN        = "@";
static const std::string DOLLAR        = "$";
static const std::string POUND         = "#";
static const std::string EXCLAMATION   = "!";
static const std::string TILDE         = "~";
static const std::string CARET         = "^";
static const std::string OPEN_PAREN    = "(";
static const std::string CLOSE_PAREN   = ")";
static const std::string PLUS          = "+";
static const std::string BACKTICK      = "`";
static const std::string EQUAL         = "=";   // =
static const std::string OPEN_BRACE    = "{";   // {
static const std::string CLOSE_BRACE   = "}";   // }
static const std::string VERT_BAR      = "|";   // |
static const std::string OPEN_BRACKET  = "[";   // [
static const std::string CLOSE_BRACKET = "]";   // ]
static const std::string BACKSLASH     = "\\";   //
static const std::string COLON         = ":";   // :
static const std::string SEMICOLON     = ";";   // ;
static const std::string LESS_THAN     = "<";   // <
static const std::string GREATER_THAN  = ">";   // >
static const std::string QUESTION      = "?";   // ?
static const std::string COMMA         = ",";   // ,
static const std::string SLASH         = "/";   // /
static const std::string SPACE         = " ";


//******************************************************************************

HttpRequest::HttpRequest(Socket& socket)
{
   Logger::logInstanceCreate("HttpRequest");
   
   streamFromSocket(socket);
}

//******************************************************************************

HttpRequest::HttpRequest(const HttpRequest& copy) noexcept :
   HttpTransaction(copy),
   m_method(copy.m_method),
   m_path(copy.m_path),
   m_arguments(copy.m_arguments)
{
   Logger::logInstanceCreate("HttpRequest");
}

//******************************************************************************

HttpRequest::HttpRequest(HttpRequest&& move) noexcept :
   HttpTransaction(move),
   m_method(std::move(move.m_method)),
   m_path(std::move(move.m_path)),
   m_arguments(std::move(move.m_arguments))
{
   Logger::logInstanceCreate("HttpRequest");
}

//******************************************************************************

HttpRequest::~HttpRequest() noexcept
{
   Logger::logInstanceDestroy("HttpRequest");
}

//******************************************************************************

HttpRequest& HttpRequest::operator=(const HttpRequest& copy) noexcept
{
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

HttpRequest& HttpRequest::operator=(HttpRequest&& move) noexcept
{
   if (this == &move) {
      return *this;
   }
   
   HttpTransaction::operator=(move);
   m_method = std::move(move.m_method);
   m_path = std::move(move.m_path);
   m_arguments = std::move(move.m_arguments);
   
   return *this;
}

//******************************************************************************

bool HttpRequest::streamFromSocket(Socket& socket)
{
   const bool isLoggingDebug = Logger::isLogging(Logger::LogLevel::Debug);
   
   if (isLoggingDebug) {
      Logger::debug("==== start of HttpRequest::streamFromSocket");
   }
   
   bool streamSuccess = false;
   
   if (HttpTransaction::streamFromSocket(socket)) {
      if (isLoggingDebug) {
         Logger::debug("calling getFirstLineValues");
      }
      
      const std::vector<std::string>& vecFirstLineValues = getFirstLineValues();

      if (3 == vecFirstLineValues.size()) {
         m_method = vecFirstLineValues[0];
         m_path = vecFirstLineValues[1];
         setProtocol(vecFirstLineValues[2]);
      } else {
         if (Logger::isLogging(Logger::LogLevel::Warning)) {
            char msg[128];
            std::snprintf(msg, 128, "number of tokens: %lu", vecFirstLineValues.size());
            Logger::warning(std::string(msg));
            
            for (const std::string& s : vecFirstLineValues) {
               Logger::warning(s);
            }
         }
         
         throw BasicException("unable to parse request line");
      }

      if (isLoggingDebug) {
         Logger::debug("HttpRequest: calling parseBody");
      }
      
      parseBody();
      streamSuccess = true;
   } else {
      throw BasicException("unable to parse headers");
   }
   
   return streamSuccess;
}

//******************************************************************************

const std::string& HttpRequest::getRequest() const noexcept
{
   return getFirstLine();
}

//******************************************************************************

const std::string& HttpRequest::getMethod() const noexcept
{
   return m_method;
}

//******************************************************************************

const std::string& HttpRequest::getPath() const noexcept
{
   return m_path;
}

//******************************************************************************

const std::string& HttpRequest::getRequestLine() const noexcept
{
   return getFirstLine();
}

//******************************************************************************

bool HttpRequest::hasArgument(const std::string& key) const noexcept
{
   return m_arguments.hasKey(key);
}

//******************************************************************************

const std::string& HttpRequest::getArgument(const std::string& key) const noexcept
{
   return m_arguments.getValue(key);
}

//******************************************************************************

void HttpRequest::getArgumentKeys(std::vector<std::string>& vecKeys) const noexcept
{
   m_arguments.getKeys(vecKeys);
}

//******************************************************************************

void HttpRequest::parseBody() noexcept
{
   const std::string& body = getBody();
   StringTokenizer st1(body, AMPERSAND);

   const auto numVariables = st1.countTokens();
   std::string pair;
   std::string::size_type posEqual;
   std::string key;
   std::string value;

   for (int i = 0; i < numVariables; ++i) {
      pair = st1.nextToken();

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

      posEqual = pair.find('=');
      if (posEqual != std::string::npos) {
         key = pair.substr(0, posEqual);
         value = pair.substr(posEqual + 1);
         StrUtils::replaceAll(value, ENC_EQUAL, EQUAL);
         m_arguments.addPair(key, value);
      }
   }
}

//******************************************************************************

