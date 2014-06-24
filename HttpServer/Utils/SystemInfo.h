// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef __HttpServer__SystemInfo__
#define __HttpServer__SystemInfo__

#include <string>


/*!
 * Wrapper/utility class around uname function
 */
class SystemInfo
{
public:
   SystemInfo();
   ~SystemInfo();
   
   std::string m_sysName;
   std::string m_nodeName;
   std::string m_release;
   std::string m_version;
   std::string m_machine;
   
};

#endif /* defined(__HttpServer__SystemInfo__) */
