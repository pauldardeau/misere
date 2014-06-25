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
   SystemInfo() noexcept;
   SystemInfo(const SystemInfo& copy) noexcept;
   SystemInfo(SystemInfo&& move) noexcept;
   ~SystemInfo() noexcept;
   
   SystemInfo& operator=(const SystemInfo& copy) noexcept;
   SystemInfo& operator=(SystemInfo&& move) noexcept;
   
   const std::string& sysName() const noexcept;
   const std::string& nodeName() const noexcept;
   const std::string& release() const noexcept;
   const std::string& version() const noexcept;
   const std::string& machine() const noexcept;
   
   bool retrievedSystemInfo() const noexcept;

   
private:
   std::string m_sysName;
   std::string m_nodeName;
   std::string m_release;
   std::string m_version;
   std::string m_machine;
   bool m_retrievedSystemInfo;
   
};

#endif /* defined(__HttpServer__SystemInfo__) */
