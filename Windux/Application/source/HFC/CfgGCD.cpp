/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgGCD.h"
#include "Configuration.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"

#include "Integer.h"
#include "Bool.h"


using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgGCD::dataType = 0;
string CfgGCD::dataTypeName("CfgGCD"); 
#endif

list<CfgFieldOffsets> CfgGCD::cfgGCDFieldList_;



CfgGCD::CfgGCD()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();
}

CfgGCD::CfgGCD(const char *data, int len, int formatType)
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

CfgGCD::~CfgGCD()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void CfgGCD::initFieldList()
{
	if (cfgGCDFieldList_.empty())
	{
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDLayoutNumber);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDColorSchemeNumber);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDAllowOperReset);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDVolume);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDPlaybackWaitTime);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDMediaServerName);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDMediaServerIP);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDFtpUserName);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDFtpPassword);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDFtpDirectory);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDPlaylistFile);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDAutoSearch);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDSearchHour);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDSearchMinute);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDSearchAmOrPM);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDIsDnsEnabled);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDDownloadProtocol);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDLanguage);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDUseTransactionalLayout);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDIdleAdvertType);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDIdleWebsiteAddress);
		cfgGCDFieldList_.push_back(fieldOffsetCfgGCDProductAdvertType);

		ConfigurationTags configurationTags((FieldOffsetsList *)&cfgGCDFieldList_);
	}
}


void CfgGCD::updateConfiguration() const
{
	Configuration::getInstance()->updateConfigurationFromUI(cfgGCDFieldList_, receivedData_);
}


bool CfgGCD::isSaveNeeded() const
{
	return Configuration::getInstance()->isSaveNeeded(cfgGCDFieldList_);
}


const char *CfgGCD::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	Configuration *config = Configuration::getInstance();

	list<CfgFieldOffsets>::iterator fieldIter;
	bool appendSeparator = false;
	HobString fieldData = "";

	switch (format)
	{
		case BrowserFormat:
		{
			fieldIter = cfgGCDFieldList_.begin();
			while (fieldIter != cfgGCDFieldList_.end())
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

			fieldIter = cfgGCDFieldList_.begin();
			while (fieldIter != cfgGCDFieldList_.end())
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
			serializedData = HobString("CfgGCD::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	
	return serializedData.c_str();
}


void CfgGCD::getFieldAsString(int fieldCode, HobString &result, int format) const
{
//	Right now we don't do anything with format but keep it in case we need it.
	result = "";

	Configuration *config = Configuration::getInstance();

	switch (fieldCode)
	{
		case fieldOffsetCfgGCDLanguage:
			switch ( config->toIntCfg(fieldCode) )
			{
				case GcdLanguage_French:
					result = FRENCHLANGNAME;
					break;
				case GcdLanguage_Spanish:
					result = SPANISHLANGNAME;
					break;
				default:
					result = ENGLISHLANGNAME;
					break;
			}
			break;
			
		default:
			result = config->toStringCfg(fieldCode);
			break;
	}
}
