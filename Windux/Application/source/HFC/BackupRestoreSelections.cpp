/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "BackupRestoreSelections.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int BackupRestoreSelections::dataType = 0;
std::string BackupRestoreSelections::dataTypeName("BackupRestoreSelections"); 
#endif

BackupRestoreSelections::BackupRestoreSelections()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

BackupRestoreSelections::BackupRestoreSelections(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			// format : fileName, mode, dataFormat, BRPluData, BRResources, BROperatorData,
			//				BRStore, BRScaleConfig, BRWrapperSettings, BRSoftkeyData,
			//          (optionally) BRCOOLData, (optionally) BRSelfServeData

			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetBRFileName:
						token.unQuoteForCSV();
						setFileName(token);
						break;
					case fieldOffsetBRaltFileName:
						token.unQuoteForCSV();
						setAltFileName(token);
						break;
					case fieldOffsetBRMode:
						setMode((BRMode)Integer::valueOf(token));
						break;
					case fieldOffsetBRFormat:
						setFormat((BRFormat)Integer::valueOf(token));
						break;
					case fieldOffsetBRPluData:
						setPluDataSelected(Bool::valueOf(token));
						break;
					case fieldOffsetBRResources:
						setResourcesSelected(Bool::valueOf(token));
						break;
					case fieldOffsetBROperatorData:
						setOperatorDataSelected(Bool::valueOf(token));
						break;
					case fieldOffsetBRStoreData:
						setStoreDataSelected(Bool::valueOf(token));
						break;
					case fieldOffsetBRScaleConfig:
						setScaleConfigSelected(Bool::valueOf(token));
						break;
					case fieldOffsetBRWrapperSettings:
						setWrapperSettingsSelected(Bool::valueOf(token));
						break;
					case fieldOffsetBRSoftkeyData:
						setSoftkeyDataSelected(Bool::valueOf(token));
						break;
					case fieldOffsetBRCOOLData:
						setCOOLDataSelected(Bool::valueOf(token));
						break;
					case fieldOffsetBRSelfServeData:
						setSelfServeDataSelected(Bool::valueOf(token));
						break;
					case fieldOffsetBRSynchronizeConfig:
						setSynchronizeConfigSelected(Bool::valueOf(token));
						break;
					case fieldOffsetBRScaleStatus:
						setScaleStatusSelected(Bool::valueOf(token));
						break;
					case fieldOffsetBRScaleStatistics:
						setScaleStatisticsSelected(Bool::valueOf(token));
						break;
					case fieldOffsetBRPrinterSettings:
						setPrinterSettingsSelected(Bool::valueOf(token));
						break;
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}


// private
BackupRestoreSelections::~BackupRestoreSelections()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void BackupRestoreSelections::init()
{
	fileName = "";
	altFileName = "";
	mode = BRIdle;
	format = BREvolution;
	brPluData = false;
	brResources = false;
	brOperatorData = false;
	brStoreData = false;
	brScaleConfig = false;
	brWrapperSettings = false;
	brSoftkeyData = false;
	brCOOLData = false;
	brSelfServeData = false;
	brSynchronizeConfigData = false;
	everyDataTypeSelected = false;
	brScaleStatusData = false;
	brScaleStatisticsData = false;
	brPrinterSettings = false;
	displayStats = true;
}

HobString BackupRestoreSelections::getFileName() const
{
	return fileName;
}
void BackupRestoreSelections::setFileName(HobString fileName)
{
	this->fileName = fileName;
}

HobString BackupRestoreSelections::getAltFileName() const
{
	return altFileName;
}
void BackupRestoreSelections::setAltFileName(HobString fileName)
{
	this->altFileName = fileName;
}

BRMode BackupRestoreSelections::getMode() const
{
	return mode;
}
void BackupRestoreSelections::setMode(BRMode mode)
{
	this->mode = mode;
}

BRFormat BackupRestoreSelections::getFormat() const
{
	return format;
}
void BackupRestoreSelections::setFormat(BRFormat format)
{
	this->format = format;
}

bool BackupRestoreSelections::isPluDataSelected() const
{
	return brPluData | everyDataTypeSelected;
}
void BackupRestoreSelections::setPluDataSelected(bool br)
{
	brPluData = br;
}

bool BackupRestoreSelections::isResourcesSelected() const
{
	return brResources | everyDataTypeSelected;
}
void BackupRestoreSelections::setResourcesSelected(bool br)
{
	brResources = br;
}

bool BackupRestoreSelections::isOperatorDataSelected() const
{
	return brOperatorData | everyDataTypeSelected;
}
void BackupRestoreSelections::setOperatorDataSelected(bool br)
{
	brOperatorData = br;
}

bool BackupRestoreSelections::isStoreDataSelected() const
{
	return brStoreData | everyDataTypeSelected;
}
void BackupRestoreSelections::setStoreDataSelected(bool br)
{
	brStoreData = br;
}

bool BackupRestoreSelections::isScaleConfigSelected() const
{
	return brScaleConfig | everyDataTypeSelected;
}
void BackupRestoreSelections::setScaleConfigSelected(bool br)
{
	brScaleConfig = br;
}

bool BackupRestoreSelections::isWrapperSettingsSelected() const
{
	return brWrapperSettings | everyDataTypeSelected;
}
void BackupRestoreSelections::setWrapperSettingsSelected(bool br)
{
	brWrapperSettings = br;
}

