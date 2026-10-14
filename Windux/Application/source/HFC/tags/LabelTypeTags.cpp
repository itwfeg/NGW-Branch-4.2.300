/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "LabelTypeTags.h"
#include "DeptTags.h"
#include "LabelType.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelTypeTags::dataType = 0;
std::string LabelTypeTags::dataTypeName("LabelTypeTags"); 
#endif

const std::string LabelTypeTags::resourceNumber	= "ri";
const std::string LabelTypeTags::resourceLength	= "le";
const std::string LabelTypeTags::resourceData	= "rs";
const std::string LabelTypeTags::sequence			= "sq";
const std::string LabelTypeTags::sequenceMax		= "qm";
const std::string LabelTypeTags::rotationOnLabelStock = "ro";

bool     LabelTypeTags::_tagsInitialized_ = false;
TagsList LabelTypeTags::labelTypeTags(endLabelTypeFieldOffsets);
TagsMap  LabelTypeTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


LabelTypeTags::LabelTypeTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the the table of tags for this class
		labelTypeTags[0]                                = emptyTag;
		labelTypeTags[fieldOffsetLabelTypeNum]          = resourceNumber;
		labelTypeTags[fieldOffsetLabelTypeIsCustom]     = emptyTag;
		labelTypeTags[fieldOffsetLabelTypeBinary]       = resourceData;
		labelTypeTags[fieldOffsetLabelTypeBinaryLength] = resourceLength;
		labelTypeTags[fieldOffsetLabelTypeSequence]     = sequence;
		labelTypeTags[fieldOffsetLabelTypeSequenceMax]  = sequenceMax;
		labelTypeTags[fieldOffsetLabelTypeDeptNum]      = DeptTags::deptNum;
		labelTypeTags[fieldOffsetLabelTypeRotationOnLabelStock] = rotationOnLabelStock;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

LabelTypeTags::~LabelTypeTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}