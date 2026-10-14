/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductStatisticTags.h"
#include "DeptTags.h"
#include "ProductStatistic.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductStatisticTags::dataType = 0;
std::string ProductStatisticTags::dataTypeName("ProductStatisticTags"); 
#endif

const std::string ProductStatisticTags::productNum		= "p#";
const std::string ProductStatisticTags::language		= "ln";	//	Changed because of overlap with LEAP_Security_TAG
const std::string ProductStatisticTags::hourNum			= "xh";
const std::string ProductStatisticTags::selectedCount	= "tS";
const std::string ProductStatisticTags::printedCount 	= "tp";
const std::string ProductStatisticTags::selectedTime 	= "St";
const std::string ProductStatisticTags::recordedDateTime = "sr";

TagsList ProductStatisticTags::productStatisticTags(endProductStatisticFieldOffsets);
TagsMap  ProductStatisticTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


ProductStatisticTags::ProductStatisticTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (pFieldList)
	{
		//initialize the table of tags for the ProductStatistic class
		productStatisticTags[0] 							= emptyTag;
		productStatisticTags[fieldOffsetPSProductNum]		= productNum;
		productStatisticTags[fieldOffsetPSLanguage]			= language;
		productStatisticTags[fieldOffsetPSHourNum]			= hourNum;
		productStatisticTags[fieldOffsetPSSelectedCount]	= selectedCount;
		productStatisticTags[fieldOffsetPSPrintedCount]		= printedCount;
		productStatisticTags[fieldOffsetPSSelectedTime]		= selectedTime;
		productStatisticTags[fieldOffsetPSRecordedDateTime]	= recordedDateTime;
		productStatisticTags[fieldOffsetPSDeptNum]			= DeptTags::deptNum;

		putTagsInMap(pFieldList);
	}
}

ProductStatisticTags::~ProductStatisticTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const TagsList &ProductStatisticTags::getMyTagsList() const
{
	return productStatisticTags;
}

TagsMap &ProductStatisticTags::getMyTagsMap() const
{
	return fieldMap;
}
