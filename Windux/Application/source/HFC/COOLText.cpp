/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLText.h"
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

#include "e_string/utilstring.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int COOLText::dataType = 0;
std::string COOLText::dataTypeName("COOLText"); 
#endif

list<COOLTextFieldOffsets> COOLText::uiFieldList;
list<COOLTextFieldOffsets> COOLText::evolFieldList;
list<COOLTextFieldOffsets> COOLText::ultimaFieldList;


COOLText::COOLText()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

COOLText::COOLText(const char *data, int len, int formatType)
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
			std::list<COOLTextFieldOffsets>::iterator fieldIter = uiFieldList.begin();
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
			if (!isReceived(fieldOffsetCOOLTextDeptNum))
			{
				setReceived(fieldOffsetCOOLTextDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			COOLTextTags coolTags;

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
					if (Debug::isTracing("COOLText.ctor"))
						Debug::trace("COOLText.ctor", string("COOLText(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
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
			if (!isReceived(fieldOffsetCOOLTextDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetCOOLTextDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			break;
	}
}

COOLText::COOLText(DBCoolText *dbRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("COOLText.ctor", string("COOLText(DBCoolText, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	dbAccess = access;

	if (dbRec)
	{
		myDbRecord = dbRec;

		deptNum = Long::valueOf(dbRec->getDeptNumber());
		recordNum = Long::valueOf(dbRec->getNumber());
		text = dbRec->getText();
		errorCode = RECORD_OK;
	}
}

COOLText::COOLText(const COOLText *textRec)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	if (textRec)
	{
		setDeptNum(textRec->getDeptNum());
		setRecordNum(textRec->getRecordNum());
		setText(textRec->getText());
		setTextType(textRec->getTextType());
	}
}

COOLText::~COOLText()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void COOLText::init()
{
	deptNum = 0;
	recordNum = RECORD_NOT_FOUND;
	text = "";
	textType = COOLTextType_Text;

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBCoolText *)NULL;

	initFieldLists();
}

// static
void COOLText::initFieldLists()
{
	if (uiFieldList.empty())
	{
		uiFieldList.push_back(fieldOffsetCOOLTextRecordNum);
		uiFieldList.push_back(fieldOffsetCOOLTextText);
	}

	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetCOOLTextDeptNum);
		evolFieldList.push_back(fieldOffsetCOOLTextRecordNum);
		evolFieldList.push_back(fieldOffsetCOOLTextText);

		COOLTextTags coolTags((FieldOffsetsList *)&evolFieldList);
	}

	if (ultimaFieldList.empty())
	{
		ultimaFieldList.push_back(fieldOffsetCOOLTextRecordNum);
		ultimaFieldList.push_back(fieldOffsetCOOLTextText);
		ultimaFieldList.push_back(fieldOffsetCOOLTextTextType);

		COOLTextTags coolTags((FieldOffsetsList *)&ultimaFieldList);
	}
}


HobRecordNum COOLText::getDeptNum() const
{
	return deptNum;
}
void COOLText::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

HobRecordNum COOLText::getRecordNum() const
{
	return recordNum;
}
void COOLText::setRecordNum(HobRecordNum recNum)
{
	recordNum = recNum;
}

const HobString &COOLText::getText() const
{
	return text;
}
void COOLText::setText(const HobString &theText)
{
	text = theText;
}

COOLText::COOLTextType COOLText::getTextType() const
{
	return textType;
}
void COOLText::setTextType(COOLTextType type)
{
	textType = type;
}
void COOLText::determineTextType(const HobString &type)
{
	if (type[0] == 'P')
		textType = COOLTextType_Pretext;
	else
		// CoolText is determined by the type being a 'C', but we'll just assume
		// that if it's not Pretext ('P'), then it's CoolText.
		textType = COOLTextType_Text;
}

HobErrorCode COOLText::getErrorCode() const
{
	return errorCode;
}
void COOLText::setErrorCode(HobErrorCode errorCode)
{
	this->errorCode = errorCode;
}


// Returns true if the field was updated, false if not
bool COOLText::updateField(int fieldOffset, const HobString &newValue)
{
	if (Debug::isTracing("COOLText"))
		Debug::trace("COOLText", string("updateField() - fieldOffset ") + inttostring(fieldOffset) + "  data " + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetCOOLTextDeptNum:
			setDeptNum(Long::valueOf(newValue));
			break;
		case fieldOffsetCOOLTextRecordNum:
			setRecordNum(Long::valueOf(newValue));
			break;
		case fieldOffsetCOOLTextText:
		{
			HobString temp = newValue;
			temp.stripChar('\x1D'); /* If comming from DG in Quantum mode it can send <GS> chars */
			setText(temp);
			break;
		}
		case fieldOffsetCOOLTextTextType:
			determineTextType(newValue);
			break;
		default:
			return false;
			break;
	}

	return true;
}

void COOLText::updateRecord(const COOLText *newData)
{
	if (!newData)
		return;

	if (Debug::isTracing("COOLText"))
	{
		Debug::trace("COOLText", string("updateRecord() - this num ") + longtostring(recordNum) + string("  this DeptNum ") + longtostring(deptNum));
		Debug::trace("COOLText", string("updateRecord() - passed in num ") + longtostring(newData->getRecordNum()) + string("  passed in DeptNum ") + longtostring(newData->getDeptNum()));
	}

	short loopCnt = 1;
	while (loopCnt < endCOOLTextFieldOffsets)
	{
		if (newData->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetCOOLTextDeptNum:
					setDeptNum(newData->getDeptNum());
					break;
				case fieldOffsetCOOLTextRecordNum:
					setRecordNum(newData->getRecordNum());
					break;
				case fieldOffsetCOOLTextText:
					setText(newData->getText());
					break;
				case fieldOffsetCOOLTextTextType:
					setTextType(newData->getTextType());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


bool COOLText::isFieldQuoted(int fieldOffset) const
{
	switch (fieldOffset)
	{
		case fieldOffsetCOOLTextText:
			return true;
			break;
		default:
			break;
	}
	return false;
}

void COOLText::getFieldAsString(int fieldOffset, HobString &result, int format) const
{
	result = "";

	switch (fieldOffset)
	{
		case fieldOffsetCOOLTextDeptNum:
			result = longtostring(deptNum);
			break;
		case fieldOffsetCOOLTextRecordNum:
			result = longtostring(recordNum);
			break;
		case fieldOffsetCOOLTextText:
			result = text;
			break;
		case fieldOffsetCOOLTextTextType:
			result = "C";
			break;
		default:
			break;
	}

	if (Debug::isTracing("COOLText.getFieldAsString"))
		Debug::trace("COOLText.getFieldAsString", string("fieldOffset ") + inttostring(fieldOffset) + string("  result ") + result + string("  format ") + inttostring(format));
}

const char *COOLText::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	list<COOLTextFieldOffsets>::iterator fieldIter;
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
			COOLTextTags coolTags;
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
			COOLTextTags coolTags;
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
			serializedData = HobString("COOLText::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}


void COOLText::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

DBCoolText *COOLText::getMyDbRecord() const
{
	return myDbRecord;
}

bool COOLText::isRecordValid() const
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int COOLText::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBCoolTextsRM *dbRM = dbAccess->getDBCoolTextsRM();
		if (dbRM)
		{
			Debug::trace("COOLText", string("deleteRecord() - num ") + longtostring(recordNum) + "  deptNum " + longtostring(deptNum));
			result = dbRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void COOLText::saveRecord()
{
	if (recordNum > 0 && dbAccess)
	{
		bool isTracing = Debug::isTracing("COOLText");

		DBCoolTextsRM *dbRM = dbAccess->getDBCoolTextsRM();
		if (!dbRM)
			return;

		if (!text.empty())
		{
			if (isTracing)
				Debug::trace("COOLText", string("saveRecord() - 1) original text = [") + text + "]  length = " + inttostring((int)text.length()));

			text.stripChar(_LF_);
			if (isTracing)
				Debug::trace("COOLText", string("saveRecord() - 1) text after _LF_ strip = [") + text + "]  length = " + inttostring((int)text.length()));

			text.stripChar(_CR_);
			if (isTracing)
				Debug::trace("COOLText", string("saveRecord() - 2) text after _CR_ strip = [") + text + "]  length = " + inttostring((int)text.length()));

			text.stripChar(_LF_);
			if (isTracing)
				Debug::trace("COOLText", string("saveRecord() - 3) text after _LF_ strip = [") + text + "]  length = " + inttostring((int)text.length()));

			text.stripChar(_CR_);
			if (isTracing)
				Debug::trace("COOLText", string("saveRecord() - 4) text after _CR_ strip = [") + text + "]  length = " + inttostring((int)text.length()));
		}

		if (text.empty())
		{
			if (!myDbRecord)
				Debug::trace("COOLText", "saveRecord() - nothing to save.");
			else
			{
				Debug::trace("COOLText", "saveRecord() - don't have any text, deleting it.");
				dbRM->deleteRecord(myDbRecord);
			}
			return;
		}

		if (!myDbRecord)
		{
			Debug::trace("COOLText", "saveRecord() - don't have a db record, creating it...");
			myDbRecord = dbRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the dbCOOLText Record
			myDbRecord->setDeptNumber(longtostring(deptNum));
			myDbRecord->setNumber(longtostring(recordNum));
			myDbRecord->setText(text);

			try
			{
				dbRM->saveRecord(myDbRecord);		//save record to the database
				Debug::trace("COOLText", string("saveRecord() - saved num ") + longtostring(recordNum) + string("  deptNum ") + longtostring(deptNum));
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "COOLText::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}

// static
COOLText *COOLText::findRecord(HobRecordNum textNum, HobRecordNum deptNum, DBAccess *dbAccess)
{
	COOLText *rec = (COOLText *)NULL;

	if (textNum <= 0 || deptNum < 0 || !dbAccess)
		return rec;

	DBCoolTextsRM *dbRM = dbAccess->getDBCoolTextsRM();
	if (!dbRM)
		return rec;

	try
	{
		DBCoolText *dbRec = dbRM->findNumber(longtostring(textNum), longtostring(deptNum));
		if (dbRec)
		{
			rec = new COOLText(dbRec, dbAccess);
			if (!rec)
				delete dbRec;
			else
			{
				if (Debug::isTracing("COOLText"))
					Debug::trace("COOLText", string("findRecord() - rec ptr: ") + ptrtostring(rec) + "  num " + longtostring(rec->getRecordNum()) + "  deptNum " + longtostring(rec->getDeptNum()));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "COOLText::findRecord() ********** database exception **********");
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return rec;
}

