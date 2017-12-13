// Copyright Paul Dardeau, SwampBits LLC 2017
// BSD License

#ifndef MISERE_URL_H
#define MISERE_URL_H

#include <string>


namespace misere
{

class Url
{
   private:
      std::string m_protocol;
      std::string m_fullText;
      std::string m_host;
      std::string m_path;
      int m_port;

   public:
      Url(const std::string& fullText);
      Url(const Url& copy);
      ~Url();
      Url& operator=(const Url& copy);
      bool operator==(const Url& compare) const;
      bool operator!=(const Url& compare) const;
      const std::string& protocol() const;
      const std::string& fullText() const;
      const std::string& host() const;
      const std::string& path() const;
      int port() const;
};

}

#endif

