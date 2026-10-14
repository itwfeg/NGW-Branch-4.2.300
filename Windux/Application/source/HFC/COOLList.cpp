/* Access Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLList.h"
#include "COOLTags.h"
#include "HfcError.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBCoolList.h"

#include "Configuration.h"
#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int COOLList::dataType = 0;
std::string COOLList::dataTypeName("COOLList"); 
#endif

std::list<COOLListFieldOffsets> COOLList::evolFieldList;


COOLList::COOLList(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			COOLListTags coolTags;

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
					if (Debug::isTracing("COOLList.ctor"))
						Debug::trace("COOLList.ctor", string("COOLList(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
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
			if (!isReceived(fieldOffsetCOOLListDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetCOOLListDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			break;
	}
}

COOLList::COOLList(DBCoolList *dbRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("COOLList.ctor", string("COOLList(DBCoolList, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	dbAccess = access;

	if (dbRec)
	{
		myDbRecord = dbRec;

		deptNum = Long::valueOf(dbRec->getDeptNumber());
		if (!dbRec->isNumberNull())
			productNum = Long::valueOf(dbRec->getNumber());
		if (!dbRec->isFirstLinkNumberNull())
			firstLinkNum = Long::valueOf(dbRec->getFirstLinkNumber());
		errorCode = RECORD_OK;
	}
}

COOLList::COOLList(COOLList *dbRec)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("COOLList.ctor", string("COOLList(COOLList) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	if (dbRec)
	{
		setDeptNum(dbRec->getDeptNum());
		setProductNum(dbRec->getProductNum());
		setFirstLinkNum(dbRec->getFirstLinkNum());
		setErrorCode(dbRec->getErrorCode());
	}
}

COOLList::~COOLList()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("COOLList.dtor", "    COOLList: " + ptrtostring(this) + "  productNum " + longtostring(productNum) + "  deptNum " + longtostring(deptNum));

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void COOLList::init()
{
	deptNum = 0;
	productNum	 = RECORD_NOT_FOUND;
	firstLinkNum = RECORD_NOT_FOUND;
	errorCode	 = RECORD_NOT_FOUND;

	dbAccess	= 0;
	myDbRecord	= 0;

	initFieldLists();
}

// static
void COOLList::initFieldLists()
{
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetCOOLListDeptNum);
		evolFieldList.push_back(fieldOffsetCOOLListProductNum);
		evolFieldList.push_back(fieldOffsetCOOLFirstLinkNum);

		COOLListTags coolTags((FieldOffsetsList *)&evolFieldList);
	}
}


HobRecordNum COOLList::getDeptNum() const
{
	return deptNum;
}
void COOLList::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

HobRecordNum COOLList::getProductNum() const
{
	return productNum;
}
void COOLList::setProductNum(HobRecordNum recNum)
{
	productNum = recNum;
}

HobRecordNum COOLList::getFirstLinkNum() const
{
	return firstLinkNum;
}
void COOLList::setFirstLinkNum(HobRecordNum recNum)
{
	firstLinkNum = recNum;
}

HobErrorCode COOLList::getErrorCode() const
{
	return errorCode;
}
void COOLList::setErrorCode(HobErrorCode code)
{
	errorCode = code;
}


bool COOLList::updateField(int fieldOffset, const HobString &newValue)
{
	if (Debug::isTracing("COOLList"))
		Debug::trace("COOLList", string("updateField() - fieldCode ") + inttostring(fieldOffset) + "  data " + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetCOOLListDeptNum:
			setDeptNum(Long::valueOf(newValue));
			break;
		case fieldOffsetCOOLListProductNum:
			setProductNum(Long::valueOf(newValue));
			break;
		case fieldOffsetCOOLFirstLinkNum:
			if (newValue.length())
				setFirstLinkNum(Long::valueOf(newValue));
			break;
		default:
			break;
	}

	return true;
}

void COOLList::updateRecord(const COOLList *newData)
{
	if (!newData)
		return;

	if (Debug::isTracing("COOLList"))
	{
		Debug::trace("COOLList", string("updateRecord() - this num ") + longtostring(productNum) + string("  this DeptNum ") + longtostring(deptNum));
		Debug::trace("COOLList", string("updateRecord() - passed in ProductNum ") + longtostring(newData->getProductNum()) + string("  passed in DeptNum ") + longtostring(newData->getDeptNum()));
	}

	short loopCnt = 1;
	while (loopCnt < endCOOLListFieldOffsets)
	{
		if (newData->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetCOOLListDeptNum:
					setDeptNum(newData->getDeptNum());
					break;
				case fieldOffsetCOOLListProductNum:
					setProductNum(newData->getProductNum());
					break;
				case fieldOffsetCOOLFirstLinkNum:
					setFirstLinkNum(newData->getFirstLinkNum());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


void COOLList::getFieldAsString(int fieldOffset, HobString &result, int format) const
{
	result = "";

	switch (fieldOffset)
	{
		case fieldOffsetCOOLListDeptNum:
				result = longtostring(deptNum);
			break;
		case fieldOffsetCOOLListProductNum:
			if (productNum != RECORD_NOT_FOUND)
				result = longtostring(productNum);
			break;
		case fieldOffsetCOOLFirstLinkNum:
			if (firstLinkNum != RECORD_NOT_FOUND)
				result = longtostring(firstLinkNum);
			break;
		default:
			break;
	}

	if (Debug::isTracing("COOLList.getFieldAsString"))
		Debug::trace("COOLList.getFieldAsString", string("fieldOffset ") + inttostring(fieldOffset) + string("  result ") + result + string("  format ") + inttostring(format));
}

const char *COOLList::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	list<COOLListFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			COOLListTags coolTags;
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

		default:
			serializedData = HobString("COOLList::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}


DBCoolList *COOLList::getMyDbRecord() const
{
	return myDbRecord;
}

void COOLList::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

bool COOLList::isRecordValid() const
{
	if (productNum > 0)
		return true;
	else
		return false;
}

int COOLList::deleteRecord()
{
	int result = 0;
	return result;
}

void COOLList::saveRecord()
{
}

void COOLList::show()
{
	if (Debug::isTracing("COOLList.show"))
	{
		Debug::trace("COOLList.show", HobString("  deptNum      =  ") + longtostring(deptNum));
		Debug::trace("COOLList.show", HobString("  productNum   =  ") + longtostring(productNum));
		Debug::trace("COOLList.show", HobString("  firstLinkNum =  ") + longtostring(firstLinkNum));
	} 
}
