/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLShortlist.h"
#include "COOLTags.h"
#include "Configuration.h"
#include "HfcError.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBCoolTextsRM.h"
#include "DBCoolText.h"
#include "DBCoolListText.h"
#include "DBCoolList.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int COOLShortlist::dataType = 0;
std::string COOLShortlist::dataTypeName("COOLShortlist"); 
#endif

list<COOLShortlistFieldOffsets> COOLShortlist::uiFieldList;
list<COOLShortlistFieldOffsets> COOLShortlist::evolFieldList;
list<COOLShortlistFieldOffsets> COOLShortlist::ultimaFieldList;
list<COOLShortlistFieldOffsets> COOLShortlist::productNumList;


COOLShortlist::COOLShortlist()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

COOLShortlist::COOLShortlist(const char *data, int len, int formatType)
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
			std::list<COOLShortlistFieldOffsets>::iterator fieldIter = uiFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != uiFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);
				if (token.length())
				{
					token.unQuoteForCSV();
					updateField(offset, token);
				}
				fieldIter++;
			}

			// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
			// If we don't receive it via comm/merge/restore, or other mechanism in the subsequent cases, then it remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetCOOLShortlistDeptNum))
			{
				setReceived(fieldOffsetCOOLShortlistDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			COOLShortlistTags coolTags;

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			int offset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
					temp = token->getTag();
					offset = (temp.size()) ? coolTags.getOffset(temp) : 0;
					if (Debug::isTracing("COOLShortlist.ctor"))
						Debug::trace("COOLShortlist.ctor", string("COOLShortlist(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
					if (offset)
					{
						setReceived(offset, true);
						updateField(offset, token->getData());
					}
				}
				delete token;
			}

			// For comm/backup/restore, if we didn't receive the deptNum with the data and the config parameter is set, set the deptNum to the currently active dept.
			// If we didn't receive the deptNum with the data and the config parameter is NOT set (normal case), deptNum remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetCOOLShortlistDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetCOOLShortlistDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		case UltimaTaggedFormat:
		{
			COOLHobShortlistTags coolTags;

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			int offset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
					temp = token->getTag();
					offset = (temp.size()) ? coolTags.getOffset(temp) : 0;
					if (Debug::isTracing("COOLShortlist.ctor"))
						Debug::trace("COOLShortlist.ctor", string("COOLShortlist(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
					if (offset)
					{
						setReceived(offset, true);
						updateField(offset, token->getData());
					}
				}
				delete token;
			}
			break;
		}

		default:
			break;
	}
}

COOLShortlist::COOLShortlist(DBCoolListText *dbCoolListText, DBCoolList *dbCoolList, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("COOLShortlist.ctor", string("COOLShortlist(DBCoolListText, DBCoolList, DBAccess) - dbCoolListText passed in ") + ptrtostring(dbCoolListText) + "  dbCoolList passed in " + ptrtostring(dbCoolList));
	init();

	dbAccess = access;

	if (dbCoolListText && dbCoolList)
	{
		if (!dbCoolList->isDeptNumberNull())
			deptNum = Long::valueOf(dbCoolList->getDeptNumber());
		if (!dbCoolList->isNumberNull())
			productNum = Long::valueOf(dbCoolList->getNumber());
		if (!dbCoolListText->isCoolTextNumberNull())
			coolTextNum = Long::valueOf(dbCoolListText->getCoolTextNumber());
		if (!dbCoolListText->isOrderNull())
			order = dbCoolListText->getOrder();
		if (!dbCoolListText->isLinkNumberNull())
			shortlistLink = Long::valueOf(dbCoolListText->getLinkNumber());
		if (!dbCoolListText->isNextLinkNumberNull())
			shortlistNextLink = Long::valueOf(dbCoolListText->getNextLinkNumber());

		errorCode = RECORD_OK;
	}
}

