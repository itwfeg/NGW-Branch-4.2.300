/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "DirectoryList.h"
#include "Configuration.h"
#include "HobString.h"

#include "Long.h"
#include "Hex.h"
#include "filesys/directory.h"

#ifdef NO_NAMESPACES
#  define util_filesystem
#endif

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int DirectoryList::dataType = 0;
std::string DirectoryList::dataTypeName("DirectoryList"); 
#endif

DirectoryList::DirectoryList(HobString startingDirectory)
	: HobList()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	HobString start;
	
	if (startingDirectory.empty())
//		start = Configuration::getInstance()->getRootDirectory());
		start = "/ram0";
	else
		start = startingDirectory;
		
	findFileNames(start);
}

DirectoryList::DirectoryList(const char *data, int len, int formatType)
	: HobList(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

DirectoryList::~DirectoryList()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


void DirectoryList::findFileNames(HobString location)
{
	if (location.empty())
		return;
		
	HobString loc = location;
	HobString dirSeparator = Configuration::getInstance()->getDirectorySeparator();
	HobString locEnd = loc.getFromEnd(dirSeparator.length());
	if (locEnd != dirSeparator)
		loc.append(dirSeparator);

	Directory systemDir(loc);
	systemDir.select(std::string("*"), Directory::SELECT_DIRS);

	HobString dir = "";
	util_filesystem::Directory::const_iterator dirIter = systemDir.begin();
	while ( dirIter != systemDir.end() )
	{
		dir = dirIter->getName();
		if (dir != "." && dir != "..")
		{
			dir.append(dirSeparator);
			myList.push_back(dir);
		}
		++dirIter;
	}

	systemDir.select(std::string("*"), Directory::SELECT_FILES);

	dirIter = systemDir.begin();
	while ( dirIter != systemDir.end() )
	{
		myList.push_back(dirIter->getName());
		++dirIter;
	}
}
