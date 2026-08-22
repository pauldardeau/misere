// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <utility>

#include "HttpTransaction.h"
#include "HTTP.h"
#include "ByteConnection.h"
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

HttpTransaction::HttpTransaction(ByteConnection* connection, bool connectionOwned, std::string leadingBytes) :
   m_body(nullptr),
   m_contentLength(0),
   m_connection(connection),
   m_connectionOwned(connectionOwned),
   m_unconsumedBytes(std::move(leadingBytes)) {
}

//******************************************************************************

HttpTransaction::HttpTransaction(const HttpTransaction& copy) :
   m_vecHeaderLines(copy.m_vecHeaderLines),
   m_vecRequestLineValues(copy.m_vecRequestLineValues),
   m_header(copy.m_header),
   m_body(nullptr),
   m_protocol(copy.m_protocol),
   m_firstHeaderLine(copy.m_firstHeaderLine),
   m_headers(copy.m_headers),
   m_method(copy.m_method),
   m_contentLength(copy.m_contentLength),
   m_connection(nullptr),
   m_connectionOwned(false),
   m_unconsumedBytes() {
}

//******************************************************************************

HttpTransaction::~HttpTransaction() {
   if ((m_connection != nullptr) && m_connectionOwned) {
      delete m_connection;
   }
}

//*****************************************************************************

HttpTransaction& HttpTransaction::operator=(const HttpTransaction& copy) {
   if (this == &copy) {
      return *this;
   }

   m_vecHeaderLines = copy.m_vecHeaderLines;
   m_vecRequestLineValues = copy.m_vecRequestLineValues;
   m_header = copy.m_header;
   //m_body = copy.m_body;
   m_protocol = copy.m_protocol;
   m_firstHeaderLine = copy.m_firstHeaderLine;
   m_headers = copy.m_headers;
   m_method = copy.m_method;
   m_contentLength = copy.m_contentLength;
   if (m_connection != nullptr) {
      if (m_connectionOwned) {
         delete m_connection;
      }
      m_connection = nullptr;
      m_connectionOwned = false;
   }
   if (!copy.m_connectionOwned) {
      m_connectionOwned = false;
   }
   m_unconsumedBytes.clear();

   return *this;
}

//******************************************************************************

