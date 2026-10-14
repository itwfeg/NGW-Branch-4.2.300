/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgOperatorAccess.h"
#include "Configuration.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"

#include "Debug.h"
#include "Integer.h"
#include "Bool.h"


using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgOperatorAccess::dataType = 0;
string CfgOperatorAccess::dataTypeName("CfgOperatorAccess"); 
#endif

list<CfgFieldOffsets> CfgOperatorAccess::cfgOAFieldList_;



CfgOperatorAccess::CfgOperatorAccess()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();
}

CfgOperatorAccess::CfgOperatorAccess(const char *data, int len, int formatType)
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

CfgOperatorAccess::~CfgOperatorAccess()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// static
void CfgOperatorAccess::initFieldList()
{
	if (cfgOAFieldList_.empty())
	{
		cfgOAFieldList_.push_back(fieldOffsetCfgOAPriceModifiable);
		cfgOAFieldList_.push_back(fieldOffsetCfgOAByCountModifiable);
		cfgOAFieldList_.push_back(fieldOffsetCfgOATareModifiable);
		cfgOAFieldList_.push_back(fieldOffsetCfgOAFixedWeightModifiable);
		cfgOAFieldList_.push_back(fieldOffsetCfgOAShelfLifeModifiable);
		cfgOAFieldList_.push_back(fieldOffsetCfgOAProductLifeModifiable);
		cfgOAFieldList_.push_back(fieldOffsetCfgOALabelTypeModifiable);
		cfgOAFieldList_.push_back(fieldOffsetCfgOADateTimeModifiable);
		cfgOAFieldList_.push_back(fieldOffsetCfgOAPortionsModifiable);

		cfgOAFieldList_.push_back(fieldOffsetCfgOAPriceAccessLevel);
		cfgOAFieldList_.push_back(fieldOffsetCfgOAByCountAccessLevel);
		cfgOAFieldList_.push_back(fieldOffsetCfgOATareAccessLevel);
		cfgOAFieldList_.push_back(fieldOffsetCfgOAFixedWeightAccessLevel);
		cfgOAFieldList_.push_back(fieldOffsetCfgOAShelfLifeAccessLevel);
		cfgOAFieldList_.push_back(fieldOffsetCfgOAProductLifeAccessLevel);
		cfgOAFieldList_.push_back(fieldOffsetCfgOALabelTypeAccessLevel);
		cfgOAFieldList_.push_back(fieldOffsetCfgOADateTimeAccessLevel);
		cfgOAFieldList_.push_back(fieldOffsetCfgOAPortionsAccessLevel);

		ConfigurationTags configurationTags((FieldOffsetsList *)&cfgOAFieldList_);
	}
}


void CfgOperatorAccess::updateConfiguration() const
{
	Configuration::getInstance()->updateConfigurationFromUI(cfgOAFieldList_, receivedData_);
}


bool CfgOperatorAccess::isSaveNeeded() const
{
	return Configuration::getInstance()->isSaveNeeded(cfgOAFieldList_);
}


const char *CfgOperatorAccess::serialize(int format) const
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
			fieldIter = cfgOAFieldList_.begin();
			while (fieldIter != cfgOAFieldList_.end())
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
/*
		case OperatorModifiesReportFormat:
		{
			ConfigurationTags configNames;

			string tempValue_;

			serializedData.assign("Fields the Operator can modify\n");
			
			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgOADateTimeModifiable)).append(1, '\t');
					tempValue_ = config->toStringCfg(fieldOffsetCfgOADateTimeModifiable);
					serializedData.append(yesNoValue(tempValue_)).append(1, '\n');
					
			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgOAPriceModifiable)).append(1, '\t');
					tempValue_ = config->toStringCfg(fieldOffsetCfgOAPriceModifiable);
					serializedData.append(yesNoValue(tempValue_)).append(1, '\n');
					
			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgOAByCountModifiable)).append(1, '\t');
					tempValue_ = config->toStringCfg(fieldOffsetCfgOAByCountModifiable);
					serializedData.append(yesNoValue(tempValue_)).append(1, '\n');
					
			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgOATareModifiable)).append(1, '\t');
					tempValue_ = config->toStringCfg(fieldOffsetCfgOATareModifiable);
					serializedData.append(yesNoValue(tempValue_)).append(1, '\n');
					
			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgOAShelfLifeModifiable)).append(1, '\t');
					tempValue_ = config->toStringCfg(fieldOffsetCfgOAShelfLifeModifiable);
					serializedData.append(yesNoValue(tempValue_)).append(1, '\n');
					
			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgOALabelTypeModifiable)).append(1, '\t');
					tempValue_ = config->toStringCfg(fieldOffsetCfgOALabelTypeModifiable);
					serializedData.append(yesNoValue(tempValue_)).append(1, '\n');
					
			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgOAFixedWeightModifiable)).append(1, '\t');
					tempValue_ = config->toStringCfg(fieldOffsetCfgOAFixedWeightModifiable);
					serializedData.append(yesNoValue(tempValue_)).append(1, '\n');
					
			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgOAProductLifeModifiable)).append(1, '\t');
					tempValue_ = config->toStringCfg(fieldOffsetCfgOAProductLifeModifiable);
					serializedData.append(yesNoValue(tempValue_)).append(1, '\n');
					
			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgOAPortionsModifiable)).append(1, '\t');
					tempValue_ = config->toStringCfg(fieldOffsetCfgOAPortionsModifiable);
					serializedData.append(yesNoValue(tempValue_)).append(1, '\n');
					
			break;
		}
*/
		case SynchronizationTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = cfgOAFieldList_.begin();
			while (fieldIter != cfgOAFieldList_.end())
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
			serializedData = HobString("CfgOperatorAccess::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}
