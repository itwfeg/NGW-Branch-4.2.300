/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgFrequentShopper.h"
#include "Configuration.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"

#include "MultiLanguageText.h"

#include "Integer.h"
#include "Bool.h"


using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgFrequentShopper::dataType = 0;
string CfgFrequentShopper::dataTypeName("CfgFrequentShopper"); 
#endif

list<CfgFieldOffsets> CfgFrequentShopper::cfgFSFieldList_;
list<CfgFieldOffsets> CfgFrequentShopper::browserFieldList_;



CfgFrequentShopper::CfgFrequentShopper()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();
}

CfgFrequentShopper::CfgFrequentShopper(const char *data, int len, int formatType)
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

CfgFrequentShopper::~CfgFrequentShopper()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// static
void CfgFrequentShopper::initFieldList()
{
	if (cfgFSFieldList_.empty())
	{
		cfgFSFieldList_.push_back(fieldOffsetCfgFSRoundingMethod);
		cfgFSFieldList_.push_back(fieldOffsetCfgFSDiscountMethod);
		cfgFSFieldList_.push_back(fieldOffsetCfgFSDiscountType);
		cfgFSFieldList_.push_back(fieldOffsetCfgFSDiscountFormula);

		cfgFSFieldList_.push_back(fieldOffsetCfgFSTotalPriceText1);
		cfgFSFieldList_.push_back(fieldOffsetCfgFSTotalPriceText2);
		cfgFSFieldList_.push_back(fieldOffsetCfgFSSavingsText1);
		cfgFSFieldList_.push_back(fieldOffsetCfgFSSavingsText2);
		cfgFSFieldList_.push_back(fieldOffsetCfgFSUnitPriceText1);
		cfgFSFieldList_.push_back(fieldOffsetCfgFSUnitPriceText2);

		cfgFSFieldList_.push_back(fieldOffsetCfgFSRoundingWeightPrecision); // FT-861

		cfgFSFieldList_.push_back(fieldOffsetCfgEnableGlobalPercentOff);	// HT-155
		cfgFSFieldList_.push_back(fieldOffsetCfgGlobalPercentageOff);	    // HT-155

		ConfigurationTags configurationTags((FieldOffsetsList *)&cfgFSFieldList_);
	}

	if (browserFieldList_.empty())
	{
		browserFieldList_.push_back(fieldOffsetCfgFSRoundingMethod);
		browserFieldList_.push_back(fieldOffsetCfgFSPerScale);
		browserFieldList_.push_back(fieldOffsetCfgFSDiscountType);
		browserFieldList_.push_back(fieldOffsetCfgFSDiscountFormula);

		browserFieldList_.push_back(fieldOffsetCfgFSTotalPriceText1);
		browserFieldList_.push_back(fieldOffsetCfgFSTotalPriceText2);
		browserFieldList_.push_back(fieldOffsetCfgFSSavingsText1);
		browserFieldList_.push_back(fieldOffsetCfgFSSavingsText2);
		browserFieldList_.push_back(fieldOffsetCfgFSUnitPriceText1);
		browserFieldList_.push_back(fieldOffsetCfgFSUnitPriceText2);

		browserFieldList_.push_back(fieldOffsetCfgFSRoundingWeightPrecision); // FT-861

		browserFieldList_.push_back(fieldOffsetCfgEnableGlobalPercentOff);    // HT-155
		browserFieldList_.push_back(fieldOffsetCfgGlobalPercentageOff);       // HT-155
	}
}


void CfgFrequentShopper::updateConfiguration() const
{
// Text fields are quoted when received from UI, so can't use the default implementation of parsing
// the input string and updating config as would be done on the next line.
//	Configuration::getInstance()->updateConfigurationFromUI(browserFieldList_, receivedData_);

	Configuration *config = Configuration::getInstance();

	if (receivedFormat_ == BrowserFormat)
	{
		HobString temp(receivedData_);
		list<CfgFieldOffsets>::const_iterator fieldIter = browserFieldList_.begin();

		HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
		HobString token;

		while ((tok.next(&token) != -1) && (fieldIter != browserFieldList_.end()))
		{
			if (token.size())
				config->updateField(*fieldIter, token);
			fieldIter++;
		}
	}
	else	
		config->updateConfigurationFromUI(cfgFSFieldList_, receivedData_);
}