COOLShortlist::COOLShortlist(COOLShortlist *shortlist)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("COOLShortlist.ctor", string("COOLShortlist(COOLShortlist) - shortlist passed in ") + ptrtostring(shortlist));
	init();

	if (shortlist)
	{
		setDeptNum(shortlist->getDeptNum());
		setProductNum(shortlist->getProductNum());
		setCoolTextNum(shortlist->getCoolTextNum());
		setOrder(shortlist->getOrder());
	}
}


COOLShortlist::~COOLShortlist()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void COOLShortlist::init()
{
	deptNum           = 0;
	productNum        = RECORD_NOT_FOUND;
	coolTextNum       = RECORD_NOT_FOUND;
	order             = 0;
	errorCode         = RECORD_NOT_FOUND;
	shortlistLink     = RECORD_NOT_FOUND;
	shortlistNextLink = RECORD_NOT_FOUND;

	dbAccess   = (DBAccess *)NULL;
	myDbRecord = (DBCoolText *)NULL;

	initFieldLists();
}

// static
void COOLShortlist::initFieldLists()
{
	if (uiFieldList.empty())
	{
		uiFieldList.push_back(fieldOffsetCOOLShortlistProductNum);
		uiFieldList.push_back(fieldOffsetCOOLShortlistTextNum);
		uiFieldList.push_back(fieldOffsetCOOLShortlistOrder);
	}

	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetCOOLShortlistDeptNum);
		evolFieldList.push_back(fieldOffsetCOOLShortlistProductNum);	//	Support for older versions.
		evolFieldList.push_back(fieldOffsetCOOLShortlistNum);
		evolFieldList.push_back(fieldOffsetCOOLShortlistTextNum);
		evolFieldList.push_back(fieldOffsetCOOLShortlistOrder);
		evolFieldList.push_back(fieldOffsetCOOLShortlistLink);
		evolFieldList.push_back(fieldOffsetCOOLShortlistNextLink);
		
		COOLShortlistTags coolTags((FieldOffsetsList *)&evolFieldList);
	}

	if (ultimaFieldList.empty())
	{
		ultimaFieldList.push_back(fieldOffsetCOOLShortlistLink);
		ultimaFieldList.push_back(fieldOffsetCOOLShortlistNextLink);
		ultimaFieldList.push_back(fieldOffsetCOOLShortlistTextNum);

		COOLHobShortlistTags coolTags((FieldOffsetsList *)&ultimaFieldList);
	}

	if (productNumList.empty())
	{
		productNumList.push_back(fieldOffsetCOOLShortlistProductNum);

		COOLHobShortlistTags coolTags((FieldOffsetsList *)&productNumList);
	}
}


HobRecordNum COOLShortlist::getDeptNum() const
{
	return deptNum;
}
void COOLShortlist::setDeptNum(HobRecordNum recNum)
{
	deptNum = recNum;
}

HobRecordNum COOLShortlist::getProductNum() const
{
	return productNum;
}
void COOLShortlist::setProductNum(HobRecordNum recNum)
{
	productNum = recNum;
}

HobRecordNum COOLShortlist::getCoolTextNum() const
{
	return coolTextNum;
}
void COOLShortlist::setCoolTextNum(HobRecordNum recNum)
{
	coolTextNum = recNum;
}

int COOLShortlist::getOrder() const
{
	return order;
}
void COOLShortlist::setOrder(int order)
{
	this->order = order;
}

HobRecordNum COOLShortlist::getShortlistLink() const
{
	return shortlistLink;
}
void COOLShortlist::setShortlistLink(HobRecordNum link)
{
	shortlistLink = link;
}

HobRecordNum COOLShortlist::getShortlistNextLink() const
{
	return shortlistNextLink;
}
void COOLShortlist::setShortlistNextLink(HobRecordNum nextLink)
{
	shortlistNextLink = nextLink;
}

HobErrorCode COOLShortlist::getErrorCode() const
{
	return errorCode;
}
void COOLShortlist::setErrorCode(HobErrorCode code)
{
	errorCode = code;
}


bool COOLShortlist::isUltimaFormat()
{
	if ((productNum == 0) && (coolTextNum > 0) && (shortlistLink > 0) && (shortlistNextLink >= 0))
		return true;
	else
		return false;
}

