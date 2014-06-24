// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include "StrUtils.h"

static const std::string EMPTY = "";
static const std::string SPACE = " ";

//******************************************************************************

void StrUtils::toLowerCase(std::string& s) noexcept
{
   if (!s.empty()) {
      for (auto &c : s) {
         c = tolower(c);
      }
   }
}

//******************************************************************************

void StrUtils::toUpperCase(std::string& s) noexcept
{
   if (!s.empty()) {
      for (auto &c : s) {
         c = toupper(c);
      }
   }
}

//******************************************************************************

bool StrUtils::startsWith(const std::string& haystack, const std::string& needle) noexcept
{
   if (!haystack.empty() && !needle.empty()) {
      const std::string::size_type haystackLength = haystack.length();
      const std::string::size_type needleLength = needle.length();
        
      if (haystackLength >= needleLength) {
         for (int i = 0; i < needleLength; ++i) {
            if (haystack[i] != needle[i]) {
               return false;
            }
         }
            
         return true;
      }
   }
   
   return false;
}

//******************************************************************************

bool StrUtils::endsWith(const std::string& haystack, const std::string& needle) noexcept
{
   if (!haystack.empty() && !needle.empty()) {
      const std::string::size_type haystackLength = haystack.length();
      const std::string::size_type needleLength = needle.length();
        
      if (haystackLength >= needleLength) {
         unsigned long int haystackIndex = haystackLength - 1;
         
         for (long i = needleLength-1; i >= 0; --i) {
            if (haystack[haystackIndex--] != needle[i]) {
               return false;
            }
         }
            
         return true;
      }
   }

   return false;
}

//******************************************************************************

bool StrUtils::containsString(const std::string& haystack, const std::string& needle) noexcept
{
   return (std::string::npos != haystack.find(needle));
}

//******************************************************************************

std::string& StrUtils::strip(std::string& s, char strip) noexcept
{
   return stripLeading(stripTrailing(s, strip), strip);
}

//******************************************************************************

std::string& StrUtils::stripTrailing(std::string& s, char strip) noexcept
{
   if (s.empty()) {
      return s;
   }
   
   const std::string::size_type stringLen = s.length();
    
   int newLength = (int) stringLen - 1;   // start at last character before NULL
    
   // Find out how many trailing characters we have
   while ((0 <= newLength) && (s[newLength] == strip)) {
      --newLength;
   }
    
   ++newLength;
    
   // Did we not have any characters to strip?
   if (newLength == stringLen) {
      return s;
   }
   
   s = s.substr(0,newLength);
   
   return s;
}

//******************************************************************************

std::string& StrUtils::stripLeading(std::string& s, char stripChar) noexcept
{
   if (s.empty()) {
      return s;
   }
   
   const std::string::size_type stringLen = s.length();
   int leadingStripChars = 0;
   
   while ((leadingStripChars < stringLen) && (s[leadingStripChars] == stripChar)) {
      ++leadingStripChars;
   }
    
   // Any leading characters to strip?
   if (leadingStripChars > 0) {
      s = s.substr(leadingStripChars);
   }
   
   return s;
}

//******************************************************************************

std::string& StrUtils::trimLeadingSpaces(std::string& s) noexcept
{
   const std::string::size_type posFirstNonSpace = s.find_first_not_of(SPACE);
    
   if ((std::string::npos != posFirstNonSpace) && (posFirstNonSpace > 0)) {
      s.erase(0, posFirstNonSpace);
   }
    
   return s;
}

//******************************************************************************

std::string StrUtils::strip(const std::string& s) noexcept
{
   return StrUtils::strip(s, ' ');
}

//******************************************************************************

std::string StrUtils::strip(const std::string& s, char strip) noexcept
{
   if (s.empty()) {
      return EMPTY;
   }
   
   const std::string::size_type len = s.length();
    
   int leadingChars = 0;
    
   for (int i = 0; i < len; ++i) {
      if (s[i] == strip) {
         ++leadingChars;
      } else {
         break;
      }
   }
    
   if (leadingChars == len) {
      return std::string(EMPTY);
   }
    
   int trailingChars = 0;
    
   for (int i = (int) len - 1; i >= 0; --i) {
      if (s[i] == strip) {
         ++trailingChars;
      } else {
         break;
      }
   }
    
   return s.substr(leadingChars, len - trailingChars);
}

//******************************************************************************

std::string& StrUtils::replaceAll(std::string& s,
                             const std::string& searchFor,
                             const std::string& replaceWith) noexcept
{
   std::string::size_type posSearchFor = s.find(searchFor);
    
   if (posSearchFor == std::string::npos) {
      return s;
   }
    
   const std::string::size_type lenSearchFor = searchFor.length();
   const std::string::size_type lenReplaceWith = replaceWith.length();
    
   do {
      s.replace(posSearchFor, lenSearchFor, replaceWith);
      posSearchFor = s.find(searchFor, posSearchFor + lenReplaceWith);
   } while (posSearchFor != std::string::npos);
    
   return s;
}

//******************************************************************************


