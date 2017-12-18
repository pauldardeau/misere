// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HttpTransaction.h"
#include "HTTP.h"
#include "Socket.h"
#include "StringTokenizer.h"
#include "BasicException.h"
#include "InvalidKeyException.h"
#include "StrUtils.h"
#include "Logger.h"
#include "ByteBuffer.h"

using namespace std;

static const std::string COLON  = ":";

using namespace misere;
using namespace chaudiere;

//******************************************************************************

HttpTransaction::HttpTransaction(chaudiere::Socket* socket) :
   m_contentLength(0),
   m_socket(socket) {
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
   m_contentLength(copy.m_contentLength),
   m_socket(NULL) {
}

//******************************************************************************

HttpTransaction::~HttpTransaction() {
   if (m_socket != NULL) {
      delete m_socket;
      m_socket = NULL;
   }
}

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
   if (m_socket != NULL) {
      delete m_socket;
      m_socket = NULL;
   }

   return *this;
}

//******************************************************************************

bool HttpTransaction::parseHeaders() {
   bool parseSuccess = false;

   if (m_vecHeaderLines.empty()) {
      printf("m_vecHeaderLines is empty, parseHeaders returning false\n");
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
      m_protocol = thirdValue;
      
      for (size_t i = 1; i < numHeaderLines; ++i) {
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

   printf("method='%s'\n", m_method.c_str());
   printf("protocol='%s'\n", m_protocol.c_str());  
   printf("content-length=%d\n", m_contentLength); 

   return parseSuccess;
}

//******************************************************************************

const std::string& HttpTransaction::getRawHeader() const {
   return m_header;
}

//******************************************************************************

const ByteBuffer* HttpTransaction::getBody() const {
   return m_body;
}

//******************************************************************************

void HttpTransaction::setBody(const ByteBuffer* body) {
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

void HttpTransaction::close() {
   if (m_socket != NULL) {
      m_socket->close();
      delete m_socket;
      m_socket = NULL;
   }
}

void HttpTransaction::setSocket(Socket* s) {
   m_socket = s;
}

Socket* HttpTransaction::takeSocket() {
   Socket* s = m_socket;
   m_socket = NULL;
   return s;
}

Socket* HttpTransaction::getSocket() {
   return m_socket;
}

void HttpTransaction::addHeader(const std::string& key, const std::string& value) {
   m_headers.addPair(key, value);
}

bool HttpTransaction::hasHeader(const std::string& key) const {
   return m_headers.hasKey(key);
}

int HttpTransaction::getContentLength() const {
   if (hasHeader("content-length")) {
      const string& lengthAsText = m_headers.getValue("content-length");
      return StrUtils::parseInt(lengthAsText);
   } else {
      return -1;
   }
}
