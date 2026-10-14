/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ResourceTags.h"
#include "Graphic.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ResourceTags::dataType = 0;
std::string ResourceTags::dataTypeName("ResourceTags"); 
#endif

const std::string ResourceTags::resourceName				= "FN";
const std::string ResourceTags::resourceResourceData	= "rs";

TagsList ResourceTags::resourceTags(endResourceTagsFieldOffsets);
TagsMap  ResourceTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


ResourceTags::ResourceTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (pFieldList)
	{
		//initialize the the table of tags for the Class class
		resourceTags[0] 								= emptyTag;
		resourceTags[fieldOffsetName]				= resourceName;
		resourceTags[fieldOffsetIsCustom]		= emptyTag;
		resourceTags[fieldOffsetResourceData]	= resourceResourceData;

		putTagsInMap(pFieldList);
	}
}

ResourceTags::~ResourceTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const TagsList &ResourceTags::getMyTagsList() const
{
	return resourceTags;
}

TagsMap &ResourceTags::getMyTagsMap() const
{
	return fieldMap;
}
