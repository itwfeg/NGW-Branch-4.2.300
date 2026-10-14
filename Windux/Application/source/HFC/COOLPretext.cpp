/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLPretext.h"
#include "COOLTags.h"
#include "HfcError.h"
#include "Configuration.h"
#include "HobDateTime.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBCoolPreTextsRM.h"
#include "DBCoolPreText.h"

#include "e_string/utilstring.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int COOLPretext::dataType = 0;
std::string COOLPretext::dataTypeName("COOLPretext"); 
#endif

std::list<COOLPretextFieldOffsets> COOLPretext::browserFieldList,
												COOLPretext::evolFieldList,
												COOLPretext::ultimaFieldList;


COOLPretext::COOLPretext()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

COOLPretext::COOLPretext(const COOLPretext *pretextRec)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	if (pretextRec)
	{
		setDeptNum(pretextRec->getDeptNum());
		setRecordNum(pretextRec->getRecordNum());
		setName1(pretextRec->getName1());
		setName2(pretextRec->getName2());
		setAllCategoriesFlag(pretextRec->isAllCategoriesFlag());
		setCategoryNum(pretextRec->getCategoryNum());
	}
}

COOLPretext::COOLPretext(const char *data, int len, int formatType)
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
			std::list<COOLPretextFieldOffsets>::iterator fieldIter = browserFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while (tok.next(&token) != -1 && fieldIter != browserFieldList.end())
			{
				offset = *fieldIter;
				if (token.length())
				{
					setReceived(offset, true);
					if (offset == fieldOffsetCOOLPretextName1 || offset == fieldOffsetCOOLPretextName2)
						token.unQuoteForCSV();
					updateField(offset, token);
				}
				fieldIter++;
			}

			// Only for the UI, if we didn't receive the deptNum with the Class info, the set the deptNum to the currently active dept.
			// If we don't receive it via comm/merge/restore, or other mechanism in the subsequent cases, then it remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetCOOLPretextDeptNum))
			{
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
				setReceived(fieldOffsetCOOLPretextDeptNum, true);
			}

			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		{
			COOLPretextTags coolTags;

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

						// When bringing in data in Ultima format, all Pretext records should report to
						// All Categories, since the category number doesn't come in with Text/Pretext.
						if (offset == fieldOffsetCOOLPretextTextTextType)
						{
							if (!isReceived(fieldOffsetCOOLPretextCategory))
							{
								categoryNum = 0;

								setReceived(fieldOffsetCOOLPretextAllCategoriesFlag, true);
								allCategoriesFlag = true;
							}
						}
						else if (offset == fieldOffsetCOOLPretextCategory)
						{
							setReceived(fieldOffsetCOOLPretextAllCategoriesFlag, true);
							allCategoriesFlag = (categoryNum == 0) ? true : false;
						}
					}
				}
				delete token;
			}

			// For comm/backup/restore, if we didn't receive the deptNum with the data and the config parameter is set, set the deptNum to the currently active dept.
			// If we didn't receive the deptNum with the data and the config parameter is NOT set (normal case), deptNum remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetCOOLPretextDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetCOOLPretextDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

COOLPretext::COOLPretext(DBCoolPreText *dbCoolPretext, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("COOLPretext", "Constructor passing in a db rec");
	init();

	dbAccess = access;

	if (dbCoolPretext)
	{
		Debug::trace("COOLPretext", "Constructor passing in a db rec - dbrec is not null");
		myDbRecord = dbCoolPretext;

		recordNum = (HobRecordNum)Long::valueOf(dbCoolPretext->getNumber());
		deptNum = Long::valueOf(dbCoolPretext->getDeptNumber());

		if (!dbCoolPretext->isName1Null())
			name1 = dbCoolPretext->getName1();
		if (!dbCoolPretext->isName2Null())
			name2 = dbCoolPretext->getName2();
		allCategoriesFlag = (dbCoolPretext->isAllCategoriesNull()) ? false : dbCoolPretext->getAllCategories();

		errorCode = RECORD_OK;
	}
//	QUESTION: How do we handle a corrupt COOLPretext record?
}

