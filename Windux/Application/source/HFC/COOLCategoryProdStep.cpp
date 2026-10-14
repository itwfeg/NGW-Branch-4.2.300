/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLCategoryProdStep.h"
#include "COOLProductionStep.h"
#include "COOLTags.h"
#include "HfcError.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBCoolCategoryProductionStepsRM.h"
#include "DBCoolCategoryProductionStep.h"
#include "DBCoolProductionStepsRM.h"
#include "DBCoolProductionStep.h"

#include "Configuration.h"
#include "e_string/utilstring.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

using namespace util_string;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int COOLCategoryProdStep::dataType = 0;
std::string COOLCategoryProdStep::dataTypeName("COOLCategoryProdStep"); 
#endif


std::list<COOLCategoryProdStepFieldOffsets> COOLCategoryProdStep::evolFieldList, COOLCategoryProdStep::ultimaFieldList;


COOLCategoryProdStep::COOLCategoryProdStep(DBCoolCategoryProductionStep *dbCoolCategoryProdStepRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool traceOn = Debug::isTracing("COOLCategoryProdStep");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "Constructor 1 passing in DBCoolCategoryProductionStep* & DBAccess*";
		Debug::trace("COOLCategoryProdStep", msgDebug);
		msgDebug.append(":  ");
	}
	init();

	if (!dbCoolCategoryProdStepRec || !access)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("One of these is null - EXITING!!!");
			msgDebug2.append("  dbCoolCategoryProdStepRec = ").append(Hex::toString((long long) dbCoolCategoryProdStepRec));
			msgDebug2.append("  access = ").append(Hex::toString((long long) access));
			Debug::trace("COOLCategoryProdStep", msgDebug2);
		}
		return;
	}
	dbAccess = access;
	myDbRecord = dbCoolCategoryProdStepRec;

	DBCoolProductionStepsRM *dbCoolProductionStepsRM = dbAccess->getDBCoolProductionStepsRM();
	if (!dbCoolProductionStepsRM)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("dbCoolProductionStepsRM is null!");
			Debug::trace("COOLCategoryProdStep", msgDebug2);
		}
	}
	else
	{
		DBCoolProductionStep *dbCoolProductionStepRec = dbCoolProductionStepsRM->find(dbCoolCategoryProdStepRec);
		if (!dbCoolProductionStepRec)
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("dbCoolProductionStepRec is null!");
				Debug::trace("COOLCategoryProdStep", msgDebug2);
			}
		}
		else
		{
			if (!dbCoolProductionStepRec->isName1Null())
				productionStep = dbCoolProductionStepRec->getName1();
			if (!dbCoolProductionStepRec->isProductionStepNumberNull())
				productionStepNum = Long::valueOf(dbCoolProductionStepRec->getProductionStepNumber());

			deptNum = Long::valueOf(dbCoolProductionStepRec->getDeptNumber());

			delete dbCoolProductionStepRec;
		}
	}

	if (!dbCoolCategoryProdStepRec->isCategoryNumberNull())
		categoryNum = Long::valueOf(dbCoolCategoryProdStepRec->getCategoryNumber());


	errorCode = RECORD_OK;
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("  categoryNum = ").append(Long::toString(categoryNum));
		msgDebug2.append("  productionStepNum = ").append(Long::toString(productionStepNum));
		msgDebug2.append("  productionStep = [").append(productionStep).append("]");
		Debug::trace("COOLCategoryProdStep", msgDebug2);
	}
//	QUESTION: How do we handle a corrupt COOLCategoryProdStep record?
}


