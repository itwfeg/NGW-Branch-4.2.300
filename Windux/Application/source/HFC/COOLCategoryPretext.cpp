/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLCategoryPretext.h"
#include "COOLTags.h"
#include "HfcError.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBCoolCategoryPreTextsRM.h"
#include "DBCoolCategoryPreText.h"
#include "DBCoolPreTextsRM.h"
#include "DBCoolPreText.h"

#include "Configuration.h"
#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int COOLCategoryPretext::dataType = 0;
std::string COOLCategoryPretext::dataTypeName("COOLCategoryPretext"); 
#endif


std::list<COOLCategoryPretextFieldOffsets> COOLCategoryPretext::evolFieldList;


COOLCategoryPretext::COOLCategoryPretext(DBCoolCategoryPreText *dbCoolCategoryPretextRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool traceOn = Debug::isTracing("COOLCategoryPretext");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "Constructor passing in 3 db recs";
		Debug::trace("COOLCategoryPretext", msgDebug);
		msgDebug.append(":  ");
	}
	init();

	if (!dbCoolCategoryPretextRec || !access)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("One of these is null - EXITING!!!");
			msgDebug2.append("  dbCoolCategoryPretextRec = ").append(Hex::toString((long long) dbCoolCategoryPretextRec));
			msgDebug2.append("  access = ").append(Hex::toString((long long) access));
			Debug::trace("COOLCategoryPretext", msgDebug2);
		}
		return;
	}
	dbAccess = access;

	myDbRecord = dbCoolCategoryPretextRec;

	if (!dbCoolCategoryPretextRec->isCategoryNumberNull())
		categoryNum = Long::valueOf(dbCoolCategoryPretextRec->getCategoryNumber());
	if (!dbCoolCategoryPretextRec->isPreTextNumberNull())
		pretextNum = Long::valueOf(dbCoolCategoryPretextRec->getPreTextNumber());
	if (!dbCoolCategoryPretextRec->isDeptNumberNull())
		deptNum = Long::valueOf(dbCoolCategoryPretextRec->getDeptNumber());

	errorCode = RECORD_OK;
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("  categoryNum = ").append(Long::toString(categoryNum));
		msgDebug2.append("  pretextNum = [").append(Long::toString(pretextNum)).append("]");
		Debug::trace("COOLCategoryPretext", msgDebug2);
	}
//	QUESTION: How do we handle a corrupt COOLCategoryPretext record?
}


