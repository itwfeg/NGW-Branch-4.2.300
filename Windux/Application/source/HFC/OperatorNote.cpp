/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "OperatorNote.h"
#include "HobOperatorTags.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBOperatorNotesRM.h"
#include "DBOperatorNote.h"
 
#include "Integer.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperatorNote::dataType = 0;
std::string OperatorNote::dataTypeName("OperatorNote"); 
#endif
 
OperatorNote::OperatorNote()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

OperatorNote::OperatorNote(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	//Debug::trace("OperatorNote", HobString("Creating OpNote from format: ") + HobString(Integer::toString(formatType)));
	switch (formatType)
	{
		case BrowserFormat:
		{
			// format : noteText

			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				//setReceived(loopCnt, (token.length() == 0) ? false : true);
				setReceived(loopCnt, true);
					
				switch (loopCnt)
				{
					case fieldOffsetOpNoteText:
					{
						token.unQuoteForCSV();
						setNoteText(token);
						break;
					}
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
			//Debug::trace("OperatorNote", "Creating OpNote from evol or dbind format");
			HobOperatorTags hobOperatorTags;			

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
					offset = (temp == HobOperatorTags::opNote) ? fieldOffsetOpNoteText : 0;
					if (offset)
					{
						setReceived(offset, true);
						temp = token->getData();
						switch (offset)
						{
							case fieldOffsetOpNoteText:
								setNoteText(temp);
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

OperatorNote::OperatorNote(DBOperatorNote *dbOperatorNote, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("OperatorNote", "Constructor passing in a db rec");
	init();

	dbAccess = access;

	if (dbOperatorNote)
	{
		Debug::trace("OperatorNote", HobString("Constructor passing in a db rec - dbrec is not null"));
		myDbRecord = dbOperatorNote;

		if (!dbOperatorNote->isTextNull())
			noteText = dbOperatorNote->getText();
		errorCode = RECORD_OK;
	}
}

// private
OperatorNote::~OperatorNote()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;
}


void OperatorNote::init()
{
	noteText = "";
	errorCode = RECORD_NOT_FOUND;

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBOperatorNote *)NULL;
}

const HobString &OperatorNote::getNoteText() const
{
	return noteText;
}
void OperatorNote::setNoteText(const HobString &noteText)
{
	this->noteText = noteText;
}


HobErrorCode OperatorNote::getErrorCode() const
{
	return errorCode;
}
void OperatorNote::setErrorCode(HobErrorCode errorCode)
{
	this->errorCode = errorCode;
}


const char *OperatorNote::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
		{
			HobString text(noteText);
			text.quoteForCSV();
			serializedData.append(text);
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			serializedData.append(HobOperatorTags::opNote);
			serializedData.append(noteText);
			break;
		}

		default:
			serializedData = HobString("OperatorNote::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


void OperatorNote::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


OperatorNote *OperatorNote::findOperatorNote(DBAccess *dbAccess)
{
	OperatorNote *readOperatorNote = (OperatorNote *)NULL;

	if (!dbAccess)
		return readOperatorNote;

	DBOperatorNotesRM *dbOperatorNotesRM = dbAccess->getDBOperatorNotesRM();
	if (!dbOperatorNotesRM)
		return readOperatorNote;

	try
	{
		DBOperatorNote *dbOperatorNoteRec = dbOperatorNotesRM->findRecord();
		if (dbOperatorNoteRec)
		{
			Debug::trace("OperatorNote", "findOperatorNote() - found this record in the db");
			readOperatorNote = new OperatorNote(dbOperatorNoteRec, dbAccess);
			if (!readOperatorNote)
				delete dbOperatorNoteRec;
			else
			{
				if (Debug::isTracing("OperatorNote"))
					Debug::trace("OperatorNote", HobString("findOperatorNote() - created the hfc OperatorNote object - OperatorNote: ") + HobString(Hex::toString((long long) readOperatorNote)) +
														HobString("  text:") + readOperatorNote->getNoteText());
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("OperatorNote::findOperatorNote() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return readOperatorNote;
}


bool OperatorNote::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}


void OperatorNote::updateNote(OperatorNote *newOperatorNote)
{
	if (!newOperatorNote)
		return;

	short loopCnt = 1;
	while (loopCnt < endOperatorNoteFieldOffsets)
	{
		if (newOperatorNote->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetOpNoteText:
					setNoteText(newOperatorNote->getNoteText());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


void OperatorNote::saveRecord()
{
	if (dbAccess)
	{
		DBOperatorNotesRM *dbOperatorNotesRM = dbAccess->getDBOperatorNotesRM();
		if (!dbOperatorNotesRM)
			return;

		Debug::trace("OperatorNote", "saveRecord() - have a DBOperatorNotesRM");

		if (!myDbRecord)
		{
			Debug::trace("OperatorNote", HobString("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbOperatorNotesRM->createRecord();
		}

		if (myDbRecord)
		{
			Debug::trace("OperatorNote", HobString("saveRecord() - have a db record, setting text to: ") + noteText);
			//	Plug the changes back into the database record
			myDbRecord->setText(noteText);

			try
			{
				dbOperatorNotesRM->saveRecord(myDbRecord);		//save record to the database
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "OperatorNote::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
	else
		Debug::trace("OperatorNote", "saveRecord() - dbAccess not set");

}


// virtual, override from basic functionality in HfcObject
EvolDeviceType OperatorNote::getMyEvolDevice() const
{
	return OperatorNotesDev;
}
