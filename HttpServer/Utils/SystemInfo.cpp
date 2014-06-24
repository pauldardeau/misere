// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#include <sys/utsname.h>

#include "SystemInfo.h"

//******************************************************************************

SystemInfo::SystemInfo()
{
   struct utsname sysinfo;
   if (0 == ::uname(&sysinfo)) {
      m_sysName = sysinfo.sysname;
      m_nodeName = sysinfo.nodename;
      m_release = sysinfo.release;
      m_version = sysinfo.version;
      m_machine = sysinfo.machine;
   }
}

//******************************************************************************

SystemInfo::~SystemInfo()
{
}

//******************************************************************************

