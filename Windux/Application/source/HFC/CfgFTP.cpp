/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgFTP.h"
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

int CfgFTP::dataType = 0;
std::string CfgFTP::dataTypeName("CfgFTP"); 
#endif

std::list<CfgFieldOffsets> CfgFTP::basicFTPConfigList_;


CfgFTP::CfgFTP()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();
}

CfgFTP::CfgFTP(const char *data, int len, int formatType)
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

CfgFTP::~CfgFTP()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// static
void CfgFTP::initFieldList()		//added ats 10012002
{
	if (basicFTPConfigList_.empty())
	{
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPServerIporHostName);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPUserName);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPPassword);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPSubDir);		//	Used only for serialize. Info only for the Browser.
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPstartTime);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPuseAutomatic);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPmonday);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPtuesday);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPwednesday);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPthursday);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPfriday);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPsaturday);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPsunday);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPdownloadFirmwareOnly);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPdownloadInstallFirmware);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPdailyUpdates);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPhourlyUpdates);                 
		basicFTPConfigList_.push_back(fieldOffsetCfgFTP15MinuteUpdates);		
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPUpdatePLUData);         
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPUpdateLabelData);		
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPUpdateFlashkeyData);    
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPUpdateOperatorData); 
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPUpdatePlaylists);		
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPUpdateAlerts);		
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPUpdateScaleConfiguration);        
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPUpdateSynchronizationConfiguration);	
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPUpdateStoreNameNumber);             
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPUpdateVideos);				
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPUpdateImages);				
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPUpdateAudio);	
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPUpdateFirmware);          
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPSendPLUData);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPSendLabelData);            
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPSendFlashkeyData);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPSendOperatorData);		
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPSendPlaylists);		
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPSendAlerts);					
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPSendScaleConfiguration);	
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPSendSynchronizationConfiguration);	
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPSendStoreNameNumber);
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPSendVideos);			
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPSendImages);		
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPSendAudio);			
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPSendFirmware);		
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPAllowOperatorSendToDept);		
		basicFTPConfigList_.push_back(fieldOffsetCfgFTPAllowOperatorUpdateNow);
		basicFTPConfigList_.push_back(fieldOffsetCfgAutoBackupDBFrequency);

		ConfigurationTags configurationTags((FieldOffsetsList *)&basicFTPConfigList_);
	}
}


void CfgFTP::updateConfiguration() const
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
		list<CfgFieldOffsets>::const_iterator fieldIter = basicFTPConfigList_.begin();

		HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
		HobString token;

		while ((tok.next(&token) != -1) && (fieldIter != basicFTPConfigList_.end()))
		{
			if (token.size())
				config->updateField(*fieldIter, token);
			fieldIter++;
		}
	}
	else	
		config->updateConfigurationFromUI(basicFTPConfigList_, receivedData_);
}


bool CfgFTP::isSaveNeeded() const
{
	return Configuration::getInstance()->isSaveNeeded(basicFTPConfigList_);
}


const char *CfgFTP::serialize(int format) const
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
			fieldIter = basicFTPConfigList_.begin();
			while (fieldIter != basicFTPConfigList_.end())
			{
				if (appendSeparator)
					serializedData.append(BrowserFormatSeparator);
				else
					appendSeparator = true;

				fieldData = config->toStringCfg(*fieldIter);

				if (isFieldQuoted(*fieldIter))
					fieldData.quoteForCSV();

				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case FTPReportFormat:
		{
			hfc::MultiLanguageText internationalize;
			ConfigurationTags configTags;

			serializedData.assign(internationalize("ConfigReport.FTP")).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFTPServerIporHostName))).append(4, '\t').
					append(config->toStringCfg(fieldOffsetCfgFTPServerIporHostName)).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFTPUserName))).append(1, '\t').
					append(config->toStringCfg(fieldOffsetCfgFTPUserName)).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFTPPassword))).append(2, '\t').
					append(config->toStringCfg(fieldOffsetCfgFTPPassword)).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFTPSubDir))).append(2, '\t').
					append(config->toStringCfg(fieldOffsetCfgFTPSubDir)).append(1, '\n');

			break;
		}

		case SynchronizationTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = basicFTPConfigList_.begin();
			while (fieldIter != basicFTPConfigList_.end())
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
			serializedData = HobString("CfgFTP::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}

	return serializedData.c_str();
}

bool CfgFTP::isFieldQuoted(int fieldOffsetCfg) const
{
	switch (fieldOffsetCfg)
	{
		case fieldOffsetCfgFTPUserName:
		case fieldOffsetCfgFTPPassword:
		case fieldOffsetCfgFTPSubDir:
			return true;
			break;

		default:
			break;
	}
	return false;
}

void CfgFTP::show()
{
	if (Debug::isTracing("Configuration"))
	{
		Configuration *config = Configuration::getInstance();
		if (config)
		{
			Debug::trace("Configuration", "*** CfgFTP parameters ***");
			Debug::trace("Configuration", HobString("FTPServerIPorHostName:") + config->toStringCfg(fieldOffsetCfgFTPServerIporHostName));
			Debug::trace("Configuration", HobString("FTPUserName:") + config->toStringCfg(fieldOffsetCfgFTPUserName));
			Debug::trace("Configuration", HobString("FTPPassword:") + config->toStringCfg(fieldOffsetCfgFTPPassword));
			Debug::trace("Configuration", HobString("FTPSubDirectory:") + config->toStringCfg(fieldOffsetCfgFTPSubDir));
		}
	}
}
