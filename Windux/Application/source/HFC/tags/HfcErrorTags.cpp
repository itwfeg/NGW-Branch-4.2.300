/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcErrorTags.h"
#include "HfcError.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int HfcErrorTags::_dataType_ = 0;
	std::string HfcErrorTags::_dataTypeName_("HfcErrorTags"); 
#endif

const std::string HfcErrorTags::recordNum 	= "";
const std::string HfcErrorTags::errorCode 	= "EC";
const std::string HfcErrorTags::recordType 	= "ER";
const std::string HfcErrorTags::failedTag 	= "FT";
const std::string HfcErrorTags::keyValue 		= "KV";

TagsList HfcErrorTags::hfcErrorTags(endHfcErrorFieldOffsets);
TagsMap  HfcErrorTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


HfcErrorTags::HfcErrorTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	if (pFieldList)
	{
		//initialize the table of tags for the Category class
		hfcErrorTags[0] 									= emptyTag;
		hfcErrorTags[fieldOffsetHfcErrRecordNum]	= recordNum;
		hfcErrorTags[fieldOffsetHfcErrErrorCode]	= errorCode;
		hfcErrorTags[fieldOffsetHfcErrRecordType]	= recordType;
		hfcErrorTags[fieldOffsetHfcErrFailedTag]	= failedTag;
		hfcErrorTags[fieldOffsetHfcErrKeyValue]	= keyValue;

		putTagsInMap(pFieldList);
	}
}

HfcErrorTags::~HfcErrorTags()
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif

}

const TagsList &HfcErrorTags::getMyTagsList() const
{
	return hfcErrorTags;
}

TagsMap &HfcErrorTags::getMyTagsMap() const
{
	return fieldMap;
}
