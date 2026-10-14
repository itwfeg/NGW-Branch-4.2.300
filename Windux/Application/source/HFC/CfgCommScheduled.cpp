/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgCommScheduled.h"
#include "Configuration.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"

#include "MultiLanguageText.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgCommScheduled::dataType = 0;
std::string CfgCommScheduled::dataTypeName("CfgCommScheduled");
#endif

std::list<CfgFieldOffsets> CfgCommScheduled::basicCommScheduledConfigList_;


CfgCommScheduled::CfgCommScheduled()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();
}

CfgCommScheduled::CfgCommScheduled(const char *data, int len, int formatType)
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

CfgCommScheduled::~CfgCommScheduled()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// static
void CfgCommScheduled::initFieldList()
{
	if (basicCommScheduledConfigList_.empty())
	{
		basicCommScheduledConfigList_.push_back(fieldOffsetCfgCommScheduledUseAutomaticMerges);
		basicCommScheduledConfigList_.push_back(fieldOffsetCfgCommScheduledMonday);
		basicCommScheduledConfigList_.push_back(fieldOffsetCfgCommScheduledTuesday);
		basicCommScheduledConfigList_.push_back(fieldOffsetCfgCommScheduledWednesday);
		basicCommScheduledConfigList_.push_back(fieldOffsetCfgCommScheduledThursday);
		basicCommScheduledConfigList_.push_back(fieldOffsetCfgCommScheduledFriday);
		basicCommScheduledConfigList_.push_back(fieldOffsetCfgCommScheduledSaturday);
		basicCommScheduledConfigList_.push_back(fieldOffsetCfgCommScheduledSunday);
		basicCommScheduledConfigList_.push_back(fieldOffsetCfgCommScheduledDailyUpdates);
		basicCommScheduledConfigList_.push_back(fieldOffsetCfgCommScheduledStartTime);
		basicCommScheduledConfigList_.push_back(fieldOffsetCfgCommScheduledHourlyUpdates);
		basicCommScheduledConfigList_.push_back(fieldOffsetCfgCommScheduledExecMergeAfterComm);
		basicCommScheduledConfigList_.push_back(fieldOffsetCfgCommScheduledMergeHistory);

		ConfigurationTags configurationTags((FieldOffsetsList *)&basicCommScheduledConfigList_);
	}
}


void CfgCommScheduled::updateConfiguration() const
{
// Text fields are quoted when received from UI, so can't use the default implementation of parsing
// the input string and updating config as would be done on the next line.
//	Configuration::getInstance()->updateConfigurationFromUI(browserFieldList_, receivedData_);

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	if (receivedFormat_ == BrowserFormat)
	{
		HobString temp(receivedData_);
		list<CfgFieldOffsets>::const_iterator fieldIter = basicCommScheduledConfigList_.begin();

		HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
		HobString token;

		while ((tok.next(&token) != -1) && (fieldIter != basicCommScheduledConfigList_.end()))
		{
			if (token.size())
				config->updateField(*fieldIter, token);
			fieldIter++;
		}
	}
	else	
		config->updateConfigurationFromUI(basicCommScheduledConfigList_, receivedData_);
}


bool CfgCommScheduled::isSaveNeeded() const
{
	return Configuration::getInstance()->isSaveNeeded(basicCommScheduledConfigList_);
}


const char *CfgCommScheduled::serialize(int format) const
{
	serializedData = "";

	Configuration *config = Configuration::getInstance();
	if (!config)
		return serializedData.c_str();

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<CfgFieldOffsets>::iterator fieldIter;
	bool appendSeparator = false;
	HobString fieldData = "";

	switch (format)
	{
		case BrowserFormat:
		{
			HobString fieldData = "";
			fieldIter = basicCommScheduledConfigList_.begin();
			while (fieldIter != basicCommScheduledConfigList_.end())
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

		case SynchronizationTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = basicCommScheduledConfigList_.begin();
			while (fieldIter != basicCommScheduledConfigList_.end())
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
			serializedData = HobString("CfgCommScheduled::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}

	return serializedData.c_str();
}

void CfgCommScheduled::show()
{
	if (Debug::isTracing("Configuration"))
	{
		Configuration *config = Configuration::getInstance();
		if (config)
		{
			Debug::trace("Configuration", "*** CfgCommScheduled parameters ***");
			Debug::trace("Configuration", HobString("CommScheduled-UseAutoMerge:") +		config->toStringCfg(fieldOffsetCfgCommScheduledUseAutomaticMerges));
			Debug::trace("Configuration", HobString("CommScheduled-Monday:") +				config->toStringCfg(fieldOffsetCfgCommScheduledMonday));
			Debug::trace("Configuration", HobString("CommScheduled-Tuesday:") +				config->toStringCfg(fieldOffsetCfgCommScheduledTuesday));
			Debug::trace("Configuration", HobString("CommScheduled-Wednesday:") +			config->toStringCfg(fieldOffsetCfgCommScheduledWednesday));
			Debug::trace("Configuration", HobString("CommScheduled-Thursday:") +			config->toStringCfg(fieldOffsetCfgCommScheduledThursday));
			Debug::trace("Configuration", HobString("CommScheduled-Friday:") +				config->toStringCfg(fieldOffsetCfgCommScheduledFriday));
			Debug::trace("Configuration", HobString("CommScheduled-Saturday:") +			config->toStringCfg(fieldOffsetCfgCommScheduledSaturday));
			Debug::trace("Configuration", HobString("CommScheduled-Sunday:") +				config->toStringCfg(fieldOffsetCfgCommScheduledSunday));
			Debug::trace("Configuration", HobString("CommScheduled-DailyUpdates:") +		config->toStringCfg(fieldOffsetCfgCommScheduledDailyUpdates));
			Debug::trace("Configuration", HobString("CommScheduled-StartTime:") +			config->toStringCfg(fieldOffsetCfgCommScheduledStartTime));
			Debug::trace("Configuration", HobString("CommScheduled-HourlyUpdates:") +		config->toStringCfg(fieldOffsetCfgCommScheduledHourlyUpdates));
			Debug::trace("Configuration", HobString("CommScheduled-ExecMergeAfterComm:") +	config->toStringCfg(fieldOffsetCfgCommScheduledExecMergeAfterComm));
			Debug::trace("Configuration", HobString("CommScheduled-MergeHistory:") +		config->toStringCfg(fieldOffsetCfgCommScheduledMergeHistory));
		}
	}
}
