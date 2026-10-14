/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PresentationMethod.h"
#include "PresentationMethodTags.h"
#include "Configuration.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBPresentationMethodsRM.h"
#include "DBPresentationMethod.h"

#include "MultiLanguageText.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PresentationMethod::dataType = 0;
std::string PresentationMethod::dataTypeName("PresentationMethod"); 
#endif

std::list<PresentationMethodFieldOffsets> PresentationMethod::fieldList;	


PresentationMethod::PresentationMethod()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

PresentationMethod::PresentationMethod(const char *data, int len, int formatType)
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
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetPMType:
						type = (short)Integer::valueOf(token);
						break;
					case fieldOffsetPMDescription:
						description = token;
						break;
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			PresentationMethodTags presentationMethodTags;			

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
					offset = (temp.size()) ? presentationMethodTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						temp = token->getData();
						switch(offset)
						{
							case fieldOffsetPMType:
								setType((short)Integer::valueOf(temp));
								break;
							case fieldOffsetPMDescription:
								setDescription(temp);
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

PresentationMethod::PresentationMethod(DBPresentationMethod *dbPresentationMethod, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("PresentationMethod", "Constructor passing in a db rec");
	init();

	dbAccess = access;

	if (dbPresentationMethod)
	{
		Debug::trace("PresentationMethod", HobString("Constructor passing in a db rec - dbrec is not null"));
		myDbRecord = dbPresentationMethod;

		type = (short) Integer::valueOf(dbPresentationMethod->getType());
		description = dbPresentationMethod->getDescription();
	}
}


PresentationMethod::~PresentationMethod()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;

	Debug::trace("PresentationMethod.Delete", "    PresentationMethod: " + ptrtostring(this) +
			"  type:" + inttostring(type) + "  description:" + description);
}


//	private
void PresentationMethod::init()
{
	type = Type_Unknown;
	description = "";

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBPresentationMethod *)NULL;

	initFieldList();
}

// static
void PresentationMethod::initFieldList()
{
	if (fieldList.empty())
	{
		fieldList.push_back(fieldOffsetPMType);
		fieldList.push_back(fieldOffsetPMDescription);

		PresentationMethodTags presentationMethodTags((FieldOffsetsList *)&fieldList);
	}
}


short PresentationMethod::getType() const
{
	return type;
}
void PresentationMethod::setType(short type)
{
	this->type = type;
}

const HobString &PresentationMethod::getDescription() const
{
	return description;
}
void PresentationMethod::setDescription(const HobString &desc)
{
	description = desc;
}


