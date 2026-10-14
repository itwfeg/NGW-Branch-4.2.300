/* Evolution Scale Project
 * COPYRIGHT (c) 2009.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgAccumulateItems.h"
#include "Configuration.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"


using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgAccumulateItems::dataType = 0;
std::string CfgAccumulateItems::dataTypeName("CfgAccumulateItems"); 
#endif

std::list<CfgFieldOffsets> CfgAccumulateItems::cfgAIFieldList_;
std::list<CfgFieldOffsets> CfgAccumulateItems::browserFieldList_;



CfgAccumulateItems::CfgAccumulateItems()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();
}

CfgAccumulateItems::CfgAccumulateItems(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();

	receivedData_.assign(data, len);
	receivedFormat_ = formatType;
}

CfgAccumulateItems::~CfgAccumulateItems()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// static
void CfgAccumulateItems::initFieldList()
{
	if (cfgAIFieldList_.empty())
	{
		cfgAIFieldList_.push_back(fieldOffsetCfgEnableAccumItems);
		cfgAIFieldList_.push_back(fieldOffsetCfgEnableItemStripMode);
		cfgAIFieldList_.push_back(fieldOffsetCfgAccumItemsReturnToStartScreen);
		cfgAIFieldList_.push_back(fieldOffsetCfgEnableAccumItemsTotalsOnly);
		cfgAIFieldList_.push_back(fieldOffsetCfgAccumItemsEANFormatString);
		cfgAIFieldList_.push_back(fieldOffsetCfgAccumItemsEANPrefix);
		cfgAIFieldList_.push_back(fieldOffsetCfgAccumItemsEANCode);
		cfgAIFieldList_.push_back(fieldOffsetCfgAccumItemsScaleId);
		cfgAIFieldList_.push_back(fieldOffsetCfgAccumItemsTransactionNumber);
		cfgAIFieldList_.push_back(fieldOffsetCfgAccumItemsUPCProductNumber);
		cfgAIFieldList_.push_back(fieldOffsetCfgReadTransactionsCompatibility);
		cfgAIFieldList_.push_back(fieldOffsetCfgAccumBarcodeUseFirstItem);
		cfgAIFieldList_.push_back(fieldOffsetCfgEnableAccumItemsAndFlashkeys);
		cfgAIFieldList_.push_back(fieldOffsetCfgReadBarcodeFromCloud);
		cfgAIFieldList_.push_back(fieldOffsetCfgBarcodeServerAddress);

		ConfigurationTags configurationTags((FieldOffsetsList *)&cfgAIFieldList_);
	}

	if (browserFieldList_.empty())
	{
		browserFieldList_.push_back(fieldOffsetCfgEnableAccumItems);
		browserFieldList_.push_back(fieldOffsetCfgEnableItemStripMode);
		browserFieldList_.push_back(fieldOffsetCfgAccumItemsReturnToStartScreen);
		browserFieldList_.push_back(fieldOffsetCfgEnableAccumItemsTotalsOnly);
		browserFieldList_.push_back(fieldOffsetCfgAccumItemsEANFormatString);
		browserFieldList_.push_back(fieldOffsetCfgAccumItemsEANPrefix);
		browserFieldList_.push_back(fieldOffsetCfgAccumItemsEANCode);
		browserFieldList_.push_back(fieldOffsetCfgAccumItemsScaleId);
		browserFieldList_.push_back(fieldOffsetCfgAccumItemsTransactionNumber);
		//browserFieldList_.push_back(fieldOffsetCfgAccumItemsUPCProductNumber);
		browserFieldList_.push_back(fieldOffsetCfgReadTransactionsCompatibility);
		browserFieldList_.push_back(fieldOffsetCfgAccumBarcodeUseFirstItem);
		browserFieldList_.push_back(fieldOffsetCfgEnableAccumItemsAndFlashkeys);
		browserFieldList_.push_back(fieldOffsetCfgReadBarcodeFromCloud);
		browserFieldList_.push_back(fieldOffsetCfgBarcodeServerAddress);
	}
}


void CfgAccumulateItems::updateConfiguration() const
{
	Configuration::getInstance()->updateConfigurationFromUI(((receivedFormat_ == BrowserFormat) ? browserFieldList_ : cfgAIFieldList_), receivedData_);
}


bool CfgAccumulateItems::isSaveNeeded() const
{
	return Configuration::getInstance()->isSaveNeeded((receivedFormat_ == BrowserFormat) ? browserFieldList_ : cfgAIFieldList_);
}


const char *CfgAccumulateItems::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	Configuration *config = Configuration::getInstance();

	std::list<CfgFieldOffsets>::iterator fieldIter;
	bool appendSeparator = false;

	switch (format)
	{
		case BrowserFormat:
		{
			HobString fieldData = "";
			fieldIter = browserFieldList_.begin();
			while (fieldIter != browserFieldList_.end())
			{
				if (appendSeparator)
					serializedData.append(BrowserFormatSeparator);
				else
					appendSeparator = true;

				getFieldAsString(*fieldIter, fieldData, BrowserFormat);
				serializedData.append(fieldData);

				fieldIter++;
			}
			break;
		}

		case SynchronizationTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = cfgAIFieldList_.begin();
			while (fieldIter != cfgAIFieldList_.end())
			{
				tag = configTags.getTag(*fieldIter);
				if (tag.size())
				{
					fieldData = "";
					getFieldAsString(*fieldIter, fieldData, EvolutionTaggedFormat);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		default:
			serializedData = HobString("CfgAccumulateItems::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}


void CfgAccumulateItems::getFieldAsString(int fieldCode, HobString &result, int format) const
{
	result = "";

	Configuration *config = Configuration::getInstance();

	switch (fieldCode)
	{
		case fieldOffsetCfgAccumItemsEANPrefix:
		{
			result = config->toStringCfg(fieldOffsetCfgAccumItemsEANPrefix);
			HobString format = config->toStringCfg(fieldOffsetCfgAccumItemsEANFormatString);
			int numChars = format.countConsecutiveChars('X');
			if (numChars > 0)
				result.getCharsWithPadding(numChars);
			// We don't want the leading zeros that getCharsWithPadding may have added, so get rid of them.
			// We still want to use getCharsWithPadding though, because it will truncate the left-most digits if it's too long.
			int temp = Integer::valueOf(result);
			result = Integer::toString(temp);
			break;
		}
		case fieldOffsetCfgAccumItemsEANCode:
		{
			result = config->toStringCfg(fieldOffsetCfgAccumItemsEANCode);
			HobString format = config->toStringCfg(fieldOffsetCfgAccumItemsEANFormatString);
			int numChars = format.countConsecutiveChars('E');
			if (numChars > 0)
				result.getCharsWithPadding(numChars);
			break;
		}
		case fieldOffsetCfgAccumItemsScaleId:
		{
			result = Integer::toString(config->toIntCfg(fieldOffsetCfgAccumItemsScaleId));
			HobString format = config->toStringCfg(fieldOffsetCfgAccumItemsEANFormatString);
			int numChars = format.countConsecutiveChars('S');
			if (numChars > 0)
				result.getCharsWithPadding(numChars);
			// We don't want the leading zeros that getCharsWithPadding may have added, so get rid of them.
			// We still want to use getCharsWithPadding though, because it will truncate the left-most digits if it's too long.
			int temp = Integer::valueOf(result);
			result = Integer::toString(temp);
			break;
		}
		case fieldOffsetCfgAccumItemsTransactionNumber:
		{
			result = Integer::toString(config->toIntCfg(fieldOffsetCfgAccumItemsTransactionNumber));
			HobString format = config->toStringCfg(fieldOffsetCfgAccumItemsEANFormatString);
			int numChars = format.countConsecutiveChars('T');
			if (numChars > 0)
				result.getCharsWithPadding(numChars);
			// We don't want the leading zeros that getCharsWithPadding may have added, so get rid of them.
			// We still want to use getCharsWithPadding though, because it will truncate the left-most digits if it's too long.
			int temp = Long::valueOf(result);
			result = Long::toString(temp);
			break;
		}
			
		default:
			result = config->toStringCfg(fieldCode);
			break;
	}
}
