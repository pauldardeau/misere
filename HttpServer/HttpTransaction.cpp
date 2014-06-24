// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cstdio>
#include <cstdlib>

#include "HttpTransaction.h"
#include "HTTP.h"
#include "Socket.h"
#include "StringTokenizer.h"
#include "BasicException.h"
#include "InvalidKeyException.h"
#include "StrUtils.h"
#include "Logger.h"

static const std::string COLON  = ":";
static const std::string EOL_NL = "\n";


//******************************************************************************

HttpTransaction::HttpTransaction() noexcept :
   m_contentLength(0)
{
}

//******************************************************************************

HttpTransaction::HttpTransaction(const HttpTransaction& copy) noexcept :
   m_vecHeaderLines(copy.m_vecHeaderLines),
   m_vecFirstLineValues(copy.m_vecFirstLineValues),
   m_header(copy.m_header),
   m_body(copy.m_body),
   m_protocol(copy.m_protocol),
   m_firstLine(copy.m_firstLine),
   m_hashHeaders(copy.m_hashHeaders),
   m_method(copy.m_method),
   m_contentLength(copy.m_contentLength)
{
}

//******************************************************************************

HttpTransaction::HttpTransaction(HttpTransaction&& move) noexcept :
   m_vecHeaderLines(std::move(move.m_vecHeaderLines)),
   m_vecFirstLineValues(std::move(move.m_vecFirstLineValues)),
   m_header(std::move(move.m_header)),
   m_body(std::move(move.m_body)),
   m_protocol(std::move(move.m_protocol)),
   m_firstLine(std::move(move.m_firstLine)),
   m_hashHeaders(std::move(move.m_hashHeaders)),
   m_method(std::move(move.m_method)),
   m_contentLength(move.m_contentLength)
{
}

//******************************************************************************

HttpTransaction& HttpTransaction::operator=(const HttpTransaction& copy) noexcept
{
   if (this == &copy) {
      return *this;
   }

   m_vecHeaderLines = copy.m_vecHeaderLines;
   m_vecFirstLineValues = copy.m_vecFirstLineValues;
   m_header = copy.m_header;
   m_body = copy.m_body;
   m_protocol = copy.m_protocol;
   m_firstLine = copy.m_firstLine;
   m_hashHeaders = copy.m_hashHeaders;
   m_method = copy.m_method;
   m_contentLength = copy.m_contentLength;

   return *this;
}

//******************************************************************************

HttpTransaction& HttpTransaction::operator=(HttpTransaction&& move) noexcept
{
   if (this == &move) {
      return *this;
   }
   
   m_vecHeaderLines = std::move(move.m_vecHeaderLines);
   m_vecFirstLineValues = std::move(move.m_vecFirstLineValues);
   m_header = std::move(move.m_header);
   m_body = std::move(move.m_body);
   m_protocol = std::move(move.m_protocol);
   m_firstLine = std::move(move.m_firstLine);
   m_hashHeaders = std::move(move.m_hashHeaders);
   m_method = std::move(move.m_method);
   m_contentLength = move.m_contentLength;
   
   return *this;
}

//******************************************************************************

bool HttpTransaction::parseHeaders() noexcept
{
   bool parseSuccess = false;
   
   m_firstLine = m_vecHeaderLines[0];
   
   StringTokenizer st(m_firstLine);
   
   const auto tokenCount = st.countTokens();
   
   if (3 <= tokenCount) {
      m_vecFirstLineValues.clear();
      m_vecFirstLineValues.reserve(3);
      
      std::string thirdValue;
      
      for (int i = 0; i < tokenCount; ++i) {
         if (i > 1) {
            thirdValue += st.nextToken();
         } else {
            m_vecFirstLineValues.push_back(st.nextToken());
         }
      }
      
      m_vecFirstLineValues.push_back(thirdValue);
      
      auto numHeaderLines = m_vecHeaderLines.size();
      
      m_method = m_vecFirstLineValues[0];
      
      for (int i = 1; i < numHeaderLines; ++i) {
         const std::string& headerLine = m_vecHeaderLines[i];
         
         const std::string::size_type posColon = headerLine.find(COLON);
         
         if (std::string::npos != posColon) {
            std::string lowerHeaderKey = headerLine.substr(0, posColon);
            StrUtils::toLowerCase(lowerHeaderKey);
            std::string value = headerLine.substr(posColon + 1);
            StrUtils::trimLeadingSpaces(value);
            m_hashHeaders[lowerHeaderKey] = value;
         }
      }
      
      if (hasHeaderValue(HTTP::HTTP_CONTENT_LENGTH)) {
         const std::string& contentLengthAsString = getHeaderValue(HTTP::HTTP_CONTENT_LENGTH);
         
         if (!contentLengthAsString.empty()) {
            const int length = std::stoi(contentLengthAsString);
            
            if (length > 0) {
               m_contentLength = length;
            }
         }
      }
      
      parseSuccess = true;
   }
   
   return parseSuccess;
}

//******************************************************************************

