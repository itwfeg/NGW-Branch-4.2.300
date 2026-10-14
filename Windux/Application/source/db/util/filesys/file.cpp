#include "PAL_platformheader.h"

#include "file.h"



#if !defined( _WIN32 )
#  include "../platform/platform.h"
#else
#  include "..\platform\platform.h"
#endif



#ifndef NO_NAMESPACES
namespace util_filesystem
{
#endif


#ifndef NO_NAMESPACES
using util_platform::Platform;
#endif
using namespace std;


DateTime File::getModified() const
{
	return Platform::getInstance().getFileModificationDate(filename_.getFullPath());
}


bool File::exists() const
{
	return Platform::getInstance().fileExists(filename_.getFullPath());
}


long64 File::size() const
{
	return Platform::getInstance().fileSize(filename_.getFullPath());
}


unsigned int File::checksum() const
{
	return Platform::getInstance().fileChecksum(filename_.getFullPath());
}


bool File::remove()
{
	return Platform::getInstance().deleteFile(filename_.getFullPath());
}


bool File::operator <<(const File& oldfile)
{
	return Platform::getInstance().copyFile(oldfile.getFullPath(), filename_.getFullPath());
}


bool File::copyTo(const std::string& destination)
{
	return Platform::getInstance().copyFile(getFullPath(), destination);
}


bool File::moveTo(const std::string& destination)
{
	return Platform::getInstance().moveFile(getFullPath(), destination);
}


bool File::appendTo(const std::string& destination)
{
	return Platform::getInstance().appendFile(getFullPath(), destination);
}



const std::string File::_empty_ = "";



#ifndef NO_NAMESPACES
} // namespace util_filesystem
#endif