COOLPretext::~COOLPretext()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void COOLPretext::init()
{
	recordNum	= RECORD_NOT_FOUND;
	deptNum     = 0;
	name1		= "";
	name2		= "";
	allCategoriesFlag = true;
	categoryNum	= 0; // was RECORD_NOT_FOUND... we want 0 for the "All" category
	errorCode	= RECORD_NOT_FOUND;

	dbAccess	= (DBAccess *)NULL;
	myDbRecord	= (DBCoolPreText *)NULL;

	initFieldList();
}

// static
void COOLPretext::initFieldList()
{
	if (browserFieldList.empty())
	{
		browserFieldList.push_back(fieldOffsetCOOLPretextNumber);
		browserFieldList.push_back(fieldOffsetCOOLPretextName1);
		browserFieldList.push_back(fieldOffsetCOOLPretextName2);
		browserFieldList.push_back(fieldOffsetCOOLPretextCategory);
	}

	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetCOOLPretextDeptNum);
		evolFieldList.push_back(fieldOffsetCOOLPretextNumber);
		evolFieldList.push_back(fieldOffsetCOOLPretextName1);
		evolFieldList.push_back(fieldOffsetCOOLPretextName2);
		evolFieldList.push_back(fieldOffsetCOOLPretextAllCategoriesFlag);

		COOLPretextTags coolTags((FieldOffsetsList *)&evolFieldList);
	}

	if (ultimaFieldList.empty())
	{
		ultimaFieldList.push_back(fieldOffsetCOOLPretextTextNum);
		ultimaFieldList.push_back(fieldOffsetCOOLPretextTextText);
		ultimaFieldList.push_back(fieldOffsetCOOLPretextTextTextType);

		COOLPretextTags coolTags((FieldOffsetsList *)&ultimaFieldList);
	}
}

HobRecordNum COOLPretext::getDeptNum() const
{
	return deptNum;
}
void COOLPretext::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}


HobRecordNum COOLPretext::getRecordNum() const
{
	return recordNum;
}
void COOLPretext::setRecordNum(HobRecordNum number)
{
	recordNum = number;
}


const HobString &COOLPretext::getName1() const
{
	return name1;
}
void COOLPretext::setName1(const HobString &name)
{
	if (name.length() > 100)
	{
		name1.assign(name, 0, 100);
		Debug::trace("ClipText", "COOLPretext: name1 clipped to [" + name1 + "]");
	}
	else
	{
		name1 = name;
		Debug::trace("ClipText", "COOLPretext: name1 is [" + name1 + "]");
	}
}


const HobString &COOLPretext::getName2() const
{
	return name2;
}
void COOLPretext::setName2(const HobString &name)
{
	if (name.length() > 100)
	{
		name2.assign(name, 0, 100);
		Debug::trace("ClipText", "COOLPretext: name2 clipped to [" + name2 + "]");
	}
	else
	{
		name2 = name;
		Debug::trace("ClipText", "COOLPretext: name2 is [" + name2 + "]");
	}
}


bool COOLPretext::isAllCategoriesFlag() const
{
	return allCategoriesFlag;
}
void COOLPretext::setAllCategoriesFlag(bool flag)
{
	allCategoriesFlag = flag;

	if (allCategoriesFlag)
	{
		categoryNum	= 0;
		Debug::trace("COOLPretext", "setAllCategoriesFlag(true):  Set categoryNum to zero");
	}
	else
	{
		categoryNum	= RECORD_NOT_FOUND;
		Debug::trace("COOLPretext", "setAllCategoriesFlag(false):  Set categoryNum to RECORD_NOT_FOUND");
	}
}


HobRecordNum COOLPretext::getCategoryNum() const
{
	return categoryNum;
}
void COOLPretext::setCategoryNum(HobRecordNum recordNum)
{
	categoryNum = recordNum;

	if (categoryNum == 0)
	{
		allCategoriesFlag = true;
		Debug::trace("COOLPretext", "setCategoryNum(0):  Set allCategoriesFlag to true");
	}
	else
	{
		allCategoriesFlag = false;
		Debug::trace("COOLPretext", "setCategoryNum(!0):  Set allCategoriesFlag to false");
	}
}


HobErrorCode COOLPretext::getErrorCode() const
{
	return errorCode;
}
void COOLPretext::setErrorCode(HobErrorCode errorCode)
{
	this->errorCode = errorCode;
}


