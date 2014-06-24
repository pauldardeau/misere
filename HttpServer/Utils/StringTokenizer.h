// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef STRINGTOKENIZER_H
#define STRINGTOKENIZER_H

#include <string>
#include <vector>

/*!
 * StringTokenizer is very similar to Java's StringTokenizer.
 */
class StringTokenizer
{
   public:
      StringTokenizer(const std::string& s) noexcept;
      StringTokenizer(const std::string& s,
                      const std::string& delimiter,
                      bool isDelimitersWithToken=false) noexcept;
      ~StringTokenizer() noexcept;
   
      bool hasMoreTokens() const noexcept;
      // throws std::out_of_range
      std::string nextToken();
      unsigned long countTokens() const noexcept;
   
      //disallow copies
      StringTokenizer(const StringTokenizer&) = delete;
      StringTokenizer(StringTokenizer&&) = delete;
      StringTokenizer& operator=(const StringTokenizer&) = delete;
      StringTokenizer& operator=(StringTokenizer&&) = delete;

   
   private:
      void init() noexcept;
         
      const std::string& m_withTokens;
      const std::string m_delimiter;
      const char* m_posTokens;
      const char* m_posDelimiter;
      std::string::size_type m_posCurrent;
      std::string::size_type m_stringLength;
      bool m_isDelimitersWithToken;
      bool m_isConstructing;
      std::vector<std::string> m_tokens;
      int m_indexToken;
};

#endif


