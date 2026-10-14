/* Access Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgMarquee.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"
#include "DBConfigurationsRM.h"

#include "Debug.h"

#include "Integer.h"
#include "Bool.h"
#include "Long.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgMarquee::dataType = 0;
std::string CfgMarquee::dataTypeName("CfgMarquee"); 
#endif

using namespace std;
std::list<CfgFieldOffsets> CfgMarquee::cfgMqFieldList;
std::list<CfgFieldOffsets> CfgMarquee::browserFieldList;


CfgMarquee::CfgMarquee()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

#if defined(LINUX_FIX)
CfgMarquee::CfgMarquee(const CfgMarquee& cfgMarquee)
    : HfcObject()
#else
CfgMarquee::CfgMarquee(const CfgMarquee& cfgMarquee)
#endif
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	
	permanentMarqueeNumber = cfgMarquee.getPermanentMarqueeNumber();
	permanentMarquee = cfgMarquee.getPermanentMarquee();
	permanentMarqueeDisplayed = cfgMarquee.isPermanentMarqueeDisplayed();
	productMarqueeDisplayed = cfgMarquee.isProductMarqueeDisplayed();
	productDescMarqueeCounter = cfgMarquee.getProductDescMarqueeCounter();
}

CfgMarquee::CfgMarquee(const char *data, int len, int formatType)
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
			HobString temp(data, len);
			std::list<CfgFieldOffsets>::iterator fieldIter = browserFieldList.begin();

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != browserFieldList.end()))
			{
				if (token.size())
				{
					if (*fieldIter == fieldOffsetCfgMqPermanentMarquee)
						token.unQuoteForCSV();
					updateField(*fieldIter, token);
				}
				fieldIter++;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

CfgMarquee::~CfgMarquee()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

//private
void CfgMarquee::init()
{
	permanentMarqueeNumber = RESERVED_PERMANENT_MARQUEE_NUMBER;
	permanentMarquee = "";
	permanentMarqueeDisplayed = true;
	productMarqueeDisplayed = false;
	productDescMarqueeCounter = 0;

	initFieldList();
}

// static
void CfgMarquee::initFieldList()
{
	if ( browserFieldList.empty() )
	{
		browserFieldList.push_back(fieldOffsetCfgMqPermanentMarquee);
		//browserFieldList.push_back(fieldOffsetCfgMqPermanentMarqueeDisplayed);
		browserFieldList.push_back(fieldOffsetCfgMqProductMarqueeDisplayed);
		browserFieldList.push_back(fieldOffsetCfgMqProductDescMarqueeCounter);
	}

	if (cfgMqFieldList.empty())
	{
		cfgMqFieldList.push_back(fieldOffsetCfgMqPermanentMarqueeNumber);
		//cfgMqFieldList.push_back(fieldOffsetCfgMqPermanentMarqueeDisplayed);
		cfgMqFieldList.push_back(fieldOffsetCfgMqProductMarqueeDisplayed);
		cfgMqFieldList.push_back(fieldOffsetCfgMqProductDescMarqueeCounter);

		ConfigurationTags configurationTags((FieldOffsetsList *)&cfgMqFieldList);
	}
}


void CfgMarquee::load(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	HobString dbVal;
	
	if (loadField(dbAccess, "permanentMarqueeNumber", dbVal))
		setPermanentMarqueeNumber(Long::valueOf(dbVal));

	//if (loadField(dbAccess, "permanentMarquee", dbVal))
	//	setPermanentMarquee(dbVal);

	//if (loadField(dbAccess, "permanentMarqueeDisplayed", dbVal))
	//	setPermanentMarquee(Bool::valueOf(dbVal));

	if (loadField(dbAccess, "productMarqueeDisplayed", dbVal))
		setProductMarqueeDisplayed(Bool::valueOf(dbVal));

	if (loadField(dbAccess, "productDescMarqueeCounter", dbVal))
		setProductDescMarqueeCounter(Integer::valueOf(dbVal));

	saveNeeded = false;
}


void CfgMarquee::updateField(int fieldCode, const HobString &token)
{
	if (fieldCode <= 0)
		return;

	bool foundField = true;
	switch (fieldCode)
	{
		case fieldOffsetCfgMqPermanentMarqueeNumber:
			setPermanentMarqueeNumber(Long::valueOf(token));
			break;
		case fieldOffsetCfgMqPermanentMarquee:
			setPermanentMarquee(token);
			foundField = false;
			break;
		case fieldOffsetCfgMqPermanentMarqueeDisplayed:
			setPermanentMarqueeDisplayed(Bool::valueOf(token));
			break;
		case fieldOffsetCfgMqProductMarqueeDisplayed:
			setProductMarqueeDisplayed(Bool::valueOf(token));
			break;
		case fieldOffsetCfgMqProductDescMarqueeCounter:
			setProductDescMarqueeCounter(Integer::valueOf(token));
			break;
		default:
			foundField = false;
			break;
	}

	if (foundField)
		setReceived(fieldCode, true);
}

void CfgMarquee::update(const CfgBase *newValues, bool scaleOnlyFields)
{
	if (!newValues)
		return;
		
	const CfgMarquee *myNewValues = dynamic_cast<const CfgMarquee *> (newValues);
	if (!myNewValues)
		return;

	int fieldOffsetCfg = fieldOffsetCfgMqPermanentMarqueeNumber;
	while (fieldOffsetCfg != endCfgMarqueeFieldOffsets)
	{
		if (myNewValues->isReceived(fieldOffsetCfg) && (!scaleOnlyFields || isFieldScaleOnly(fieldOffsetCfg)))
		{
			switch (fieldOffsetCfg)
			{
				case fieldOffsetCfgMqPermanentMarqueeNumber:
					if (getPermanentMarqueeNumber() != myNewValues->getPermanentMarqueeNumber())
					{
						setPermanentMarqueeNumber(myNewValues->getPermanentMarqueeNumber());
						saveNeeded = true;
					}
					break;

				case fieldOffsetCfgMqPermanentMarquee:
					if (getPermanentMarquee() != myNewValues->getPermanentMarquee())
					{
						setPermanentMarquee(myNewValues->getPermanentMarquee());
						saveNeeded = true;
					}
					break;

				case fieldOffsetCfgMqPermanentMarqueeDisplayed:
					if (isPermanentMarqueeDisplayed() != myNewValues->isPermanentMarqueeDisplayed())
					{
						setPermanentMarqueeDisplayed(myNewValues->isPermanentMarqueeDisplayed());
						saveNeeded = true;
					}
					break;

				case fieldOffsetCfgMqProductMarqueeDisplayed:
					if (isProductMarqueeDisplayed() != myNewValues->isProductMarqueeDisplayed())
					{
						setProductMarqueeDisplayed(myNewValues->isProductMarqueeDisplayed());
						saveNeeded = true;
					}
					break;

				case fieldOffsetCfgMqProductDescMarqueeCounter:
					if (getProductDescMarqueeCounter() != myNewValues->getProductDescMarqueeCounter())
					{
						setProductDescMarqueeCounter(myNewValues->getProductDescMarqueeCounter());
						saveNeeded = true;
					}
					break;

				default:
					break;
			}
		}

		++fieldOffsetCfg;
	}
}


void CfgMarquee::save(DBAccess *dbAccess, const CfgBase *newValues, bool scaleOnlyFields)
{
	if (newValues)
		update(newValues, scaleOnlyFields);
	
	Debug::trace("Configuration.CfgMarquee", "save():   saveNeeded = " + HobString(Bool::toString(saveNeeded)));
	if (saveNeeded && dbAccess)
	{
		Debug::trace("Configuration.CfgMarquee", "saving data ...");

		saveField(dbAccess, "permanentMarqueeNumber", Long::toString(permanentMarqueeNumber));
		//saveField(dbAccess, "permanentMarquee", permanentMarquee);
		//saveField(dbAccess, "permanentMarqueeDisplayed", Bool::toString(permanentMarqueeDisplayed));
		saveField(dbAccess, "productMarqueeDisplayed", Bool::toString(productMarqueeDisplayed));
		saveField(dbAccess, "productDescMarqueeCounter", Integer::toString(productDescMarqueeCounter));

		saveNeeded = false;
	}
	else
		Debug::trace("Configuration.CfgMarquee", "save method called, but no fields modified");	
}


void CfgMarquee::operator=(const CfgMarquee &cfgMarquee)
{
	permanentMarqueeNumber = cfgMarquee.getPermanentMarqueeNumber();
	permanentMarquee = cfgMarquee.getPermanentMarquee();
	permanentMarqueeDisplayed = cfgMarquee.isPermanentMarqueeDisplayed();
	productMarqueeDisplayed = cfgMarquee.isProductMarqueeDisplayed();
	productDescMarqueeCounter = cfgMarquee.getProductDescMarqueeCounter();
}


void CfgMarquee::getFieldAsString(int fieldCode, HobString &result, int /* format */) const
{
//	Right now we don't do anything with format but keep it in case we need it.
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetCfgMqPermanentMarqueeNumber:
			result = Long::toString(permanentMarqueeNumber);
			break;
		case fieldOffsetCfgMqPermanentMarquee:
			result = permanentMarquee;
			break;
		case fieldOffsetCfgMqPermanentMarqueeDisplayed:
			result = Bool::toString(permanentMarqueeDisplayed);
			break;
		case fieldOffsetCfgMqProductMarqueeDisplayed:
			result = Bool::toString(productMarqueeDisplayed);
			break;
		case fieldOffsetCfgMqProductDescMarqueeCounter:
			result = Integer::toString(productDescMarqueeCounter);
			break;
		default:
			break;
	}
}


