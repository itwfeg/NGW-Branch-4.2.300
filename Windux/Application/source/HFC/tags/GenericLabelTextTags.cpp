/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "GenericLabelTextTags.h"
#include "GenericLabelText.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int GenericLabelTextTags::dataType = 0;
std::string GenericLabelTextTags::dataTypeName("GenericLabelTextTags"); 
#endif

const std::string GenericLabelTextTags::textNum  = "y#";
const std::string GenericLabelTextTags::textText = "sy";

TagsList GenericLabelTextTags::genericLabelTextTags(endGenericLabelTextOffsets);
TagsMap  GenericLabelTextTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


GenericLabelTextTags::GenericLabelTextTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (pFieldList)
	{
		//initialize the the table of tags for the GenericLabelText class
		genericLabelTextTags[0] 								= emptyTag;
		genericLabelTextTags[fieldOffsetGenericTextNum]	= textNum;
		genericLabelTextTags[fieldOffsetGenericTextText]= textText;

		putTagsInMap(pFieldList);
	}
}

GenericLabelTextTags::~GenericLabelTextTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const TagsList &GenericLabelTextTags::getMyTagsList() const
{
	return genericLabelTextTags;
}

TagsMap &GenericLabelTextTags::getMyTagsMap() const
{
	return fieldMap;
}
