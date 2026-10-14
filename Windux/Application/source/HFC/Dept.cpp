/* HT Series Scale Project
 * COPYRIGHT (c) 2017.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#include "Dept.h"
#include "DeptTags.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBDeptsRM.h"
#include "DBDept.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Dept::dataType = 0;
std::string Dept::dataTypeName("Dept"); 
#endif

list<DeptFieldOffsets> Dept::uiFieldList, Dept::evolFieldList;


Dept::Dept()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

Dept::Dept(const char *data, int len, int formatType)
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
			std::list<DeptFieldOffsets>::iterator fieldIter = uiFieldList.begin();
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

			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			DeptTags deptsTags;

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
					offset = (temp.size()) ? deptsTags.getOffset(temp) : 0;
					if (Debug::isTracing("Dept.ctor"))
						Debug::trace("Dept.ctor", string("Dept(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
					if (offset)
					{
						setReceived(offset, true);
						updateField(offset, token->getData());
						temp = token->getData();
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

Dept::Dept(DBDept *dbRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("Dept.ctor", string("Dept(DBDept, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	dbAccess = access;

	if (dbRec)
	{
		Debug::trace("Dept", HobString("Constructor passing in a db rec - dbrec is not null"));
		myDbRecord = dbRec;

		deptNum    = Long::valueOf(dbRec->getNumber());
		deptName1  = dbRec->getName1();
		deptName2  = dbRec->getName2();
	}
}


Dept::~Dept()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("Dept.dtor", "    Dept: " + ptrtostring(this) + "  num " + longtostring(deptNum) + "  deptName1 " + deptName1);

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void Dept::init()
{
	deptNum = (HobRecordNum) RECORD_NOT_FOUND;
	deptName1 = "";
	deptName2 = "";

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBDept *)NULL;

	initFieldLists();
}

// static
void Dept::initFieldLists()
{
	if (uiFieldList.empty())
	{
		uiFieldList.push_back(fieldOffsetDeptNum);
		uiFieldList.push_back(fieldOffsetDeptName1);
		uiFieldList.push_back(fieldOffsetDeptName2);
	}

	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetDeptNum);
		evolFieldList.push_back(fieldOffsetDeptName1);
		evolFieldList.push_back(fieldOffsetDeptName2);

		DeptTags deptsTags((FieldOffsetsList *)&evolFieldList);
	}
}


HobRecordNum Dept::getDeptNum() const
{
	return deptNum;
}
void Dept::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

const HobString &Dept::getDeptName1() const
{
	return deptName1;
}
void Dept::setDeptName1(const HobString &name)
{
	deptName1 = name;
}

const HobString &Dept::getDeptName2() const
{
	return deptName2;
}
void Dept::setDeptName2(const HobString &name)
{
	deptName2 = name;
}


// Returns true if the field was updated, false if not
bool Dept::updateField(int fieldOffset, const HobString &newValue)
{
	if (Debug::isTracing("Dept"))
		Debug::trace("Dept", string("updateField() - fieldOffset ") + inttostring(fieldOffset) + "  data " + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetDeptNum:
			setDeptNum(Long::valueOf(newValue));
			break;
		case fieldOffsetDeptName1:
			setDeptName1(newValue);
			break;
		case fieldOffsetDeptName2:
			setDeptName2(newValue);
			break;
		default:
			return false;
			break;
	}

	return true;
}

void Dept::updateRecord(const Dept *newData)
{
	if (!newData)
		return;

	if (Debug::isTracing("Dept"))
	{
		Debug::trace("Dept", string("updateRecord() - this num ") + longtostring(deptNum) + string("  this deptName1 ") + deptName1);
		Debug::trace("Dept", string("updateRecord() - passed in num ") + longtostring(newData->getDeptNum()) + string("  passed in deptName1 ") + newData->getDeptName1());
	}

	short loopCnt = 1;
	while (loopCnt < endDeptFieldOffsets)
	{
		if (newData->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetDeptNum:
					setDeptNum(newData->getDeptNum());
					break;
				case fieldOffsetDeptName1:
					setDeptName1(newData->getDeptName1());
					break;
				case fieldOffsetDeptName2:
					setDeptName2(newData->getDeptName2());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


bool Dept::isFieldQuoted(int fieldOffset) const
{
	switch (fieldOffset)
	{
		case fieldOffsetDeptName1:
		case fieldOffsetDeptName2:
			return true;
			break;
		default:
			break;
	}
	return false;
}

void Dept::getFieldAsString(int fieldOffset, HobString &result, int format) const
{
	result = "";

	switch (fieldOffset)
	{
		case fieldOffsetDeptNum:
			result = longtostring(deptNum);
			break;
		case fieldOffsetDeptName1:
			result = deptName1;
			break;
		case fieldOffsetDeptName2:
			result = deptName2;
			break;
		default:
			break;
	}

	if (Debug::isTracing("Dept.getFieldAsString"))
		Debug::trace("Dept.getFieldAsString", string("fieldOffset ") + inttostring(fieldOffset) + string("  result ") + result + string("  format ") + inttostring(format));
}

const char *Dept::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	list<DeptFieldOffsets>::iterator fieldIter;
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
			DeptTags deptsTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = deptsTags.getTag(*fieldIter);
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
			serializedData = HobString("Dept::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}


void Dept::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

bool Dept::isRecordValid() const
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int Dept::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBDeptsRM *dbRM = dbAccess->getDBDeptsRM();
		if (dbRM)
		{
			Debug::trace("Dept", string("deleteRecord() - num ") + longtostring(deptNum) + "  deptName1 " + deptName1);
			result = dbRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void Dept::saveRecord()
{
	if (deptNum >= 0 && dbAccess)
	{
		DBDeptsRM *dbRM = dbAccess->getDBDeptsRM();
		if (!dbRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("Dept", HobString("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the DBDept record
			myDbRecord->setNumber(longtostring(deptNum));
			myDbRecord->setName1(deptName1);
			myDbRecord->setName2(deptName2);

			try
			{
				dbRM->saveRecord(myDbRecord);		//save record to the database
				Debug::trace("Dept", string("saveRecord() - saved deptNum ") + longtostring(deptNum) + "  deptName1 " + deptName1);
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "Dept::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}

// static
Dept *Dept::findRecord(HobRecordNum deptNum, DBAccess *dbAccess)
{
	Dept *rec = (Dept *)NULL;

	if (deptNum < 0 || !dbAccess)
		return rec;

	DBDeptsRM *dbRM = dbAccess->getDBDeptsRM();
	if (!dbRM)
		return rec;

	try
	{
		DBDept *dbRec = dbRM->findNumber(HobString(Long::toString(deptNum)));
		if (dbRec)
		{
			rec = new Dept(dbRec, dbAccess);
			if (!rec)
				delete dbRec;
			else
			{
				if (Debug::isTracing("Dept"))
					Debug::trace("Dept", string("findRecord() - rec ptr: ") + ptrtostring(rec) + "  num " + longtostring(rec->getDeptNum()) + "  deptName1 " + rec->getDeptName1());
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("Dept::findRecord() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return rec;
}


// static
void Dept::verifyReservedRecords(DBAccess *dbAccess)
{
	if (!dbAccess)
	{
		Debug::trace("Dept", "verifyReservedRecords - no dbAccess passed in!");
		return;
	}

	Debug::trace("Dept", "verifying all reserved Depts ...");
	Dept *rec = Dept::findRecord(0, dbAccess);
	if (!rec)
	{
		Debug::trace("Dept", "creating reserved Dept 0");
		rec = new Dept();
		if (rec)
		{
			rec->setDeptNum(0);
			rec->setDeptName1("Default");
			rec->setDeptName2("Default");
			rec->setDBAccess(dbAccess);
			rec->saveRecord();
		}
	}
	if (rec)
	{
		delete rec;
		rec = (Dept *)NULL;
	}
}