bool COOLShortlist::isWinduxFormat()
{
	if ((productNum > 0) && (coolTextNum > 0) && (order >= 0))
		return true;
	else
		return false;
}


bool COOLShortlist::updateField(int fieldOffset, const HobString &newValue)
{
	if (Debug::isTracing("COOLShortlist"))
		Debug::trace("COOLShortlist", string("updateField() - fieldOffset ") + inttostring(fieldOffset) + "  data " + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetCOOLShortlistDeptNum:
			setDeptNum(Long::valueOf(newValue));
			break;
		case fieldOffsetCOOLShortlistProductNum:
		case fieldOffsetCOOLShortlistNum:
			setProductNum(Long::valueOf(newValue));
			break;
		case fieldOffsetCOOLShortlistTextNum:
			setCoolTextNum(Long::valueOf(newValue));
			break;
		case fieldOffsetCOOLShortlistOrder:
			setOrder(Integer::valueOf(newValue));
			break;
		case fieldOffsetCOOLShortlistLink:
			if (newValue.length())
				setShortlistLink(Long::valueOf(newValue));
			break;
		case fieldOffsetCOOLShortlistNextLink:
			if (newValue.length())
				setShortlistNextLink(Long::valueOf(newValue));
			break;
		default:
			return false;
			break;
	}

	return true;
}

