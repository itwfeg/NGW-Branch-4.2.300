/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgEmail.h"
#include "Configuration.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"
#include "DBQuery.h"
#include "DBConfigurationsRM.h"

#include "MultiLanguageText.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgEmail::dataType = 0;
std::string CfgEmail::dataTypeName("CfgEmail"); 
#endif

std::list<CfgFieldOffsets> CfgEmail::basicEmailConfigList;
std::list<CfgFieldOffsets> CfgEmail::synchronizationList;


CfgEmail::CfgEmail()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

#if defined(LINUX_FIX)
CfgEmail::CfgEmail(const CfgEmail& cfgEmail)
    : HfcObject()
#else
CfgEmail::CfgEmail(const CfgEmail& cfgEmail)
#endif
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	from				= cfgEmail.getFrom();
	toList				= cfgEmail.getToList();
	smtpIpAddrOrDnsName	= cfgEmail.getSmtpIpAddrOrDnsName();
}

CfgEmail::CfgEmail(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	HobString temp(data, len);
	std::list<CfgFieldOffsets>::iterator fieldIter;
	int offset;

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			fieldIter = basicEmailConfigList.begin();
			while ((tok.next(&token) != -1) && (fieldIter != basicEmailConfigList.end()))
			{
				offset = *fieldIter;
				if (token.length())
				{
					if ((offset == fieldOffsetCfgToList) || (offset == fieldOffsetCfgFrom))
						token.unQuoteForCSV();


					/*int result =*/ updateField(offset, token);
				}
				fieldIter++;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			Debug::trace("Configuration.CfgEmail", "CfgEmail():  No support for this format! " + HobString(Integer::toString(formatType)));
			break;
	}
}

CfgEmail::~CfgEmail()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


//private
void CfgEmail::init()
{
	from = "";
	toList = "";

	initFieldList();
}


// static
void CfgEmail::initFieldList()		//added ats 10012002
{
	if (basicEmailConfigList.empty())
	{
		basicEmailConfigList.push_back(fieldOffsetCfgFrom);
		basicEmailConfigList.push_back(fieldOffsetCfgToList);
		basicEmailConfigList.push_back(fieldOffsetCfgSmtpIpAddrOrDnsName);
		basicEmailConfigList.push_back(fieldOffsetCfgIsDnsEnabled);		//	Used only for serialize. Info only for the Browser.
	}

	if (synchronizationList.empty())
	{
		synchronizationList.push_back(fieldOffsetCfgFrom);
		synchronizationList.push_back(fieldOffsetCfgToList);
		synchronizationList.push_back(fieldOffsetCfgSmtpIpAddrOrDnsName);

		ConfigurationTags configurationTags((FieldOffsetsList *)&synchronizationList);
	}
}


void CfgEmail::load(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	HobString dbVal;

	if (loadField(dbAccess, "from", dbVal))
		from = dbVal;

	if (loadField(dbAccess, "toList", dbVal))
		toList = dbVal;

	if (loadField(dbAccess, "smtpIpAddrOrDnsName", dbVal))
		setSmtpIpAddrOrDnsName(dbVal);

	saveNeeded = false;
}


void CfgEmail::updateField(int fieldCode, const HobString &token)
{
	if (fieldCode <= 0)
		return;

	if (Debug::isTracing("Configuration.CfgEmail"))
		Debug::trace("Configuration.CfgEmail", "updateField() fieldCode = " + HobString(Integer::toString(fieldCode)) + " token = " + token + "*");
	bool foundField = true;
	switch (fieldCode)
	{
		case fieldOffsetCfgFrom:
			setFrom(token);
			break;

		case fieldOffsetCfgToList:
			setToList(token);
			break;

		case fieldOffsetCfgSmtpIpAddrOrDnsName:
			setSmtpIpAddrOrDnsName(token);
			break;

		default:
			foundField = false;
			break;
	}

	if (Debug::isTracing("Configuration.CfgEmail"))
		Debug::trace("Configuration.CfgEmail", "updateField() foundField = " + HobString(Bool::toString(foundField)));
	if (foundField)
		setReceived(fieldCode, true);
}