COOLCategoryPretext::COOLCategoryPretext(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	Debug::trace("COOLCategoryPretext", "Constructor 2 passing const char *data, int len, int formatType ");

	switch (formatType)
	{
		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
//		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		{
			COOLCategoryPretextTags coolTags;

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
			if (!isReceived(fieldOffsetCOOLCatPretextDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetCOOLCatPretextDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}


COOLCategoryPretext::~COOLCategoryPretext()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void COOLCategoryPretext::init()
{
	deptNum     = 0;
	pretextNum	= RECORD_NOT_FOUND;
	categoryNum	= RECORD_NOT_FOUND;
	errorCode	= RECORD_NOT_FOUND;

	dbAccess			= 0;
	myDbRecord			= 0;

	initFieldList();
}

// static
void COOLCategoryPretext::initFieldList()
{
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetCOOLCatPretextDeptNum);
		evolFieldList.push_back(fieldOffsetCOOLCatPretextCategoryNum);
		evolFieldList.push_back(fieldOffsetCOOLCatPretextPretextNum);

		COOLCategoryPretextTags coolTags((FieldOffsetsList *)&evolFieldList);
	}
}

HobRecordNum COOLCategoryPretext::getDeptNum() const
{
	return deptNum;
}
void COOLCategoryPretext::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

HobRecordNum COOLCategoryPretext::getPretextNum() const
{
	return pretextNum;
}
void COOLCategoryPretext::setPretextNum(HobRecordNum recordNum)
{
	pretextNum = recordNum;
}


HobRecordNum COOLCategoryPretext::getCategoryNum() const
{
	return categoryNum;
}
void COOLCategoryPretext::setCategoryNum(HobRecordNum recordNum)
{
	categoryNum = recordNum;
}


HobErrorCode COOLCategoryPretext::getErrorCode() const
{
	return errorCode;
}
void COOLCategoryPretext::setErrorCode(HobErrorCode errorCode)
{
	this->errorCode = errorCode;
}


int COOLCategoryPretext::updateField(int fieldCode, const HobString &token)
{
	bool traceOn = Debug::isTracing("COOLCategoryPretext");
	HobString myDebug;
	if (traceOn)
	{
		myDebug = "updateField() - fieldCode: ";
		myDebug.append(Integer::toString(fieldCode)).append("  data: ").append(token);
		Debug::trace("COOLCategoryPretext", myDebug);
	}

	if (fieldCode < 0)
		return -1;

	switch (fieldCode)
	{
		case fieldOffsetCOOLCatPretextCategoryNum:
			setCategoryNum((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetCOOLCatPretextPretextNum:
			setPretextNum((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetCOOLCatPretextDeptNum:
			setDeptNum((HobRecordNum)Long::valueOf(token));
			break;
		default:
			if (traceOn)
				Debug::trace("COOLCategoryPretext", myDebug + "fieldCode unknown!!!");
			break;
	}

	return 0;
}


void COOLCategoryPretext::update(const COOLCategoryPretext *newCOOLCategoryPretextRec)
{
	if (!newCOOLCategoryPretextRec)
		return;

	short loopCnt = 1;
	while (loopCnt < endCOOLCategoryPretextFieldOffsets)
	{
		if (newCOOLCategoryPretextRec->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetCOOLCatPretextCategoryNum:
					setCategoryNum(newCOOLCategoryPretextRec->getCategoryNum());
					break;
				case fieldOffsetCOOLCatPretextPretextNum:
					setPretextNum(newCOOLCategoryPretextRec->getPretextNum());
					break;
				case fieldOffsetCOOLCatPretextDeptNum:
					setDeptNum(newCOOLCategoryPretextRec->getDeptNum() );
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


void COOLCategoryPretext::getFieldAsString(int fieldCode, HobString &result, int /* format */) const
{
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetCOOLCatPretextDeptNum:
			result = Long::toString(deptNum);
		break;
		case fieldOffsetCOOLCatPretextCategoryNum:
			if (categoryNum > RECORD_NOT_FOUND)
				result = Long::toString(categoryNum);
			break;
		case fieldOffsetCOOLCatPretextPretextNum:
			if (pretextNum > RECORD_NOT_FOUND)
				result = Long::toString(pretextNum);
			break;
		default:
			break;
	}
}


const char *COOLCategoryPretext::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

#if 1
	std::list<COOLCategoryPretextFieldOffsets>::iterator fieldIter;
#endif
	switch (format)
	{
		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
#if 1
			COOLCategoryPretextTags coolTags;
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
#endif
			break;
		}

		// case UltimaTaggedFormat:    We don't write out Cool Pretext records in this format

		default:
			serializedData = HobString("COOLCategoryPretext::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


void COOLCategoryPretext::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


DBCoolCategoryPreText *COOLCategoryPretext::getMyDbRecord()
{
	return myDbRecord;
}


COOLCategoryPretext *COOLCategoryPretext::findCOOLCategoryPretext(HobRecordNum categoryNum, HobRecordNum pretextNum, HobRecordNum deptNum, DBAccess *dbAccess)
{
	COOLCategoryPretext *coolCategoryPretext = 0;

	if (!dbAccess)
		return coolCategoryPretext;

	if (Debug::isTracing("COOLCategoryPretext.find"))
	{
		HobString myDebug("Looking for category number: ");
		myDebug.append(Long::toString(categoryNum));
		myDebug.append(" for pretext number: ").append(Long::toString(pretextNum));
		myDebug.append(" for dept number: ").append(Long::toString(deptNum));
		Debug::trace("COOLCategoryPretext.find", myDebug);
	}

	DBCoolCategoryPreTextsRM	*dbCoolCategoryPretextsRM = dbAccess->getDBCoolCategoryPreTextsRM();
	if (!dbCoolCategoryPretextsRM)
		return coolCategoryPretext;

	try
	{
		DBCoolCategoryPreText *dbCoolCategoryPretext = dbCoolCategoryPretextsRM->find(Long::toString(categoryNum), Long::toString(pretextNum), Long::toString(deptNum));
		if (dbCoolCategoryPretext)
		{
			Debug::trace("COOLCategoryPretext.find", "Found category in database.");

			coolCategoryPretext = new COOLCategoryPretext(dbCoolCategoryPretext, dbAccess);
			if (!coolCategoryPretext)
				delete dbCoolCategoryPretext;
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "COOLCategoryPretext::findCOOLCategoryPretext() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("   DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info:").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
	return coolCategoryPretext;
}

bool COOLCategoryPretext::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int COOLCategoryPretext::deleteRecord()
{
	int result = 0;

	if (myDbRecord && dbAccess)
	{
		DBCoolCategoryPreTextsRM *dbCoolCategoryPretextsRM = dbAccess->getDBCoolCategoryPreTextsRM();
		if (dbCoolCategoryPretextsRM)
		{
			if (Debug::isTracing("COOLCategoryPretext"))
			{
				HobString myDebug("Deleting COOLCategoryPretext:  pretextNum = ");
				myDebug.append(Long::toString(pretextNum));
				myDebug.append("  categoryNum = ").append(Long::toString(categoryNum));
				Debug::trace("COOLCategoryPretext", myDebug);
			}
			result = dbCoolCategoryPretextsRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void COOLCategoryPretext::saveRecord()
{
	bool traceOn = Debug::isTracing("COOLCategoryPretext");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "saveRecord()";
		Debug::trace("COOLCategoryPretext", msgDebug);
		msgDebug.append(":  ");
	}
	if (!dbAccess)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("dbAccess is null - EXITING!!!");
			Debug::trace("COOLCategoryPretext", msgDebug2);
		}
		return;
	}

	DBCoolCategoryPreTextsRM *dbCoolCategoryPretextsRM = dbAccess->getDBCoolCategoryPreTextsRM();
	if (!dbCoolCategoryPretextsRM)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("dbCoolCategoryPretextsRM is null - EXITING!!!");
			Debug::trace("COOLCategoryPretext", msgDebug2);
		}
		return;
	}

	if (!myDbRecord)
	{
		myDbRecord = dbCoolCategoryPretextsRM->find(Long::toString(categoryNum), Long::toString(pretextNum), Long::toString(deptNum));
		if (!myDbRecord)
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Don't have a db record - creating it.");
				Debug::trace("COOLCategoryPretext", msgDebug2);
			}
			myDbRecord = dbCoolCategoryPretextsRM->createRecord(Long::toString(categoryNum), Long::toString(pretextNum), Long::toString(deptNum));
		}
	}
	if (myDbRecord)
	{
		myDbRecord->setCategoryNumber(Long::toString(categoryNum));
		myDbRecord->setPreTextNumber(Long::toString(pretextNum));
		myDbRecord->setDeptNumber(Long::toString(deptNum));

		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("Saving the new db record.");
			Debug::trace("COOLCategoryPretext", msgDebug2);
		}
		try
		{
			dbCoolCategoryPretextsRM->saveRecord(myDbRecord);		//	Save the record to the database.
		}
		catch (DBException &dbe)
		{
			Debug::trace("DBAccess.exception", "COOLCategoryPretext::saveRecord() ********** database exception **********");
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}
}


void COOLCategoryPretext::show()
{
	if (Debug::isTracing("COOLCategoryPretext"))
	{
		HobString msgDebug = "pretextNum = [";
		msgDebug.append(Long::toString(pretextNum));
		Debug::trace("COOLCategoryPretext", msgDebug);

		msgDebug = "categoryNum = ";
		msgDebug.append(Long::toString(categoryNum));
		Debug::trace("COOLCategoryPretext", msgDebug);
	}
}
