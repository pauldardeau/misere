// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <stdio.h>
#include <stdlib.h>

#include "HttpTransaction.h"
#include "HTTP.h"
#include "Socket.h"
#include "StringTokenizer.h"
#include "BasicException.h"
#include "InvalidKeyException.h"
#include "StrUtils.h"
#include "Logger.h"
#include "CharBuffer.h"

using namespace std;

static const std::string COLON  = ":";
static const std::string EOL_NL = "\n";

using namespace misere;
using namespace chaudiere;

//******************************************************************************

HttpTransaction::HttpTransaction() :
   m_contentLength(0) {
}

//******************************************************************************

HttpTransaction::HttpTransaction(const HttpTransaction& copy) :
   m_vecHeaderLines(copy.m_vecHeaderLines),
   m_vecRequestLineValues(copy.m_vecRequestLineValues),
   m_header(copy.m_header),
   m_body(copy.m_body),
   m_protocol(copy.m_protocol),
   m_requestLine(copy.m_requestLine),
   m_headers(copy.m_headers),
   m_method(copy.m_method),
   m_contentLength(copy.m_contentLength) {
}

//******************************************************************************

HttpTransaction& HttpTransaction::operator=(const HttpTransaction& copy) {
   if (this == &copy) {
      return *this;
   }

   m_vecHeaderLines = copy.m_vecHeaderLines;
   m_vecRequestLineValues = copy.m_vecRequestLineValues;
   m_header = copy.m_header;
   m_body = copy.m_body;
   m_protocol = copy.m_protocol;
   m_requestLine = copy.m_requestLine;
   m_headers = copy.m_headers;
   m_method = copy.m_method;
   m_contentLength = copy.m_contentLength;

   return *this;
}

//******************************************************************************

bool HttpTransaction::parseHeaders() {
   bool parseSuccess = false;

   if (m_vecHeaderLines.empty()) {
      return false;
   }
   
   m_requestLine = m_vecHeaderLines[0];
   StringTokenizer st(m_requestLine);
   const int tokenCount = st.countTokens();
   
   if (3 <= tokenCount) {
      m_vecRequestLineValues.clear();
      m_vecRequestLineValues.reserve(3);
      string thirdValue;
      
      for (int i = 0; i < tokenCount; ++i) {
         if (i > 1) {
            thirdValue += st.nextToken();
         } else {
            m_vecRequestLineValues.push_back(st.nextToken());
         }
      }
      
      m_vecRequestLineValues.push_back(thirdValue);
      size_t numHeaderLines = m_vecHeaderLines.size();
      m_method = m_vecRequestLineValues[0];
      
      for (int i = 1; i < numHeaderLines; ++i) {
         const string& headerLine = m_vecHeaderLines[i];
         const string::size_type posColon = headerLine.find(COLON);
         
         if (std::string::npos != posColon) {
            std::string lowerHeaderKey = headerLine.substr(0, posColon);
            StrUtils::toLowerCase(lowerHeaderKey);
            std::string value = headerLine.substr(posColon + 1);
            StrUtils::trimLeadingSpaces(value);
            m_headers.addPair(lowerHeaderKey, value);
         }
      }
      
      if (hasHeaderValue(HTTP::HTTP_CONTENT_LENGTH)) {
         const std::string& contentLengthAsString =
            getHeaderValue(HTTP::HTTP_CONTENT_LENGTH);
         
         if (!contentLengthAsString.empty()) {
            const int length = StrUtils::parseInt(contentLengthAsString);
            
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

bool HttpTransaction::streamFromSocket(Socket& socket) {
   const bool isLoggingDebug = Logger::isLogging(Debug);
   
   bool streamSuccess = false;
   bool done = false;
   bool inHeader = true;
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
               ::snprintf(msg, 128,
                          "contentLength=%d",
                          contentLength);
               Logger::debug(std::string(msg));
            }

            char small_buffer[1024];
            CharBuffer large_buffer;
            char* buffer;

            if (contentLength > 1023) {
               large_buffer.ensureCapacity(contentLength + 1);
               buffer = large_buffer.data();
            } else {
               buffer = small_buffer;
            }

            try {
               if (socket.read(buffer, contentLength)) {
                  buffer[contentLength] = '\0';
                  
                  if (isLoggingDebug) {
                     Logger::debug(std::string("HttpTransaction (body) socket.read: ") + std::string(buffer));
                  }
                  
                  m_body += buffer;
               }
            } catch(const std::exception& e) {
               Logger::error(string("HTTPTransaction::streamFromSocket exception caught: ") + string(e.what()));
            } catch (...) {
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

const std::string& HttpTransaction::getRawHeader() const {
   return m_header;
}

//******************************************************************************

const std::string& HttpTransaction::getBody() const {
   return m_body;
}

//******************************************************************************

void HttpTransaction::setBody(const std::string& body) {
   m_body = body;
}

//******************************************************************************

bool HttpTransaction::hasHeaderValue(const std::string& headerKey) const {
   string lowerHeaderKey = headerKey;
   StrUtils::toLowerCase(lowerHeaderKey);
   return m_headers.hasKey(lowerHeaderKey);
}

//******************************************************************************

const std::string& HttpTransaction::getHeaderValue(const std::string& headerKey) const {
   string lowerHeaderKey = headerKey;
   StrUtils::toLowerCase(lowerHeaderKey);

   if (m_headers.hasKey(lowerHeaderKey)) {
      return m_headers.getValue(lowerHeaderKey);
   }

   throw InvalidKeyException(headerKey);
}

//******************************************************************************

void HttpTransaction::setHeaderValue(const std::string& key,
                                     const std::string& value) {
   std::string lowerHeaderKey = key;
   StrUtils::toLowerCase(lowerHeaderKey);
   m_headers.addPair(lowerHeaderKey, value);
}

//******************************************************************************

void HttpTransaction::getHeaderKeys(std::vector<std::string>& vecHeaderKeys) const {
   m_headers.getKeys(vecHeaderKeys);
}

//******************************************************************************

const std::string& HttpTransaction::getProtocol() const {
   return m_protocol;
}

//******************************************************************************

const std::string& HttpTransaction::getRequestMethod() const {
   return m_method;
}

//******************************************************************************

const std::string& HttpTransaction::getRequestPath() const {
   return m_vecRequestLineValues[1];
}

//******************************************************************************

void HttpTransaction::setProtocol(const std::string& protocol) {
   m_protocol = protocol;
}

//******************************************************************************

const std::vector<std::string>& HttpTransaction::getRequestLineValues() const {
   return m_vecRequestLineValues;
}

//******************************************************************************

const std::string& HttpTransaction::getRequestLine() const {
   return m_requestLine;
}

//******************************************************************************

void HttpTransaction::populateWithHeaders(KeyValuePairs& headers) {
   headers = m_headers;
}

//******************************************************************************

