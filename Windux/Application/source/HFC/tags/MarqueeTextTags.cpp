/* Access Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "MarqueeTextTags.h"
#include "DeptTags.h"
#include "ProductText.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int MarqueeTextTags::dataType = 0;
std::string MarqueeTextTags::dataTypeName("MarqueeTextTags"); 
#endif

bool     MarqueeTextTags::_tagsInitialized_ = false;
TagsList MarqueeTextTags::marqueeTextTags(endTextFieldOffsets);
TagsMap  MarqueeTextTags::fieldMap;  //example pair: r#,fieldOffsetTextNum

const std::string MarqueeTextTags::marqueeNumber 	= "m#";
const std::string MarqueeTextTags::marqueeText 		= "mm";


MarqueeTextTags::MarqueeTextTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the the table of tags for this class
		marqueeTextTags[0]                        = emptyTag;
		marqueeTextTags[fieldOffsetTextNum]       = marqueeNumber;
		marqueeTextTags[fieldOffsetTextErrorCode] = emptyTag;
		marqueeTextTags[fieldOffsetTextText]      = marqueeText;
		marqueeTextTags[fieldOffsetTextDeptNum]   = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

MarqueeTextTags::~MarqueeTextTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}