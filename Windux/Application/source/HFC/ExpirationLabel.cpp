/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ExpirationLabel.h"
#include "ExpirationLabelTags.h"
#include "Configuration.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "HobDateTime.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBExpirationLabelsRM.h"
#include "DBExpirationLabel.h"
#include "DBQuery.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ExpirationLabel::dataType = 0;
std::string ExpirationLabel::dataTypeName("ExpirationLabel"); 
#endif

list<ExpirationLabelFieldOffsets> ExpirationLabel::evolFieldList;


ExpirationLabel::ExpirationLabel(DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	dbAccess = access;
}


ExpirationLabel::ExpirationLabel(const char *data, int len, int formatType)
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
			// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
			// If we don't receive it via comm/merge/restore, or other mechanism in the subsequent cases, then it remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetExpLabelDeptNum))
			{
				setReceived(fieldOffsetExpLabelDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			ExpirationLabelTags expirationLabelTags;

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
					offset = (temp.size()) ? expirationLabelTags.getOffset(temp) : 0;
					if (Debug::isTracing("ExpirationLabel.ctor"))
						Debug::trace("ExpirationLabel.ctor", string("ExpirationLabel(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
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
			if (!isReceived(fieldOffsetExpLabelDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetExpLabelDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			break;
	}
}

ExpirationLabel::ExpirationLabel(DBExpirationLabel *dbRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("ExpirationLabel.ctor", string("ExpirationLabel(DBExpirationLabel, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	dbAccess = access;

	if (dbRec)
	{
		myDbRecord = dbRec;

		deptNum = Long::valueOf(dbRec->getDeptNumber());
		pluNumber = Long::valueOf(dbRec->getPLU());

		if (!dbRec->isProductDescriptionNull())
			pluDescription = dbRec->getProductDescription();
		if (!dbRec->isExpirationNull())
			expirationDateTime.convertFromDBFormat(dbRec->getExpiration().c_str());
	}
}


ExpirationLabel::~ExpirationLabel()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("ExpirationLabel.dtor", "    ExpirationLabel: " + ptrtostring(this) + "  num " + longtostring(pluNumber) + "  deptNum " + longtostring(deptNum));

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void ExpirationLabel::init()
{
	deptNum = 0;
	pluNumber = RECORD_NOT_FOUND;
	pluDescription = "";
	// no need to initialize expirationDateTime

	dbAccess = 0;
	myDbRecord = 0;

	initFieldLists();
}

// static
void ExpirationLabel::initFieldLists()
{
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetExpLabelDeptNum);
		evolFieldList.push_back(fieldOffsetExpLabelPluNumber);
		evolFieldList.push_back(fieldOffsetExpLabelPluDescription);
		evolFieldList.push_back(fieldOffsetExpLabelExpirationDateTime);

		ExpirationLabelTags expirationLabelTags((FieldOffsetsList *)&evolFieldList);
	}
}


HobRecordNum ExpirationLabel::getDeptNum() const
{
	return deptNum;
}
void ExpirationLabel::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

HobRecordNum ExpirationLabel::getPluNumber() const
{
	return pluNumber;
}
void ExpirationLabel::setPluNumber(const HobRecordNum num)
{
	pluNumber = num;
}

const HobString &ExpirationLabel::getPluDescription() const
{
	return pluDescription;
}
void ExpirationLabel::setPluDescription(const HobString &desc)
{
	pluDescription = desc;
}

const HobDateTime &ExpirationLabel::getExpirationDateTime() const
{
	return expirationDateTime;
}
void ExpirationLabel::setExpirationDateTime(const HobDateTime &dateTime)
{
	expirationDateTime = dateTime;
}


// Returns true if the field was updated, false if not
bool ExpirationLabel::updateField(int fieldOffset, const HobString &newValue)
{
	if (Debug::isTracing("ExpirationLabel"))
		Debug::trace("ExpirationLabel", string("updateField() - fieldOffset ") + inttostring(fieldOffset) + "  data " + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetExpLabelDeptNum:
			setDeptNum(Long::valueOf(newValue));
			break;
		case fieldOffsetExpLabelPluNumber:
			setPluNumber(Long::valueOf(newValue));
			break;
		case fieldOffsetExpLabelPluDescription:
			setPluDescription(newValue);
			break;
		case fieldOffsetExpLabelExpirationDateTime:
		{
			HobDateTime newDateTime;
			newDateTime.convertFromDBFormat(&newValue);
			setExpirationDateTime(newDateTime);
			break;
		}
		default:
			return false;
			break;
	}

	return true;
}

void ExpirationLabel::updateRecord(ExpirationLabel *newData)
{
	if (!newData)
		return;

	if (Debug::isTracing("ExpirationLabel"))
	{
		Debug::trace("ExpirationLabel", string("updateRecord() - this num ") + longtostring(pluNumber) + string("  this DeptNum ") + longtostring(deptNum));
		Debug::trace("ExpirationLabel", string("updateRecord() - passed in num ") + longtostring(newData->getPluNumber()) + string("  passed in DeptNum ") + longtostring(newData->getDeptNum()));
	}

	short loopCnt = 1;
	while (loopCnt < endExpirationLabelFieldOffsets)
	{
		if (newData->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetExpLabelDeptNum:
					setDeptNum(newData->getDeptNum());
				break;
				case fieldOffsetExpLabelPluNumber:
					setPluNumber(newData->getPluNumber());
					break;
				case fieldOffsetExpLabelPluDescription:
					setPluDescription(newData->getPluDescription());
					break;
				case fieldOffsetExpLabelExpirationDateTime:
					setExpirationDateTime(newData->getExpirationDateTime());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


bool ExpirationLabel::isFieldQuoted(int fieldOffset) const
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

void ExpirationLabel::getFieldAsString(int fieldOffset, HobString &result, int format) const
{
	result = "";

	switch (fieldOffset)
	{
		case fieldOffsetExpLabelDeptNum:
			result = longtostring(deptNum);
			break;
		case fieldOffsetExpLabelPluNumber:
			if (pluNumber > 0)
				result = longtostring(pluNumber);
			break;
		case fieldOffsetExpLabelPluDescription:
			result = pluDescription;
			break;
		case fieldOffsetExpLabelExpirationDateTime:
			result = expirationDateTime.convertToDBFormat( (format == EvolutionTaggedFormat || format == DBIndependentFormatFormat || format == SynchronizationTaggedFormat) ? FormatDateTime_Windux : FormatDateTime_HTSeries );
			break;
		default:
			break;
	}

	if (Debug::isTracing("ExpirationLabel.getFieldAsString"))
		Debug::trace("ExpirationLabel.getFieldAsString", string("fieldOffset ") + inttostring(fieldOffset) + string("  result ") + result + string("  format ") + inttostring(format));
}

const char *ExpirationLabel::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	list<ExpirationLabelFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			ExpirationLabelTags expirationLabelTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = expirationLabelTags.getTag(*fieldIter);
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
			serializedData = string("ExpirationLabel::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}


void ExpirationLabel::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

bool ExpirationLabel::isRecordValid() const
{
	if (myDbRecord)
		return myDbRecord->isRecordValid();
	else
		return false;
}

int ExpirationLabel::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBExpirationLabelsRM *dbRM = dbAccess->getDBExpirationLabelsRM();
		if (dbRM)
		{
			Debug::trace("ExpirationLabel", string("deleteRecord() - num ") + longtostring(pluNumber) + "  deptNum " + longtostring(deptNum));
			result = dbRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void ExpirationLabel::saveRecord()
{
	if (pluNumber > 0 && dbAccess)
	{
		DBExpirationLabelsRM *dbRM = dbAccess->getDBExpirationLabelsRM();
		if (!dbRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("ExpirationLabel", string("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the dbExpirationLabel
			myDbRecord->setDeptNumber(longtostring(deptNum));
			myDbRecord->setPLU(longtostring(pluNumber));

			if (pluDescription == "")
				myDbRecord->setProductDescriptionNull();
			else
			{
				pluDescription.stripFontSequences();
				if (pluDescription.size() > 120)
					myDbRecord->setProductDescription(pluDescription.substr(0, 120));
				else
					myDbRecord->setProductDescription(pluDescription);
			}

			myDbRecord->setExpiration(expirationDateTime.convertToDBFormat());

			try
			{
				dbRM->saveRecord(myDbRecord);		//save record to the database
				Debug::trace("ExpirationLabel", string("saveRecord() - saved num ") + longtostring(pluNumber) + string("  deptNum ") + longtostring(deptNum));
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "ExpirationLabel::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}

// static
ExpirationLabel *ExpirationLabel::findRecord(HobRecordNum productNum, HobRecordNum deptNum, DBAccess *dbAccess)
{
	ExpirationLabel *rec = 0;

	if (productNum <= 0 || deptNum < 0 || !dbAccess)
		return rec;

	DBExpirationLabelsRM *dbRM = dbAccess->getDBExpirationLabelsRM();
	if (!dbRM)
		return rec;

	try
	{
		DBQuery<DBExpirationLabel> *query = dbRM->retrieveLinkedToPLU(longtostring(productNum), longtostring(deptNum));
		if (query)
		{
			DBQuery<DBExpirationLabel>::const_iterator iter = query->begin();
			if (iter != query->end())
			{
				DBExpirationLabel *dbRec = new DBExpirationLabel(*iter);
				if (dbRec)
				{
					rec = new ExpirationLabel(dbRec, dbAccess);
					if (!rec)
						delete dbRec;
					else
					{
						if (Debug::isTracing("ExpirationLabel"))
							Debug::trace("ExpirationLabel", string("findRecord() - rec ptr: ") + ptrtostring(rec) + "  num " + longtostring(rec->getPluNumber()) + "  deptNum " + longtostring(rec->getDeptNum()));
					}
				}
			}
			delete query;
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("ExpirationLabel::findRecord() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return rec;
}


void ExpirationLabel::show()
{
	if (Debug::isTracing("ExpirationLabel"))
	{
		HobString myDebug("show():");
		myDebug.append("\n*******************************************************");
		myDebug.append("\n   pluNumber:     ").append(Long::toString(pluNumber));
		myDebug.append("\n   pluDescription:  ").append(pluDescription);
		myDebug.append("\n   expirationDateTime: ").append(expirationDateTime.convertToDBFormat());
		myDebug.append("\n*******************************************************");
		Debug::trace("ExpirationLabel", myDebug);
	}
}
