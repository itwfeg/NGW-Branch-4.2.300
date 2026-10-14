/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */
#include "Statistics.h"
#include "StatisticsTags.h"
#include "ConfigurationTags.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int StatisticsTags::dataType = 0;
std::string StatisticsTags::dataTypeName("StatisticsTags"); 
#endif
const std::string StatisticsTags::scaleType            = "st";
const std::string StatisticsTags::freeFlashMemory      = "??";
const std::string StatisticsTags::totalFlashMemory     = "??";
const std::string StatisticsTags::freeRamMemory        = "fm";
const std::string StatisticsTags::totalRamMemory       = "me";
const std::string StatisticsTags::productsCount        = "#p";
const std::string StatisticsTags::classesCount         = "#c";
const std::string StatisticsTags::expandedTextCount    = "#r";
const std::string StatisticsTags::specialMessagesCount = "#s";
const std::string StatisticsTags::productNotesCount    = "#N";
const std::string StatisticsTags::nutrifactsCount      = "#n";
const std::string StatisticsTags::labelTypesCount      = "#l";
const std::string StatisticsTags::graphicsCount        = "#g";
const std::string StatisticsTags::fontsCount           = "#f";
const std::string StatisticsTags::operatorsCount       = "#o";
const std::string StatisticsTags::totalsCount          = "#t";

TagsList StatisticsTags::statisticsTags(endStatisticsFieldOffsets);
TagsMap  StatisticsTags::fieldMap;	//example pair: r#,fieldOffsetTextNum


StatisticsTags::StatisticsTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (pFieldList)
	{
		//initialize the the table of tags for the Statistics class
		statisticsTags[0] 								= emptyTag;
		statisticsTags[fieldOffsetFreeFlashMemory] 		= freeFlashMemory;
		statisticsTags[fieldOffsetUsedFlashMemory]		= totalFlashMemory;
		statisticsTags[fieldOffsetFreeRamMemory]		= freeRamMemory;
		statisticsTags[fieldOffsetUsedRamMemory]		= totalRamMemory;
		statisticsTags[fieldOffsetProductsCount]		= productsCount;
		statisticsTags[fieldOffsetClassesCount]			= classesCount;
		statisticsTags[fieldOffsetExpandedTextCount]	= expandedTextCount;
		statisticsTags[fieldOffsetSpecialMessagesCount] = specialMessagesCount;
		statisticsTags[fieldOffsetProductNotesCount]	= productNotesCount;
		statisticsTags[fieldOffsetNutrifactsCount]		= nutrifactsCount;
		statisticsTags[fieldOffsetLabelTypesCount]		= labelTypesCount;
		statisticsTags[fieldOffsetGraphicsCount]		= graphicsCount;
		statisticsTags[fieldOffsetFontsCount]			= fontsCount;
		statisticsTags[fieldOffsetOperatorsCount]		= operatorsCount;
		statisticsTags[fieldOffsetTotalsCount]			= totalsCount;
		statisticsTags[fieldOffsetScaleName]			= scaleType;
		statisticsTags[fieldOffsetFirmwareVersionStat]	= ConfigurationTags::getFirmwareVersionTag();
		statisticsTags[fieldOffsetFirmwareRevisionStat]	= ConfigurationTags::getFirmwareRevisionTag();

		putTagsInMap(pFieldList);
	}
}

StatisticsTags::~StatisticsTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const TagsList &StatisticsTags::getMyTagsList() const
{
	return statisticsTags;
}

TagsMap &StatisticsTags::getMyTagsMap() const
{
	return fieldMap;
}
