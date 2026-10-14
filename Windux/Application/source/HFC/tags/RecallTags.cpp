/* Fastrack Scale Project
 * COPYRIGHT (c) 2013.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "RecallTags.h"
#include "Recall.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RecallTags::dataType = 0;
std::string RecallTags::dataTypeName("RecallTags"); 
#endif

const std::string RecallTags::recallNum 		= "Rn";
const std::string RecallTags::recallName		= "RN";
const std::string RecallTags::recallText 		= "RT";
const std::string RecallTags::receivedDateTime	= "RD";
const std::string RecallTags::acknowledged		= "RA";

TagsList RecallTags::recallTags(endRecallFieldOffsets);
TagsMap  RecallTags::fieldMap;	//example pair: r#,fieldOffsetTextNum


RecallTags::RecallTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (pFieldList)
	{
		//initialize the the table of tags for the HobOperator class
		recallTags[0] 							= emptyTag;
		recallTags[fieldOffsetRecallNum] 		= recallNum;
		recallTags[fieldOffsetRecallName] 		= recallName;
		recallTags[fieldOffsetRecallText] 		= recallText;
		recallTags[fieldOffsetReceivedDateTime] = receivedDateTime;
		recallTags[fieldOffsetAcknowledged]		= acknowledged;

		putTagsInMap(pFieldList);
	}
}

RecallTags::~RecallTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const TagsList &RecallTags::getMyTagsList() const
{
	return recallTags;
}

TagsMap &RecallTags::getMyTagsMap() const
{
	return fieldMap;
}
