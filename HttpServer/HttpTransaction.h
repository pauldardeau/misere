// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef HTTPTRANSACTION_H
#define HTTPTRANSACTION_H


#include <string>
#include <vector>
#include <unordered_map>

class Socket;

/*!
 * HttpTransaction is an abstract base class that provides common logic
 * for both HTTP requests and responses.
 */
class HttpTransaction
{
   public:
      HttpTransaction() noexcept;
      HttpTransaction(const HttpTransaction& copy) noexcept;
      HttpTransaction(HttpTransaction&& move) noexcept;
      virtual ~HttpTransaction() noexcept {}

      HttpTransaction& operator=(const HttpTransaction& copy) noexcept;
      HttpTransaction& operator=(HttpTransaction&& move) noexcept;

      virtual bool streamFromSocket(Socket& socket);

      const std::string& getRawHeader() const noexcept;
      const std::string& getBody() const noexcept;
      void setBody(const std::string& body) noexcept;
      bool hasHeaderValue(const std::string& headerKey) const noexcept;
      // throws InvalidKeyException
      const std::string& getHeaderValue(const std::string& headerKey) const;
      void getHeaderValues(std::vector< std::string >&) const noexcept;
      void setHeaderValue(const std::string& key, const std::string& value) noexcept;
      const std::string& getProtocol() const noexcept;
   
      void populateWithHeaders(std::unordered_map<std::string, std::string>& hashTable) noexcept;

   
   protected:
      const std::vector<std::string>& getFirstLineValues() const noexcept;
      const std::string& getFirstLine() const noexcept;
      void setProtocol(const std::string& protocol) noexcept;
      bool parseHeaders() noexcept;


   private:
      std::vector<std::string> m_vecHeaderLines;
      std::vector<std::string> m_vecFirstLineValues;
      std::string m_header;
      std::string m_body;
      std::string m_protocol;
      std::string m_firstLine;
      std::unordered_map<std::string,std::string> m_hashHeaders;
      std::string m_method;
      int m_contentLength;

};

#endif