bool CfgFrequentShopper::isSaveNeeded() const
{
	return Configuration::getInstance()->isSaveNeeded(browserFieldList_);
}


const char *CfgFrequentShopper::serialize(int format) const
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

				if (*fieldIter == fieldOffsetCfgFSPerScale)
					fieldData = Bool::toString(config->isPerScale());
				else
					fieldData = config->toStringCfg(*fieldIter);

				if (isFieldQuoted(*fieldIter))
					fieldData.quoteForCSV();

				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case FrequentShopperReportFormat:
		{
			hfc::MultiLanguageText internationalize;
			ConfigurationTags configTags;

			std::string tempValue;

			serializedData.assign(internationalize("ConfigReport.FreqShopper")).append(1, '\n');
			
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFSRoundingMethod))).append(3, '\t');
			tempValue = config->toStringCfg(fieldOffsetCfgFSRoundingMethod);
			serializedData.append(tempValue).append(1, '\n');
					
			serializedData.append(2, ' ').append(internationalize("ConfigReport.PerScale")).append(5, '\t');
			tempValue = Bool::toString(config->isPerScale());
			serializedData.append(yesNoValue(tempValue)).append(1, '\n');
					
//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFSDiscountMethod))).append(1, '\t');
//			tempValue = config->toStringCfg(fieldOffsetCfgFSDiscountMethod);
//			serializedData.append(tempValue).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFSDiscountType))).append(2, '\t');
			tempValue = config->toStringCfg(fieldOffsetCfgFSDiscountType);
			serializedData.append(tempValue).append(1, '\n');
					
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFSDiscountFormula))).append(1, '\t');
			tempValue = config->toStringCfg(fieldOffsetCfgFSDiscountFormula);
			serializedData.append(tempValue).append(2, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFSUnitPriceText1))).append(3, '\t');
			tempValue = config->toStringCfg(fieldOffsetCfgFSUnitPriceText1);
			serializedData.append(tempValue).append(1, '\n');
					
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFSTotalPriceText1))).append(3, '\t');
			tempValue = config->toStringCfg(fieldOffsetCfgFSTotalPriceText1);
			serializedData.append(tempValue).append(1, '\n');
					
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFSSavingsText1))).append(3, '\t');
			tempValue = config->toStringCfg(fieldOffsetCfgFSSavingsText1);
			serializedData.append(tempValue).append(1, '\n');
					
//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFSUnitPriceText2))).append(3, '\t');
//			tempValue = config->toStringCfg(fieldOffsetCfgFSUnitPriceText2);
//			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFSTotalPriceText2))).append(3, '\t');
//			tempValue = config->toStringCfg(fieldOffsetCfgFSTotalPriceText2);
//			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFSSavingsText2))).append(3, '\t');
//			tempValue = config->toStringCfg(fieldOffsetCfgFSSavingsText2);
//			serializedData.append(tempValue).append(1, '\n');

			break;
		}

		case SynchronizationTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = cfgFSFieldList_.begin();
			while (fieldIter != cfgFSFieldList_.end())
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
			serializedData = HobString("CfgFrequentShopper::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}


bool CfgFrequentShopper::isFieldQuoted(int fieldOffsetCfg) const
{
	switch (fieldOffsetCfg)
	{
		case fieldOffsetCfgFSTotalPriceText1:
		case fieldOffsetCfgFSTotalPriceText2:
		case fieldOffsetCfgFSSavingsText1:
		case fieldOffsetCfgFSSavingsText2:
		case fieldOffsetCfgFSUnitPriceText1:
		case fieldOffsetCfgFSUnitPriceText2:
			return true;
			break;

		default:
			break;
	}
	return false;
}
