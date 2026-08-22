// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <utility>

#include "HttpResponse.h"
#include "HTTP.h"
#include "ByteConnection.h"
#include "BasicException.h"
#include "HttpException.h"
#include "Logger.h"
#include "StrUtils.h"
#include "ByteBuffer.h"

static const std::string TEXT_HTML = "text/html";

using namespace std;
using namespace misere;
using namespace chaudiere;

//******************************************************************************

HttpResponse::HttpResponse() :
   m_statusCodeAsInteger(200) {

   LOG_INSTANCE_CREATE("HttpResponse")
   setContentType(TEXT_HTML);
}

//******************************************************************************

HttpResponse::HttpResponse(const HttpResponse& copy) :
   HttpTransaction(copy),
   m_statusCode(copy.m_statusCode),
   m_reasonPhrase(copy.m_reasonPhrase),
   m_statusCodeAsInteger(copy.m_statusCodeAsInteger) {
   LOG_INSTANCE_CREATE("HttpResponse")
}

//******************************************************************************

HttpResponse::HttpResponse(ByteConnection* connection, std::string leadingBytes) :
   HttpTransaction(connection, true, std::move(leadingBytes)) {
   LOG_INSTANCE_CREATE("HttpResponse")

   if (!streamFromConnection()) {
      throw BasicException("unable to construct HttpResponse from ByteConnection");
   }
}

//******************************************************************************

HttpResponse::~HttpResponse() {
   LOG_INSTANCE_DESTROY("HttpResponse")
}

//******************************************************************************

HttpResponse& HttpResponse::operator=(const HttpResponse& copy) {
   if (this == &copy) {
      return *this;
   }

   HttpTransaction::operator=(copy);
   m_statusCode = copy.m_statusCode;
   m_reasonPhrase = copy.m_reasonPhrase;
   m_statusCodeAsInteger = copy.m_statusCodeAsInteger;

   return *this;
}

//******************************************************************************