void COOLShortlist::updateRecord(const COOLShortlist *newData)
{
	if (!newData)
		return;

	if (Debug::isTracing("COOLShortlist"))
	{
		Debug::trace("COOLShortlist", string("updateRecord() - this num ") + longtostring(productNum) + string("  this DeptNum ") + longtostring(deptNum));
		Debug::trace("COOLShortlist", string("updateRecord() - passed in num ") + longtostring(newData->getProductNum()) + string("  passed in DeptNum ") + longtostring(newData->getDeptNum()));
	}

	short loopCnt = 1;
	while (loopCnt < endCOOLShortlistFieldOffsets)
	{
		if (newData->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetCOOLShortlistDeptNum:
					setDeptNum(newData->getDeptNum());
					break;
				case fieldOffsetCOOLShortlistProductNum:
				case fieldOffsetCOOLShortlistNum:
					setProductNum(newData->getProductNum());
					break;
				case fieldOffsetCOOLShortlistTextNum:
					setCoolTextNum(newData->getCoolTextNum());
					break;
				case fieldOffsetCOOLShortlistOrder:
					setOrder(newData->getOrder());
					break;
				case fieldOffsetCOOLShortlistLink:
					setShortlistLink(newData->getShortlistLink());
					break;
				case fieldOffsetCOOLShortlistNextLink:
					setShortlistNextLink(newData->getShortlistNextLink());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


bool COOLShortlist::isFieldQuoted(int fieldOffset) const
{
/*
	switch (fieldOffset)
	{
		//case fieldOffset...:
			//return true;
			//break;
		default:
			break;
	}
*/
	return false;
}

void COOLShortlist::getFieldAsString(int fieldOffset, HobString &result, int format) const
{
	result = "";

	switch (fieldOffset)
	{
		case fieldOffsetCOOLShortlistDeptNum:
			result = longtostring(deptNum);
			break;
		case fieldOffsetCOOLShortlistProductNum:
		case fieldOffsetCOOLShortlistNum:
			if (productNum != RECORD_NOT_FOUND)
				result = longtostring(productNum);
			break;
		case fieldOffsetCOOLShortlistTextNum:
			if (coolTextNum != RECORD_NOT_FOUND)
				result = longtostring(coolTextNum);
			break;
		case fieldOffsetCOOLShortlistOrder:
			result = inttostring(order);
			break;
		case fieldOffsetCOOLShortlistLink:
			if (shortlistLink != RECORD_NOT_FOUND)
				result = longtostring(shortlistLink);
			break;
		case fieldOffsetCOOLShortlistNextLink:
			if (shortlistNextLink != RECORD_NOT_FOUND)
				result = longtostring(shortlistNextLink);
			break;
		default:
			break;
	}

	if (Debug::isTracing("COOLShortlist.getFieldAsString"))
		Debug::trace("COOLShortlist.getFieldAsString", string("fieldOffset ") + inttostring(fieldOffset) + string("  result ") + result + string("  format ") + inttostring(format));
}

const char *COOLShortlist::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<COOLShortlistFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case BrowserFormat:
		{
			bool firstTime = true;
			HobString fieldData = "";
			fieldIter = uiFieldList.begin();
			while (fieldIter != uiFieldList.end())
			{
				if (firstTime)
					firstTime = false;
				else
					serializedData.append(BrowserFormatSeparator);

				getFieldAsString(*fieldIter, fieldData, format);
				if (isFieldQuoted(*fieldIter))
					fieldData.quoteForCSV();

				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			COOLShortlistTags coolTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				//	Don't write out this legacy setting. We just want to be able to read it from older Access versions.
				if (*fieldIter != fieldOffsetCOOLShortlistProductNum)
				{
					tag = coolTags.getTag(*fieldIter);
					if (tag.size())
					{
						getFieldAsString(*fieldIter, fieldData, EvolutionTaggedFormat);

						serializedData.append(tag);
						serializedData.append(fieldData);
						serializedData.append(UnitSeparator);
					}
				}
				fieldIter++;
			}
			break;
		}

		case UltimaTaggedFormat:
		{
			COOLShortlistTags coolTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = ultimaFieldList.begin();
			while (fieldIter != ultimaFieldList.end())
			{
				tag = coolTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData, UltimaTaggedFormat);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		default:
			serializedData = string("COOLShortlist::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}


DBCoolText *COOLShortlist::getMyDbRecord() const
{
	return myDbRecord;
}

void COOLShortlist::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

bool COOLShortlist::isRecordValid() const
{
	//if (myDbRecord)
	 	//return myDbRecord->isRecordValid();
	//else
		//return false;
	return true;
}

int COOLShortlist::deleteRecord()
{
	int result = 0;
/*
	if (myDbRecord && dbAccess)
	{
		DBCoolTextsRM *dbRM = dbAccess->getDBCoolTextsRM();
		if (dbRM)
		{
			Debug::trace("COOLShortlist", string("deleteRecord() - num ") + longtostring(productNum) + "  deptNum " + longtostring(deptNum));
			result = dbRM->deleteRecord(myDbRecord);
		}
	}
*/
	return result;
}

void COOLShortlist::saveRecord()
{
/*
	if (dbAccess)
	{
		DBCoolTextsRM *dbRM = dbAccess->getDBCoolTextsRM();
		if (!dbRM)
			return;

		if (text.empty())
		{
			if (!myDbRecord)
				Debug::trace("COOLShortlist", "saveRecord() - nothing to save.");
			else
			{
				Debug::trace("COOLShortlist", "saveRecord() - don't have any text, deleting it.");
				dbRM->deleteRecord(myDbRecord);
			}
			return;
		}

		if (!myDbRecord)
		{
			Debug::trace("COOLShortlist", "saveRecord() - don't have a db record, creating it...");
			myDbRecord = dbRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the dbCOOLCountry Record
			myDbRecord->setDeptNumber(longtostring(deptNum));
			myDbRecord->setNumber(longtostring(productNum));
			myDbRecord->setOrder(text);

			try
			{
				dbRM->saveRecord(myDbRecord);		//save record to the database
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "COOLText::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
			Debug::trace("COOLShortlist", string("saveRecord() - saved num ") + longtostring(productNum) + string("  deptNum ") + longtostring(deptNum));
		}
	}
*/
}


void COOLShortlist::show()
{
	if (Debug::isTracing("COOLShortlist.show"))
	{
		Debug::trace("COOLShortlist.show", HobString("  productNum   =  ") + longtostring(productNum));
		Debug::trace("COOLShortlist.show", HobString("  coolTextNum =  ") + longtostring(coolTextNum));
		Debug::trace("COOLShortlist.show", HobString("  order    =  ") + longtostring(order));
	} 
}
