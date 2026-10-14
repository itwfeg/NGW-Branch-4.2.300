/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgPrintedFields.h"
#include "Configuration.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"

#include "MultiLanguageText.h"

#include "Integer.h"
#include "Bool.h"


using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgPrintedFields::dataType = 0;
std::string CfgPrintedFields::dataTypeName("CfgPrintedFields"); 
#endif

std::list<CfgFieldOffsets> CfgPrintedFields::cfgPFFieldList_;	



CfgPrintedFields::CfgPrintedFields()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();
}

CfgPrintedFields::CfgPrintedFields(const char *data, int len, int formatType)
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

CfgPrintedFields::~CfgPrintedFields()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// static
void CfgPrintedFields::initFieldList()
{
	if (cfgPFFieldList_.empty())
	{
		cfgPFFieldList_.push_back(fieldOffsetCfgPFSellByDatePrinted);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFSellByDateJulian);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFPackedOnDatePrinted);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFPackedOnDateJulian);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFProductLifeDatePrinted);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFProductLifeDateJulian);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFUnitPricePrinted);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFByCountPrinted);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFBarCodePrinted);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFCurrencySymbolPrinted);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFTextNamesPreprinted);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFWeightOnByCountPrinted);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFOneOnByCountPrinted);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFSecondCheckDigit);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFNetWtOnFixedWeightAlways);
		cfgPFFieldList_.push_back(fieldOffsetCfgPFFixedWeightFormat);
		cfgPFFieldList_.push_back(fieldOffsetCfgPriceChangedLabelIndicator);
		cfgPFFieldList_.push_back(fieldOffsetCfgUnitPricePrintMethod);

		ConfigurationTags configurationTags((FieldOffsetsList *)&cfgPFFieldList_);
	}
}


void CfgPrintedFields::updateConfiguration() const
{
	Configuration::getInstance()->updateConfigurationFromUI(cfgPFFieldList_, receivedData_);
}


bool CfgPrintedFields::isSaveNeeded() const
{
	return Configuration::getInstance()->isSaveNeeded(cfgPFFieldList_);
}


const char *CfgPrintedFields::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	Configuration *config = Configuration::getInstance();

	list<CfgFieldOffsets>::iterator fieldIter;
	bool appendSeparator = false;

	switch (format)
	{
		case BrowserFormat:
		{
			HobString fieldData = "";
			fieldIter = cfgPFFieldList_.begin();
			while (fieldIter != cfgPFFieldList_.end())
			{
				if (appendSeparator)
					serializedData.append(BrowserFormatSeparator);
				else
					appendSeparator = true;

				fieldData = config->toStringCfg(*fieldIter);
				if (*fieldIter == fieldOffsetCfgPriceChangedLabelIndicator)
					fieldData.quoteForCSV();
				serializedData.append(fieldData);

				fieldIter++;
			}
			break;
		}

		case FieldsToPrintReportFormat:
		{
			hfc::MultiLanguageText internationalize;
			ConfigurationTags configTags;

			string tempValue_;

			serializedData.assign(internationalize("ConfigReport.FieldsToPrint")).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPFUnitPricePrinted))).append(5, '\t');
			tempValue_ = config->toStringCfg(fieldOffsetCfgPFUnitPricePrinted);
			serializedData.append(yesNoValue(tempValue_)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPFByCountPrinted))).append(5, '\t');
			tempValue_ = config->toStringCfg(fieldOffsetCfgPFByCountPrinted);
			serializedData.append(yesNoValue(tempValue_)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPFWeightOnByCountPrinted))).append(2, '\t');
			tempValue_ = config->toStringCfg(fieldOffsetCfgPFWeightOnByCountPrinted);
			serializedData.append(yesNoValue(tempValue_)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPFOneOnByCountPrinted))).append(2, '\t');
			tempValue_ = config->toStringCfg(fieldOffsetCfgPFOneOnByCountPrinted);
			serializedData.append(yesNoValue(tempValue_)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPFBarCodePrinted))).append(5, '\t');
			tempValue_ = config->toStringCfg(fieldOffsetCfgPFBarCodePrinted);
			serializedData.append(yesNoValue(tempValue_)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPFSecondCheckDigit))).append(3, '\t');
			tempValue_ = config->toStringCfg(fieldOffsetCfgPFSecondCheckDigit);
			serializedData.append(yesNoValue(tempValue_)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPFTextNamesPreprinted))).append(2, '\t');
			tempValue_ = config->toStringCfg(fieldOffsetCfgPFTextNamesPreprinted);
			serializedData.append(yesNoValue(tempValue_)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPFCurrencySymbolPrinted))).append(3, '\t');
			tempValue_ = config->toStringCfg(fieldOffsetCfgPFCurrencySymbolPrinted);
			serializedData.append(yesNoValue(tempValue_)).append(2, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPFSellByDatePrinted))).append(4, '\t');
			tempValue_ = config->toStringCfg(fieldOffsetCfgPFSellByDatePrinted);
			serializedData.append(yesNoValue(tempValue_)).append(1, '\n');

			serializedData.append(4, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPFSellByDateJulian))).append(2, '\t');
			tempValue_ = config->toStringCfg(fieldOffsetCfgPFSellByDateJulian);
			serializedData.append(yesNoValue(tempValue_)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPFPackedOnDatePrinted))).append(3, '\t');
			tempValue_ = config->toStringCfg(fieldOffsetCfgPFPackedOnDatePrinted);
			serializedData.append(yesNoValue(tempValue_)).append(1, '\n');

			serializedData.append(4, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPFPackedOnDateJulian))).append(1, '\t');
			tempValue_ = config->toStringCfg(fieldOffsetCfgPFPackedOnDateJulian);
			serializedData.append(yesNoValue(tempValue_)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPFProductLifeDatePrinted))).append(3, '\t');
			tempValue_ = config->toStringCfg(fieldOffsetCfgPFProductLifeDatePrinted);
			serializedData.append(yesNoValue(tempValue_)).append(1, '\n');

			serializedData.append(4, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPFProductLifeDateJulian))).append(1, '\t');
			tempValue_ = config->toStringCfg(fieldOffsetCfgPFProductLifeDateJulian);
			serializedData.append(yesNoValue(tempValue_)).append(1, '\n');

			break;
		}

		case SynchronizationTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = cfgPFFieldList_.begin();
			while (fieldIter != cfgPFFieldList_.end())
			{
				tag = configTags.getTag(*fieldIter);
				if (tag.size())
				{
					fieldData = config->toStringCfg(*fieldIter);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		default:
			serializedData = HobString("CfgPrintedFields::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}
