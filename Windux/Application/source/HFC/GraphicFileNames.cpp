/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include <algorithm>


#include "GraphicFileNames.h"
#include "Configuration.h"
#include "HobTokenizerCSV.h"
#include "filesys/directory.h"
#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#ifdef NO_NAMESPACES
#  define util_filesystem
#endif


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int GraphicFileNames::dataType = 0;
std::string GraphicFileNames::dataTypeName("GraphicFileNames"); 
#endif


using namespace std;


GraphicFileNames::GraphicFileNames(short graphicType, short graphicSubType, const std::string* rootDir)
	: HobList()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	serializeModifiedDateTimes = false;

	findFileNames(graphicType, graphicSubType, rootDir);
}

GraphicFileNames::GraphicFileNames(const char *data, int len, int formatType)
	: HobList(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	serializeModifiedDateTimes = false;
}

GraphicFileNames::~GraphicFileNames()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


void GraphicFileNames::findFileNames(short graphicType, short graphicSubType, const std::string* rootDir, Directory::Sortby sort)
{
	HobString token;
	HobString myExtensions;
	util_filesystem::Directory::const_iterator dirIter;
	
	Configuration *config = Configuration::getInstance();

	string systemUserRootDirectory = config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR);

	bool isTracing = Debug::isTracing("GraphicFileNames"); // MSVC2010

#ifndef EVOL_NDEBUG
	if (isTracing)
		Debug::trace("GraphicFileNames", "findFileNames(): graphicType = " + HobString(Long::toString(graphicType)));
