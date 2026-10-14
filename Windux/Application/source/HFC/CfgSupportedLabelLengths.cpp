/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgSupportedLabelLengths.h"
#include "Configuration.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"

#include "Debug.h"
#include "Integer.h"
#include "Bool.h"


using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgSupportedLabelLengths::dataType = 0;
string CfgSupportedLabelLengths::dataTypeName("CfgSupportedLabelLengths"); 
#endif

list<CfgFieldOffsets> CfgSupportedLabelLengths::cfgSLLFieldList_;



CfgSupportedLabelLengths::CfgSupportedLabelLengths()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();
}

CfgSupportedLabelLengths::CfgSupportedLabelLengths(const char *data, int len, int formatType)
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

CfgSupportedLabelLengths::~CfgSupportedLabelLengths()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// static
void CfgSupportedLabelLengths::initFieldList()
{
	if (cfgSLLFieldList_.empty())
	{
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_1_000);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_1_500);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_1_750);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_2_000);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_2_375);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_2_500);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_3_000);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_3_500);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_4_000);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_4_500);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_5_000);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_5_500);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_6_000);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_6_500);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_7_000);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_7_500);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_8_000);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_8_500);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_9_000);
		cfgSLLFieldList_.push_back(fieldOffsetCfgEnableLabelLength_9_500);

		ConfigurationTags configurationTags((FieldOffsetsList *)&cfgSLLFieldList_);
	}
}


void CfgSupportedLabelLengths::updateConfiguration() const
{
	Configuration::getInstance()->updateConfigurationFromUI(cfgSLLFieldList_, receivedData_);
}


bool CfgSupportedLabelLengths::isSaveNeeded() const
{
	return Configuration::getInstance()->isSaveNeeded(cfgSLLFieldList_);
}


const char *CfgSupportedLabelLengths::serialize(int format) const
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
			fieldIter = cfgSLLFieldList_.begin();
			while (fieldIter != cfgSLLFieldList_.end())
			{
				if (appendSeparator)
					serializedData.append(BrowserFormatSeparator);
				else
					appendSeparator = true;

				fieldData = config->toStringCfg(*fieldIter);
				serializedData.append(fieldData);

				fieldIter++;
			}
			break;
		}

		default:
			serializedData = HobString("CfgSupportedLabelLengths::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}
