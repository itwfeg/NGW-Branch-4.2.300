/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SpecialMessageTags.h"
#include "DeptTags.h"
#include "ProductText.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SpecialMessageTags::dataType = 0;
std::string SpecialMessageTags::dataTypeName("SpecialMessageTags"); 
#endif

const std::string SpecialMessageTags::specialTextNumber 	= "s#";
const std::string SpecialMessageTags::specialTextText 	= "sm";

bool     SpecialMessageTags::_tagsInitialized_ = false;
TagsList SpecialMessageTags::specialtextTags(endTextFieldOffsets);
TagsMap  SpecialMessageTags::fieldMap;	//example pair: s#,fieldOffsetTextNum


SpecialMessageTags::SpecialMessageTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the the table of tags for the SpecialMessage class
		specialtextTags[0] 							= emptyTag;
		specialtextTags[fieldOffsetTextNum] 		= specialTextNumber; 
		specialtextTags[fieldOffsetTextErrorCode]	= emptyTag;
		specialtextTags[fieldOffsetTextText] 		= specialTextText;
		specialtextTags[fieldOffsetTextDeptNum]	= DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

SpecialMessageTags::~SpecialMessageTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}