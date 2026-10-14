/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductStatistic.h"
#include "ProductStatisticTags.h"
#include "Configuration.h"
#include "HobDateTime.h"

//#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBProductStatisticsRM.h"
#include "DBProductStatistic.h"
#include "DBProductsRM.h"
#include "DBProduct.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductStatistic::dataType = 0;
std::string ProductStatistic::dataTypeName("ProductStatistic"); 
#endif

list<ProductStatisticFieldOffsets> ProductStatistic::evolFieldList;	


ProductStatistic::ProductStatistic()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

ProductStatistic::ProductStatistic(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			ProductStatisticTags productStatisticTags;

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
					offset = (temp.size()) ? productStatisticTags.getOffset(temp) : 0;
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
			if (!isReceived(fieldOffsetPSDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetPSDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			break;
	}
}

ProductStatistic::ProductStatistic(DBProductStatistic *dbRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("ProductStatistic.ctor", string("ProductStatistic(DBProductStatistic, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	dbAccess = access;

	if (dbRec)
	{
		myDbRecord = dbRec;

		if (dbAccess)
		{
			DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
			if (dbProductsRM)
			{
				DBProduct *dbProductRec = dbProductsRM->findLinkedRecord(dbRec);
				if (dbProductRec)
				{
					deptNum = Long::valueOf(dbProductRec->getDeptNumber());
					productNum = Long::valueOf(dbProductRec->getPLU());
					delete dbProductRec;
				}
			}
		}

		language          = dbRec->getLanguage();
		hourNum           = Integer::valueOf(dbRec->getHourNumber());
		selectedCount     = dbRec->getTotalSelections();
		selectedTime      = dbRec->getTimeToSelect();
		printedCount      = dbRec->getTotalLabelsPrinted();
		recordedDateTime_ = dbRec->getRecordedDateTime();
		errorCode         = RECORD_OK;
	}
}


ProductStatistic::~ProductStatistic()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("ProductStatistic.dtor", "    ProductStatistic: " + ptrtostring(this) + "  num " + longtostring(productNum) + "  deptNum " + longtostring(deptNum));

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void ProductStatistic::init()
{
	deptNum = 0;
	productNum = RECORD_NOT_FOUND;
	language = "";
	hourNum = 0;
	selectedCount = 0;
	selectedTime = 0;
	printedCount = 0;
	recordedDateTime_.clear();

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBProductStatistic *)NULL;

	errorCode = RECORD_NOT_FOUND;

	initFieldLists();
}

// static
void ProductStatistic::initFieldLists()
{
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetPSDeptNum);
		evolFieldList.push_back(fieldOffsetPSProductNum);
		evolFieldList.push_back(fieldOffsetPSLanguage);
		evolFieldList.push_back(fieldOffsetPSHourNum);
		evolFieldList.push_back(fieldOffsetPSSelectedCount);
		evolFieldList.push_back(fieldOffsetPSPrintedCount);
		evolFieldList.push_back(fieldOffsetPSSelectedTime);
		evolFieldList.push_back(fieldOffsetPSRecordedDateTime);

		ProductStatisticTags productStatisticTags((FieldOffsetsList *)&evolFieldList);
	}
}


HobRecordNum ProductStatistic::getDeptNum() const
{
	return deptNum;
}
void ProductStatistic::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

HobRecordNum ProductStatistic::getProductNum() const
{
	return productNum;
}
void ProductStatistic::setProductNum(HobRecordNum num)
{
	productNum = num;
}

const HobString &ProductStatistic::getLanguage() const
{
	return language;
}
void ProductStatistic::setLanguage(const HobString &lang)
{
	language = lang;
}

int ProductStatistic::getHourNum() const
{
	return hourNum;
}
void ProductStatistic::setHourNum(int hour)
{
	hourNum = hour;
}

int ProductStatistic::getSelectedCount() const
{
	return selectedCount;
}
void ProductStatistic::setSelectedCount(int count)
{
	selectedCount = count;
}

int ProductStatistic::getPrintedCount() const
{
	return printedCount;
}
void ProductStatistic::setPrintedCount(int count)
{
	printedCount = count;
}