COOLCategoryProdStep::COOLCategoryProdStep(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	Debug::trace("COOLCategoryProdStep", "Constructor 2 passing const char *data, int len, int formatType ");

	switch (formatType)
	{
		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		{
			COOLCategoryProductionStepTags coolTags;

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
			if (!isReceived(fieldOffsetCOOLCatProdDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetCOOLCatProdDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}


COOLCategoryProdStep::COOLCategoryProdStep(COOLProductionStep *coolProductionStepRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool traceOn = Debug::isTracing("COOLCategoryProdStep");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "Constructor passing in CoolProductionStep* & DBAccess*";
		Debug::trace("COOLCategoryProdStep", msgDebug);
		msgDebug.append(":  ");
	}
	init();

	if (!coolProductionStepRec || !access)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("One of these is null - EXITING!!!");
			msgDebug2.append("  coolProductionStepRec = ").append(Hex::toString((long long) coolProductionStepRec));
			msgDebug2.append("  access = ").append(Hex::toString((long long) access));
			Debug::trace("COOLCategoryProdStep", msgDebug2);
		}
		return;
	}
	dbAccess = access;

	productionStep = coolProductionStepRec->getName1();
	productionStepNum = coolProductionStepRec->getProductionStepNum();
	if (coolProductionStepRec->isAllCategoriesFlag())
		categoryNum = 0;

	errorCode = RECORD_OK;
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("  categoryNum = ").append(Long::toString(categoryNum));
		msgDebug2.append("  productionStepNum = ").append(Long::toString(productionStepNum));
		msgDebug2.append("  productionStep = [").append(productionStep).append("]");
		Debug::trace("COOLCategoryProdStep", msgDebug2);
	}
//	QUESTION: How do we handle a corrupt COOLCategoryProdStep record?
}


COOLCategoryProdStep::~COOLCategoryProdStep()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void COOLCategoryProdStep::init()
{
	deptNum             = 0;
	productionStep		= "";
	categoryNum	= RECORD_NOT_FOUND;
	errorCode	= RECORD_NOT_FOUND;
	productionStepNum	= RECORD_NOT_FOUND;

	dbAccess			= 0;
	myDbRecord			= 0;

	initFieldList();
}

// static
void COOLCategoryProdStep::initFieldList()
{
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetCOOLCatProdDeptNum);
		evolFieldList.push_back(fieldOffsetCOOLCatProdStepCategoryNum);
		evolFieldList.push_back(fieldOffsetCOOLCatProdStepStepName);

		COOLCategoryProductionStepTags coolTags((FieldOffsetsList *)&evolFieldList);
	}

	if (ultimaFieldList.empty())
	{
		ultimaFieldList.push_back(fieldOffsetCOOLCatProdStepStepName);
		ultimaFieldList.push_back(fieldOffsetCOOLCatProdStepCategoryNum);

		COOLCategoryProductionStepTags coolTags((FieldOffsetsList *)&ultimaFieldList);
	}
}

HobRecordNum COOLCategoryProdStep::getDeptNum() const
{
	return deptNum;
}
void COOLCategoryProdStep::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

const HobString &COOLCategoryProdStep::getProductionStep() const
{
	return productionStep;
}
void COOLCategoryProdStep::setProductionStep(const HobString &name)
{
	productionStep = name;
}


HobRecordNum COOLCategoryProdStep::getCategoryNum() const
{
	return categoryNum;
}
void COOLCategoryProdStep::setCategoryNum(HobRecordNum recordNum)
{
	categoryNum = recordNum;
}


HobErrorCode COOLCategoryProdStep::getErrorCode() const
{
	return errorCode;
}
void COOLCategoryProdStep::setErrorCode(HobErrorCode errorCode)
{
	this->errorCode = errorCode;
}