bool HttpTransaction::parseHeaders() {
   bool parseSuccess = false;

   if (m_vecHeaderLines.empty()) {
      return false;
   }

   m_firstHeaderLine = m_vecHeaderLines[0];
   std::vector<std::string> vecTokens = StrUtils::split(m_firstHeaderLine, " ");
   const int tokenCount = vecTokens.size();

   if (3 <= tokenCount) {
      m_vecRequestLineValues.clear();
      m_vecRequestLineValues.reserve(3);
      string thirdValue;

      for (int i = 0; i < tokenCount; ++i) {
         if (i > 1) {
            thirdValue += vecTokens[i];
         } else {
            m_vecRequestLineValues.push_back(vecTokens[i]);
         }
      }

      m_vecRequestLineValues.push_back(thirdValue);
      m_method = m_vecRequestLineValues[0];
      m_protocol = thirdValue;

      for (const auto& headerLine : m_vecHeaderLines) {
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

const std::string& HttpTransaction::getRawHeader() const {
   return m_header;
}

//******************************************************************************

const chaudiere::ByteBuffer* HttpTransaction::getBody() const {
   return m_body.get();
}

//******************************************************************************

ByteBuffer* HttpTransaction::takeBody() {
   return m_body.release();
}

//******************************************************************************

void HttpTransaction::setBody(ByteBuffer* body) {
   m_body.reset(body);
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

void HttpTransaction::setRequestLineValues(const std::vector<std::string>& requestLineValues) {
   m_vecRequestLineValues = requestLineValues;
}

//******************************************************************************

const std::string& HttpTransaction::getFirstHeaderLine() const {
   return m_firstHeaderLine;
}

//******************************************************************************

void HttpTransaction::populateWithHeaders(KeyValuePairs& headers) {
   std::vector<std::string> keys;
   m_headers.getKeys(keys);

   for (const auto& key : keys) {
      headers.addPair(key, m_headers.getValue(key));
   }
}

//******************************************************************************

void HttpTransaction::close() {
   if (m_connection != nullptr) {
      m_connection->close();
      if (m_connectionOwned) {
         delete m_connection;
      }
      m_connection = nullptr;
      m_connectionOwned = false;
   }
}

//*****************************************************************************

void HttpTransaction::setConnection(ByteConnection* c, bool connectionOwned) {
   m_connection = c;
   m_connectionOwned = connectionOwned;
}

//*****************************************************************************

ByteConnection* HttpTransaction::takeConnection() {
   ByteConnection* c = m_connection;
   m_connection = nullptr;
   m_connectionOwned = false;
   return c;
}

//*****************************************************************************

ByteConnection* HttpTransaction::getConnection() {
   return m_connection;
}

//*****************************************************************************

void HttpTransaction::addHeader(const std::string& key, const std::string& value) {
   m_headers.addPair(key, value);
}

//*****************************************************************************

bool HttpTransaction::hasHeader(const std::string& key) const {
   return m_headers.hasKey(key);
}

//*****************************************************************************

int HttpTransaction::getContentLength() const {
   if (hasHeader("content-length")) {
      const string& lengthAsText = m_headers.getValue("content-length");
      return StrUtils::parseInt(lengthAsText);
   } else {
      return -1;
   }
}

//*****************************************************************************

bool HttpTransaction::streamFromConnection() {
   int contentLength = -1;  // unknown
   int bytes_read;
   ByteConnection* c = getConnection();

   if (nullptr == c) {
      return false;
   }

   static const std::string HEADER_TERMINATOR = "\r\n\r\n";

   // read in chunks rather than one byte per read() call, scanning the
   // accumulated buffer for the blank line that ends the headers; any
   // bytes read past that point are the start of the body (or the start
   // of the *next* transaction on this connection) and are handed to the
   // body-reading loop below, or preserved via setUnconsumedBytes() at
   // the end, instead of being re-read from the connection
   //
   // seeded with whatever a previous transaction on this connection
   // over-read and handed off via takeUnconsumedBytes() - this may
   // already contain this entire request/response (and then some), in
   // which case the terminator check below finds it before a single new
   // read() call is made
   std::string buffered = takeUnconsumedBytes();
   std::string headers;
   bool foundHeaderEnd = false;
   char chunk[8192];

   std::string::size_type posTerminator = buffered.find(HEADER_TERMINATOR);
   if (posTerminator != std::string::npos) {
      headers = buffered.substr(0, posTerminator);
      buffered.erase(0, posTerminator + HEADER_TERMINATOR.length());
      foundHeaderEnd = true;
   }

   while (!foundHeaderEnd) {
      bytes_read = c->read(chunk, sizeof(chunk));

      if (bytes_read <= 0) {
         return false;
      }

      buffered.append(chunk, bytes_read);

      posTerminator = buffered.find(HEADER_TERMINATOR);
      if (posTerminator != std::string::npos) {
         headers = buffered.substr(0, posTerminator);
         buffered.erase(0, posTerminator + HEADER_TERMINATOR.length());
         foundHeaderEnd = true;
      }
   }

   int lineIndex = 0;
   std::vector<std::string> vecHeaders = StrUtils::split(headers, "\r\n");
   for (const std::string& token : vecHeaders) {
      if (lineIndex == 0) {
         m_firstHeaderLine = token;

         // populate the first-line-derived fields (method/protocol and the
         // raw 3-token breakdown) the same way parseHeaders() does, since
         // that method is never actually invoked on this code path
         std::vector<std::string> vecTokens = StrUtils::split(m_firstHeaderLine, " ");
         const std::size_t tokenCount = vecTokens.size();

         if (3 <= tokenCount) {
            m_vecRequestLineValues.clear();
            m_vecRequestLineValues.reserve(3);
            string thirdValue;

            for (std::size_t i = 0; i < tokenCount; ++i) {
               if (i > 1) {
                  if (i > 2) {
                     thirdValue += " ";
                  }
                  thirdValue += vecTokens[i];
               } else {
                  m_vecRequestLineValues.push_back(vecTokens[i]);
               }
            }

            m_vecRequestLineValues.push_back(thirdValue);
            m_method = m_vecRequestLineValues[0];
            m_protocol = thirdValue;
         }
      }
      if (!token.empty()) {
         string::size_type posColon = token.find(":");
         if (posColon != string::npos) {
            string key = StrUtils::strip(token.substr(0, posColon));
            string value = StrUtils::strip(token.substr(posColon+1, token.length() - 1
));
            if (!key.empty() && !value.empty()) {
               StrUtils::toLowerCase(key);
               addHeader(key, value);
            }
         }
      }
      lineIndex++;
   }

   contentLength = getContentLength();

   if (contentLength > 0) {
      ByteBuffer* bb = new ByteBuffer(contentLength);
      int offset = 0;

      // serve whatever was already read past the header terminator first
      if (!buffered.empty()) {
         const int fromBuffer = std::min((int) buffered.size(), contentLength);
         memcpy(bb->data(), buffered.data(), fromBuffer);
         offset = fromBuffer;
         buffered.erase(0, fromBuffer);
      }

      int remainingBytes = contentLength - offset;
      int bytesToRead = 8192;
      char buffer[8192];
      while (remainingBytes > 0) {
         if (remainingBytes < bytesToRead) {
            bytesToRead = remainingBytes;
         }
         bytes_read = c->read(buffer, bytesToRead);
         if (bytes_read > 0) {
            memcpy((void*) (bb->data()+offset), buffer, bytes_read);
            offset += bytes_read;
            remainingBytes -= bytes_read;
         } else {
            delete bb;
            return false;
         }
      }
      setBody(bb);
   }

   // whatever remains in buffered - whether there was no body at all, or
   // buffered held more than this transaction's body - belongs to the
   // next transaction on this connection
   setUnconsumedBytes(buffered);
   return true;
}

//*****************************************************************************

bool HttpTransaction::isConnectionOwned() const {
   return m_connectionOwned;
}

//*****************************************************************************

void HttpTransaction::setConnectionOwned(bool connectionOwned) {
   m_connectionOwned = connectionOwned;
}

//*****************************************************************************

std::string HttpTransaction::takeUnconsumedBytes() {
   std::string bytes;
   bytes.swap(m_unconsumedBytes);
   return bytes;
}

//*****************************************************************************

void HttpTransaction::setUnconsumedBytes(const std::string& bytes) {
   m_unconsumedBytes = bytes;
}

//*****************************************************************************

