/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgSynchronization.h"
#include "Configuration.h"
#include "ConfigurationTags.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "Bool.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
	
	int CfgSynchronization::_dataType_ = 0;
	string CfgSynchronization::_dataTypeName_("CfgSynchronization");
#endif


std::list<CfgFieldOffsets> CfgSynchronization::fieldList;
std::list<CfgFieldOffsets> CfgSynchronization::cfgItemCheckList;



CfgSynchronization::CfgSynchronization()
	: receivedScaleSynchronizationType_(ScaleSynchronizationTypeInvalid), receivedPrimaryScale_(""), savedSyncTypeChanged_(false), savedPrimaryScaleChanged_(false), savedNewSyncType_(ScaleSynchronizationTypeInvalid), savedNewPrimaryScale_("")
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	initFieldList();
}

CfgSynchronization::CfgSynchronization(const char *data, int len, int formatType)
	: receivedScaleSynchronizationType_(ScaleSynchronizationTypeInvalid), receivedPrimaryScale_(""), savedSyncTypeChanged_(false), savedPrimaryScaleChanged_(false), savedNewSyncType_(ScaleSynchronizationTypeInvalid), savedNewPrimaryScale_("")
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	initFieldList();

	receivedData_.assign(data, len);
	receivedFormat_ = formatType;
}

CfgSynchronization::~CfgSynchronization()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

// static
void CfgSynchronization::initFieldList()
{
	if ( fieldList.empty() )
	{
		fieldList.push_back(fieldOffsetCfgScaleSynchronizationType);
		fieldList.push_back(fieldOffsetCfgPrimaryScale);

		ConfigurationTags configurationTags((FieldOffsetsList*)&fieldList);
	}
	
	if ( cfgItemCheckList.empty() )
	{
		cfgItemCheckList.push_back(fieldOffsetCfgSyncConfigurations);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncEmailAlerts);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncProductDefaults);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncClasses);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncExpandedTexts);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncSpecialMessages);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncNutritionFacts);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncProductNotes);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncMarqueeTexts);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncTransactions);

		cfgItemCheckList.push_back(fieldOffsetCfgSyncTransactionTypes);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncCategories);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncSpecials);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncCoolCategories);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncCoolCountries);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncCoolProductionSteps);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncCoolPreTexts);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncCoolTexts);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncCoolListTexts);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncCoolLists);

		cfgItemCheckList.push_back(fieldOffsetCfgSyncCategoryCountries);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncCategoryProductionSteps);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncCategoryPreTexts);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncProducts);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncExpirationLabels);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncLabels);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncLabelTexts);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncOperatorNotes);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncOperators);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncSoftkeyGroups);
					                         
		cfgItemCheckList.push_back(fieldOffsetCfgSyncSoftkeyStyles);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncSoftkeyFeatures);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncSoftkeys);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncLabelGraphics);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncCategoryGraphics);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncProductGraphics);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncSoftkeyGraphics);
		cfgItemCheckList.push_back(fieldOffsetCfgSyncSpecialsGraphics);

		ConfigurationTags configurationTags((FieldOffsetsList*)&cfgItemCheckList);
	}
}


void CfgSynchronization::updateConfiguration()
{
	Configuration *config = Configuration::getInstance();
	if (!config) return;

	switch (receivedFormat_)
	{
		case BrowserFormat:
		{
			HobString temp(receivedData_);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			if ( tok.next(&token) != -1 && token.length() )
			{
				receivedScaleSynchronizationType_ = (ScaleSynchronizationType)util_string::stringtoint(token);
				if ( tok.next(&token) != -1 && token.length() )
					receivedPrimaryScale_ = token;
			}
			break;
		}
		case SyncListFormat:
			Configuration::getInstance()->updateConfigurationFromUI(cfgItemCheckList, receivedData_);
			break;
		case DBIndependentFormatFormat:
		case EvolutionTaggedFormat:
		{
			Configuration *config = Configuration::getInstance();

			ConfigurationTags	configTags;			
			int offset = 0;
			HobString evolData(receivedData_);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					temp = token->getTag();
					offset = (temp.size()) ? configTags.getOffset(temp) : 0;
					if (offset)
					{
						temp = token->getData();
						
						if (offset == fieldOffsetCfgScaleSynchronizationType)
							receivedScaleSynchronizationType_ = (ScaleSynchronizationType)util_string::stringtoint(temp);
							
						// SOF-5414: this values was not being updated correctly on Db merge
						else if (offset == fieldOffsetCfgPrimaryScale)
							receivedPrimaryScale_ = temp;

						else
							config->updateField(offset, temp);
					}
				}	
				delete token;
			}
			break;
		}	
		default:
			Debug::trace("Configuration.CfgSynchronization", "updateConfiguration():  No support for this format! " + util_string::inttostring(receivedFormat_));
			break;
	}
}


bool CfgSynchronization::isSaveNeeded() const
{
	return Configuration::getInstance()->isSaveNeeded(cfgItemCheckList) || Configuration::getInstance()->isSaveNeeded(fieldList);
}


const char * CfgSynchronization::serialize(int format) const
{
	Configuration *config = Configuration::getInstance();
	if (!config) return 0;

	serializedData = "";

	if ( getFormatType() != UnknownFormat )
		format = getFormatType();

	std::list<CfgFieldOffsets>::iterator fieldIter;
	bool appendSeparator = false;
	//int offset;
	HobString fieldData = "";

	switch ( format )
	{
		case BrowserFormat:
		{
			fieldIter = fieldList.begin();
			while ( fieldIter != fieldList.end() )
			{
				if ( appendSeparator )
					serializedData.append(BrowserFormatSeparator);
				else
					appendSeparator = true;

				fieldData = config->toStringCfg(*fieldIter);
				serializedData.append(fieldData);
				
				fieldIter++;
			}

			break;
		}

		case SyncListFormat:
		{
			fieldIter = cfgItemCheckList.begin();
			while (fieldIter != cfgItemCheckList.end())
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

		case DBIndependentFormatFormat:
		case EvolutionTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag = "";

			fieldIter = fieldList.begin();
			while ( fieldIter != fieldList.end() )
			{
				tag = configTags.getTag(*fieldIter);
				if ( tag.size() )
				{
					fieldData = config->toStringCfg(*fieldIter);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}

				fieldIter++;
			}

			fieldIter = cfgItemCheckList.begin();
			while ( fieldIter != cfgItemCheckList.end() )
			{
				tag = configTags.getTag(*fieldIter);
				if ( tag.size() )
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

		case SynchronizationTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag = "";

			fieldIter = cfgItemCheckList.begin();
			while ( fieldIter != cfgItemCheckList.end() )
			{
				tag = configTags.getTag(*fieldIter);
				if ( tag.size() )
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
			serializedData = "CfgSynchronization::serialize - unknown format:" + util_string::inttostring(format);
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}
