/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgPrepack.h"
#include "Configuration.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"
#include "DBConfigurationsRM.h"

#include "Integer.h"
#include "Bool.h"


using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgPrepack::dataType = 0;
std::string CfgPrepack::dataTypeName("CfgPrepack"); 
#endif

std::list<CfgFieldOffsets> CfgPrepack::cfgPPFieldList_;
std::list<CfgFieldOffsets> CfgPrepack::browserFieldList_;
std::list<CfgFieldOffsets> CfgPrepack::rotationOverrideFieldList_;



CfgPrepack::CfgPrepack()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();
}

CfgPrepack::CfgPrepack(const char *data, int len, int formatType)
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

CfgPrepack::~CfgPrepack()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// static
void CfgPrepack::initFieldList()
{
	if (cfgPPFieldList_.empty())
	{
		cfgPPFieldList_.push_back(fieldOffsetCfgEnableSellBy);
		cfgPPFieldList_.push_back(fieldOffsetCfgEnableBestBefore);
		cfgPPFieldList_.push_back(fieldOffsetCfgEnableProdImg);
		cfgPPFieldList_.push_back(fieldOffsetCfgEnableUpcNumber);
		cfgPPFieldList_.push_back(fieldOffsetCfgEnableFilmWidth);
		cfgPPFieldList_.push_back(fieldOffsetCfgEnableFilmTension);
		cfgPPFieldList_.push_back(fieldOffsetCfgEnableWrapTypes);
		cfgPPFieldList_.push_back(fieldOffsetCfgEnableCountryOfOrigin);
		cfgPPFieldList_.push_back(fieldOffsetCfgEnableTrackingNumber);
		cfgPPFieldList_.push_back(fieldOffsetCfgEnableProductionEntry);
		cfgPPFieldList_.push_back(fieldOffsetCfgProductionEntryContinuousPrint);
		cfgPPFieldList_.push_back(fieldOffsetCfgEnableProductionPlanning);
		cfgPPFieldList_.push_back(fieldOffsetCfgProductionPlanningFrame);
		cfgPPFieldList_.push_back(fieldOffsetCfgProductionPlanningUpdate);
		cfgPPFieldList_.push_back(fieldOffsetCfgPluSearchRange);
		cfgPPFieldList_.push_back(fieldOffsetCfgAppendingPluNumberToName);
		cfgPPFieldList_.push_back(fieldOffsetCfgNumOfDescCharsToUse);
		cfgPPFieldList_.push_back(fieldOffsetCfgEnableRunTotals);
		cfgPPFieldList_.push_back(fieldOffsetCfgEnablePackedOn);

		cfgPPFieldList_.push_back(fieldOffsetCfgRotationOverrideSetting1);
		cfgPPFieldList_.push_back(fieldOffsetCfgRotationOverrideSetting2);
		cfgPPFieldList_.push_back(fieldOffsetCfgRotationOverrideSetting3);
		cfgPPFieldList_.push_back(fieldOffsetCfgRotationOverrideValue1);
		cfgPPFieldList_.push_back(fieldOffsetCfgRotationOverrideValue2);
		cfgPPFieldList_.push_back(fieldOffsetCfgRotationOverrideValue3);

		cfgPPFieldList_.push_back(fieldOffsetCfgCapacityBarLocation);
		cfgPPFieldList_.push_back(fieldOffsetCfgPopupKeypadLocation);

		cfgPPFieldList_.push_back(fieldOffsetCfgEnableSelfServe);
		cfgPPFieldList_.push_back(fieldOffsetCfgPrimaryLabelStockRotated);
		cfgPPFieldList_.push_back(fieldOffsetCfgSecondaryLabelStockRotated);
		// 30

		ConfigurationTags configurationTags((FieldOffsetsList *)&cfgPPFieldList_);
	}

	if (browserFieldList_.empty())
	{
		browserFieldList_.push_back(fieldOffsetCfgEnableSellBy);
		browserFieldList_.push_back(fieldOffsetCfgEnableBestBefore);
		browserFieldList_.push_back(fieldOffsetCfgEnableProdImg);
		browserFieldList_.push_back(fieldOffsetCfgEnableUpcNumber);
		browserFieldList_.push_back(fieldOffsetCfgEnableFilmWidth);
		browserFieldList_.push_back(fieldOffsetCfgEnableFilmTension);
		browserFieldList_.push_back(fieldOffsetCfgEnableWrapTypes);
		browserFieldList_.push_back(fieldOffsetCfgEnableCountryOfOrigin);
		browserFieldList_.push_back(fieldOffsetCfgEnableTrackingNumber);
		browserFieldList_.push_back(fieldOffsetCfgEnableProductionEntry);
		browserFieldList_.push_back(fieldOffsetCfgProductionEntryContinuousPrint);
		browserFieldList_.push_back(fieldOffsetCfgEnableProductionPlanning);
		browserFieldList_.push_back(fieldOffsetCfgProductionPlanningFrame);
		browserFieldList_.push_back(fieldOffsetCfgProductionPlanningUpdate);
		browserFieldList_.push_back(fieldOffsetCfgAppendingPluNumberToName);
		browserFieldList_.push_back(fieldOffsetCfgNumOfDescCharsToUse);
		browserFieldList_.push_back(fieldOffsetCfgEnableRunTotals);
		browserFieldList_.push_back(fieldOffsetCfgEnablePackedOn);
		browserFieldList_.push_back(fieldOffsetCfgLeftRunscreenContent);
		browserFieldList_.push_back(fieldOffsetCfgMiddleRunscreenDynamicContent);
		browserFieldList_.push_back(fieldOffsetCfgMiddleRunscreenKeypadContent);
		browserFieldList_.push_back(fieldOffsetCfgCapacityBarLocation);
		browserFieldList_.push_back(fieldOffsetCfgPopupKeypadLocation);
		browserFieldList_.push_back(fieldOffsetCfgEnableSelfServe);
		browserFieldList_.push_back(fieldOffsetCfgRightScreenContent);
		browserFieldList_.push_back(fieldOffsetCfgBottomScreenContent);
		browserFieldList_.push_back(fieldOffsetCfgDefaultSoftkeyFontPointSize);

		browserFieldList_.push_back(fieldOffsetCfgPrimaryLabelStockRotated);
		browserFieldList_.push_back(fieldOffsetCfgSecondaryLabelStockRotated);
		// 29
	}

	if (rotationOverrideFieldList_.empty())
	{
		rotationOverrideFieldList_.push_back(fieldOffsetCfgRotationOverrideSetting1);
		rotationOverrideFieldList_.push_back(fieldOffsetCfgRotationOverrideSetting2);
		rotationOverrideFieldList_.push_back(fieldOffsetCfgRotationOverrideSetting3);
		rotationOverrideFieldList_.push_back(fieldOffsetCfgRotationOverrideValue1);
		rotationOverrideFieldList_.push_back(fieldOffsetCfgRotationOverrideValue2);
		rotationOverrideFieldList_.push_back(fieldOffsetCfgRotationOverrideValue3);
	}
}


void CfgPrepack::updateConfiguration() const
{
	Debug::trace("Configuration.Update", HobString("receivedFormat_:") + HobString(Integer::toString(receivedFormat_)));
	Configuration::getInstance()->updateConfigurationFromUI(((receivedFormat_ == BrowserFormat) ? browserFieldList_ : ((receivedFormat_ == RotationOverrideFormat) ? rotationOverrideFieldList_ : cfgPPFieldList_)), receivedData_);
}


bool CfgPrepack::isSaveNeeded() const
{
	return Configuration::getInstance()->isSaveNeeded(cfgPPFieldList_);
}


const char *CfgPrepack::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<CfgFieldOffsets>::iterator fieldIter;
	bool appendSeparator = false;

	Configuration *config = Configuration::getInstance();

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

			fieldIter = cfgPPFieldList_.begin();
			while (fieldIter != cfgPPFieldList_.end())
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

		case RotationOverrideFormat:
		{
			HobString fieldData = "";
			fieldIter = rotationOverrideFieldList_.begin();
			while (fieldIter != rotationOverrideFieldList_.end())
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
			serializedData = HobString("CfgPrepack::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}
