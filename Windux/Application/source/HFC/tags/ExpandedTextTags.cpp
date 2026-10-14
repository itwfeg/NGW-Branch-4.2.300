/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ExpandedTextTags.h"
#include "DeptTags.h"
#include "ProductText.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ExpandedTextTags::dataType = 0;
std::string ExpandedTextTags::dataTypeName("ExpandedTextTags"); 
#endif

const std::string ExpandedTextTags::expandedTextNumber = "r#";
const std::string ExpandedTextTags::expandedTextText   = "rt";

bool     ExpandedTextTags::_tagsInitialized_ = false;
TagsList ExpandedTextTags::expandedTextTags(endTextFieldOffsets);
TagsMap  ExpandedTextTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


ExpandedTextTags::ExpandedTextTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the the table of tags for the ExpandedText class
		expandedTextTags[0]                        = emptyTag;
		expandedTextTags[fieldOffsetTextNum]       = expandedTextNumber;
		expandedTextTags[fieldOffsetTextErrorCode] = emptyTag;
		expandedTextTags[fieldOffsetTextText]      = expandedTextText;
		expandedTextTags[fieldOffsetTextDeptNum]    = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

ExpandedTextTags::~ExpandedTextTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