const HobString& CfgMarquee::getField(int fieldCode, int format)
{
	getFieldAsString(fieldCode, tempValue_, format);
	return tempValue_;
}


const char *CfgMarquee::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<CfgFieldOffsets>::iterator fieldIter;
	bool appendSeparator = false;

	switch (format)
	{
		case BrowserFormat:
		{
			HobString fieldData = "";
			fieldIter = browserFieldList.begin();
			while (fieldIter != browserFieldList.end())
			{
				if (appendSeparator)
					serializedData.append(BrowserFormatSeparator);
				else
					appendSeparator = true;

				getFieldAsString(*fieldIter, fieldData, BrowserFormat);
				if (*fieldIter == fieldOffsetCfgMqPermanentMarquee)
					fieldData.quoteForCSV();
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

			fieldIter = cfgMqFieldList.begin();
			while (fieldIter != cfgMqFieldList.end())
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
			serializedData = HobString("CfgMarquee::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}


void CfgMarquee::show()
{
	Debug::trace("Configuration", "*** CfgMarquee parameters ***");
	Debug::trace("Configuration", HobString("permanentMarqueeNumber:") + HobString(Long::toString(permanentMarqueeNumber)));
	Debug::trace("Configuration", HobString("permanentMarquee:") + permanentMarquee);
	Debug::trace("Configuration", HobString("permanentMarqueeDisplayed:") + HobString(Bool::toString(permanentMarqueeDisplayed)));
	Debug::trace("Configuration", HobString("productMarqueeDisplayed:") + HobString(Bool::toString(productMarqueeDisplayed)));
	Debug::trace("Configuration", HobString("productDescMarqueeCounter:") + HobString(Integer::toString(productDescMarqueeCounter)));
}
