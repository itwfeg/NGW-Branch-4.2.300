/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgStoreInfo.h"
#include "Configuration.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "Integer.h"
#include "Bool.h"


using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgStoreInfo::dataType = 0;
std::string CfgStoreInfo::dataTypeName("CfgStoreInfo"); 
#endif

list<CfgFieldOffsets> CfgStoreInfo::browserFieldList_;
list<CfgFieldOffsets> CfgStoreInfo::evolFieldList_;
list<CfgFieldOffsets> CfgStoreInfo::ultimaFieldList_;



CfgStoreInfo::CfgStoreInfo()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();
}

CfgStoreInfo::CfgStoreInfo(const char *data, int len, int formatType)
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

CfgStoreInfo::~CfgStoreInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

//static
void CfgStoreInfo::initFieldList()
{
	if (browserFieldList_.empty())
	{
		browserFieldList_.push_back(fieldOffsetCfgStoreName);
		browserFieldList_.push_back(fieldOffsetCfgStoreId);
		browserFieldList_.push_back(fieldOffsetCfgStoreGraphic);
		browserFieldList_.push_back(fieldOffsetCfgDeptName_NOT_USED);

		ConfigurationTags configurationTags((FieldOffsetsList *)&browserFieldList_);

	}

	if (evolFieldList_.empty())
	{
		evolFieldList_.push_back(fieldOffsetCfgStoreName);
		evolFieldList_.push_back(fieldOffsetCfgStoreId);
		evolFieldList_.push_back(fieldOffsetCfgStoreGraphic);
		evolFieldList_.push_back(fieldOffsetCfgDeptName_NOT_USED);

		ConfigurationTags configurationTags((FieldOffsetsList *)&evolFieldList_);
	}

	if (ultimaFieldList_.empty())
	{
		ultimaFieldList_.push_back(fieldOffsetCfgStoreName);
		ultimaFieldList_.push_back(fieldOffsetCfgStoreId);

		ConfigurationTags configurationTags((FieldOffsetsList *)&ultimaFieldList_);
	}
}


void CfgStoreInfo::updateConfiguration() const
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
	{
		// receivedFormat_ is either SynchronizationTaggedFormat, EvolutionTaggedFormat, or UltimaTaggedFormat
		ConfigurationTags configTags;			

		HobString evolData(receivedData_);

		//use the UnitSeparator to parse the string
		HobTokenizerUS tok(&evolData, UnitSeparator);
		HobString temp = "";

		HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
		if (token)
		{
			int offset = 0;
			while (tok.next(token) != -1)
			{
				temp = token->getTag();
				offset = (temp.size()) ? configTags.getOffset(temp) : 0;
				if (offset)
				{
					temp = token->getData();
					config->updateField(offset, temp);
				}
			}	
			delete token;
		}
	}
}


bool CfgStoreInfo::isSaveNeeded() const
{
	return Configuration::getInstance()->isSaveNeeded(browserFieldList_);
}


const char *CfgStoreInfo::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	Configuration *config = Configuration::getInstance();

	std::list<CfgFieldOffsets>::iterator fieldIter;
	bool appendSeparator = false;
	HobString fieldData = "";

	switch (format)
	{
		case BrowserFormat:
		{
			fieldIter = browserFieldList_.begin();
			while (fieldIter != browserFieldList_.end())
			{
				if (appendSeparator)
					serializedData.append(BrowserFormatSeparator);
				else
					appendSeparator = true;

				fieldData = config->toStringCfg(*fieldIter);
				if (*fieldIter == fieldOffsetCfgStoreName)
					fieldData.quoteForCSV();
				else if (*fieldIter == fieldOffsetCfgDeptName_NOT_USED)
					fieldData.quoteForCSV();

				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag = "";

			fieldIter = evolFieldList_.begin();
			while (fieldIter != evolFieldList_.end())
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

		case UltimaTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag = "";

			fieldIter = ultimaFieldList_.begin();
			while (fieldIter != ultimaFieldList_.end())
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
			serializedData = HobString("CfgStoreInfo::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}