void CfgEmail::update(const CfgBase *newValues, bool scaleOnlyFields)
{
	Debug::trace("Configuration.CfgEmail", "update() newValues = " + ptrtostring(newValues));

	if (!newValues)
		return;

	const CfgEmail *myNewValues = dynamic_cast<const CfgEmail *> (newValues);
	if (!myNewValues)
		return;

	short loopCnt = fieldOffsetCfgFrom;
	while (loopCnt < endCfgEmailFieldOffsets)
	{
		if (myNewValues->isReceived(loopCnt) && (!scaleOnlyFields || isFieldScaleOnly(loopCnt)))
		{
			switch (loopCnt)
			{
				case fieldOffsetCfgFrom:
					setFrom(myNewValues->getFrom());
					break;

				case fieldOffsetCfgToList:
					setToList(myNewValues->getToList());
					break;

				case fieldOffsetCfgSmtpIpAddrOrDnsName:
					setSmtpIpAddrOrDnsName(myNewValues->getSmtpIpAddrOrDnsName());
					break;

				default:
					break;
			}
		}
		loopCnt++;
	}
}


void CfgEmail::save(DBAccess *dbAccess, const CfgBase *newValues, bool scaleOnlyFields)
{
	if (newValues)
		update(newValues, scaleOnlyFields);

	Debug::trace("Configuration.CfgEmail", "save():   saveNeeded = " + HobString(Bool::toString(saveNeeded)));
	if (saveNeeded && dbAccess)
	{
		Debug::trace("Configuration.CfgEmail", "saving data ...");

		saveField(dbAccess, "from",	from);
		saveField(dbAccess, "toList",	toList);

		HobString	tempStr;
		smtpIpAddrOrDnsName.getAsString(tempStr);		//	Get the string version of the IpAddress
		saveField(dbAccess, "smtpIpAddrOrDnsName",	tempStr);

		if (Debug::isTracing("Configuration.CfgEmail"))
		{
			HobString myDebug("saving CfgEmail:  toList = ");
			myDebug.append(toList).append(" from = ").append(from).append("   smtpIpAddrOrDnsName = ").append(tempStr);
			Debug::trace("Configuration.CfgEmail", myDebug);

		}

		saveNeeded = false;
	}
	else
		Debug::trace("Configuration.CfgEmail", "save method called, but no fields modified");
}


void CfgEmail::operator=(const CfgEmail &cfgEmail)
{
	from		= cfgEmail.getFrom();
	toList	= cfgEmail.getToList();
	smtpIpAddrOrDnsName	= cfgEmail.getSmtpIpAddrOrDnsName();
}


void CfgEmail::getFieldAsString(int fieldCode, HobString &result, int format) const
{
//	Right now we don't do anything with format but keep it in case we need it.
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetCfgFrom:
			result = from;
			if (format == BrowserFormat)
				result.quoteForCSV();
			break;

		case fieldOffsetCfgToList:
			result = toList;
			if (format == BrowserFormat)
				result.quoteForCSV();
			break;

		case fieldOffsetCfgSmtpIpAddrOrDnsName:
			smtpIpAddrOrDnsName.getAsString(result);
			break;

		//case fieldOffsetCfgIsDnsEnabled:
		//	result = HobString(Bool::toString(Configuration::getInstance()->isDnsEnabled()));
		//	break;

		default:
			break;
	}
	Debug::trace("Configuration.CfgEmail", HobString("getFieldAsString:  fieldCode = ") + HobString(Integer::toString(fieldCode)) + HobString("    result = ") + result + HobString("*"));
}


const HobString& CfgEmail::getField(int fieldCode, int format) const
{
	getFieldAsString(fieldCode, tempValue_, format);

	return tempValue_;
}


