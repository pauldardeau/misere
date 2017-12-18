// Copyright Paul Dardeau, SwampBits LLC 2017
// BSD License

#include <exception>
#include "Url.h"
#include "StrUtils.h"

using namespace std;
using namespace chaudiere;
using namespace misere;


Url::Url() :
   m_port(0)
{
}

Url::Url(const std::string& fullText) :
   m_fullText(fullText),
   m_port(0)
{
   const string::size_type posColon = fullText.find(":");
   if (posColon != string::npos) {
      m_protocol = fullText.substr(0, posColon);
      const string::size_type posDoubleSlash = fullText.find("//");
      if (posDoubleSlash > posColon && posDoubleSlash != string::npos) {
         const string::size_type posSlash = fullText.find("/", posDoubleSlash + 2);
         if (posSlash != string::npos) {
            m_host = fullText.substr(posDoubleSlash + 2,
                                     posSlash - posDoubleSlash - 2);
            const string::size_type posPortColon = m_host.find(":");
            if (posPortColon != string::npos) {
                std::string portAsText = m_host.substr(posPortColon + 1,
                                                       m_host.length() - posPortColon - 1);
                m_port = StrUtils::parseInt(portAsText);
                if (m_port < 0) {
                   // negative values not allowed
                   throw invalid_argument("negative port values not valid");
                }
                m_host = m_host.substr(0, posPortColon);
            }

            m_path = fullText.substr(posSlash,
                                     fullText.size() - posDoubleSlash);
         } else {
            m_path = "/";
         }
      }
   }

   if (m_fullText.length() == 0) {
      throw invalid_argument("no full text");
   }

   if (m_host.length() == 0) {
      throw invalid_argument("missing host");
   }

   if (m_path.length() == 0) {
      throw invalid_argument("missing path");
   }

   if (m_protocol.length() == 0) {
      throw invalid_argument("missing protocol");
   }

   if (m_port < 0) {
      throw invalid_argument("negative port");
   }
}

Url::Url(const Url& copy) :
   m_protocol(copy.m_protocol),
   m_fullText(copy.m_fullText),
   m_host(copy.m_host),
   m_path(copy.m_path),
   m_port(copy.m_port)
{
}

Url::~Url()
{
}

Url& Url::operator=(const Url& copy)
{
   if (this == &copy) {
      return *this;
   }

   m_protocol = copy.m_protocol;
   m_fullText = copy.m_fullText;
   m_host = copy.m_host;
   m_path = copy.m_path;
   m_port = copy.m_port;

   return *this;
}

bool Url::operator==(const Url& compare) const
{
   return m_fullText == compare.m_fullText;
}

bool Url::operator!=(const Url& compare) const
{
   return m_fullText != compare.m_fullText;
}

const std::string& Url::protocol() const
{
   return m_protocol;
}

const std::string& Url::fullText() const
{
   return m_fullText;
}

const std::string& Url::host() const
{
   return m_host;
}

const std::string& Url::path() const
{
   return m_path;
}

int Url::port() const
{
   return m_port;
}

