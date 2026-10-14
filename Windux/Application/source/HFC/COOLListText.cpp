/* Access Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLListText.h"
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
#include "DBCoolListsRM.h"
#include "DBCoolList.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int COOLListText::dataType = 0;
std::string COOLListText::dataTypeName("COOLListText"); 
#endif

list<COOLListTextFieldOffsets> COOLListText::uiFieldList;
list<COOLListTextFieldOffsets> COOLListText::evolFieldList;
list<COOLListTextFieldOffsets> COOLListText::ultimaFieldList;


COOLListText::COOLListText(const char *data, int len, int formatType)
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
			std::list<COOLListTextFieldOffsets>::iterator fieldIter = uiFieldList.begin();
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
			if (!isReceived(fieldOffsetCOOLListTextDeptNum))
			{
				setReceived(fieldOffsetCOOLListTextDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			COOLListTextTags coolTags;

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			int offset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp, HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
					temp = token->getTag();
					offset = (temp.size()) ? coolTags.getOffset(temp) : 0;
					if (Debug::isTracing("COOLListText.ctor"))
						Debug::trace("COOLListText.ctor", string("COOLListText(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
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
			if (!isReceived(fieldOffsetCOOLListTextDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetCOOLListTextDeptNum, true);
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
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp, HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
					temp = token->getTag();
					offset = (temp.size()) ? coolTags.getOffset(temp) : 0;
					if (Debug::isTracing("COOLListText.ctor"))
						Debug::trace("COOLListText.ctor", string("COOLListText(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
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

COOLListText::COOLListText(DBCoolListText *dbRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("COOLListText.ctor", string("COOLListText(DBCoolListText, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	dbAccess = access;

	if (dbRec)
	{
		myDbRecord = dbRec;

		DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
		if (dbCoolListsRM)
		{
			DBCoolList *dbCoolLists = dbCoolListsRM->find(dbRec);
			if (dbCoolLists)
			{
				if (!dbCoolLists->isNumberNull())
					productNum = Long::valueOf(dbCoolLists->getNumber());
				delete dbCoolLists;
			}
		}

		deptNum = Long::valueOf(dbRec->getDeptNumber());
		if (!dbRec->isCoolTextNumberNull())
			coolTextNum = Long::valueOf(dbRec->getCoolTextNumber());
		if (!dbRec->isOrderNull())
			order = dbRec->getOrder();
		if (!dbRec->isLinkNumberNull())
			linkNum = Long::valueOf(dbRec->getLinkNumber());
		if (!dbRec->isNextLinkNumberNull())
			nextLinkNum = Long::valueOf(dbRec->getNextLinkNumber());

		errorCode = RECORD_OK;
	}
}

COOLListText::COOLListText(COOLListText *shortlist)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	if (shortlist)
	{
		setDeptNum(shortlist->getDeptNum());
		setOrder(shortlist->getOrder());
		setLinkNum(shortlist->getLinkNum());
		setNextLinkNum(shortlist->getNextLinkNum());
		setCoolTextNum(shortlist->getCoolTextNum());
		setProductNum(shortlist->getProductNum());
		setErrorCode(shortlist->getErrorCode());
	}
}


COOLListText::~COOLListText()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void COOLListText::init()
{
	deptNum     = 0;
	coolTextNum	= RECORD_NOT_FOUND;
	order		= 0;
	errorCode	= RECORD_NOT_FOUND;
	linkNum		= RECORD_NOT_FOUND;
	nextLinkNum	= RECORD_NOT_FOUND;
	productNum	= RECORD_NOT_FOUND;

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBCoolListText *)NULL;

	initFieldLists();
}

// static
void COOLListText::initFieldLists()
{
	if (uiFieldList.empty())
	{
//		uiFieldList.push_back(fieldOffsetCOOLShortlistProductNum);
		uiFieldList.push_back(fieldOffsetCOOLListTextTextNum);
		uiFieldList.push_back(fieldOffsetCOOLListTextOrder);
	}

	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetCOOLListTextDeptNum);
		evolFieldList.push_back(fieldOffsetCOOLListTextOrder);
		evolFieldList.push_back(fieldOffsetCOOLListTextLinkNum);
		evolFieldList.push_back(fieldOffsetCOOLListTextNextLinkNum);
		evolFieldList.push_back(fieldOffsetCOOLListTextTextNum);
		evolFieldList.push_back(fieldOffsetCOOLListTextProductNum);

		COOLListTextTags coolTags((FieldOffsetsList *)&evolFieldList);
	}

	if (ultimaFieldList.empty())
	{
		ultimaFieldList.push_back(fieldOffsetCOOLListTextLinkNum);
		ultimaFieldList.push_back(fieldOffsetCOOLListTextNextLinkNum);
		ultimaFieldList.push_back(fieldOffsetCOOLListTextTextNum);

		COOLHobShortlistTags coolTags((FieldOffsetsList *)&ultimaFieldList);
	}
}


HobRecordNum COOLListText::getDeptNum() const
{
	return deptNum;
}
void COOLListText::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

int COOLListText::getOrder() const
{
	return order;
}
void COOLListText::setOrder(int order)
{
	this->order = order;
}

HobRecordNum COOLListText::getLinkNum() const
{
	return linkNum;
}
void COOLListText::setLinkNum(HobRecordNum link)
{
	linkNum = link;
}

HobRecordNum COOLListText::getNextLinkNum() const
{
	return nextLinkNum;
}
void COOLListText::setNextLinkNum(HobRecordNum nextLink)
{
	nextLinkNum = nextLink;
}

HobRecordNum COOLListText::getCoolTextNum() const
{
	return coolTextNum;
}
void COOLListText::setCoolTextNum(HobRecordNum recNum)
{
	coolTextNum = recNum;
}

HobRecordNum COOLListText::getProductNum() const
{
	return productNum;
}
void COOLListText::setProductNum(HobRecordNum recNum)
{
	productNum = recNum;
}

HobErrorCode COOLListText::getErrorCode() const
{
	return errorCode;
}
void COOLListText::setErrorCode(HobErrorCode code)
{
	errorCode = code;
}


bool COOLListText::updateField(int fieldOffset, const HobString &newValue)
{
	if (Debug::isTracing("COOLListText"))
		Debug::trace("COOLListText", string("updateField() - fieldOffset ") + inttostring(fieldOffset) + "  data " + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetCOOLListTextDeptNum:
			setDeptNum(Long::valueOf(newValue));
			break;
		case fieldOffsetCOOLListTextOrder:
			setOrder(Integer::valueOf(newValue));
			break;
		case fieldOffsetCOOLListTextLinkNum:
			if (newValue.length())
				setLinkNum(Long::valueOf(newValue));
			break;
		case fieldOffsetCOOLListTextNextLinkNum:
			if (newValue.length())
				setNextLinkNum(Long::valueOf(newValue));
			break;
		case fieldOffsetCOOLListTextTextNum:
			setCoolTextNum(Long::valueOf(newValue));
			break;
		case fieldOffsetCOOLListTextProductNum:
			setProductNum(Long::valueOf(newValue));
			break;
		default:
			return false;
			break;
	}

	return true;
}

void COOLListText::updateRecord(const COOLListText *newData)
{
	if (!newData)
		return;

	short loopCnt = 1;
	while (loopCnt < endCOOLListTextFieldOffsets)
	{
		if (newData->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetCOOLListTextDeptNum:
					setDeptNum(newData->getDeptNum());
					break;
				case fieldOffsetCOOLListTextOrder:
					setOrder(newData->getOrder());
					break;
				case fieldOffsetCOOLListTextLinkNum:
					setLinkNum(newData->getLinkNum());
					break;
				case fieldOffsetCOOLListTextNextLinkNum:
					setNextLinkNum(newData->getNextLinkNum());
					break;
				case fieldOffsetCOOLListTextTextNum:
					setCoolTextNum(newData->getCoolTextNum());
					break;
				case fieldOffsetCOOLListTextProductNum:
					setProductNum(newData->getProductNum());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


bool COOLListText::isFieldQuoted(int fieldOffset) const
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

void COOLListText::getFieldAsString(int fieldOffset, HobString &result, int format) const
{
	result = "";

	switch (fieldOffset)
	{
		case fieldOffsetCOOLListTextOrder:
			result = inttostring(order);
			break;
		case fieldOffsetCOOLListTextLinkNum:
			if (linkNum != RECORD_NOT_FOUND)
				result = longtostring(linkNum);
			break;
		case fieldOffsetCOOLListTextNextLinkNum:
			if (nextLinkNum != RECORD_NOT_FOUND)
				result = longtostring(nextLinkNum);
			break;
		case fieldOffsetCOOLListTextTextNum:
			if (coolTextNum != RECORD_NOT_FOUND)
				result = longtostring(coolTextNum);
			break;
		case fieldOffsetCOOLListTextProductNum:
			if (productNum != RECORD_NOT_FOUND)
				result = longtostring(productNum);
			break;
		default:
			break;
	}

	if (Debug::isTracing("COOLListText.getFieldAsString"))
		Debug::trace("COOLListText.getFieldAsString", string("fieldOffset ") + inttostring(fieldOffset) + string("  result ") + result + string("  format ") + inttostring(format));
}

const char *COOLListText::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	list<COOLListTextFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			COOLListTextTags coolTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = coolTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData, EvolutionTaggedFormat);

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
			COOLListTextTags coolTags;
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
			serializedData = string("COOLListText::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}


DBCoolListText *COOLListText::getMyDbRecord() const
{
	return myDbRecord;
}

void COOLListText::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

bool COOLListText::isRecordValid() const
{
	if (productNum > 0)
		return true;
	else
		return false;
}

int COOLListText::deleteRecord()
{
	int result = 0;
	return result;
}

void COOLListText::saveRecord()
{
}

void COOLListText::show()
{
	if (Debug::isTracing("COOLListText.show"))
	{
		Debug::trace("COOLListText.show", HobString("  deptNum     =  ") + HobString(Long::toString(deptNum)));
		Debug::trace("COOLListText.show", HobString("  order       =  ") + HobString(Integer::toString(order)));
		Debug::trace("COOLListText.show", HobString("  linkNum     =  ") + HobString(Long::toString(linkNum)));
		Debug::trace("COOLListText.show", HobString("  nextLinkNum =  ") + HobString(Long::toString(nextLinkNum)));
		Debug::trace("COOLListText.show", HobString("  coolTextNum =  ") + HobString(Long::toString(coolTextNum))); 
		Debug::trace("COOLListText.show", HobString("  productNum =  ") + HobString(Long::toString(productNum))); 
	} 
}
