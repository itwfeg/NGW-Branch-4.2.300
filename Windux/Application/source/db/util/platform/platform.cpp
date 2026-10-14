#include "PAL_platformheader.h"

#include "platform.h"



#if defined (WIN32_OS) 
#  include "win32.h"
#  include "..\thread\lock.h"
#elif defined ( __linux__ )
#  include "LinuxUtil.h"
#  include "../thread/lock.h"
#elif defined(VXWORKS)
#  include "vxworksUtil.h"
#  include "../thread/lock.h"
#endif


#ifndef NO_NAMESPACES
namespace util_platform
{
#endif


#ifndef NO_NAMESPACES
	using namespace util_thread;
#endif


Mutex<> Platform::_singletonMutex_ = Mutex<>();


Platform& Platform::getInstance()
{
#if defined (WIN32_OS)
    static Win32* os = 0;
#elif   defined( __linux__ )
    static LinuxUtil* os = 0;
#elif   defined(VXWORKS)
    static VxWorksUtil* os = 0;
#endif

    if ( !os )
    {
        Lock< Mutex<> > lock(_singletonMutex_);

        if ( !os )
        {
#if defined (WIN32_OS)
            os = new Win32();
#elif   defined( __linux__ )
            os = new LinuxUtil();
#elif   defined(VXWORKS)
            os = new VxWorksUtil();
#endif
        }
    }

    return *os;
}


#ifdef VXWORKS
STATUS fileSystemFreeSpace(const char* fileSystem)
{
	return Platform::getInstance().fileSystemFreeSpace(fileSystem);
}


STATUS fileSystemUsedSpace(const char* fileSystem)
{
	return Platform::getInstance().fileSystemUsedSpace(fileSystem);
}


STATUS fileSystemTotalSpace(const char* fileSystem)
{
	return Platform::getInstance().fileSystemTotalSpace(fileSystem);
}
#endif


#ifndef NO_NAMESPACES
} // namespace util_platform
#endif
