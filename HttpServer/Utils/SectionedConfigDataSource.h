// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef SECTIONEDCONFIGDATASOURCE_H
#define SECTIONEDCONFIGDATASOURCE_H

#include <string>

class KeyValuePairs;

/*!
 * SectionedConfigDataSource is an interface for working with configuration
 * mechanisms based on sections, keys, and values.
 */
class SectionedConfigDataSource
{
public:
   virtual ~SectionedConfigDataSource() noexcept {}
   
   virtual bool hasSection(const std::string& sectionName) const noexcept = 0;
   virtual bool readSection(const std::string& sectionName,
                            KeyValuePairs& settings) const noexcept = 0;
   virtual bool getSectionKeyValue(const std::string& section,
                                   const std::string& key,
                                   std::string& value) const noexcept = 0;

};

#endif