#endif
	switch (graphicType)
	{
		case Graphic::TYPE_LABELGRAPHIC:
			myExtensions = config->toStringCfgI(CfgInternal::GRAPHICSEXTENSIONS);
			myFilename.setPath(config->toStringCfgI(CfgInternal::USERGRAPHICSLOCATION));
			myFilename2.setPath(config->toStringCfgI(CfgInternal::SYSTEMGRAPHICSLOCATION));
			break;
		case Graphic::TYPE_USERLABELGRAPHIC:
			myExtensions = config->toStringCfgI(CfgInternal::GRAPHICSEXTENSIONS);
			myFilename.setPath(config->toStringCfgI(CfgInternal::USERGRAPHICSLOCATION));
			break;
		case Graphic::TYPE_ROMLABELGRAPHIC:
			myExtensions = config->toStringCfgI(CfgInternal::GRAPHICSEXTENSIONS);
			myFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMGRAPHICSLOCATION));
			break;
		case Graphic::TYPE_SPECIALSGRAPHIC:
			myExtensions = config->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDEFAULTEXTENSION);
			myFilename.setPath(systemUserRootDirectory);
			myFilename.appendPath(config->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDIR));
			if (isTracing)
				Debug::trace("GraphicFileNames", "findFileNames(): will find Specials graphics in: " + myFilename.getPath());
			break;
		case Graphic::TYPE_PRODUCTGRAPHIC:
			myFilename.setPath(systemUserRootDirectory);
			myFilename.appendPath(config->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDIR));
			myFilename2.setPath(systemUserRootDirectory);
			myFilename2.appendPath(config->toStringCfgI(CfgInternal::SYSTEMPRODUCTGRAPHICSDIR));
			if (isTracing)
				Debug::trace("GraphicFileNames", "findFileNames(): will find Product graphics in: " + myFilename.getPath() + "  or: " + myFilename2.getPath());
			break;
		case Graphic::TYPE_CATEGORYGRAPHIC:
			myFilename.setPath(systemUserRootDirectory);
			myFilename.appendPath(config->toStringCfgI(CfgInternal::CATEGORYGRAPHICSDIR));
			myFilename2.setPath(systemUserRootDirectory);
			myFilename2.appendPath(config->toStringCfgI(CfgInternal::SYSTEMCATEGORYGRAPHICSDIR));
				if (isTracing)
				Debug::trace("GraphicFileNames", "findFileNames(): will find Category graphics in: " + myFilename.getPath() + "  or: " + myFilename2.getPath());
			break;
		case Graphic::TYPE_STREAMINGGRAPHIC:
			myFilename.setPath(systemUserRootDirectory);
			myFilename.appendPath(config->toStringCfgI(CfgInternal::STREAMINGGRAPHICSDIR));
			if (isTracing)
				Debug::trace("GraphicFileNames", "findFileNames(): will find SlideShow graphics in: " + myFilename.getPath());
			break;
		case Graphic::TYPE_SELFSERVEGRAPHIC:
			myFilename.setPath(systemUserRootDirectory);
			myFilename.appendPath(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDIR));
			if (isTracing)
				Debug::trace("GraphicFileNames", "findFileNames(): will find Self Service graphics in: " + myFilename.getPath());
			break;
		case Graphic::TYPE_SOFTKEYGRAPHIC:
			myFilename.setPath(systemUserRootDirectory);
			myFilename.appendPath(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDIR));			//	We need a place in Config to store this!
			if (isTracing)
				Debug::trace("GraphicFileNames", "findFileNames(): will find Softkey graphics in: " + myFilename.getPath());
			break;
		case Graphic::TYPE_PLANOGRAMGRAPHIC:
			myFilename.setPath(systemUserRootDirectory);
			myFilename.appendPath(config->toStringCfgI(CfgInternal::PLANOGRAMGRAPHICSDIR));			//	We need a place in Config to store this!
			if (isTracing)
				Debug::trace("GraphicFileNames", "findFileNames(): will find Planogram graphics in: " + myFilename.getPath());
			break;
		case Graphic::TYPE_VIDEOS:
			myFilename.setPath(systemUserRootDirectory);
			myFilename.appendPath(config->toStringCfgI(CfgInternal::VIDEOSDIR));			//	We need a place in Config to store this!
			if (isTracing)
				Debug::trace("GraphicFileNames", "findFileNames(): will find Videos in: " + myFilename.getPath());
			break;
		default:
			if (isTracing)
				Debug::trace("GraphicFileNames", "findFileNames(): Invalid graphicType!!!");
			return;
			break;
	}

	destFileName = myFilename;
	destFileName2 = myFilename2;

	if (rootDir != 0)
	{
		string newFilePath(myFilename.getFullPath());
		string newFilePath2(myFilename2.getFullPath());

		string baseEvolDir = systemUserRootDirectory;

		char c = newFilePath.at(baseEvolDir.size());

#if !defined ( __linux__ )		
		string::size_type offsetAddr  = (c == '\\')? 1 : 0;
#else
		string::size_type offsetAddr = (c == '/')? 1 : 0;
#endif		
		string::size_type indexAppendPath = baseEvolDir.size() + offsetAddr;

		myFilename.setPath(*rootDir);
		if (newFilePath.size() > indexAppendPath)
			myFilename.appendPath(newFilePath.substr(baseEvolDir.size() + offsetAddr));

//	PTR 715: We have a discrepancy for the name of the web page directory. For vxWorks and Windows it is
//			 "webPages". For Linux it is "www". We want to maintain our standard directory structure on a
//			 thumbdrive to import Graphics to keep things simple. But, if someone sets up the drive with
//			 a "www" directory instead of "webPages", we should be able to use it, regardless of the
//			 operating system. [CCD June 2011].
		Directory tempDir(myFilename);		//	Does the web page directory exist on the thumbdrive?
		if (!tempDir.exists())
		{
			if (isTracing)
				Debug::trace("GraphicFileNames", "findFileNames():  myFullPath = " + myFilename.getPath() + " does not exist. Trying the other dir name.");
			std::string linWebDir = config->toStringCfgI(CfgInternal::LINUXWEBPAGEDIRNAME);
			std::string winWebDir = config->toStringCfgI(CfgInternal::GENERALWEBPAGEDIRNAME);

#if defined ( __linux__ )
			//	The "www" directory does not exist. Start over - checking for "webPages".
			std::string::size_type pos = newFilePath.find(linWebDir);
			if (pos != std::string::npos)
				newFilePath.replace(pos, linWebDir.size(), winWebDir);
#else
			//	The "webPages" directory does not exist. Start over - checking for "www".
			std::string::size_type pos = newFilePath.find(winWebDir);
			if (pos != std::string::npos)
				newFilePath.replace(pos, winWebDir.size(), linWebDir);
#endif

			myFilename.setPath(*rootDir);
			if (newFilePath.size() > indexAppendPath)
				myFilename.appendPath(newFilePath.substr(baseEvolDir.size() + offsetAddr));
		}
	}
	if (isTracing)
		Debug::trace("GraphicFileNames", "findFileNames(): myFullPath = " + myFilename.getPath());

	if ( graphicSubType == Graphic::SUBTYPE_USER || graphicSubType == Graphic::SUBTYPE_USERANDSYSTEM )
	{
		Directory userDir(myFilename);

		if (isTracing)
			Debug::trace("GraphicFileNames", "findFileNames(): userDir = " + userDir.getFullPath());

		if (myExtensions.empty())
		{
			//	Get the list of all files in the directory (They should be graphics only!)
			if (isTracing)
				Debug::trace("GraphicFileNames", "findFileNames(): Reading all files in this directory.");

			userDir.selectAllFiles(sort);

			dirIter = userDir.begin();

			if (dirIter == userDir.end( ))
				if (isTracing)
					Debug::trace("GraphicFileNames", "findFileNames(): userDir.begin() == userDir.end()  !!!");

			// If there is anything in the list, clear it out first before repopulating it with the file names.
			if (myList.size())
				myList.clear();

			while (dirIter != userDir.end( ))
			{
				myList.push_back(dirIter->getName());
				++dirIter;
			}
		}
		else
		{
			HobString extensions(myExtensions);
			HobTokenizerCSV extensionsTok(&extensions, BrowserFormatSeparator);

			if (isTracing)
				Debug::trace("GraphicFileNames", "findFileNames(): extensions = " + extensions);

			//	Get the list of Graphics in the directory
			int result = extensionsTok.first(&token);
			if (result != -1)
				myFilename.setFileNameExtension(token);
			while (result != -1)
			{
				token.insert(0, "*");		//	In order to get all files with this extension (e.g. "*.png").
				if (isTracing)
					Debug::trace("GraphicFileNames", "findFileNames(): token = " + token);
				userDir.select(token, Directory::SELECT_FILES);

				dirIter = userDir.begin();
				if (dirIter == userDir.end( ))
					if (isTracing)
						Debug::trace("GraphicFileNames", "findFileNames(): userDir.begin() == userDir.end()  !!!");
				while (dirIter != userDir.end( ))
				{
					myList.push_back(dirIter->getName());
					++dirIter;
				}
				result = extensionsTok.next(&token);
			}
		}
	}

	if ( graphicSubType == Graphic::SUBTYPE_SYSTEM || graphicSubType == Graphic::SUBTYPE_USERANDSYSTEM )
	{
		if (graphicType == Graphic::TYPE_LABELGRAPHIC && !myExtensions.empty())
		{
			//Reset the list of extensions (i.e. get a new one), otherwise, we wouldn't get the list of graphics below.
			HobString extensions(myExtensions);
			HobTokenizerCSV extensionsTok(&extensions, BrowserFormatSeparator);

			//	Then, get the list of Graphics in dir2, adding each to the list if they aren't already in the list.
			//	(Because a user/custom graphic may already exist with the same name.)
			Directory systemDir(myFilename2);

			int result = extensionsTok.first(&token);
			if (result != -1)
				myFilename2.setFileNameExtension(token);
			while (result != -1)
			{
				token.insert(0, "*");		//	In order to get all files with this extension (e.g. "*.png").
				systemDir.select(token);

				dirIter = systemDir.begin();
				while (dirIter != systemDir.end())
				{
					std::list<HobString>::iterator pos;
					pos = find(myList.begin(), myList.end(), HobString(dirIter->getName()));
					if (pos == myList.end())
						myList.push_back(dirIter->getName());
					++dirIter;
				}
				result = extensionsTok.next(&token);
			}
		}

		if ( graphicType == Graphic::TYPE_PRODUCTGRAPHIC || graphicType == Graphic::TYPE_CATEGORYGRAPHIC )
		{
			Directory systemDir(myFilename2);
			if (isTracing)
				Debug::trace("GraphicFileNames", "findFileNames(): systemDir = " + systemDir.getFullPath());

			systemDir.selectAllFiles();

			dirIter = systemDir.begin();

			if (dirIter == systemDir.end( ))
				if (isTracing)
					Debug::trace("GraphicFileNames", "findFileNames(): systemDir.begin() == systemDir.end()  !!!");
			while (dirIter != systemDir.end( ))
			{
				HobString name = dirIter->getName();
				if (!exists(name))
					myList.push_back(name);
				++dirIter;
			}
		}
	}

	// SOF-4258
	// exclude thumbnail images
	HobList_type excludedImagesList;

	for ( HobList_type::const_iterator i = myList.begin(); i != myList.end(); ++i )
		if ( HobList_type::value_type::npos == i->find( ".thumb." ) )
			excludedImagesList.push_back( *i );

	myList = excludedImagesList;

	//	Sort the filenames
	// We already sort the files, based on the passed in sort method, when we get them from the Directory object,
	// so we don't want to resort the list, because it will sort the list alphabetically, which may not be the sort
	// method passed in, such as getting the files by modification date.  So I'm commenting out the following line.
	// FT-1212
	// We do indeed need this sort when sorting by name otherwise the list could be unsorted when handling
	// dirs of system/user directories.
	if ( Directory::NAME_SORT == sort )
		myList.sort();
}


