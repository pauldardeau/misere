// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <cstdio>
#include <cstdlib>

#include "HttpResponse.h"
#include "HTTP.h"
#include "Socket.h"
#include "BasicException.h"
#include "HttpException.h"
#include "Logger.h"
#include "StringTokenizer.h"
#include "StrUtils.h"
#include "ByteBuffer.h"

static const std::string TEXT_HTML = "text/html";

using namespace std;
using namespace misere;
using namespace chaudiere;

//******************************************************************************

HttpResponse::HttpResponse() :
   m_statusCodeAsInteger(200) {

   Logger::logInstanceCreate("HttpResponse");
   setContentType(TEXT_HTML);
}

//******************************************************************************

HttpResponse::HttpResponse(const HttpResponse& copy) :
   HttpTransaction(copy),
   m_statusCode(copy.m_statusCode),
   m_reasonPhrase(copy.m_reasonPhrase),
   m_statusCodeAsInteger(copy.m_statusCodeAsInteger) {
   Logger::logInstanceCreate("HttpResponse");
}

//******************************************************************************

HttpResponse::HttpResponse(Socket* socket) :
   HttpTransaction(socket) {
   Logger::logInstanceCreate("HttpResponse");

   printf("HttpResponse ctor: calling streamFromSocket\n");
   if (!streamFromSocket()) {
      throw BasicException("unable to construct HttpResponse from Socket");
   }
}

//******************************************************************************

HttpResponse::~HttpResponse() {
   Logger::logInstanceDestroy("HttpResponse");
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

bool HttpResponse::streamFromSocket2() {
   std::string sbInput;
   std::string inputLine;
   std::string method;
   int contentLength = -1;  // unknown
   bool readingHeaders = true;
   bool onEOL = false;
   int eolNL = 0;
   char charRead;
   int bytes_read;
   string headers;
   Socket* s = getSocket();

   while (readingHeaders) {
      bytes_read = s->readSocket(&charRead, 1);
      if (bytes_read > 0) {
         if (charRead == '\r') {
            onEOL = true;
         } else {
            if (charRead == '\n') {
               if (onEOL) {
                  eolNL++;
                  if (eolNL == 2) {
                     readingHeaders = false;
                     headers = headers.substr(0, headers.length() - 1);
                  }
               }
            } else {
               onEOL = false;
               eolNL = 0;
            }
         }

         if (readingHeaders) {
            headers += charRead;
         }
      } else {
         if (bytes_read == -1) {
            return false;
         }
      }
   }

   printf("headers received: %s\n", headers.c_str());

   int lineIndex = 0;
   StringTokenizer st(headers, "\r\n");
   while (st.hasMoreTokens()) {
      const string& token = st.nextToken();
      if (lineIndex == 0) {
         StringTokenizer stStatus(token, " ");
         if (stStatus.countTokens() == 3) {
            stStatus.nextToken();
            const string& statusCodeAsText = stStatus.nextToken();
            m_statusCode = statusCodeAsText;
            m_statusCodeAsInteger = StrUtils::parseInt(m_statusCode);
         }
      }
      if (token.length() > 0) {
         string::size_type posColon = token.find(":");
         if (posColon != string::npos) {
            string key = StrUtils::strip(token.substr(0, posColon));
            string value = StrUtils::strip(token.substr(posColon+1, token.length() - 1
));
            if (key.length() > 0 && value.length() > 0) {
               StrUtils::toLowerCase(key);
               addHeader(key, value);
            }
         }
      }
      lineIndex++;
   }

   contentLength = getContentLength();
   printf("HttpRequest::streamFromSocket2 contentLength=%d\n", contentLength);

   if (contentLength > 0) {
      ByteBuffer* bb = new ByteBuffer(contentLength);
      int remainingBytes = contentLength;
      int offset = 0;
      int bytesToRead = 8192;
      char buffer[8192];
      while (remainingBytes > 0) {
         if (remainingBytes < bytesToRead) {
            bytesToRead = remainingBytes;
         }
         bytes_read = s->readSocket(buffer, bytesToRead);
         if (bytes_read > 0) {
            memcpy((void*) (bb->data()+offset), buffer, bytes_read);
            offset += bytes_read;
            remainingBytes -= bytes_read;
         } else {
            return false;
         }
      }
      setBody(bb);
   }
   return true;
}

bool HttpResponse::streamFromSocket() {
   if (Logger::isLogging(Debug)) {
      Logger::debug("******** start of HttpResponse::streamFromSocket");
   }
   
   bool streamSuccess = false;
   printf("HttpResponse: calling streamFromSocket2\n");

   if (streamFromSocket2()) {
      printf("HttpResponse: streamFromSocket succeeded\n");
      const std::vector<std::string>& vecRequestLineValues =
         getRequestLineValues();

      if (3 == vecRequestLineValues.size()) {
         setProtocol(vecRequestLineValues[0]);
         m_statusCode = vecRequestLineValues[1];
         m_reasonPhrase = vecRequestLineValues[2];
         m_statusCodeAsInteger = StrUtils::parseInt(m_statusCode);

         if (0 == m_statusCodeAsInteger) {
            Logger::error("unable to parse status code");
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
      Logger::error("unable to parse headers");
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
      const std::string& contentLength = getHeaderValue(HTTP::HTTP_CONTENT_LENGTH);
      lengthValue = atoi(contentLength.c_str());
   }

   return lengthValue;
}

//******************************************************************************

void HttpResponse::setContentLength(int contentLength) {
   char lengthText[40];
   snprintf(lengthText, 40, "%d", contentLength);
   setHeaderValue(HTTP::HTTP_CONTENT_LENGTH, std::string(lengthText)); 
}

//******************************************************************************

