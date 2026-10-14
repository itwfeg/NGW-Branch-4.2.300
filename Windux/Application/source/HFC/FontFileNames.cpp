/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "FontFileNames.h"
#include "HobTokenizerCSV.h"
#include "filesys/directory.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#ifdef NO_NAMESPACES
#  define util_filesystem
#endif

#include <algorithm>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

LOCAL int dataType = 0;
LOCAL string dataTypeName = "FontFileNames";
#endif

FontFileNames::FontFileNames(short fontTypes)
	: HobList()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = registerDataType(dataTypeName);

	addInstance(dataType);
#endif

	findFontNames(fontTypes);
}

FontFileNames::FontFileNames(const char *data, int len, int formatType)
	: HobList(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = registerDataType(dataTypeName);

	addInstance(dataType);
#endif

}

FontFileNames::~FontFileNames()
{
#ifdef USE_ACCOUNTANT
	removeInstance(dataType);
#endif

}

void FontFileNames::findFontNames(short fontTypes)
{
	HobString token;
	util_filesystem::Directory::const_iterator dirIter;

	HobString extensions(Configuration::getInstance()->toStringCfgI(CfgInternal::FONTEXTENSIONS));
	Debug::trace("FontFileNames", HobString("extensions: ") + extensions);			

	HobString dir;
	if (fontTypes == Font::FONTTYPE_ALL || fontTypes == Font::FONTTYPE_CUSTOM)
	{
		HobTokenizerCSV extensionsTok(&extensions, BrowserFormatSeparator);

		//	First, get the list of User Fonts
		dir = Configuration::getInstance()->toStringCfgI(CfgInternal::USERFONTSDIR);
		Debug::trace("FontFileNames", "dir: " + dir);

		Directory userDir(dir);

		Debug::trace("FontFileNames", "--------------------------------------");
		//cout << userDir;
		Debug::trace("FontFileNames", "--------------------------------------");

		int result = extensionsTok.first(&token);
		while (result != -1)
		{
			token.insert(0, "*.");		//	In order to get all files with this extension (e.g. "*.png").
			Debug::trace("FontFileNames", "token: " + token);

			userDir.select(token);		// the "." is not needed. It is assumed because this is an ext.

			dirIter = userDir.begin();

			Debug::trace("FontFileNames", "*************************");
			while (dirIter != userDir.end())
			{
				Debug::trace("FontFileNames", HobString("dirIter->getName(): ") + dirIter->getName());
				myList.push_back(dirIter->getName());
				dirIter++;
			}
			Debug::trace("FontFileNames", "*************************");

			result = extensionsTok.next(&token);
			Debug::trace("FontFileNames", HobString("result: ") + HobString(Integer::toString(result)));			
		}
	}

	if (fontTypes == Font::FONTTYPE_ALL || fontTypes == Font::FONTTYPE_ROM)
	{
		// do nothing because we don't do System Fonts
	}


	//	Sort the filenames
	myList.sort();
	//	And get rid of duplicate filenames
	// Shouldn't need to do this anymore, we don't put duplicate names in the list,
	// especially since we are tacking on the flag indicating whether the font is custom or ROM.
	//myList.unique();

	//	Eliminate the extension portion of the font filename
	list<HobString>::iterator iter = myList.begin();
	while(iter != myList.end())
	{
		Debug::trace("FontFileNames", HobString("filename: ") + *iter);

		string::size_type extPos = iter->find_last_of(".");
		if (extPos != string::npos)
			iter->erase(extPos);

		Debug::trace("FontFileNames", HobString("filename: ") + *iter);

		iter++;
	}
}

const char *FontFileNames::serialize(int format)
{
	switch (format)
	{
		case BrowserFormat:
		{
			serializedData = "";
			serializedData.append(Integer::toString((int)myList.size() + 1));
			serializedData.append(BrowserFormatSeparator);

			HobString listData = "";
			HobString listElement = "";

			listData.append(Configuration::getInstance()->toStringCfgI(CfgInternal::NOFONTSELECTEDNAME));	//	IMPORTANT!!! WE NEED TO INTERNATIONALIZE THIS TEXT.
			if (myList.size())
				listData.append(BrowserFormatSeparator);

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
				// They indicate if the font is a ROM or custom font, and isn't part of the name.
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



/*
void FontFileNames::findFontNames()
{
	HobString token;
	util_filesystem::Directory::const_iterator dirIter;

	HobString extensions(Configuration::getInstance()->getFontsExtensions());
	HobTokenizerCSV extensionsTok(&extensions, BrowserFormatSeparator);

	//	First, get the list of System Fonts
	HobString dir(Configuration::getInstance()->toStringCfgI(CfgInternal::SYSTEMFONTSDIR));
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

	//	Then, get the list of User Fonts
	dir = Configuration::getInstance()->toStringCfgI(CfgInternal::USERFONTSDIR);
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

	//	Eliminate the extension portion of the font filename
	list<HobString>::iterator iter = myList.begin();
	while(iter != myList.end())
	{
		string::size_type extPos = iter->find_last_of(".");
		if (extPos != string::npos)
			iter->erase(extPos, (iter->length() - extPos + 1));
		iter++;
	}
	myList.push_front(Configuration::getInstance()->getNoFontSelectedName());	//	IMPORTANT!!! WE NEED TO INTERNATIONALIZE THIS TEXT.
}
*/