int ProductStatistic::getSelectedTime() const
{
	return selectedTime;
}
void ProductStatistic::setSelectedTime(int selTime)
{
	selectedTime = selTime;
}

HobErrorCode ProductStatistic::getErrorCode() const
{
	return errorCode;
}
void ProductStatistic::setErrorCode(HobErrorCode code)
{
	errorCode = code;
}


// Returns true if the field was updated, false if not
bool ProductStatistic::updateField(int fieldOffset, const HobString &newValue)
{
	if (Debug::isTracing("ProductStatistic"))
		Debug::trace("ProductStatistic", string("updateField() - fieldOffset ") + inttostring(fieldOffset) + "  data " + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetPSDeptNum:
			setDeptNum(Long::valueOf(newValue));
			break;
		case fieldOffsetPSProductNum:
			setProductNum(Long::valueOf(newValue));
			break;
		case fieldOffsetPSLanguage:
			setLanguage(newValue);
			break;
		case fieldOffsetPSHourNum:
			setHourNum(Integer::valueOf(newValue));
			break;
		case fieldOffsetPSSelectedCount:
			setSelectedCount(Integer::valueOf(newValue));
			break;
		case fieldOffsetPSSelectedTime:
			setSelectedTime(Integer::valueOf(newValue));
			break;
		case fieldOffsetPSPrintedCount:
			setPrintedCount(Integer::valueOf(newValue));
			break;
		case fieldOffsetPSRecordedDateTime:
		{
			HobDateTime hdt;
			hdt.convertFromDBFormat(newValue);
			setRecordedDateTime(hdt);
			break;
		}
		default:
			return false;
			break;
	}

	return true;
}


