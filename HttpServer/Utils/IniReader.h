// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef IniReader_h
#define IniReader_h

#include <string>

#include "SectionedConfigDataSource.h"

class KeyValuePairs;

/*!
 * IniReader is a utility class that knows how to read and parse a .ini
 * configuration file. .ini configuration files were popularized on Win32
 * platforms as a simple form of configuration files.
 */
class IniReader : public SectionedConfigDataSource
{
public:
   // throws BasicException
   IniReader(const std::string& iniFile);
   virtual ~IniReader() noexcept;
    
   virtual bool readSection(const std::string& section,
                            KeyValuePairs& mapSectionValues) const noexcept override;
    
   virtual bool getSectionKeyValue(const std::string& section,
                                   const std::string& key,
                                   std::string& value) const noexcept override;
    
   virtual bool hasSection(const std::string& section) const noexcept override;
    
    
protected:
   bool readFile() noexcept;
   std::string bracketedSection(const std::string& sectionName) const noexcept;
   
    
private:
   std::string m_iniFile;
   std::string m_fileContents;
};


#endif