void PresentationMethod::updatePresentationMethod(const PresentationMethod *newPresentationMethod)
{
	if (!newPresentationMethod)
		return;

	short loopCnt = 1;
	while (loopCnt < endPresentationMethodFieldOffsets)
	{
		if (newPresentationMethod->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetPMType:
					setType(newPresentationMethod->getType());
					break;
				case fieldOffsetPMDescription:
					setDescription(newPresentationMethod->getDescription());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


const char *PresentationMethod::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
			for (int loopCnt = 1; loopCnt < endPresentationMethodFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetPMType:
						serializedData.append(Integer::toString(type));
						break;
					case fieldOffsetPMDescription:
						serializedData.append(description);
						break;
					default:
						break;
				}
			}
			break;

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			PresentationMethodTags presentationMethodTags;
			HobString tag = "";
			HobString fieldData = "";
			
			std::list<PresentationMethodFieldOffsets>::iterator fieldIter = fieldList.begin();
			while (fieldIter != fieldList.end())
			{
				tag = presentationMethodTags.getTag(*fieldIter);
				switch (*fieldIter)
				{
					case fieldOffsetPMType:
						fieldData =	Integer::toString(type);
						break;
					case fieldOffsetPMDescription:
						fieldData =	description;
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
			serializedData = HobString("PresentationMethod::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


void PresentationMethod::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


const DBPresentationMethod *PresentationMethod::getMyDbRecord() const
{
	return myDbRecord;
}


// static
PresentationMethod *PresentationMethod::findMethod(short type, DBAccess *dbAccess)
{
	PresentationMethod *readPresentationMethod = (PresentationMethod *)NULL;

	if (type <= 0 || !dbAccess)
		return readPresentationMethod;

	DBPresentationMethodsRM	*dbPresentationMethodsRM = dbAccess->getDBPresentationMethodsRM();
	if (!dbPresentationMethodsRM)
		return readPresentationMethod;

	try
	{
		DBPresentationMethod *dbPresentationMethodRec = dbPresentationMethodsRM->find(HobString(Integer::toString(type)));
		if (dbPresentationMethodRec)
		{
			Debug::trace("PresentationMethod", "findMethod() - found this record in the db");
			readPresentationMethod = new PresentationMethod(dbPresentationMethodRec, dbAccess);
			if (!readPresentationMethod)
				delete dbPresentationMethodRec;
			else
			{
				if (Debug::isTracing("PresentationMethod"))
					Debug::trace("PresentationMethod", "findMethod() - found PresentationMethod: " +
							ptrtostring(readPresentationMethod) + "  type:" + inttostring(type));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("PresentationMethod::findMethod() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return readPresentationMethod;
}


bool PresentationMethod::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int PresentationMethod::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBPresentationMethodsRM	*dbPresentationMethodsRM = dbAccess->getDBPresentationMethodsRM();
		if (dbPresentationMethodsRM)
		{
			Debug::trace("PresentationMethod", HobString("Deleting PresentationMethod ") + 
															HobString("  type:") + HobString(Integer::toString(type)) +
															HobString("  description:") + description);
			result = dbPresentationMethodsRM->deleteRecord(myDbRecord);	
		}
	}
	return result;							
}

void PresentationMethod::saveRecord()
{
	if (type > 0 && dbAccess)
	{
		DBPresentationMethodsRM	*dbPresentationMethodsRM = dbAccess->getDBPresentationMethodsRM();
		if (!dbPresentationMethodsRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("PresentationMethod", HobString("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbPresentationMethodsRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the dbPresentationMethod Record
		   myDbRecord->setType(Integer::toString(type));
		   myDbRecord->setDescription(description);

			try
			{
				dbPresentationMethodsRM->saveRecord(myDbRecord);		//save record to the database
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "PresentationMethod::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}


// static
void PresentationMethod::verifyReservedRecords(DBAccess *dbAccess)
{
	if (!dbAccess)
	{
		Debug::trace("PresentationMethod", "verifying reserved Presentation Methods - no database manager passed in!");
		return;
	}

	Debug::trace("PresentationMethod", "verifying all reserved Presentation Methods ...");
	PresentationMethod *reservedPresentationMethod = (PresentationMethod *)NULL;
	hfc::MultiLanguageText internationalize;

	reservedPresentationMethod = findMethod((HobRecordNum)1, dbAccess);
	if (!reservedPresentationMethod)
	{
		Debug::trace("PresentationMethod", "creating reserved Presentation Method 1");
		reservedPresentationMethod = new PresentationMethod();
		if (reservedPresentationMethod)
		{
			reservedPresentationMethod->setType(Type_MostCommon);
			reservedPresentationMethod->setDescription(internationalize("PresentationMethod.TopSellers"));
			reservedPresentationMethod->setDBAccess(dbAccess);
			reservedPresentationMethod->saveRecord();
		}
	}
	else
		Debug::trace("PresentationMethod", "reserved PresentationMethod 1 already exists");

	if (reservedPresentationMethod)
	{
		delete reservedPresentationMethod;
		reservedPresentationMethod = (PresentationMethod *)NULL;
	}

	reservedPresentationMethod = findMethod((HobRecordNum)2, dbAccess);
	if (!reservedPresentationMethod)
	{
		Debug::trace("PresentationMethod", "creating reserved Presentation Method 2");
		reservedPresentationMethod = new PresentationMethod();
		if (reservedPresentationMethod)
		{
			reservedPresentationMethod->setType(Type_MostCommon);
			reservedPresentationMethod->setDescription(internationalize("PresentationMethod.Specials"));
			reservedPresentationMethod->setDBAccess(dbAccess);
			reservedPresentationMethod->saveRecord();
		}
	}
	else
		Debug::trace("PresentationMethod", "reserved PresentationMethod 2 already exists");

	if (reservedPresentationMethod)
	{
		delete reservedPresentationMethod;
		reservedPresentationMethod = (PresentationMethod *)NULL;
	}


	reservedPresentationMethod = findMethod((HobRecordNum)3, dbAccess);
	if (!reservedPresentationMethod)
	{
		Debug::trace("PresentationMethod", "creating reserved Presentation Method 3");
		reservedPresentationMethod = new PresentationMethod();
		if (reservedPresentationMethod)
		{
			reservedPresentationMethod->setType(Type_Specials);
			reservedPresentationMethod->setDescription(internationalize("PresentationMethod.KeyboardSearch"));
			reservedPresentationMethod->setDBAccess(dbAccess);
			reservedPresentationMethod->saveRecord();
		}
	}
	else
		Debug::trace("PresentationMethod", "reserved PresentationMethod 3 already exists");

	if (reservedPresentationMethod)
	{
		delete reservedPresentationMethod;
		reservedPresentationMethod = (PresentationMethod *)NULL;
	}

	reservedPresentationMethod = findMethod((HobRecordNum)4, dbAccess);
	if (!reservedPresentationMethod)
	{
		Debug::trace("PresentationMethod", "creating reserved Presentation Method 4");
		reservedPresentationMethod = new PresentationMethod();
		if (reservedPresentationMethod)
		{
			reservedPresentationMethod->setType(Type_Specials);
			reservedPresentationMethod->setDescription(internationalize("PresentationMethod.PLUEntry"));
			reservedPresentationMethod->setDBAccess(dbAccess);
			reservedPresentationMethod->saveRecord();
		}
	}
	else
		Debug::trace("PresentationMethod", "reserved PresentationMethod 4 already exists");

	if (reservedPresentationMethod)
	{
		delete reservedPresentationMethod;
		reservedPresentationMethod = (PresentationMethod *)NULL;
	}

	reservedPresentationMethod = findMethod((HobRecordNum)5, dbAccess);
	if (!reservedPresentationMethod)
	{
		Debug::trace("PresentationMethod", "creating reserved Presentation Method 5");
		reservedPresentationMethod = new PresentationMethod();
		if (reservedPresentationMethod)
		{
			reservedPresentationMethod->setType(Type_Category);
			reservedPresentationMethod->setDescription(internationalize("PresentationMethod.CategorySubCategory"));
			reservedPresentationMethod->setDBAccess(dbAccess);
			reservedPresentationMethod->saveRecord();
		}
	}
	else
		Debug::trace("PresentationMethod", "reserved PresentationMethod 5 already exists");

	if (reservedPresentationMethod)
	{
		delete reservedPresentationMethod;
		reservedPresentationMethod = (PresentationMethod *)NULL;
	}

	reservedPresentationMethod = findMethod((HobRecordNum)6, dbAccess);
	if (!reservedPresentationMethod)
	{
		Debug::trace("PresentationMethod", "creating reserved Presentation Method 6");
		reservedPresentationMethod = new PresentationMethod();
		if (reservedPresentationMethod)
		{
			reservedPresentationMethod->setType(Type_Category);
			reservedPresentationMethod->setDescription(internationalize("PresentationMethod.StartScreen"));
			reservedPresentationMethod->setDBAccess(dbAccess);
			reservedPresentationMethod->saveRecord();
		}
	}
	else
		Debug::trace("PresentationMethod", "reserved PresentationMethod 6 already exists");

	if (reservedPresentationMethod)
	{
		delete reservedPresentationMethod;
		reservedPresentationMethod = (PresentationMethod *)NULL;
	}
}