bool HttpResponse::streamFromConnection2() {
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
   // over-read and handed off via takeUnconsumedBytes()
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
   for (const std::string& headerLine : vecHeaders) {
      if (lineIndex == 0) {
         std::vector<std::string> vecStatusTokens = StrUtils::split(headerLine, " ");
         // reason phrases are routinely multiple words ("Not Found",
         // "Internal Server Error"), so StrUtils::split can produce more
         // than 3 tokens here -- rejoin everything from the 3rd token on
         // instead of requiring an exact count of 3
         if (vecStatusTokens.size() >= 3) {
            std::string reasonPhrase = vecStatusTokens[2];
            for (std::size_t i = 3; i < vecStatusTokens.size(); ++i) {
               reasonPhrase += " ";
               reasonPhrase += vecStatusTokens[i];
            }

            std::vector<std::string> requestLineValues;
            requestLineValues.push_back(vecStatusTokens[0]);
            requestLineValues.push_back(vecStatusTokens[1]);
            requestLineValues.push_back(reasonPhrase);

            m_statusCode = vecStatusTokens[1];
            m_statusCodeAsInteger = StrUtils::parseInt(m_statusCode);
            setRequestLineValues(requestLineValues);
         }
      }
      if (!headerLine.empty()) {
         string::size_type posColon = headerLine.find(":");
         if (posColon != string::npos) {
            string key = StrUtils::strip(headerLine.substr(0, posColon));
            string value = StrUtils::strip(headerLine.substr(posColon+1, headerLine.size() - 1
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
   // buffered held more than this response's body - belongs to the next
   // transaction on this connection
   setUnconsumedBytes(buffered);
   return true;
}

bool HttpResponse::streamFromConnection() {
   if (Logger::isLogging(LogLevel::Debug)) {
      LOG_DEBUG("******** start of HttpResponse::streamFromConnection")
   }

   bool streamSuccess = false;

   if (streamFromConnection2()) {
      const std::vector<std::string>& vecRequestLineValues =
         getRequestLineValues();

      if (3 == vecRequestLineValues.size()) {
         setProtocol(vecRequestLineValues[0]);
         m_statusCode = vecRequestLineValues[1];
         m_reasonPhrase = vecRequestLineValues[2];
         m_statusCodeAsInteger = StrUtils::parseInt(m_statusCode);

         if (0 == m_statusCodeAsInteger) {
            LOG_ERROR("unable to parse status code")
            return false;
         } else if (m_statusCodeAsInteger >= 500) {
            std::string reasonPhrase;

            switch (m_statusCodeAsInteger) {
               case 500:
                  reasonPhrase = "Internal Server Error";
                  break;

               case 501:
                  reasonPhrase = "Not Implemented";
                  break;

               case 502:
                  reasonPhrase = "Bad Gateway";
                  break;

               case 503:
                  reasonPhrase = "Service Unavailable";
                  break;

               case 504:
                  reasonPhrase = "Gateway Timeout";
                  break;

               case 505:
                  reasonPhrase = "HTTP Version not supported";
                  break;
            }

            throw HttpException(m_statusCodeAsInteger, reasonPhrase);

         } else if ((m_statusCodeAsInteger >= 400) &&
                    (m_statusCodeAsInteger < 500)) {
            std::string reasonPhrase;

            switch(m_statusCodeAsInteger) {
               case 400:
                  reasonPhrase = "Bad Request";
                  break;

               case 401:
                  reasonPhrase = "Unauthorized";
                  break;

               case 402:
                  reasonPhrase = "Payment Required";
                  break;

               case 403:
                  reasonPhrase = "Forbidden";
                  break;

               case 404:
                  reasonPhrase = "Not Found";
                  break;

               case 405:
                  reasonPhrase = "Method Not Allowed";
                  break;

               case 406:
                  reasonPhrase = "Not Acceptable";
                  break;

               case 407:
                  reasonPhrase = "Proxy Authentication Required";
                  break;

               case 408:
                  reasonPhrase = "Request Timeout";
                  break;

               case 409:
                  reasonPhrase = "Conflict";
                  break;

               case 410:
                  reasonPhrase = "Gone";
                  break;

               case 411:
                  reasonPhrase = "Length Required";
                  break;

               case 412:
                  reasonPhrase = "Precondition Failed";
                  break;

               case 413:
                  reasonPhrase = "Request Entity Too Large";
                  break;

               case 414:
                  reasonPhrase = "Request-URI Too Large";
                  break;

               case 415:
                  reasonPhrase = "Unsupported Media Type";
                  break;

               case 416:
                  reasonPhrase = "Request range not satisfiable";
                  break;

               case 417:
                  reasonPhrase = "Expectation Failed";
                  break;
            }

            throw HttpException(m_statusCodeAsInteger, reasonPhrase);
         }
      }

      streamSuccess = true;
   } else {
      LOG_ERROR("unable to parse headers")
   }

   return streamSuccess;
}

//******************************************************************************

int HttpResponse::getStatusCode() const {
   return m_statusCodeAsInteger;
}

//******************************************************************************

void HttpResponse::setStatusCode(int statusCode) {
   m_statusCodeAsInteger = statusCode;
}

//******************************************************************************

const std::string& HttpResponse::getReasonPhrase() const {
   return m_reasonPhrase;
}

//******************************************************************************

bool HttpResponse::hasContentEncoding() const {
   return hasHeaderValue(HTTP::HTTP_CONTENT_ENCODING);
}

//******************************************************************************

bool HttpResponse::hasContentType() const {
   return hasHeaderValue(HTTP::HTTP_CONTENT_TYPE);
}

//******************************************************************************

const std::string& HttpResponse::getContentEncoding() const {
   return getHeaderValue(HTTP::HTTP_CONTENT_ENCODING);
}

//******************************************************************************

const std::string& HttpResponse::getContentType() const {
   return getHeaderValue(HTTP::HTTP_CONTENT_TYPE);
}

//******************************************************************************

void HttpResponse::setContentEncoding(const std::string& contentEncoding) {
   setHeaderValue(HTTP::HTTP_CONTENT_ENCODING, contentEncoding);
}

//******************************************************************************

void HttpResponse::setContentType(const std::string& contentType) {
   setHeaderValue(HTTP::HTTP_CONTENT_TYPE, contentType);
}

//******************************************************************************

void HttpResponse::close() {
   //TODO: implement HttpResponse::close
}

//******************************************************************************

int HttpResponse::getContentLength() const {
   int lengthValue = 0;

   if (hasHeaderValue(HTTP::HTTP_CONTENT_LENGTH)) {
      lengthValue =
         StrUtils::parseInt(getHeaderValue(HTTP::HTTP_CONTENT_LENGTH));
   }

   return lengthValue;
}

//******************************************************************************

void HttpResponse::setContentLength(int contentLength) {
   setHeaderValue(HTTP::HTTP_CONTENT_LENGTH, StrUtils::toString(contentLength));
}

//******************************************************************************