bool BackupRestoreSelections::isSoftkeyDataSelected() const
{
	return brSoftkeyData | everyDataTypeSelected;
}
void BackupRestoreSelections::setSoftkeyDataSelected(bool br)
{
	brSoftkeyData = br;
}

bool BackupRestoreSelections::isCOOLDataSelected() const
{
	//return brPluData | brCOOLData | everyDataTypeSelected;
	return brCOOLData | everyDataTypeSelected;		// use this return if we ever separate these from being part of PLU Data
}
void BackupRestoreSelections::setCOOLDataSelected(bool br)
{
	brCOOLData = br;
}

bool BackupRestoreSelections::isSelfServeDataSelected() const
{
	//return brPluData | brSelfServeData | everyDataTypeSelected;
	return brSelfServeData | everyDataTypeSelected;		// use this return if we ever separate these from being part of PLU Data
}
void BackupRestoreSelections::setSelfServeDataSelected(bool br)
{
	brSelfServeData = br;
}

bool BackupRestoreSelections::isSynchronizeConfigSelected() const
{
	return brSynchronizeConfigData | everyDataTypeSelected;
}
void BackupRestoreSelections::setSynchronizeConfigSelected(bool br)
{
	brSynchronizeConfigData = br;
}

bool BackupRestoreSelections::isEveryDataTypeSelected() const
{
	return everyDataTypeSelected;
}
void BackupRestoreSelections::setEveryDataTypeSelected(bool br)
{
	everyDataTypeSelected = br;
}

bool BackupRestoreSelections::isScaleStatusSelected() const
{
	return brScaleStatusData;
}
void BackupRestoreSelections::setScaleStatusSelected(bool br)
{
	brScaleStatusData = br;
}

bool BackupRestoreSelections::isScaleStatisticsSelected() const
{
	return brScaleStatisticsData;
}
void BackupRestoreSelections::setScaleStatisticsSelected(bool br)
{
	brScaleStatisticsData = br;
}

bool BackupRestoreSelections::isPrinterSettingsSelected() const
{
	return brPrinterSettings;
}
void BackupRestoreSelections::setPrinterSettingsSelected(bool br)
{
	brPrinterSettings = br;
}

bool BackupRestoreSelections::isDisplayStats() const
{
	return displayStats;
}
void BackupRestoreSelections::setDisplayStats(bool ds)
{
	displayStats = ds;
}


BackupRestoreSelections& BackupRestoreSelections::operator=(const BackupRestoreSelections& brs)
{
	fileName = brs.getFileName();
	altFileName = brs.getAltFileName();
	mode = brs.getMode();
	format = brs.getFormat();
	brPluData = brs.isPluDataSelected();
	brResources = brs.isResourcesSelected();
	brOperatorData = brs.isOperatorDataSelected();
	brStoreData = brs.isStoreDataSelected();
	brScaleConfig = brs.isScaleConfigSelected();
	brWrapperSettings = brs.isWrapperSettingsSelected();
	brSoftkeyData = brs.isSoftkeyDataSelected();
	brCOOLData = brs.isCOOLDataSelected();
	brSelfServeData = brs.isSelfServeDataSelected();
	brSynchronizeConfigData = brs.isSynchronizeConfigSelected();
	everyDataTypeSelected = brs.isEveryDataTypeSelected();
	brScaleStatusData = brs.isScaleStatusSelected();
	brScaleStatisticsData = brs.isScaleStatisticsSelected();
	brPrinterSettings = brs.isPrinterSettingsSelected();
	displayStats = brs.isDisplayStats();
	return *this;
}

const char *BackupRestoreSelections::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
			for (int loopCnt = 1; loopCnt < endBRSelectionFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetBRFileName:
					{
						HobString tempName(fileName);
						tempName.quoteForCSV();
						serializedData.append(tempName);
						break;
					}
					case fieldOffsetBRaltFileName:
					{
						HobString tempName(altFileName);
						tempName.quoteForCSV();
						serializedData.append(tempName);
						break;
					}
					case fieldOffsetBRMode:
						serializedData.append(Integer::toString((int)mode));
						break;
					case fieldOffsetBRFormat:
						serializedData.append(Integer::toString((int)format));
						break;
					case fieldOffsetBRPluData:
						serializedData.append(Bool::toString(brPluData));
						break;
					case fieldOffsetBRResources:
						serializedData.append(Bool::toString(brResources));
						break;
					case fieldOffsetBROperatorData:
						serializedData.append(Bool::toString(brOperatorData));
						break;
					case fieldOffsetBRStoreData:
						serializedData.append(Bool::toString(brStoreData));
						break;
					case fieldOffsetBRScaleConfig:
						serializedData.append(Bool::toString(brScaleConfig));
						break;
					case fieldOffsetBRWrapperSettings:
						serializedData.append(Bool::toString(brWrapperSettings));
						break;
					case fieldOffsetBRSoftkeyData:
						serializedData.append(Bool::toString(brSoftkeyData));
						break;
					case fieldOffsetBRCOOLData:
						serializedData.append(Bool::toString(brCOOLData));
						break;
					case fieldOffsetBRSelfServeData:
						serializedData.append(Bool::toString(brSelfServeData));
						break;
					case fieldOffsetBRSynchronizeConfig:
						serializedData.append(Bool::toString(brSynchronizeConfigData));
						break;
					default:
						break;
				}
			}
			break;

		default:
			serializedData = HobString("BackupRestoreSelections::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}


