/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLProductionStep.h"
#include "COOLTags.h"
#include "HfcError.h"
#include "Configuration.h"
#include "HobDateTime.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBCoolProductionStepsRM.h"
#include "DBCoolProductionStep.h"
#include "DBCoolCategoryProductionStepsRM.h"
#include "DBCoolCategoryProductionStep.h"

#include "e_string/utilstring.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int COOLProductionStep::dataType = 0;
std::string COOLProductionStep::dataTypeName("COOLProductionStep"); 
#endif

std::list<COOLProductionStepFieldOffsets> COOLProductionStep::browserFieldList,
														COOLProductionStep::evolFieldList,
														COOLProductionStep::ultimaFieldList;


COOLProductionStep::COOLProductionStep()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

COOLProductionStep::COOLProductionStep(const COOLProductionStep *productionStepRec)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	if (productionStepRec)
	{
		setDeptNum(productionStepRec->getDeptNum());
		setName1(productionStepRec->getName1());
		setName2(productionStepRec->getName2());
		setAllCategoriesFlag(productionStepRec->isAllCategoriesFlag());
		setCategoryNum(productionStepRec->getCategoryNum());
		setProductionStepNum(productionStepRec->getProductionStepNum());
	}
}

COOLProductionStep::COOLProductionStep(const char *data, int len, int formatType)
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
			std::list<COOLProductionStepFieldOffsets>::iterator fieldIter = browserFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while (tok.next(&token) != -1 && fieldIter != browserFieldList.end())
			{
				offset = *fieldIter;
				if (token.length())
				{
					setReceived(offset, true);
					if (offset == fieldOffsetCOOLProductionStepName1 || offset == fieldOffsetCOOLProductionStepName2)
						token.unQuoteForCSV();
					updateField(offset, token);
				}
				fieldIter++;
			}

			// Only for the UI, if we didn't receive the deptNum with the Class info, the set the deptNum to the currently active dept.
			// If we don't receive it via comm/merge/restore, or other mechanism in the subsequent cases, then it remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetCOOLProductionStepDeptNum))
			{
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
				setReceived(fieldOffsetCOOLProductionStepDeptNum, true);
			}

			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		{
			COOLProductionStepTags coolTags;

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

						if (offset == fieldOffsetCOOLProductionStepCategory && !isReceived(fieldOffsetCOOLProductionStepAllCategoriesFlag))
						{
							setReceived(fieldOffsetCOOLProductionStepAllCategoriesFlag, true);
							allCategoriesFlag = (categoryNum == 0) ? true : false;
						}
					}
				}
				delete token;
			}

			// For comm/backup/restore, if we didn't receive the deptNum with the data and the config parameter is set, set the deptNum to the currently active dept.
			// If we didn't receive the deptNum with the data and the config parameter is NOT set (normal case), deptNum remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetCOOLProductionStepDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetCOOLProductionStepDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

COOLProductionStep::COOLProductionStep(DBCoolProductionStep *dbCoolProductionStep, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("COOLProductionStep", "Constructor passing in a db rec");
	init();

	dbAccess = access;

	if (dbCoolProductionStep)
	{
		Debug::trace("COOLProductionStep", "Constructor passing in a db rec - dbrec is not null");
		myDbRecord = dbCoolProductionStep;

		if (!dbCoolProductionStep->isName1Null())
			name1 = dbCoolProductionStep->getName1();

		if (!dbCoolProductionStep->isName2Null())
			name2 = dbCoolProductionStep->getName2();

		deptNum = Long::valueOf(dbCoolProductionStep->getDeptNumber());

		allCategoriesFlag = (dbCoolProductionStep->isAllCategoriesNull()) ? false : dbCoolProductionStep->getAllCategories();

		if (!dbCoolProductionStep->isProductionStepNumberNull())
			productionStepNum = Long::valueOf(dbCoolProductionStep->getProductionStepNumber());

		errorCode = RECORD_OK;
	}
//	QUESTION: How do we handle a corrupt COOLProductionStep record?
}

COOLProductionStep::~COOLProductionStep()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void COOLProductionStep::init()
{
	name1		= "";
	name2		= "";
	deptNum     = 0;
	allCategoriesFlag = true;
	categoryNum			= 0; // was RECORD_NOT_FOUND... we want 0 for the "All" category
	productionStepNum	= RECORD_NOT_FOUND;
	errorCode			= RECORD_NOT_FOUND;

	dbAccess	= (DBAccess *)NULL;
	myDbRecord	= (DBCoolProductionStep *)NULL;

	initFieldList();
}