int COOLPretext::updateField(int fieldCode, const HobString &token)
{
	if (Debug::isTracing("COOLPretext"))
	{
		HobString myDebug("updateField() - fieldCode: ");
		myDebug.append(Integer::toString(fieldCode)).append("  data: ").append(token);
		Debug::trace("COOLPretext", myDebug);
	}

	if (fieldCode < 0)
		return -1;

	switch (fieldCode)
	{
		case fieldOffsetCOOLPretextNumber:
		case fieldOffsetCOOLPretextTextNum:
			setRecordNum((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetCOOLPretextName1:
		case fieldOffsetCOOLPretextTextText: 
			setName1(token);
			break;
		case fieldOffsetCOOLPretextName2:
			setName2(token);
			break;
		case fieldOffsetCOOLPretextAllCategoriesFlag:
			setAllCategoriesFlag(Bool::valueOf(token));
			break;
		case fieldOffsetCOOLPretextCategory:
			setCategoryNum((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetCOOLPretextDeptNum:
			setDeptNum((HobRecordNum)Long::valueOf(token));
			break;
		default:
			break;
	}

	return 0;
}


void COOLPretext::update(const COOLPretext *newCOOLPretextText)
{
	if (!newCOOLPretextText)
		return;

	short loopCnt = 1;
	while (loopCnt < endCOOLPretextFieldOffsets)
	{
		if(newCOOLPretextText->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetCOOLPretextDeptNum:
					setDeptNum(newCOOLPretextText->getDeptNum());
				break;
				case fieldOffsetCOOLPretextNumber:
					setRecordNum(newCOOLPretextText->getRecordNum());
					break;
				case fieldOffsetCOOLPretextName1:
					setName1(newCOOLPretextText->getName1());
					break;
				case fieldOffsetCOOLPretextName2:
					setName2(newCOOLPretextText->getName2());
					break;
				case fieldOffsetCOOLPretextAllCategoriesFlag:
					setAllCategoriesFlag(newCOOLPretextText->isAllCategoriesFlag());
					break;
				case fieldOffsetCOOLPretextCategory:
					setCategoryNum(newCOOLPretextText->getCategoryNum());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


void COOLPretext::getFieldAsString(int fieldCode, HobString &result, int /* format */) const
{
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetCOOLPretextDeptNum:
			result = longtostring(deptNum);
		break;
		case fieldOffsetCOOLPretextNumber:
		case fieldOffsetCOOLPretextTextNum:
			result =	Long::toString(recordNum);
			break;
		case fieldOffsetCOOLPretextName1:
		case fieldOffsetCOOLPretextTextText: 
			result =	name1;
			break;
		case fieldOffsetCOOLPretextName2:
			result =	name2;
			break;
		case fieldOffsetCOOLPretextAllCategoriesFlag:
			result =	Bool::toString(allCategoriesFlag);
			break;
		case fieldOffsetCOOLPretextCategory:
			if (allCategoriesFlag)
				result = "0";
			else if (categoryNum > RECORD_NOT_FOUND)
				result = Long::toString(categoryNum);
			break;
		case fieldOffsetCOOLPretextTextTextType:
			result = "P";
			break;
		default:
			break;
	}
}


const char *COOLPretext::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<COOLPretextFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case BrowserFormat:
		{
			bool firstTime = true;
			HobString fieldData = "";
			fieldIter = browserFieldList.begin();
			while (fieldIter != browserFieldList.end())
			{
				if (firstTime)
					firstTime = false;
				else
					serializedData.append(BrowserFormatSeparator);

				getFieldAsString(*fieldIter, fieldData, BrowserFormat);
				if (*fieldIter == fieldOffsetCOOLPretextName1 || *fieldIter == fieldOffsetCOOLPretextName2)
					fieldData.quoteForCSV();

				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			Debug::trace("COOLPretext", "serialize():  EvolutionTaggedFormat");
			COOLPretextTags coolTags;
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
			Debug::trace("COOLPretext", "serialize():  UltimaTaggedFormat");
			COOLPretextTags coolTags;
			HobString tag = "";
			HobString fieldData = "";

			std::list<COOLPretextFieldOffsets>::iterator fieldIter = ultimaFieldList.begin();
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
			serializedData = HobString("COOLPretext::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


void COOLPretext::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


DBCoolPreText *COOLPretext::getMyDbRecord()
{
	return myDbRecord;
}


COOLPretext *COOLPretext::findCOOLPretext(HobRecordNum recNum, HobRecordNum deptNum, DBAccess *dbAccess)
{
	COOLPretext *readCoolPretext = (COOLPretext *)NULL;

	if (Debug::isTracing("COOLPretext.find"))
	{
		HobString myDebug("Looking for Pretext number: ");
		myDebug.append(Long::toString(recNum));
		Debug::trace("COOLPretext.find", myDebug);
	}

	if (!dbAccess)
		return readCoolPretext;

	DBCoolPreTextsRM	*dbCoolPretextsRM = dbAccess->getDBCoolPreTextsRM();
	if (!dbCoolPretextsRM)
		return readCoolPretext;

	try
	{
		DBCoolPreText *dbCoolPretextTextRec = dbCoolPretextsRM->findNumber(longtostring(recNum), longtostring(deptNum) );
		if (dbCoolPretextTextRec)
		{
			Debug::trace("COOLPretext.find", "Found pretext in database.");

			readCoolPretext = new COOLPretext(dbCoolPretextTextRec, dbAccess);
			if (!readCoolPretext)
				delete dbCoolPretextTextRec;
			else
			{
/*
				if (Debug::isTracing("COOLPretext.find"))
				{
					HobString myDebug("COOLPretext object: ");
					myDebug.append(Hex::toString((long) readCoolPretext);
					myDebug.append("   COOLPretext number: ").append(Long::toString(readCoolPretext->getRecordNum()));
					Debug::trace("COOLPretext.find", myDebug);
				}
*/
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "COOLPretext::findCOOLPretext() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("   DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info:").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}

	return readCoolPretext;
}


bool COOLPretext::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int COOLPretext::deleteRecord()
{
	int result = 0;

	if (myDbRecord && dbAccess)
	{
		DBCoolPreTextsRM	*dbCoolPretextsRM = dbAccess->getDBCoolPreTextsRM();
		if (dbCoolPretextsRM)
		{
			if (Debug::isTracing("COOLPretext"))
			{
				HobString myDebug("Deleting COOLPretext: ");
				myDebug.append(name1);
				Debug::trace("COOLPretext", myDebug);
			}
			result = dbCoolPretextsRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void COOLPretext::saveRecord()
{
	if (recordNum <= 0)
	{
		if (Debug::isTracing("COOLPretext"))
		{
			HobString myDebug("saveRecord(): Pretext number is ");
			myDebug.append(Long::toString(recordNum)).append(" - exiting without saving this record!!!");
			Debug::trace("COOLPretext", myDebug);
		}
		return;
	}

	if (!dbAccess)
		return;

	DBCoolPreTextsRM	*dbCoolPretextsRM = dbAccess->getDBCoolPreTextsRM();
	if (!dbCoolPretextsRM)
		return;

	if (!name1.empty())
	{
		name1.stripChar(_LF_);
		name1.stripChar(_CR_);
	}

	if (!name2.empty())
	{
		name2.stripChar(_LF_);
		name2.stripChar(_CR_);
	}

	if (!myDbRecord)
	{
		Debug::trace("COOLPretext", "saveRecord() - don't have a db record, creating it...");
		myDbRecord = dbCoolPretextsRM->createRecord();
	}

	if (myDbRecord)
	{
		Debug::trace("COOLPretext", "Going to set data elements");

		//	Plug the changes back into the dbCOOLPretext Record
		myDbRecord->setNumber(Long::toString(recordNum));
		myDbRecord->setDeptNumber(longtostring(deptNum));

		if (name1.empty())
			myDbRecord->setName1Null();
		else
			myDbRecord->setName1(name1);
		if (name2.empty())
			myDbRecord->setName2Null();
		else
			myDbRecord->setName2(name2);
		myDbRecord->setAllCategories(allCategoriesFlag);

		try
		{
			dbCoolPretextsRM->saveRecord(myDbRecord);		//save record to the database
		}
		catch (DBException &dbe)
		{
			Debug::trace("DBAccess.exception", "COOLPretext::saveRecord() ********** database exception **********");
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
		Debug::trace("COOLPretext", "Saved to the database");
	}
}
