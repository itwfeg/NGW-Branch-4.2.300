/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "FlashFileTags.h"
#include "FlashFile.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int FlashFileTags::dataType = 0;
std::string FlashFileTags::dataTypeName("FlashFileTags"); 
#endif

const std::string FlashFileTags::flashFileName	= "F1";
const std::string FlashFileTags::flashFileDate	= "F2";

TagsList FlashFileTags::flashFileTags(endFlashFileFieldOffsets);
TagsMap  FlashFileTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


FlashFileTags::FlashFileTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (pFieldList)
	{
		//initialize the table of tags for the FlashFile class
		flashFileTags[0] 						= emptyTag;
		flashFileTags[fieldOffsetFlashFileName]	= flashFileName;
		flashFileTags[fieldOffsetFlashFileDate]	= flashFileDate;

		putTagsInMap(pFieldList);
	}
}

FlashFileTags::~FlashFileTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const TagsList &FlashFileTags::getMyTagsList() const
{
	return flashFileTags;
}

TagsMap &FlashFileTags::getMyTagsMap() const
{
	return fieldMap;
}