// static
void COOLProductionStep::initFieldList()
{
	if (browserFieldList.empty())
	{
		browserFieldList.push_back(fieldOffsetCOOLProductionStepName1);
		browserFieldList.push_back(fieldOffsetCOOLProductionStepName2);
		browserFieldList.push_back(fieldOffsetCOOLProductionStepCategory);
	}

	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetCOOLProductionStepDeptNum);
		evolFieldList.push_back(fieldOffsetCOOLProductionStepName1);
		evolFieldList.push_back(fieldOffsetCOOLProductionStepName2);
		evolFieldList.push_back(fieldOffsetCOOLProductionStepAllCategoriesFlag);
		evolFieldList.push_back(fieldOffsetCOOLProductionStepNum);					//	To retain the step num from an Ultima/Quantum file.

		COOLProductionStepTags coolTags((FieldOffsetsList *)&evolFieldList);
	}

	if (ultimaFieldList.empty())
	{
		ultimaFieldList.push_back(fieldOffsetCOOLProductionStepNum);
		ultimaFieldList.push_back(fieldOffsetCOOLProductionStepName1);
		ultimaFieldList.push_back(fieldOffsetCOOLProductionStepCategory);

		COOLProductionStepTags coolTags((FieldOffsetsList *)&ultimaFieldList);
	}
}

HobRecordNum COOLProductionStep::getDeptNum() const
{
	return deptNum;
}
void COOLProductionStep::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

const HobString &COOLProductionStep::getName1() const
{
	return name1;
}
void COOLProductionStep::setName1(const HobString &name)
{
	if (name.length() > 100)
	{
		name1.assign(name, 0, 100);
		Debug::trace("ClipText", "COOLProductionStep: name1 clipped to [" + name1 + "]");
	}
	else
	{
		name1 = name;
		Debug::trace("ClipText", "COOLProductionStep: name1 is [" + name1 + "]");
	}
}


const HobString &COOLProductionStep::getName2() const
{
	return name2;
}
void COOLProductionStep::setName2(const HobString &name)
{
	if (name.length() > 100)
	{
		name2.assign(name, 0, 100);
		Debug::trace("ClipText", "COOLProductionStep: name2 clipped to [" + name2 + "]");
	}
	else
	{
		name2 = name;
		Debug::trace("ClipText", "COOLProductionStep: name2 is [" + name2 + "]");
	}
}


bool COOLProductionStep::isAllCategoriesFlag() const
{
	return allCategoriesFlag;
}
void COOLProductionStep::setAllCategoriesFlag(bool flag)
{
	allCategoriesFlag = flag;

	if (allCategoriesFlag)
	{
		categoryNum	= 0;
		Debug::trace("COOLProductionStep", "setAllCategoriesFlag(true):  Set categoryNum to zero");
	}
	else
	{
		categoryNum	= RECORD_NOT_FOUND;
		Debug::trace("COOLProductionStep", "setAllCategoriesFlag(false):  Set categoryNum to RECORD_NOT_FOUND");
	}
}


HobRecordNum COOLProductionStep::getCategoryNum() const
{
	return categoryNum;
}
void COOLProductionStep::setCategoryNum(HobRecordNum recordNum)
{
	categoryNum = recordNum;

	if (categoryNum == 0)
	{
		allCategoriesFlag = true;
		Debug::trace("COOLProductionStep", "setCategoryNum(0):  Set allCategoriesFlag to true");
	}
	else
	{
		allCategoriesFlag = false;
		Debug::trace("COOLProductionStep", "setCategoryNum(!0):  Set allCategoriesFlag to false");
	}
}


HobRecordNum COOLProductionStep::getProductionStepNum() const
{
	return productionStepNum;
}
void COOLProductionStep::setProductionStepNum(HobRecordNum num)
{
	productionStepNum = num;
}


HobErrorCode COOLProductionStep::getErrorCode() const
{
	return errorCode;
}
void COOLProductionStep::setErrorCode(HobErrorCode errorCode)
{
	this->errorCode = errorCode;
}


