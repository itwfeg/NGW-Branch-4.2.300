/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ScaleStatusTags.h"
#include "ScaleStatus.h"
#include "ConfigurationTags.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ScaleStatusTags::dataType = 0;
std::string ScaleStatusTags::dataTypeName("ScaleStatusTags"); 
#endif

const std::string ScaleStatusTags::scaleCondition = "sc";
const std::string ScaleStatusTags::scaleError     = "se";
const std::string ScaleStatusTags::scaleStatus    = "ss";
const std::string ScaleStatusTags::scaleType      = "st";

TagsList ScaleStatusTags::scaleStatusTags(endScaleStatusFieldOffsets);
TagsMap  ScaleStatusTags::fieldMap;	//example pair: r#,fieldOffsetTextNum


ScaleStatusTags::ScaleStatusTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (pFieldList)
	{
		//initialize the the table of tags for the Statistics class
		scaleStatusTags[0] 									= emptyTag;
		scaleStatusTags[fieldOffsetScaleCondition]	= scaleCondition;
		scaleStatusTags[fieldOffsetScaleError]			= scaleError;
		scaleStatusTags[fieldOffsetScaleStatus]		= scaleStatus;
		scaleStatusTags[fieldOffsetStatusScaleType]	= scaleType;
		scaleStatusTags[fieldOffsetStatusScaleModel] = ConfigurationTags::scaleModel;

		putTagsInMap(pFieldList);
	}
}

ScaleStatusTags::~ScaleStatusTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const TagsList &ScaleStatusTags::getMyTagsList() const
{
	return scaleStatusTags;
}

TagsMap &ScaleStatusTags::getMyTagsMap() const
{
	return fieldMap;
}