bool HttpTransaction::streamFromSocket(Socket& socket)
{
   const bool isLoggingDebug = Logger::isLogging(Logger::LogLevel::Debug);
   
   bool streamSuccess = false;
   bool done = false;
   bool inHeader = true;
   bool startedReading = false;
   bool headersParsed = false;

   std::string sbInput;
   std::string inputLine;
   std::string method;
   int contentLength = -1;  // unknown

   while (!done) {
      
      if ((contentLength > -1) && (sbInput.length() >= contentLength)) {
         m_body = sbInput;
         done = true;
         continue;
      }

      if (inHeader) {
         if (socket.readLine(inputLine)) {
            startedReading = true;

            if (!inputLine.empty()) {
               sbInput += inputLine;
               sbInput += "\n";

               if (inHeader) {
                  m_vecHeaderLines.push_back(inputLine);
               }
            } else {
               if (inHeader) {
                  inHeader = false;
                  m_header = sbInput;
                  sbInput.erase();
                  streamSuccess = parseHeaders();
                  headersParsed = true;
               } else {
                  m_body = sbInput;
               }

               inHeader = false;

               if (contentLength < 1) {
                  done = true;
               }
            }
         } else {
            done = true;
            m_body = sbInput;
         }
      } else {
         // not in header anymore
         if (contentLength > 0) {
            if (isLoggingDebug) {
               char msg[128];
               std::snprintf(msg, 128, "contentLength=%d", contentLength);
               Logger::debug(std::string(msg));
            }

            try {
               std::unique_ptr<char []> buffer(new char[contentLength + 1]);

               if (socket.read(buffer.get(), contentLength)) {
                  buffer[contentLength] = '\0';
                  
                  if (isLoggingDebug) {
                     Logger::debug(std::string("HttpTransaction (body) socket.read: ") + std::string(buffer.get()));
                  }
                  
                  m_body += buffer.get();
               }
            }
            catch(const std::exception& e)
            {
               Logger::error(std::string("HTTPTransaction::streamFromSocket exception caught: ") + std::string(e.what()));
            }
            catch (...)
            {
               Logger::error("HTTPTransaction::streamFromSocket unknown exception caught");
            }

            done = true;
         } else {
            // I don't think that this code would ever be called
            if (isLoggingDebug) {
               Logger::debug("HttpTransaction: no content-length present, calling readLine in loop");
            }

            done = false;

            while (!done) {
               if (socket.readLine(inputLine)) {
                  m_body += inputLine;
               } else {
                  done = true;
               }
            }
         }
      }
   }
   
   if ((m_method.length() == 0) && !headersParsed) {
      streamSuccess = parseHeaders();
   }
   
   return streamSuccess;
}

//******************************************************************************

const std::string& HttpTransaction::getRawHeader() const noexcept
{
   return m_header;
}

//******************************************************************************

const std::string& HttpTransaction::getBody() const noexcept
{
   return m_body;
}

//******************************************************************************

void HttpTransaction::setBody(const std::string& body) noexcept
{
   m_body = body;
}

//******************************************************************************

bool HttpTransaction::hasHeaderValue(const std::string& headerKey) const noexcept
{
   std::string lowerHeaderKey = headerKey;
   StrUtils::toLowerCase(lowerHeaderKey);
   return (m_hashHeaders.end() != m_hashHeaders.find(lowerHeaderKey));
}

//******************************************************************************

const std::string& HttpTransaction::getHeaderValue(const std::string& headerKey) const
{
   std::string lowerHeaderKey = headerKey;
   StrUtils::toLowerCase(lowerHeaderKey);

   auto it = m_hashHeaders.find(lowerHeaderKey);

   if (m_hashHeaders.end() != it) {
      return (*it).second;
   }

   throw InvalidKeyException(headerKey);
}

//******************************************************************************

void HttpTransaction::setHeaderValue(const std::string& key, const std::string& value) noexcept
{
   std::string lowerHeaderKey = key;
   StrUtils::toLowerCase(lowerHeaderKey);
   m_hashHeaders[lowerHeaderKey] = value;
}

//******************************************************************************

void HttpTransaction::getHeaderValues(std::vector<std::string>& vecHeaderKeys) const noexcept
{
   auto it = m_hashHeaders.begin();
   const auto itEnd = m_hashHeaders.end();

   for ( ; it != itEnd; ++it) {
      vecHeaderKeys.push_back((*it).first);
   }
}

//******************************************************************************

const std::string& HttpTransaction::getProtocol() const noexcept
{
   return m_protocol;
}

//******************************************************************************

void HttpTransaction::setProtocol(const std::string& protocol) noexcept
{
   m_protocol = protocol;
}

//******************************************************************************

const std::vector<std::string>& HttpTransaction::getFirstLineValues() const noexcept
{
   return m_vecFirstLineValues;
}

//******************************************************************************

const std::string& HttpTransaction::getFirstLine() const noexcept
{
   return m_firstLine;
}

//******************************************************************************

void HttpTransaction::populateWithHeaders(std::unordered_map<std::string, std::string>& hashTable) noexcept
{
   for (auto kv : m_hashHeaders) {
      hashTable[kv.first] = kv.second;
   }
}

//******************************************************************************
