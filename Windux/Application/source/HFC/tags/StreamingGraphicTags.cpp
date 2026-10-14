/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "StreamingGraphicTags.h"
#include "StreamingGraphic.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int StreamingGraphicTags::dataType = 0;
std::string StreamingGraphicTags::dataTypeName("StreamingGraphicTags"); 
#endif

const std::string StreamingGraphicTags::streamingGraphicNum 		= "G#";
const std::string StreamingGraphicTags::nextStreamingGraphicNum 	= "Gn";
const std::string StreamingGraphicTags::imageFilename 				= "i1";
const std::string StreamingGraphicTags::displayTime 					= "Gd";
const std::string StreamingGraphicTags::actionType 					= "Gt";
const std::string StreamingGraphicTags::actionParameter 				= "Gp";

TagsList StreamingGraphicTags::streamingGraphicTags(endStreamingGraphicFieldOffsets);
TagsMap  StreamingGraphicTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


StreamingGraphicTags::StreamingGraphicTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (pFieldList)
	{
		//initialize the table of tags for the StreamingGraphic class
		streamingGraphicTags[0] 									= emptyTag;
		streamingGraphicTags[fieldOffsetSGNum]					= streamingGraphicNum;
		streamingGraphicTags[fieldOffsetSGNextNum]			= nextStreamingGraphicNum;
		streamingGraphicTags[fieldOffsetSGImage]				= imageFilename;
		streamingGraphicTags[fieldOffsetSGDisplayTime]		= displayTime;
		streamingGraphicTags[fieldOffsetSGActionType]		= actionType;
		streamingGraphicTags[fieldOffsetSGActionParameter]	= actionParameter;

		putTagsInMap(pFieldList);
	}
}

StreamingGraphicTags::~StreamingGraphicTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const TagsList &StreamingGraphicTags::getMyTagsList() const
{
	return streamingGraphicTags;
}

TagsMap &StreamingGraphicTags::getMyTagsMap() const
{
	return fieldMap;
}