void ProductStatistic::updateRecord(const ProductStatistic *newData)
{
	if (!newData)
		return;

	if (Debug::isTracing("ProductStatistic"))
	{
		Debug::trace("ProductStatistic", string("updateRecord() - this num ") + longtostring(productNum) + string("  this DeptNum ") + longtostring(deptNum));
		Debug::trace("ProductStatistic", string("updateRecord() - passed in num ") + longtostring(newData->getProductNum()) + string("  passed in DeptNum ") + longtostring(newData->getDeptNum()));
	}

	short loopCnt = 1;
	while (loopCnt < endProductStatisticFieldOffsets)
	{
		if (newData->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetPSDeptNum:
					setDeptNum(newData->getDeptNum());
					break;
				case fieldOffsetPSProductNum:
					setProductNum(newData->getProductNum());
					break;
				case fieldOffsetPSLanguage:
					setLanguage(newData->getLanguage());
					break;
				case fieldOffsetPSHourNum:
					setHourNum(newData->getHourNum());
					break;
				case fieldOffsetPSSelectedCount:
					setSelectedCount(newData->getSelectedCount());
					break;
				case fieldOffsetPSPrintedCount:
					setPrintedCount(newData->getPrintedCount());
					break;
				case fieldOffsetPSSelectedTime:
					setSelectedTime(newData->getSelectedTime());
					break;
				case fieldOffsetPSRecordedDateTime:
					setRecordedDateTime(newData->getRecordedDateTime());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


bool ProductStatistic::isFieldQuoted(int fieldOffset) const
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

void ProductStatistic::getFieldAsString(int fieldOffset, HobString &result, int format) const
{
	result = "";

	switch (fieldOffset)
	{
		case fieldOffsetPSDeptNum:
			result = longtostring(deptNum);
			break;
		case fieldOffsetPSProductNum:
			result = longtostring(productNum);
			break;
		case fieldOffsetPSLanguage:
			result = language;
			break;
		case fieldOffsetPSHourNum:
			result = inttostring(hourNum);
			break;
		case fieldOffsetPSSelectedCount:
			result = inttostring(selectedCount);
			break;
		case fieldOffsetPSPrintedCount:
			result = inttostring(printedCount);
			break;
		case fieldOffsetPSSelectedTime:
			result = inttostring(selectedTime);
			break;
		case fieldOffsetPSRecordedDateTime:
		{
			HobDateTime hdt;
			hdt.convertFromDBFormat(recordedDateTime_);
			result = hdt.convertToDBFormat(FormatDateTime_Windux);
			break;
		}
		default:
			break;
	}

	if (Debug::isTracing("ProductStatistic.getFieldAsString"))
		Debug::trace("ProductStatistic.getFieldAsString", string("fieldOffset ") + inttostring(fieldOffset) + string("  result ") + result + string("  format ") + inttostring(format));
}

const char *ProductStatistic::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	list<ProductStatisticFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			ProductStatisticTags productStatisticTags;
			HobString tag = "";
			HobString fieldData = "";
			
			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = productStatisticTags.getTag(*fieldIter);
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
			serializedData = string("ProductStatistic::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}


void ProductStatistic::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


bool ProductStatistic::isRecordValid() const
{
	if (myDbRecord)
		return myDbRecord->isRecordValid();
	else
		return false;
}

int ProductStatistic::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBProductStatisticsRM *dbRM = dbAccess->getDBProductStatisticsRM();
		if (dbRM)
		{
			Debug::trace("ProductStatistic", string("deleteRecord() - num ") + longtostring(productNum) + "  deptNum " + longtostring(deptNum) + "  language " + language + "  hour " + inttostring(hourNum));
			result = dbRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void ProductStatistic::saveRecord()
{
	if (productNum > 0 && dbAccess)
	{
		DBProductStatisticsRM *dbRM = dbAccess->getDBProductStatisticsRM();
		if (!dbRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("ProductStatistic", string("saveRecord() - don't have a db record, creating it..."));
			DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
			if (dbProductsRM)
			{
				DBProduct *dbProductRec = dbProductsRM->findPLU(longtostring(productNum), longtostring(deptNum));
				if (dbProductRec)
				{
					myDbRecord = dbRM->createRecord(dbProductRec);
					delete dbProductRec;
				}
			}
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the DBProductStatistic record
			if (language.size())
				myDbRecord->setLanguage(language);
			myDbRecord->setHourNumber(inttostring(hourNum));
			myDbRecord->setTotalSelections(selectedCount);
			myDbRecord->setTotalLabelsPrinted(printedCount);
			myDbRecord->setTimeToSelect(selectedTime);
			myDbRecord->setRecordedDateTime(recordedDateTime_);

			try
			{
				dbRM->saveRecord(myDbRecord);		//save record to the database
				Debug::trace("ProductStatistic", string("saveRecord() - saved num ") + longtostring(productNum) + string("  deptNum ") + longtostring(deptNum) + "  language " + language + "  hour " + inttostring(hourNum));
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "ProductStatistic::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}

// static
ProductStatistic *ProductStatistic::findRecord(HobRecordNum productNum, HobRecordNum deptNum, const std::string &lang, int hour, DBAccess *dbAccess)
{
	ProductStatistic *rec = (ProductStatistic *)NULL;

	if (productNum <= 0 || deptNum < 0 || !dbAccess)
		return rec;

	DBProductStatisticsRM *dbRM = dbAccess->getDBProductStatisticsRM();
	if (!dbRM)
		return rec;

	DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
	if (!dbProductsRM)
		return rec;

	try
	{
		DBProduct *dbProductRec = dbProductsRM->findPLU(longtostring(productNum), longtostring(deptNum));
		if (dbProductRec)
		{
			DBProductStatistic *dbRec = dbRM->find(dbProductRec, lang, inttostring(hour));
			if (dbRec)
			{
				rec = new ProductStatistic(dbRec, dbAccess);
				if (!rec)
					delete dbRec;
				else
				{
					if (Debug::isTracing("ProductStatistic"))
						Debug::trace("ProductStatistic", string("findRecord() - rec ptr: ") + ptrtostring(rec) + "  num " + dbProductRec->getPLU() + "  deptNum " + dbProductRec->getDeptNumber() + "  language " + rec->getLanguage() + "  hour " + inttostring(rec->getHourNum()));
				}
			}
			delete dbProductRec;
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("ProductStatistic::findProductStatistic() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return rec;
}
