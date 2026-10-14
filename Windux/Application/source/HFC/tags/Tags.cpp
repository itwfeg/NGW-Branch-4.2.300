/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Tags.h"
#include "Debug.h"
#include "HobString.h"
#include "Integer.h"

#include <iostream> 

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Tags::dataType = 0;
std::string Tags::dataTypeName("Tags"); 
#endif

const std::string Tags::emptyTag = "";
const std::string Tags::taskNameTag = "TN";

const std::string Tags::hookActionTag = "HA";		// perform hook action (send command to hook)
const std::string Tags::hookRequestTag = "HR";		// handle hook request (command from hook to session)

Tags::Tags()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

Tags::~Tags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void Tags::putTagsInMap(FieldOffsetsList *pFieldList)
{
	if (!pFieldList || pFieldList->empty())
		return;

	const TagsList &myTagsList = getMyTagsList();

	TagsMap &myTagsMap = getMyTagsMap();

	FieldOffsetsList::iterator iter = pFieldList->begin();

  	//initialize the offset map with the known keys
  	while (iter != pFieldList->end())
  	{
  		//check the map for the key, if not found, add to the map
		if (myTagsMap.find(myTagsList[*iter]) == myTagsMap.end())
		{
			myTagsMap.insert(make_pair(myTagsList[*iter], *iter));
		}
		iter++;
	}
}

const std::string &Tags::getTag(unsigned int index) const
{
	const TagsList &myTagsList = getMyTagsList();

	//used for backup, field offsets are only known at this point
	if (myTagsList.empty() || index <= 0 || index > myTagsList.size() - 1)
	{
	  	Debug::trace("Tags", "myTagsList is empty");
 		return myTagsList[0];

	}

	return myTagsList[index];
}

UINT16 Tags::getOffset(const char *tag) const
{
	return getOffset(std::string(tag));
}

UINT16 Tags::getOffset(const std::string tag) const
{
	TagsMap &myTagsMap = getMyTagsMap();

	//used for restore, the tag will only be known at this point
	if (myTagsMap.empty())
	{
	  	//Debug::trace("Tags", "myTagsMap is empty");
		return 0;
	}

	if (myTagsMap.find(tag) != myTagsMap.end())
	{
		UINT16 offset = myTagsMap[tag];
  		//Debug::trace("Tags", HobString("found offset for tag: ") + tag + HobString("  offset: ") + HobString(Integer::toString(offset)));
		return offset;
	}
	else
	{
  		//Debug::trace("Tags", HobString("no match found for tag: ") + tag);
		return 0;
	}
}


void Tags::toString() const
{
	TagsMap::const_iterator it;
	const TagsMap &myTagsMap = getMyTagsMap();

	std::string ittag = "";
	UINT16 itnum = 0;

	it = myTagsMap.begin();
	while (it != myTagsMap.end())
	{
		ittag = it->first;
		itnum = it->second;
		std::cout << "myTagsMap tag:" << ittag << "   num:" << itnum << std::endl;
		it++;
	}
}
