/* Fastrack Scale Project
 * COPYRIGHT (c) 2013.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Recall.h"
#include "RecallTags.h"
#include "HfcError.h"
#include "Configuration.h"
#include "filesys/filename.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBRecallsRM.h"
#include "DBRecall.h"

#include "MultiLanguageText.h"
#include "MessageLogger.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Recall::dataType = 0;
std::string Recall::dataTypeName("Recall"); 
#endif

std::list<RecallFieldOffsets> Recall::fieldList;


Recall::Recall()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

Recall::Recall(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool traceOn = Debug::isTracing("Recall");
	HobString msgDebug = "";

	if (traceOn)
	{
		msgDebug = "Constructor passing in const char *data";
		Debug::trace("Recall", msgDebug);
	}
	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);

				switch (loopCnt)
				{
					case fieldOffsetRecallName:
						token.unQuoteForCSV();  // Can now unquote it even if it's not quoted
						recallName = token;
						break;
					case fieldOffsetRecallText:
						token.unQuoteForCSV();
						recallText = token;
						break;
					case fieldOffsetReceivedDateTime:
						// We don't receive this back from the UI
						//token.unQuoteForCSV();
						// receivedDateTime = token;
						break;
					case fieldOffsetAcknowledged:
						acknowledged = Bool::valueOf(token);
						break;
					case fieldOffsetRecallNum:
						recallNum = (HobRecordNum)Long::valueOf(token);
						break;
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			RecallTags recallTags;

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
					offset = (temp.size()) ? recallTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						temp = token->getData();
						switch (offset)
						{
							case fieldOffsetRecallName:
								setRecallName(temp);
								break;
							case fieldOffsetRecallText:
								setRecallText(temp);
								break;
							case fieldOffsetReceivedDateTime:
							{
								HobDateTime newDateTime;
								newDateTime.convertFromDBFormat(temp);
								setReceivedDateTime(newDateTime);
								break;
							}
							case fieldOffsetAcknowledged:
								setAcknowledged(Bool::valueOf(temp));
								break;
							case fieldOffsetRecallNum:
								setRecallNum((HobRecordNum)Long::valueOf(temp));
								break;
							default:
								break;
						}
					}
				}
				delete token;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

Recall::Recall(DBRecall *dbRecall, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool traceOn = Debug::isTracing("Recall");
	HobString msgDebug = "";

	if (traceOn)
	{
		msgDebug = "Constructor passing in a db rec";
		Debug::trace("Recall", msgDebug);
	}
	init();

	dbAccess = access;

	if (dbRecall)
	{
		if (traceOn)
			Debug::trace("Recall", msgDebug + " - dbrec is not null");

		myDbRecord = dbRecall;

		if (!dbRecall->isNameNull())
			recallName = dbRecall->getName();
		if (!dbRecall->isTextNull())
			recallText = dbRecall->getText();
		if (!dbRecall->isReceivedDateTimeNull())
			receivedDateTime.convertFromDBFormat(dbRecall->getReceivedDateTime().c_str());
		if (!dbRecall->isAcknowledgedNull())
			acknowledged = dbRecall->getAcknowledged();
		if (!dbRecall->isNumberNull())
			recallNum = (HobRecordNum) Long::valueOf(dbRecall->getNumber());
	}
}


Recall::~Recall()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;

	Debug::trace("Recall.Delete", "    Recall: " + ptrtostring(this) + "  Recall name: " + recallName);
}


//	private
void Recall::init()
{
	recallName = "";
	recallText = "";
	//receivedDateTime by default gets initialized to now
	acknowledged = false;
	recallNum = (HobRecordNum) RECORD_NOT_FOUND;

	errorCode = RECORD_NOT_FOUND;

	dbAccess = 0;
	myDbRecord = 0;

	initFieldList();
}


// static
void Recall::initFieldList()
{
	if (fieldList.empty())
	{
		fieldList.push_back(fieldOffsetRecallName);
		fieldList.push_back(fieldOffsetRecallText);
		fieldList.push_back(fieldOffsetReceivedDateTime);
		fieldList.push_back(fieldOffsetAcknowledged);
		fieldList.push_back(fieldOffsetRecallNum);

		RecallTags recallTags((FieldOffsetsList *)&fieldList);
	}
}


void Recall::updateRecall(const Recall *newRecall)
{
	if (!newRecall)
		return;

	short loopCnt = 1;
	while (loopCnt < endRecallFieldOffsets)
	{
		if (newRecall->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetRecallName:
					setRecallName(newRecall->getRecallName());
					break;
				case fieldOffsetRecallText:
					setRecallText(newRecall->getRecallText());
					break;
				case fieldOffsetReceivedDateTime:
					setReceivedDateTime(newRecall->getReceivedDateTime());
					break;
				case fieldOffsetAcknowledged:
					setAcknowledged(newRecall->isAcknowledged());
					break;
				case fieldOffsetRecallNum:
					setRecallNum(newRecall->getRecallNum());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


const char *Recall::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
			for (int loopCnt = 1; loopCnt < endRecallFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetRecallName:
					{
						HobString tempName(recallName);
						tempName.quoteForCSV();
						serializedData.append(tempName);
						break;
					}
					case fieldOffsetRecallText:
					{
						HobString tempName(recallText);
						tempName.quoteForCSV();
						serializedData.append(tempName);
						break;
					}
					case fieldOffsetReceivedDateTime:
					{
						HobString tempName(receivedDateTime.convertToDBFormat());
						tempName.quoteForCSV();
						serializedData.append(tempName);
						break;
					}
					case fieldOffsetAcknowledged:
						serializedData.append(Bool::toString(acknowledged));
						break;
					case fieldOffsetRecallNum:
						serializedData.append(Long::toString((long)recallNum));
						break;
					default:
						break;
				}
			}
			break;

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			RecallTags recallTags;
			HobString tag = "";
			HobString fieldData = "";

			std::list<RecallFieldOffsets>::iterator fieldIter = fieldList.begin();
			while (fieldIter != fieldList.end())
			{
				tag = recallTags.getTag(*fieldIter);
				switch (*fieldIter)
				{
					case fieldOffsetRecallName:
						fieldData = recallName;
						break;
					case fieldOffsetRecallText:
						fieldData =	recallText;
						break;
					case fieldOffsetReceivedDateTime:
						fieldData =	receivedDateTime.convertToDBFormat(FormatDateTime_Windux);
						break;
					case fieldOffsetAcknowledged:
						fieldData =	Bool::toString(acknowledged);
						break;
					case fieldOffsetRecallNum:
						fieldData =	Long::toString((long)recallNum);
						break;
					default:
						tag = "";
						fieldData = "";
						break;
				}

				if (tag.size())
				{
					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		default:
			serializedData = HobString("Recall::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


void Recall::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


DBRecall *Recall::getMyDbRecord() const
{
	return myDbRecord;
}


Recall *Recall::findRecall(const std::string &name, DBAccess *dbAccess)
{
	Recall *readRecall = 0;

	if (!name.size() || !dbAccess)
		return readRecall;

	DBRecallsRM	*dbRecallsRM = dbAccess->getDBRecallsRM();
	if (!dbRecallsRM)
		return readRecall;

	try
	{
		DBRecall *dbRecallRec = dbRecallsRM->findName(name);
		if (dbRecallRec)
		{
			Debug::trace("Recall", "findRecall() - found this record in the db");
			readRecall = new Recall(dbRecallRec, dbAccess);
			if (!readRecall)
				delete dbRecallRec;
			else
			{
				if (Debug::isTracing("Recall"))
					Debug::trace("Recall", "findRecall() - created the hfc Recall object - Recall: " + ptrtostring(readRecall) +
													"  Recall name:" + readRecall->getRecallName());
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("Recall::findRecall() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return readRecall;
}


bool Recall::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int Recall::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBRecallsRM *dbRecallsRM = dbAccess->getDBRecallsRM();
		if (dbRecallsRM)
		{
			Debug::trace("Recall", "Deleting Recall name: " + recallName);
			result = dbRecallsRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void Recall::saveRecord()
{
	if (recallName.size() && dbAccess)
	{
		DBRecallsRM	*dbRecallsRM = dbAccess->getDBRecallsRM();
		if (!dbRecallsRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("Recall", HobString("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbRecallsRM->createRecord();
		}

		if (myDbRecord)
		{
			if (recallText != myDbRecord->getText())
			{
				hfc::MultiLanguageText internationalize;
				MessageLogger::logMessage(MessageLogger::InfoLog, internationalize("recallSaved") + "  " + receivedDateTime.convertToDBFormat() + "  " + recallText);
			}

			//	Plug the changes back into the dbRecall record
			myDbRecord->setName(recallName);
			myDbRecord->setText(recallText);
			myDbRecord->setReceivedDateTime(receivedDateTime.convertToDBFormat());
			myDbRecord->setAcknowledged(acknowledged);
			myDbRecord->setNumber(Long::toString(recallNum));

			try
			{
				dbRecallsRM->saveRecord(myDbRecord);		//	Save record to the database
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "Recall::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				if (Debug::isTracing("DBAccess.exception"))
				{
					HobString msgDebug = "   DBException: type: ";
					msgDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
					Debug::trace("DBAccess.exception", msgDebug);
				}
			}
		}
	}
}


// virtual, override from basic functionality in HfcObject
HobString Recall::getMyKeyValue() const
{
	return recallName;
}

// virtual, override from basic functionality in HfcObject
EvolDeviceType Recall::getMyEvolDevice() const
{
	return RecallDev;
}
