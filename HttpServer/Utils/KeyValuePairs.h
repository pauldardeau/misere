// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef KEYVALUEPAIRS_H
#define KEYVALUEPAIRS_H

#include <string>
#include <vector>
#include <unordered_map>


/*!
 * KeyValuePairs is a collection of key/value pairs where the key and the
 * value are both strings.
 */
class KeyValuePairs
{
   public:
      typedef std::size_t size_type;
   
      KeyValuePairs() noexcept;
      KeyValuePairs(const KeyValuePairs& copy) noexcept;
      KeyValuePairs(KeyValuePairs&& move) noexcept;
      ~KeyValuePairs() noexcept;

      KeyValuePairs& operator=(const KeyValuePairs& copy) noexcept;
      KeyValuePairs& operator=(KeyValuePairs&& move) noexcept;

      void getKeys(std::vector<std::string>& keys) const noexcept;

      bool hasKey(const std::string& key) const noexcept;

      // throws InvalidKeyException
      const std::string& getValue(const std::string& key) const;

      void addPair(const std::string& key, const std::string& value) noexcept;

      bool removePair(const std::string& key) noexcept;

      void clear() noexcept;
   
      size_type size() const noexcept;
   
      bool empty() const noexcept;
   

   private:
      std::unordered_map<std::string,std::string> m_keyValues;
};


#endif