const char *CfgEmail::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<CfgFieldOffsets>::iterator fieldIter;
	bool appendSeparator = false;
	int offset;
	HobString fieldData = "";

	switch (format)
	{
		case BrowserFormat:
		{
			fieldIter = basicEmailConfigList.begin();
			while (fieldIter != basicEmailConfigList.end())
			{
				if (appendSeparator)
					serializedData.append(BrowserFormatSeparator);
				else
					appendSeparator = true;

				offset = *fieldIter;
				getFieldAsString(offset, fieldData, format);

				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case EmailReportFormat:
		{
			hfc::MultiLanguageText internationalize;
			ConfigurationTags configTags;

			serializedData.assign(internationalize("ConfigReport.Email")).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgSmtpIpAddrOrDnsName))).append(4, '\t').
					append(getField(fieldOffsetCfgSmtpIpAddrOrDnsName, EvolutionTaggedFormat)).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFrom))).append(1, '\t').
					append(getField(fieldOffsetCfgFrom, EvolutionTaggedFormat)).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgToList))).append(2, '\t').
					append(getField(fieldOffsetCfgToList, EvolutionTaggedFormat)).append(1, '\n');
			break;
		}

		case SynchronizationTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = synchronizationList.begin();
			while (fieldIter != synchronizationList.end())
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
			serializedData = HobString("CfgEmail::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}

	return serializedData.c_str();
}


bool CfgEmail::isChanged()
{
	Debug::trace("Configuration.CfgEmail", HobString("isChanged:") + HobString(Bool::toString(saveNeeded)));
	return saveNeeded;
}


void CfgEmail::show()
{
	if (Debug::isTracing("Configuration"))
	{
		Debug::trace("Configuration", "*** CfgEmail parameters ***");
		Debug::trace("Configuration", HobString("from:") + from);
		Debug::trace("Configuration", HobString("toList:") + toList);
		HobString	tempStr;
		smtpIpAddrOrDnsName.getAsString(tempStr);
		Debug::trace("Configuration", HobString("smtpIpAddrOrDnsName:") + tempStr);
	}
}

const HobString &CfgEmail::getFrom() const
{
	return from;
}
void CfgEmail::setFrom(const HobString &name)
{
	if (Debug::isTracing("Configuration.CfgEmail"))
		Debug::trace("Configuration.CfgEmail", HobString("setFrom(") + name + HobString(")"));
	if (from != name)
	{
		if (Debug::isTracing("Configuration.CfgEmail"))
			Debug::trace("Configuration.CfgEmail", HobString("setFrom():  from WAS *") + from + HobString("*"));
		from = name;
		saveNeeded = true;
		if (Debug::isTracing("Configuration.CfgEmail"))
			Debug::trace("Configuration.CfgEmail", HobString("setFrom():  from IS *") + from + HobString("*"));
	}
}

const HobString &CfgEmail::getToList() const
{
	return toList;
}
void CfgEmail::setToList(const HobString &name)
{
	if (Debug::isTracing("Configuration.CfgEmail"))
		Debug::trace("Configuration.CfgEmail", HobString("setToList(") + name + HobString(")"));
	if (toList != name)
	{
		if (Debug::isTracing("Configuration.CfgEmail"))
			Debug::trace("Configuration.CfgEmail", HobString("setToList():  toList WAS *") + toList + HobString("*"));
		toList = name;
		saveNeeded = true;
		if (Debug::isTracing("Configuration.CfgEmail"))
			Debug::trace("Configuration.CfgEmail", HobString("setToList():  toList IS *") + toList + HobString("*"));
	}
}

const HobIpAddress &CfgEmail::getSmtpIpAddrOrDnsName() const
{
	return smtpIpAddrOrDnsName;
}
void CfgEmail::setSmtpIpAddrOrDnsName(const HobString &address)
{
	if (Debug::isTracing("Configuration.CfgEmail"))
		Debug::trace("Configuration.CfgEmail", "setIPAddress1:   address string = " + address);
	HobString tempAddr = address;
	HobIpAddress newAddress(tempAddr);
	setSmtpIpAddrOrDnsName(newAddress);
}
void CfgEmail::setSmtpIpAddrOrDnsName(const HobIpAddress &newAddress)
{
	if (smtpIpAddrOrDnsName != newAddress)
	{
		smtpIpAddrOrDnsName = newAddress;
		saveNeeded = true;
		Debug::trace("Configuration.CfgEmail", "Setting:  smtpIpAddrOrDnsName");
	}
}