const char *GraphicFileNames::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			serializedData = "";
			serializedData.append(Integer::toString((int) myList.size()));
			serializedData.append(BrowserFormatSeparator);

			HobString listData = "";
			HobString listElement = "";

			list <HobString>::const_iterator iter = myList.begin();
			int loopCnt = 0;
			while (iter != myList.end())
			{
				if (loopCnt++)
				{
					listData.append(BrowserFormatSeparator);
					if (spacingElements)
						listData.append(" ");
				}
				// The last two characters are a ',' and a '0' or '1'.
				// They indicate if the graphic is a ROM or custom graphic, and isn't part of the name.
				// So when serializing the list, remove these from the file name.
				listElement = (*iter);
//				listElement = listElement.substr(0, listElement.size() - 2);

				listData.append(listElement);
				++iter;
			}
			listData.quoteForCSV();
			serializedData.append(listData);
		}
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}

const Filename &GraphicFileNames::getMyFilename() const
{
	return myFilename;
}
const Filename &GraphicFileNames::getMyFilename2() const
{
	return myFilename2;
}

const Filename &GraphicFileNames::getDestFilename() const
{
	return destFileName;
}
const Filename &GraphicFileNames::getDestFilename2() const
{
	return destFileName2;
}

/*
void GraphicFileNames::findFileNames()
{
	HobString token;
	util_filesystem::Directory::const_iterator dirIter;

	HobString extensions(Configuration::getInstance()->toStringCfgI(CfgInternal::GRAPHICSEXTENSIONS));
	HobTokenizerCSV extensionsTok(&extensions, BrowserFormatSeparator);

	//	First, get the list of System Graphics
	HobString dir(Configuration::getInstance()->toStringCfgI(CfgInternal::SYSTEMGRAPHICSLOCATION));
	Directory systemDir(dir);

	while (extensionsTok.next(&token) != -1)
	{
		token.insert(0, "*");		//	In order to get all files with this extension (e.g. "*.png").
		systemDir.select(token);

		dirIter = systemDir.begin();
		while ( dirIter != systemDir.end() )
		{
			myList.push_back(dirIter->getName() + ",0");
			++dirIter;
		}
	}

	//	Then, get the list of User Graphics
	dir = Configuration::getInstance()->toStringCfgI(CfgInternal::USERGRAPHICSLOCATION);
	Directory userDir(dir);

	int result = extensionsTok.first(&token);
	while (result != -1)
	{
		token.insert(0, "*");		//	In order to get all files with this extension (e.g. "*.png").
		userDir.select(token);

		dirIter = userDir.begin();
		while ( dirIter != userDir.end() )
		{
			myList.push_back(dirIter->getName() + ",1");
			++dirIter;
		}
		result = extensionsTok.next(&token);
	}
	//	Sort the filenames
	myList.sort();
	//	And get rid of duplicate filenames
	myList.unique();

	//	Eliminate the extension portion of the graphic filename
	list<HobString>::iterator iter = myList.begin();
	while(iter != myList.end())
	{
		string::size_type extPos = iter->find_last_of(".");
		if (extPos != string::npos)
			iter->erase(extPos, (iter->length() - extPos + 1));
		iter++;
	}
	myList.push_front(Configuration::getInstance()->toStringCfgI(CfgInternal::NOGRAPHICSELECTEDNAME));	//	IMPORTANT!!! WE NEED TO INTERNATIONALIZE THIS TEXT.
}
*/
