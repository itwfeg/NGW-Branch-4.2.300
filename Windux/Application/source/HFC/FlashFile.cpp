/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "FlashFile.h"
#include "FlashFileTags.h"
#include "Configuration.h"
//#include "SessionEvent.h"

#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBFlashFilesRM.h"
#include "DBFlashFile.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int FlashFile::dataType = 0;
std::string FlashFile::dataTypeName("FlashFile"); 
#endif

std::list<FlashFileFieldOffsets> FlashFile::fieldList;


FlashFile::FlashFile(DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (Debug::isTracing("FlashFile"))
	{
		HobString myDebug("Constructor:  access = ");
		myDebug.append(Hex::toString((long long) access));
		Debug::trace("FlashFile", myDebug);
	}

	init();

	dbAccess = access;
}

FlashFile::FlashFile(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (Debug::isTracing("FlashFile"))
	{
		HobString myDebug("Constructor:  len = ");
		myDebug.append(Integer::toString(len)).append(",   formatType = ").append(Integer::toString(formatType));
		Debug::trace("FlashFile", myDebug);
	}

	init();

	switch (formatType)
	{
		case DBIndependentFormatFormat:
		{
			FlashFileTags flashFileTags;

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
					offset = (temp.size()) ? flashFileTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						temp = token->getData();
						switch (offset)
						{
							case fieldOffsetFlashFileName:
								setFileName(temp);
								break;
							case fieldOffsetFlashFileDate:
								setFileDate(temp);
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

FlashFile::FlashFile(edb::DBFlashFile *dbFlashFile, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (Debug::isTracing("FlashFile"))
	{
		HobString myDebug("Constructor:  dbFlashFile = ");
		myDebug.append(Hex::toString((long long) dbFlashFile)).append(",   access = ").append(Hex::toString((long long) access));
		Debug::trace("FlashFile", myDebug);
	}
	init();

	dbAccess = access;

	if (dbFlashFile)
	{
		Debug::trace("FlashFile", "Constructor passing in a db rec - dbrec is not null");
		myDbRecord = dbFlashFile;

		fileName = dbFlashFile->getName();
		fileDate = dbFlashFile->getDate();
		errorCode = RECORD_OK;
	}
}


FlashFile::~FlashFile()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;

	if (Debug::isTracing("FlashFile.Delete"))
	{
		HobString myDebug("FlashFile: ");
		myDebug.append(Hex::toString((long long) this)).append("   filename: ").append(fileName);
		Debug::trace("FlashFile.Delete", myDebug);
	}
}


//	private
void FlashFile::init()
{
	fileName = "";
	fileDate = "";
	errorCode = RECORD_NOT_FOUND;

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBFlashFile *)NULL;

	initFieldList();
}


// static
void FlashFile::initFieldList()
{
	if (fieldList.empty())
	{
		fieldList.push_back(fieldOffsetFlashFileName);
		fieldList.push_back(fieldOffsetFlashFileDate);

		FlashFileTags flashFileTags((FieldOffsetsList *)&fieldList);
	}
}


const HobString &FlashFile::getFileName() const
{
	return fileName;
}
void FlashFile::setFileName(const HobString &name)
{
	fileName = name;
}


const HobString &FlashFile::getFileDate() const
{
	return fileDate;
}
void FlashFile::setFileDate(const HobString &date)
{
	fileDate = date;
}


HobErrorCode FlashFile::getErrorCode() const
{
	return errorCode;
}
void FlashFile::setErrorCode(HobErrorCode code)
{
	errorCode = code;
}


void FlashFile::updateFlashFile(const FlashFile *newFlashFile)
{
	if (!newFlashFile)
		return;

	short loopCnt = 1;
	while (loopCnt < endFlashFileFieldOffsets)
	{
		if (newFlashFile->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetFlashFileName:
					setFileName(newFlashFile->getFileName());
					break;
				case fieldOffsetFlashFileDate:
					setFileDate(newFlashFile->getFileDate());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


const char *FlashFile::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case DBIndependentFormatFormat:
		{
			FlashFileTags flashFileTags;
			HobString tag = "";
			HobString fieldData = "";

			std::list<FlashFileFieldOffsets>::iterator fieldIter = fieldList.begin();
			while (fieldIter != fieldList.end())
			{
				tag = flashFileTags.getTag(*fieldIter);
				switch (*fieldIter)
				{
					case fieldOffsetFlashFileName:
						fieldData =	fileName;
						break;
					case fieldOffsetFlashFileDate:
						fieldData =	fileDate;
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
		{
			serializedData = "FlashFile::serialize - unknown format:";
			serializedData.append(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
		}
	}
	return serializedData.data();
}


void FlashFile::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


DBFlashFile *FlashFile::getMyDbRecord()
{
	return myDbRecord;
}


FlashFile *FlashFile::findFlashFile(const HobString &fileName, DBAccess *dbAccess)
{
	FlashFile *readFlashFile = (FlashFile *)NULL;

	if (fileName.empty() || !dbAccess)
		return readFlashFile;

	DBFlashFilesRM	*dbFlashFilesRM = dbAccess->getDBFlashFilesRM();
	if (!dbFlashFilesRM)
		return readFlashFile;

	try
	{
		DBFlashFile *dbFlashFileRec = dbFlashFilesRM->findName(fileName);
		if (dbFlashFileRec)
		{
			Debug::trace("FlashFile", "findFlashFile() - found this record in the db");
			readFlashFile = new FlashFile(dbFlashFileRec, dbAccess);
			if (!readFlashFile)
				delete dbFlashFileRec;
			else
			{
				if (Debug::isTracing("FlashFile"))
				{
					HobString myDebug("findFlashFile(): ");
					myDebug.append(" created the hfc FlashFile object - FlashFile: ").append(Hex::toString((long long) readFlashFile));
					myDebug.append("  filename: ").append(readFlashFile->getFileName());
					Debug::trace("FlashFile", myDebug);
				}
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "FlashFile::findFlashFile() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
	return readFlashFile;
}


bool FlashFile::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}


int FlashFile::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBFlashFilesRM	*dbFlashFilesRM = dbAccess->getDBFlashFilesRM();
		if (dbFlashFilesRM)
		{
			Debug::trace("FlashFile", "Deleting FlashFile: " + fileName);
			result = dbFlashFilesRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}


void FlashFile::saveRecord()
{
	if (!fileName.empty() && dbAccess)
	{
		DBFlashFilesRM	*dbFlashFilesRM = dbAccess->getDBFlashFilesRM();
		if (!dbFlashFilesRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("FlashFile", "saveRecord() - don't have a db record, creating it...");
			myDbRecord = dbFlashFilesRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the dbFlashFileRecord
			myDbRecord->setName(fileName);
			myDbRecord->setDate(fileDate);
			Debug::trace("FlashFile", "saveRecord(): " + fileName);

			try
			{
				dbFlashFilesRM->saveRecord(myDbRecord);		//save record to the database
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "FlashFile::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}