int COOLCategoryProdStep::updateField(int fieldCode, const HobString &token)
{
	bool traceOn = Debug::isTracing("COOLCategoryProdStep");
	HobString myDebug;
	if (traceOn)
	{
		myDebug = "updateField() - fieldCode: ";
		myDebug.append(Integer::toString(fieldCode)).append("  data: ").append(token);
		Debug::trace("COOLCategoryProdStep", myDebug);
	}

	if (fieldCode < 0)
		return -1;

	switch (fieldCode)
	{
		case fieldOffsetCOOLCatProdDeptNum:
			setDeptNum((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetCOOLCatProdStepCategoryNum:
			setCategoryNum((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetCOOLCatProdStepStepName:
			setProductionStep(token);
			break;
		default:
			if (traceOn)
				Debug::trace("COOLCategoryProdStep", myDebug + "fieldCode unknown!!!");
			break;
	}

	return 0;
}


void COOLCategoryProdStep::update(const COOLCategoryProdStep *newCOOLProductionStep)
{
	if (!newCOOLProductionStep)
		return;

	short loopCnt = 1;
	while (loopCnt < endCOOLCategoryProdStepFieldOffsets)
	{
		if (newCOOLProductionStep->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetCOOLCatProdDeptNum:
					setProductionStep(newCOOLProductionStep->getProductionStep());
				break;
				case fieldOffsetCOOLCatProdStepCategoryNum:
					setCategoryNum(newCOOLProductionStep->getCategoryNum());
					break;
				case fieldOffsetCOOLCatProdStepStepName:
					setProductionStep(newCOOLProductionStep->getProductionStep());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


void COOLCategoryProdStep::getFieldAsString(int fieldCode, HobString &result, int /* format */) const
{
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetCOOLCatProdDeptNum:
			result = Long::toString(deptNum);
			break;
		case fieldOffsetCOOLCatProdStepCategoryNum:
			if (categoryNum > RECORD_NOT_FOUND)
				result = Long::toString(categoryNum);
			break;
		case fieldOffsetCOOLCatProdStepStepName:
			result = productionStep;
			break;
		case fieldOffsetCOOLCatProdStepNum:
			if (productionStepNum > RECORD_NOT_FOUND)
				result = Long::toString(productionStepNum);
			break;
		default:
			break;
	}
}


const char *COOLCategoryProdStep::serialize(int format) const
{
	bool traceOn = Debug::isTracing("COOLCategoryProdStep.serialize");
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<COOLCategoryProdStepFieldOffsets>::iterator fieldIter;

	switch (format)
	{
		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			COOLCategoryProductionStepTags coolTags;
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
			COOLCategoryProductionStepTags coolTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = ultimaFieldList.begin();
			while (fieldIter != ultimaFieldList.end())
			{
				tag = coolTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData, UltimaTaggedFormat);
					if (traceOn)
					{
						HobString msgDebug2 = "tag = ";
						msgDebug2.append(tag);
						msgDebug2.append("  tagString = [").append(fieldData).append("]");
						Debug::trace("COOLCategoryProdStep.serialize", msgDebug2);
					}

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		default:
			serializedData = HobString("COOLCategoryProdStep::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


void COOLCategoryProdStep::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


DBCoolCategoryProductionStep *COOLCategoryProdStep::getMyDbRecord()
{
	return myDbRecord;
}


COOLCategoryProdStep *COOLCategoryProdStep::findCOOLCategoryProductionStep(HobRecordNum categoryNum, HobRecordNum deptNum, DBCoolProductionStep *dbCoolProductionStep, DBAccess *dbAccess)
{
	COOLCategoryProdStep *coolCategoryProdStep = 0;

	if (!dbCoolProductionStep || !dbAccess)
		return coolCategoryProdStep;

	if (Debug::isTracing("COOLCategoryProductionStep.find"))
	{
		HobString myDebug("Looking for category number: ");
		myDebug.append(Long::toString(categoryNum)).append(" for step [").append(dbCoolProductionStep->getName1()).append("]");
		Debug::trace("COOLCategoryProductionStep.find", myDebug);
	}

	DBCoolCategoryProductionStepsRM	*dbCoolCategoryProductionStepsRM = dbAccess->getDBCoolCategoryProductionStepsRM();
	if (!dbCoolCategoryProductionStepsRM)
		return coolCategoryProdStep;

	try
	{
		DBCoolCategoryProductionStep *dbCoolCategoryProductionStep = dbCoolCategoryProductionStepsRM->find(Long::toString(categoryNum), Long::toString(deptNum), dbCoolProductionStep);
		if (dbCoolCategoryProductionStep)
		{
			Debug::trace("COOLCategoryProductionStep.find", "Found category in database.");

			coolCategoryProdStep = new COOLCategoryProdStep(dbCoolCategoryProductionStep, dbAccess);
			if (!coolCategoryProdStep)
				delete dbCoolCategoryProductionStep;
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "COOLCategoryProductionStep::findCOOLCategoryProductionStep() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("   DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info:").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
	return coolCategoryProdStep;
}

bool COOLCategoryProdStep::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int COOLCategoryProdStep::deleteRecord()
{
	int result = 0;

	if (myDbRecord && dbAccess)
	{
		DBCoolCategoryProductionStepsRM *dbCoolCategoryProdStepsRM = dbAccess->getDBCoolCategoryProductionStepsRM();
		if (dbCoolCategoryProdStepsRM)
		{
			if (Debug::isTracing("COOLCategoryProdStep"))
			{
				HobString myDebug("Deleting COOLCategoryProdStep:  productionStep = ");
				myDebug.append(productionStep);
				myDebug.append("  categoryNum = ").append(Long::toString(categoryNum));
				Debug::trace("COOLCategoryProdStep", myDebug);
			}
			result = dbCoolCategoryProdStepsRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void COOLCategoryProdStep::saveRecord()
{
	bool traceOn = Debug::isTracing("COOLCategoryProdStep");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "saveRecord()";
		Debug::trace("COOLCategoryProdStep", msgDebug);
		msgDebug.append(":  ");
	}
	if (!dbAccess)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("dbAccess is null - EXITING!!!");
			Debug::trace("COOLCategoryProdStep", msgDebug2);
		}
		return;
	}

	COOLProductionStep *coolProductionStep = 0;

	DBCoolCategoryProductionStepsRM	*dbCoolCategoryProdStepsRM = dbAccess->getDBCoolCategoryProductionStepsRM();
	if (!dbCoolCategoryProdStepsRM)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("dbCoolCategoryProdStepsRM is null - EXITING!!!");
			Debug::trace("COOLCategoryProdStep", msgDebug2);
		}
		return;
	}

	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("productionStep = [").append(productionStep).append("]");
		Debug::trace("COOLCategoryProdStep", msgDebug2);
	}
	if (!productionStep.empty())
	{
		productionStep.stripChar(_LF_);
		productionStep.stripChar(_CR_);
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("After stripChar, productionStep = [").append(productionStep).append("]");
		Debug::trace("COOLCategoryProdStep", msgDebug2);
	}

	coolProductionStep = COOLProductionStep::findCOOLProductionStep(productionStep, longtostring(deptNum), dbAccess);
	if (!coolProductionStep)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("Can't find a dbCoolProductionStep record - creating it...");
			Debug::trace("COOLCategoryProdStep", msgDebug2);
		}
		coolProductionStep = new COOLProductionStep();
		if (coolProductionStep)
		{
			coolProductionStep->setDBAccess(dbAccess);
			coolProductionStep->setName1(productionStep);
			coolProductionStep->setDeptNum(deptNum);
			coolProductionStep->saveRecord();
		}
	}
	if (!coolProductionStep)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("coolProductionStep is null - EXITING!!!");
			Debug::trace("COOLCategoryProdStep", msgDebug2);
		}
		return;
	}

	if (!myDbRecord)
	{
		myDbRecord = dbCoolCategoryProdStepsRM->find(Long::toString(categoryNum), Long::toString(deptNum), coolProductionStep->getMyDbRecord());
		if (!myDbRecord)
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Don't have a db record - creating it.");
				Debug::trace("COOLCategoryProdStep", msgDebug2);
			}
			myDbRecord = dbCoolCategoryProdStepsRM->createRecord(Long::toString(categoryNum), coolProductionStep->getMyDbRecord());
		}
	}
	if (myDbRecord)
	{
		myDbRecord->setCategoryNumber(Long::toString(categoryNum));
		myDbRecord->setDeptNumber(Long::toString(deptNum));

		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("Saving the new db record.");
			Debug::trace("COOLCategoryProdStep", msgDebug2);
		}
		try
		{
			dbCoolCategoryProdStepsRM->saveRecord(myDbRecord);		//	Save the record to the database.
		}
		catch (DBException &dbe)
		{
			Debug::trace("DBAccess.exception", "COOLCategoryProdStep::saveRecord() ********** database exception **********");
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}
	if (coolProductionStep)
		delete coolProductionStep;
}

void COOLCategoryProdStep::show()
{
	if (Debug::isTracing("COOLCategoryProdStep"))
	{
		HobString msgDebug = "productionStep = [";
		msgDebug.append(productionStep).append("]");
		Debug::trace("COOLCategoryProdStep", msgDebug);

		msgDebug = "categoryNum = ";
		msgDebug.append(Long::toString(categoryNum));
		Debug::trace("COOLCategoryProdStep", msgDebug);

		msgDebug = "productionStepNum = ";
		msgDebug.append(Long::toString(productionStepNum));
		Debug::trace("COOLCategoryProdStep", msgDebug);
	}
}