int COOLProductionStep::updateField(int fieldCode, const HobString &token)
{
	if (Debug::isTracing("COOLProductionStep"))
	{
		HobString myDebug("updateField() - fieldCode: ");
		myDebug.append(Integer::toString(fieldCode)).append("  data: ").append(token);
		Debug::trace("COOLProductionStep", myDebug);
	}

	if (fieldCode < 0)
		return -1;

	switch (fieldCode)
	{
		case fieldOffsetCOOLProductionStepName1:
			setName1(token);
			break;
		case fieldOffsetCOOLProductionStepName2:
			setName2(token);
			break;
		case fieldOffsetCOOLProductionStepAllCategoriesFlag:
			setAllCategoriesFlag(Bool::valueOf(token));
			break;
		case fieldOffsetCOOLProductionStepCategory:
			setCategoryNum((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetCOOLProductionStepNum:
			setProductionStepNum((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetCOOLProductionStepDeptNum:
			setDeptNum((HobRecordNum)Long::valueOf(token));
			break;
		default:
			break;
	}

	return 0;
}


void COOLProductionStep::update(const COOLProductionStep *newCoolProductionStep)
{
	if (!newCoolProductionStep)
		return;

	short loopCnt = 1;
	while (loopCnt < endCOOLProductionStepFieldOffsets)
	{
		if(newCoolProductionStep->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetCOOLProductionStepName1:
					setName1(newCoolProductionStep->getName1());
					break;
				case fieldOffsetCOOLProductionStepName2:
					setName2(newCoolProductionStep->getName2());
					break;
				case fieldOffsetCOOLProductionStepAllCategoriesFlag:
					setAllCategoriesFlag(newCoolProductionStep->isAllCategoriesFlag());
					break;
				case fieldOffsetCOOLProductionStepCategory:
					setCategoryNum(newCoolProductionStep->getCategoryNum());
					break;
				case fieldOffsetCOOLProductionStepNum:
					setProductionStepNum(newCoolProductionStep->getProductionStepNum());
					break;
				case fieldOffsetCOOLProductionStepDeptNum:
					setDeptNum(newCoolProductionStep->getDeptNum() );
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


void COOLProductionStep::getFieldAsString(int fieldCode, HobString &result, int /* format */) const
{
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetCOOLProductionStepName1:
			result =	name1;
			break;
		case fieldOffsetCOOLProductionStepName2:
			result =	name2;
			break;
		case fieldOffsetCOOLProductionStepAllCategoriesFlag:
			result =	Bool::toString(allCategoriesFlag);
			break;
		case fieldOffsetCOOLProductionStepCategory:
			if (allCategoriesFlag)
				result = "0";
			else if (categoryNum > RECORD_NOT_FOUND)
				result = Long::toString(categoryNum);
			break;
		case fieldOffsetCOOLProductionStepNum:
			if (productionStepNum > RECORD_NOT_FOUND)
				result =	Long::toString(productionStepNum);
			break;
		case fieldOffsetCOOLProductionStepDeptNum:
			if (deptNum > RECORD_NOT_FOUND)
				result = Long::toString(deptNum);
			break;
		default:
			break;
	}
}


const char *COOLProductionStep::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<COOLProductionStepFieldOffsets>::iterator fieldIter;
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
				if (*fieldIter == fieldOffsetCOOLProductionStepName1 || *fieldIter == fieldOffsetCOOLProductionStepName2)
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
			COOLProductionStepTags coolTags;
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
			COOLProductionStepTags coolTags;
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
			serializedData = HobString("COOLProductionStep::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


void COOLProductionStep::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


DBCoolProductionStep *COOLProductionStep::getMyDbRecord()
{
	return myDbRecord;
}


// static
COOLProductionStep *COOLProductionStep::findCOOLProductionStep(HobString stepName, HobString deptNum, DBAccess *dbAccess)
{
	COOLProductionStep *readCOOLProductionStepText = (COOLProductionStep *)NULL;

	if (Debug::isTracing("COOLProductionStep.find"))
	{
		HobString myDebug("Looking for step name: ");
		myDebug.append(stepName);
		Debug::trace("COOLProductionStep.find", myDebug);
	}

	if (!dbAccess)
		return readCOOLProductionStepText;

	DBCoolProductionStepsRM	*dbCoolProductionStepsRM = dbAccess->getDBCoolProductionStepsRM();
	if (!dbCoolProductionStepsRM)
		return readCOOLProductionStepText;

	try
	{
		DBCoolProductionStep *dbCoolProductionStepTextRec = dbCoolProductionStepsRM->findProductionStep(stepName, deptNum);
		if (dbCoolProductionStepTextRec)
		{
			Debug::trace("COOLProductionStep.find", "Found production step in database.");

			readCOOLProductionStepText = new COOLProductionStep(dbCoolProductionStepTextRec, dbAccess);
			if (!readCOOLProductionStepText)
				delete dbCoolProductionStepTextRec;
			else
			{
/*
				if (Debug::isTracing("COOLProductionStep.find"))
				{
					HobString myDebug("COOLProductionStep object: ");
					myDebug.append(Hex::toString((long) readCOOLProductionStepText);
					myDebug.append("   COOLProductionStep number: ").append(Long::toString(readCOOLProductionStepText->getRecordNum()));
					Debug::trace("COOLProductionStep.find", myDebug);
				}
*/
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "COOLProductionStep::findCOOLProductionStep() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("   DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info:").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}

	return readCOOLProductionStepText;
}


bool COOLProductionStep::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int COOLProductionStep::deleteRecord()
{
	int result = 0;

	if (myDbRecord && dbAccess)
	{
		DBCoolProductionStepsRM	*dbCoolProductionStepsRM = dbAccess->getDBCoolProductionStepsRM();
		if (dbCoolProductionStepsRM)
		{
			if (Debug::isTracing("COOLProductionStep"))
			{
				HobString myDebug("Deleting COOLProductionStep: ");
				myDebug.append(name1);
				Debug::trace("COOLProductionStep", myDebug);
			}
			result = dbCoolProductionStepsRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void COOLProductionStep::saveRecord()
{
	bool traceOn = Debug::isTracing("COOLProductionStep");
	HobString msgDebug;
	if (traceOn)
		msgDebug = "saveRecord():  ";

	if (!dbAccess)
		return;

	DBCoolProductionStepsRM	*dbCoolProductionStepsRM = dbAccess->getDBCoolProductionStepsRM();
	if (!dbCoolProductionStepsRM)
		return;

	if (!name1.empty())
	{
		name1.stripChar(_LF_);
		name1.stripChar(_CR_);
	}
	HobString tempName1 = name1;
	tempName1.stripWhiteSpace(HobString::LEADING_TRAILING);

	if (!name2.empty())
	{
		name2.stripChar(_LF_);
		name2.stripChar(_CR_);
	}
	HobString tempName2 = name2;
	tempName2.stripWhiteSpace(HobString::LEADING_TRAILING);

	bool deleteRec = (name1.empty() && name2.empty()) || (tempName1.empty() && tempName2.empty());
	if (deleteRec)
	{
		if (!myDbRecord)
		{
			if (traceOn)
				Debug::trace("COOLProductionStep", msgDebug + "Nothing to save.");
		}
		else
		{
			if (traceOn)
				Debug::trace("COOLProductionStep", msgDebug + "Don't have a Step, deleting it.");
			dbCoolProductionStepsRM->deleteRecord(myDbRecord);
		}
		return;
	}

	if (!myDbRecord)
	{
		if (traceOn)
			Debug::trace("COOLProductionStep", msgDebug + "Don't have a db record, creating it...");
		myDbRecord = dbCoolProductionStepsRM->createRecord();
	}

	if (myDbRecord)
	{
		if (traceOn)
			Debug::trace("COOLProductionStep", msgDebug + "Going to set data elements");

		//	Plug the changes back into the dbCoolProductionStep Record
		if (name1.empty())
			myDbRecord->setName1Null();
		else
			myDbRecord->setName1(name1);
		if (name2.empty())
			myDbRecord->setName2Null();
		else
			myDbRecord->setName2(name2);

		myDbRecord->setDeptNumber(Long::toString(deptNum));

		myDbRecord->setAllCategories(allCategoriesFlag);

		if (productionStepNum > 0)
			myDbRecord->setProductionStepNumber(Long::toString(productionStepNum));

		try
		{
			dbCoolProductionStepsRM->saveRecord(myDbRecord);		//save record to the database
		}
		catch (DBException &dbe)
		{
			Debug::trace("DBAccess.exception", "COOLProductionStep::saveRecord() ********** database exception **********");
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}

		//	Now that the COOLProductionStep record is saved, check the Category number
		if (!allCategoriesFlag && categoryNum > 0)
		{
			DBCoolCategoryProductionStepsRM	*dbCoolCategoryProductionStepsRM = dbAccess->getDBCoolCategoryProductionStepsRM();
			if (dbCoolCategoryProductionStepsRM)
			{
				DBCoolCategoryProductionStep *dbCoolCategoryProductionStep = dbCoolCategoryProductionStepsRM->find(Long::toString(categoryNum), Long::toString(deptNum), myDbRecord);
				if (!dbCoolCategoryProductionStep)
				{
					if (traceOn)
						Debug::trace("COOLProductionStep", msgDebug + "Don't have a COOLCategoryProductionStep db record - creating it.");

					dbCoolCategoryProductionStep = dbCoolCategoryProductionStepsRM->createRecord(Long::toString(categoryNum), myDbRecord);
				}
				if (dbCoolCategoryProductionStep)
				{
					if (traceOn)
						Debug::trace("COOLProductionStep", msgDebug + "Saving the COOLCategoryProductionStep db record.");

					try
					{
						dbCoolCategoryProductionStep->setCategoryNumber(Long::toString(categoryNum));
						dbCoolCategoryProductionStepsRM->saveRecord(dbCoolCategoryProductionStep);		//	Save the record to the database.
					}
					catch (DBException &dbe)
					{
						Debug::trace("DBAccess.exception", "dbCoolCategoryProductionStepsRM->saveRecord() in COOLProductionStep::saveRecord()  ********** database exception **********");
						DBException::Type t = dbe.getType();
						Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
					}
				}
			}
		}
		if (traceOn)
			Debug::trace("COOLProductionStep", msgDebug + "Saved to the database");
	}
}
