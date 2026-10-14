/* Access Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */
#include "CuttingTest.h"
#include "CuttingTestTags.h"

#include "MetSig.h"
#include "Configuration.h"
#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "HobToken.h"
#include "HobDateTime.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBCuttingTestsRM.h"
#include "DBCuttingTest.h"
#include "DBTransactionsRM.h"
#include "DBTransaction.h"
#include "DBQuery.h"

#include "Long.h"
#include "Double.h"
#include "Bool.h"
#include "Hex.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CuttingTest::_dataType_ = 0;
std::string CuttingTest::_dataTypeName_("CuttingTest"); 
#endif

FieldOffsetsList CuttingTest::_fieldList_;


CuttingTest::CuttingTest(int testNum, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();

	this->testNum = testNum;
	dbAccess = access;
}

CuttingTest::CuttingTest(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			FieldOffsetsList::iterator fieldIter = _fieldList_.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != _fieldList_.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);

				if (token.length())
				{
					if (isFieldQuoted(offset))
						token.unQuoteForCSV();
					updateField(offset, token);
				}
				fieldIter++;
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			CuttingTestTags cutTestTags;

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			int offset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp, HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
					temp = token->getTag();
					offset = (temp.size()) ? cutTestTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						temp = token->getData();
						updateField(offset, temp);
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

CuttingTest::CuttingTest(edb::DBCuttingTest *dbCuttingTestRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();

	dbAccess = access;

	if (dbCuttingTestRec)
	{
		myDbRecord = dbCuttingTestRec;

		testNum = Long::valueOf(dbCuttingTestRec->getTestNumber());
		operatorId = dbCuttingTestRec->getOperatorNumber();
		vendorNum = dbCuttingTestRec->getVendorNumber();
		productNum = dbCuttingTestRec->getProductNumber();
		primalWeight = dbCuttingTestRec->getPrimalWeight();
		primalCost = dbCuttingTestRec->getPrimalCost();

		HobDateTime tempDate;

		HobString tempDateStr = dbCuttingTestRec->getStartDate();
		tempDate.convertFromDBFormat(&tempDateStr);
		setStartDate(tempDate);

		HobDateTime tempDate2;
		tempDateStr = dbCuttingTestRec->getEndDate();
		tempDate2.convertFromDBFormat(&tempDateStr);
		setEndDate(tempDate2);

		lotNumber = dbCuttingTestRec->getLotNumber();

		tempDateStr = dbCuttingTestRec->getPackingDate();
		tempDate.convertFromDBFormat(&tempDateStr);
		setPackingDate(tempDate);

		packingHours = Integer::valueOf(dbCuttingTestRec->getPackingHours());
		packingMinutes = Integer::valueOf(dbCuttingTestRec->getPackingMinutes());
		comments = dbCuttingTestRec->getComments();
		finalComments = dbCuttingTestRec->getFinalComments();
		status = (CuttingTest::CutTestStatus) dbCuttingTestRec->getStatus();
		percentTotalShrinkage = Integer::valueOf(dbCuttingTestRec->getPercentTotalShrinkage());
		totalWeightOfCutProducts = Long::valueOf(dbCuttingTestRec->getTotalWeightOfCutProducts());
		totalPriceOfCutProducts = Long::valueOf(dbCuttingTestRec->getTotalPriceOfCutProducts());

		errorCode = RECORD_OK;
	}
}

CuttingTest::~CuttingTest()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void CuttingTest::init()
{
	testNum = RECORD_NOT_FOUND;
	operatorId = "";
	vendorNum = "";
	productNum = "";
	primalWeight = 0;
	primalCost = 0;

	lotNumber = "";
	packingHours = 0;
	packingMinutes = 0;
	comments = "";
	finalComments = "";
	status = UnknownStatus;
	percentTotalShrinkage = 0;
	totalWeightOfCutProducts = 0;
	totalPriceOfCutProducts = 0;

	errorCode = RECORD_NOT_FOUND;

	dbAccess = 0;
	myDbRecord = 0;

	initFieldList();
}

// static
void CuttingTest::initFieldList()
{
	if (_fieldList_.empty())
	{
		_fieldList_.push_back(fieldOffsetCuttingTestNum);
		_fieldList_.push_back(fieldOffsetCuttingTestOperator);
		_fieldList_.push_back(fieldOffsetCuttingTestVendorNum);
		_fieldList_.push_back(fieldOffsetCuttingTestProductNum);
		_fieldList_.push_back(fieldOffsetCuttingTestPrimalWeight);
		_fieldList_.push_back(fieldOffsetCuttingTestPrimalCost);
		_fieldList_.push_back(fieldOffsetCuttingTestStartDateTime);
		_fieldList_.push_back(fieldOffsetCuttingTestEndDateTime);
		_fieldList_.push_back(fieldOffsetCuttingTestLotNumber);
		_fieldList_.push_back(fieldOffsetCuttingTestPackingDate);
		_fieldList_.push_back(fieldOffsetCuttingTestPackingHours);
		_fieldList_.push_back(fieldOffsetCuttingTestPackingMinutes);
		_fieldList_.push_back(fieldOffsetCuttingTestComments);
		_fieldList_.push_back(fieldOffsetCuttingTestFinalComments);
		_fieldList_.push_back(fieldOffsetCuttingTestStatus);
		_fieldList_.push_back(fieldOffsetCuttingTestPercentTotalShrinkage);
		_fieldList_.push_back(fieldOffsetCuttingTestTotalWeightOfCutProducts);
		_fieldList_.push_back(fieldOffsetCuttingTestTotalPriceOfCutProducts);

		CuttingTestTags cutTestTags(&_fieldList_);
	}
}

void CuttingTest::operator=(const CuttingTest& cutTest)
{
	setTestNumber(cutTest.getTestNumber());
	setOperatorId(cutTest.getOperatorId());
	setVendorNumber(cutTest.getVendorNumber());
	setProductNumber(cutTest.getProductNumber());
	setPrimalWeight(cutTest.getPrimalWeight());
	setPrimalCost(cutTest.getPrimalCost());
	setStartDate(cutTest.getStartDate());
	setEndDate(cutTest.getEndDate());
	setLotNumber(cutTest.getLotNumber());
	setPackingDate(cutTest.getPackingDate());
	setPackingHours(cutTest.getPackingHours());
	setPackingMinutes(cutTest.getPackingMinutes());
	setComments(cutTest.getComments());
	setFinalComments(cutTest.getFinalComments());
	setStatus(cutTest.getStatus());
	setErrorCode(cutTest.getErrorCode());
	setPercentTotalShrinkage(cutTest.getPercentTotalShrinkage());
	setTotalWeightOfCutProducts(cutTest.getTotalWeightOfCutProducts());
	setTotalPriceOfCutProducts(cutTest.getTotalPriceOfCutProducts());
}


HobRecordNum CuttingTest::getTestNumber() const
{
	return testNum;
}
void CuttingTest::setTestNumber(HobRecordNum testNum)
{
	this->testNum = testNum;
}


const HobString &CuttingTest::getOperatorId() const
{
	return operatorId;
}
void CuttingTest::setOperatorId(const HobString &id)
{
	operatorId = id;
}


const HobString &CuttingTest::getVendorNumber() const
{
	return vendorNum;
}
void CuttingTest::setVendorNumber(const HobString &value)
{
	vendorNum = value;
}


const HobString &CuttingTest::getProductNumber() const
{
	return productNum;
}
void CuttingTest::setProductNumber(const HobString &value)
{
	productNum = value;
}


HobWeight CuttingTest::getPrimalWeight() const
{
	return primalWeight;
}
void CuttingTest::setPrimalWeight(HobWeight weight)
{
	primalWeight = weight;
}


HobPrice CuttingTest::getPrimalCost() const
{
	return primalCost;
}
void CuttingTest::setPrimalCost(HobPrice price)
{
	primalCost = price;
}


HobDateTime CuttingTest::getStartDate() const
{
	return startDate;
}
void CuttingTest::setStartDate(HobDateTime date)
{
	startDate = date;
}


HobDateTime CuttingTest::getEndDate() const
{
	return endDate;
}
void CuttingTest::setEndDate(HobDateTime date)
{
	endDate = date;
}


const HobString &CuttingTest::getLotNumber() const
{
	return lotNumber;
}
void CuttingTest::setLotNumber(const HobString &num)
{
	lotNumber = num;
}

HobDateTime CuttingTest::getPackingDate() const
{
	return packingDate;
}
void CuttingTest::setPackingDate(const HobDateTime date)
{
	packingDate = date;
}

int CuttingTest::getPackingHours() const
{
	return packingHours;
}
void CuttingTest::setPackingHours(int hours)
{
	packingHours = hours;
}

int CuttingTest::getPackingMinutes() const
{
	return packingMinutes;
}
void CuttingTest::setPackingMinutes(int minutes)
{
	packingMinutes = minutes;
}

const HobString &CuttingTest::getComments() const
{
	return comments;
}
void CuttingTest::setComments(const HobString &comm)
{
	comments = comm;
}

const HobString &CuttingTest::getFinalComments() const
{
	return finalComments;
}
void CuttingTest::setFinalComments(const HobString &finalComm)
{
	finalComments = finalComm;
}

CuttingTest::CutTestStatus CuttingTest::getStatus() const
{
	return status;
}
void CuttingTest::setStatus(CuttingTest::CutTestStatus stat)
{
	status = stat;
}

int CuttingTest::getPercentTotalShrinkage() const
{
	return percentTotalShrinkage;
}
void CuttingTest::setPercentTotalShrinkage(const int pct)
{
	percentTotalShrinkage = pct;
}

HobWeight CuttingTest::getTotalWeightOfCutProducts() const
{
	return totalWeightOfCutProducts;
}
void CuttingTest::setTotalWeightOfCutProducts(const HobWeight weight)
{
	totalWeightOfCutProducts = weight;
}

HobPrice CuttingTest::getTotalPriceOfCutProducts() const
{
	return totalPriceOfCutProducts;
}
void CuttingTest::setTotalPriceOfCutProducts(const HobPrice price)
{
	totalPriceOfCutProducts = price;
}


HobErrorCode CuttingTest::getErrorCode() const
{
	return errorCode;
}
void CuttingTest::setErrorCode(HobErrorCode errorCode)
{
	this->errorCode = errorCode;
}


void CuttingTest::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}
DBCuttingTest *CuttingTest::getMyDbRecord()
{
	return myDbRecord;
}


CuttingTest *CuttingTest::findCuttingTest(HobRecordNum testNum, DBAccess *dbAccess)
{
	CuttingTest *readCuttingTest = 0;

	if (testNum <= 0 || !dbAccess)
		return readCuttingTest;

	DBCuttingTestsRM *dbCuttingTestsRM = dbAccess->getDBCuttingTestsRM();
	if (!dbCuttingTestsRM)
		return readCuttingTest;

	try
	{
		DBCuttingTest *dbCuttingTestRec = dbCuttingTestsRM->find(HobString(Long::toString(testNum)));
		if (dbCuttingTestRec)
		{
			readCuttingTest = new CuttingTest(dbCuttingTestRec, dbAccess);
			if (!readCuttingTest)
				delete dbCuttingTestRec;
			else
			{
				if (Debug::isTracing("CuttingTest"))
				{
					HobString myDebug("findCuttingTest(): CuttingTest # ");
					myDebug.append(Long::toString(readCuttingTest->getTestNumber())).append(":  ");
					myDebug.append(Hex::toString((long long) readCuttingTest));
					Debug::trace("CuttingTest", myDebug);
				}
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "CuttingTest::findCuttingTest() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}

	return readCuttingTest;
}


bool CuttingTest::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int CuttingTest::deleteRecord()
{
	int result = 0;

	if (myDbRecord && dbAccess)
	{
		DBCuttingTestsRM *dbCuttingTestsRM = dbAccess->getDBCuttingTestsRM();
		if (dbCuttingTestsRM)
		{
			if (Debug::isTracing("CuttingTest"))
			{
				HobString myDebug = "deleteRecord():  Deleting CuttingTest# ";
				myDebug.append(Long::toString(testNum));
				Debug::trace("CuttingTest", myDebug);
			}
			result = dbCuttingTestsRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void CuttingTest::saveRecord()
{
	bool traceOn = Debug::isTracing("CuttingTest");
	HobString msgDebug("saveRecord():   ");

	if (!(testNum > 0))
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("Invalid testNum: ").append(Long::toString(testNum));
			Debug::trace("CuttingTest", msgDebug2);
		}
		return;
	}
	if (!dbAccess)
	{
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "dbAccess is NULL!");
		return;
	}

	DBCuttingTestsRM *dbCuttingTestsRM = dbAccess->getDBCuttingTestsRM();
	if (!dbCuttingTestsRM)
	{
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "dbCuttingTestsRM is NULL!");
		return;
	}

	if (!myDbRecord)
	{
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "Don't have a db record, creating it...");
		myDbRecord = dbCuttingTestsRM->createRecord(Long::toString(testNum));
	}

	if (myDbRecord)
	{
		HobString result;

		//	Plug the changes back into the dbCuttingTestRecord
		getFieldAsString(fieldOffsetCuttingTestNum, result);
		myDbRecord->setTestNumber(result);
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "CuttingTestNum = " + result);

		getFieldAsString(fieldOffsetCuttingTestOperator, result);
		myDbRecord->setOperatorNumber(result);
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "OperatorNum = " + result);

		myDbRecord->setVendorNumber(vendorNum);
		myDbRecord->setProductNumber(productNum);
		if (traceOn)
		{
			Debug::trace("CuttingTest", msgDebug + "VendorNum = " + vendorNum);
			Debug::trace("CuttingTest", msgDebug + "ProductNum = " + productNum);
		}

		myDbRecord->setPrimalWeight(primalWeight);
		myDbRecord->setPrimalCost(primalCost);
		if (traceOn)
		{
			getFieldAsString(fieldOffsetCuttingTestPrimalWeight, result);
			Debug::trace("CuttingTest", msgDebug + "PrimalWeight = " + result);
			getFieldAsString(fieldOffsetCuttingTestPrimalCost, result);
			Debug::trace("CuttingTest", msgDebug + "PrimalCost = " + result);
		}

		getFieldAsString(fieldOffsetCuttingTestStartDateTime, result);
		myDbRecord->setStartDate(result);
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "StartDate = " + result);

		getFieldAsString(fieldOffsetCuttingTestEndDateTime, result);
		myDbRecord->setEndDate(result);
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "EndDate = " + result);

		myDbRecord->setLotNumber(lotNumber);
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "lotNumber = " + lotNumber);

		getFieldAsString(fieldOffsetCuttingTestPackingDate, result);
		myDbRecord->setPackingDate(result);
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "PackingDate = " + result);

		getFieldAsString(fieldOffsetCuttingTestPackingHours, result);
		myDbRecord->setPackingHours(result);
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "PackingHours = " + result);

		getFieldAsString(fieldOffsetCuttingTestPackingMinutes, result);
		myDbRecord->setPackingMinutes(result);
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "PackingMinutes = " + result);

		myDbRecord->setComments(comments);
		myDbRecord->setFinalComments(finalComments);
		if (traceOn)
		{
			Debug::trace("CuttingTest", msgDebug + "Comments = " + comments);
			Debug::trace("CuttingTest", msgDebug + "FinalComments = " + finalComments);
		}

		myDbRecord->setStatus( (int)status );
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "Status = " + result);

		getFieldAsString(fieldOffsetCuttingTestPercentTotalShrinkage, result);
		myDbRecord->setPercentTotalShrinkage( result );
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "percent Total Shrinkage = " + result);

		getFieldAsString(fieldOffsetCuttingTestTotalWeightOfCutProducts, result);
		myDbRecord->setTotalWeightOfCutProducts( result );
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "total Weight Of Cut Products = " + result);

		getFieldAsString(fieldOffsetCuttingTestTotalPriceOfCutProducts, result);
		myDbRecord->setTotalPriceOfCutProducts( result );
		if (traceOn)
			Debug::trace("CuttingTest", msgDebug + "totalPriceOfCutProducts = " + result);


		try
		{
			dbCuttingTestsRM->saveRecord(myDbRecord);		//save record to the database
		}
		catch (DBException &dbe)
		{
			Debug::trace("DBAccess.exception", "CuttingTest::saveRecord() ********** database exception **********");
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}
}


const char *CuttingTest::serialize(int format) const
{
	bool traceOn = Debug::isTracing("CuttingTest");
	HobString msgDebug;
	if (traceOn)
		msgDebug = "serialize():   ";

	HobString fieldData = "";

	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
		{
			if (traceOn)
				Debug::trace("CuttingTest", "serialize(BrowserFormat)");
			FieldOffsetsList::iterator fieldIter = _fieldList_.begin();
			bool firstTime = true;
			HobString fieldData = "";
			fieldIter = _fieldList_.begin();
			while (fieldIter != _fieldList_.end())
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
		{
			if (traceOn)
			{
				if (format == EvolutionTaggedFormat)
					Debug::trace("CuttingTest", "serialize(EvolutionTaggedFormat)");
				else
					Debug::trace("CuttingTest", "serialize(DBIndependentFormatFormat)");
			}
			CuttingTestTags cutTestTags;
			HobString tag = "";

			FieldOffsetsList::iterator fieldIter = _fieldList_.begin();
			while (fieldIter != _fieldList_.end())
			{
				tag = cutTestTags.getTag(*fieldIter);
				getFieldAsString(*fieldIter, fieldData, format);

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
			serializedData = HobString("CuttingTest::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	if (traceOn)
		Debug::trace("CuttingTest", msgDebug + "serializedData = [" + serializedData + "]");
	return serializedData.c_str();
}


void CuttingTest::getFieldAsString(int fieldCode, HobString &result, int format) const
{
	bool traceOn = Debug::isTracing("CuttingTest");
	HobString myDebug("getFieldAsString():   ");
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetCuttingTestNum:
		{
			result = Long::toString(testNum);
			if (traceOn)
				myDebug.append("testNum = ");
			break;
		}
		case fieldOffsetCuttingTestOperator:
		{
			result = operatorId;
			if (traceOn)
				myDebug.append("operatorId = ");
			break;
		}
		case fieldOffsetCuttingTestPrimalWeight:
		{
			switch (format)
			{
				case EvolutionTaggedFormat:
				case DBIndependentFormatFormat:
					// Costco - use the External Weight Precision to display the number of digits after the decimal for the cut tests
					if (CutTestOption_Classic == Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled))
						result = Long::toString(primalWeight / 10);
					else
						result = Long::toString(primalWeight);
					break;
				case BrowserFormat:
				case BrowserFormatPLUOperateMode:
					Configuration::getInstance()->formatWeight(primalWeight, result);
					break;
				default:
					Configuration::getInstance()->formatWeight(primalWeight, result, false);
					break;
			}
			if (traceOn)
				myDebug.append("primalWeight = ");
			break;
		}
		case fieldOffsetCuttingTestVendorNum:
		{
			result = vendorNum;
			if (traceOn)
				myDebug.append("vendorNum = ");
			break;
		}
		case fieldOffsetCuttingTestProductNum:
		{
			result = productNum;
			if (traceOn)
				myDebug.append("productNum = ");
			break;
		}
		case fieldOffsetCuttingTestPrimalCost:
		{
			switch (format)
			{
				case EvolutionTaggedFormat:
				case DBIndependentFormatFormat:
					result = Long::toString(primalCost);
					break;
				case BrowserFormat:
				case BrowserFormatPLUOperateMode:
					Configuration::getInstance()->formatPrice(primalCost, result);
					break;
				default:
					Configuration::getInstance()->formatPrice(primalCost, result, false);
					break;
			}
			if (traceOn)
				myDebug.append("primalCost = ");
			break;
		}
		case fieldOffsetCuttingTestStartDateTime:
		{
			result = startDate.convertToDBFormat( (format == EvolutionTaggedFormat || format == DBIndependentFormatFormat) ? FormatDateTime_Windux : FormatDateTime_HTSeries );
			if (traceOn)
				myDebug.append("startDate = ");
			break;
		}
		case fieldOffsetCuttingTestEndDateTime:
		{
			result = endDate.convertToDBFormat( (format == EvolutionTaggedFormat || format == DBIndependentFormatFormat) ? FormatDateTime_Windux : FormatDateTime_HTSeries );
			if (traceOn)
				myDebug.append("endDate = ");
			break;
		}

		case fieldOffsetCuttingTestLotNumber:
		{
			result = lotNumber;
			if (traceOn)
				myDebug.append("lotNumber = ");
			break;
		}
		case fieldOffsetCuttingTestPackingDate:
		{
			result = packingDate.convertToDBFormat( (format == EvolutionTaggedFormat || format == DBIndependentFormatFormat) ? FormatDateTime_Windux : FormatDateTime_HTSeries );
			if (traceOn)
				myDebug.append("packingDate = ");
			break;
		}
		case fieldOffsetCuttingTestPackingHours:
		{
			result = Integer::toString(packingHours);
			if (traceOn)
				myDebug.append("packingHours = ");
			break;
		}
		case fieldOffsetCuttingTestPackingMinutes:
		{
			result = Integer::toString(packingMinutes);
			if (traceOn)
				myDebug.append("packingMinutes = ");
			break;
		}
		case fieldOffsetCuttingTestComments:
		{
			result = comments;
			if (traceOn)
				myDebug.append("comments = ");
			break;
		}
		case fieldOffsetCuttingTestFinalComments:
		{
			result = finalComments;
			if (traceOn)
				myDebug.append("finalComments = ");
			break;
		}
		case fieldOffsetCuttingTestStatus:
		{
			result = Integer::toString(status);
			if (traceOn)
				myDebug.append("status = ");
			break;
		}
		case fieldOffsetCuttingTestPercentTotalShrinkage:
		{
			result = Integer::toString(percentTotalShrinkage);
			if (traceOn)
				myDebug.append("percentTotalShrinkage = ");
			break;
		}
		case fieldOffsetCuttingTestTotalWeightOfCutProducts:
		{
			result = Long::toString(totalWeightOfCutProducts);
			if (traceOn)
				myDebug.append("totalWeightOfCutProducts = ");
			break;
		}
		case fieldOffsetCuttingTestTotalPriceOfCutProducts:
		{
			result = Long::toString(totalPriceOfCutProducts);
			if (traceOn)
				myDebug.append("totalPriceOfCutProducts = ");
			break;
		}

#if 0
		case fieldOffsetCuttingTestErrorCode:
		{
			result = Long::toString(errorCode);
			if (traceOn)
				myDebug.append("errorCode = ");
			break;
		}
#endif
		default:
			result = "";
			break;
	}
	if (traceOn)
	{
		myDebug.append(result);
		Debug::trace("CuttingTest", myDebug);
	}
}

bool CuttingTest::isFieldQuoted(int fieldOffset) const
{
	switch (fieldOffset)
	{
		case fieldOffsetCuttingTestOperator:
		case fieldOffsetCuttingTestVendorNum:
		case fieldOffsetCuttingTestStartDateTime:
		case fieldOffsetCuttingTestEndDateTime:
		case fieldOffsetCuttingTestLotNumber:
		case fieldOffsetCuttingTestPackingDate:
		case fieldOffsetCuttingTestComments:
		case fieldOffsetCuttingTestFinalComments:
			return true;
			break;

		default:
			break;
	}
	return false;
}


void CuttingTest::updateField(int fieldCode, const HobString &token)
{
	Debug::trace("CuttingTest", HobString("updateField() - fieldCode: ") + HobString(Integer::toString(fieldCode)) + HobString("  data: ") + token);

	switch (fieldCode)
	{
		case fieldOffsetCuttingTestNum:
			setTestNumber((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetCuttingTestOperator:
			setOperatorId(token);
			break;
		case fieldOffsetCuttingTestVendorNum:
			setVendorNumber(token);
			break;
		case fieldOffsetCuttingTestProductNum:
			setProductNumber(token);
			break;
		case fieldOffsetCuttingTestPrimalWeight:
		{
			setPrimalWeight( MetSig::determineImplicitDecimalWeight(token) );
			break;
		}
		case fieldOffsetCuttingTestPrimalCost:
		{
			setPrimalCost( MetSig::determineImplicitDecimalPrice(token) );
			break;
		}
		case fieldOffsetCuttingTestStartDateTime:
		{
			HobDateTime tempDate;
			HobString tempStr = token;
			tempDate.convertFromDBFormat(&tempStr);
			setStartDate(tempDate);
			break;
		}
		case fieldOffsetCuttingTestEndDateTime:
		{
			HobDateTime tempDate;
			HobString tempStr = token;
			tempDate.convertFromDBFormat(&tempStr);
			setEndDate(tempDate);
			break;
		}
		case fieldOffsetCuttingTestLotNumber:
		{
			setLotNumber(token);
			break;
		}
		case fieldOffsetCuttingTestPackingDate:
		{
			HobDateTime tempDate;
			HobString tempStr = token;
			tempDate.convertFromDBFormat(&tempStr);
			setPackingDate(tempDate);
			break;
		}
		case fieldOffsetCuttingTestPackingHours:
		{
			int newHours = Integer::valueOf(token);
			setPackingHours(newHours);
			break;
		}
		case fieldOffsetCuttingTestPackingMinutes:
		{
			int newMinutes = Integer::valueOf(token);
			setPackingMinutes(newMinutes);
			break;
		}
		case fieldOffsetCuttingTestComments:
		{
			setComments(token);
			break;
		}
		case fieldOffsetCuttingTestFinalComments:
		{
			setFinalComments(token);
			break;
		}
		case fieldOffsetCuttingTestStatus:
		{
			int stat = Integer::valueOf(token);
			setStatus((CuttingTest::CutTestStatus) stat);
			break;
		}
		case fieldOffsetCuttingTestPercentTotalShrinkage:
		{
			int pct = Integer::valueOf(token);
			setPercentTotalShrinkage(pct);
			break;
		}
		case fieldOffsetCuttingTestTotalWeightOfCutProducts:
		{
			HobWeight weight = Long::valueOf(token);
			setTotalWeightOfCutProducts(weight);
			break;
		}
		case fieldOffsetCuttingTestTotalPriceOfCutProducts:
		{
			HobPrice price = Integer::valueOf(token);
			setTotalPriceOfCutProducts(price);
			break;
		}

		default:
			break;
	}
}


void CuttingTest::update(const CuttingTest *newCuttingTest)
{
	if (!newCuttingTest)
		return;

	short loopCnt = 1;
	while (loopCnt < endCuttingTestFieldOffsets)
	{
		if (newCuttingTest->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetCuttingTestNum:
					setTestNumber(newCuttingTest->getTestNumber());
					break;
				case fieldOffsetCuttingTestOperator:
					setOperatorId(newCuttingTest->getOperatorId());
					break;
				case fieldOffsetCuttingTestVendorNum:
					setVendorNumber(newCuttingTest->getVendorNumber());
					break;
				case fieldOffsetCuttingTestProductNum:
					setProductNumber(newCuttingTest->getProductNumber());
					break;
				case fieldOffsetCuttingTestPrimalWeight:
					setPrimalWeight(newCuttingTest->getPrimalWeight());
					break;
				case fieldOffsetCuttingTestPrimalCost:
					setPrimalCost(newCuttingTest->getPrimalCost());
					break;
				case fieldOffsetCuttingTestStartDateTime:
					setStartDate(newCuttingTest->getStartDate());
					break;
				case fieldOffsetCuttingTestEndDateTime:
					setEndDate(newCuttingTest->getEndDate());
					break;
				case fieldOffsetCuttingTestLotNumber:
					setLotNumber(newCuttingTest->getLotNumber());
					break;
				case fieldOffsetCuttingTestPackingDate:
					setPackingDate(newCuttingTest->getPackingDate());
					break;
				case fieldOffsetCuttingTestPackingHours:
					setPackingHours(newCuttingTest->getPackingHours());
					break;
				case fieldOffsetCuttingTestPackingMinutes:
					setPackingMinutes(newCuttingTest->getPackingMinutes());
					break;
				case fieldOffsetCuttingTestComments:
					setComments(newCuttingTest->getComments());
					break;
				case fieldOffsetCuttingTestFinalComments:
					setFinalComments(newCuttingTest->getFinalComments());
					break;
				case fieldOffsetCuttingTestStatus:
					setStatus(newCuttingTest->getStatus());
					break;
				case fieldOffsetCuttingTestPercentTotalShrinkage:
					setPercentTotalShrinkage(newCuttingTest->getPercentTotalShrinkage());
					break;
				case fieldOffsetCuttingTestTotalWeightOfCutProducts:
					setTotalWeightOfCutProducts(newCuttingTest->getTotalWeightOfCutProducts());
					break;
				case fieldOffsetCuttingTestTotalPriceOfCutProducts:
					setTotalPriceOfCutProducts(newCuttingTest->getTotalPriceOfCutProducts());
					break;
				default:
					break;
					}
				}
		loopCnt++;
	}
}


void CuttingTest::show()
{
	if (Debug::isTracing("CuttingTest"))
	{
		HobString result;
		HobString msgDebug("show():  ");

		getFieldAsString(fieldOffsetCuttingTestNum, result);
		Debug::trace("CuttingTest", msgDebug + " CuttingTest # " + result);

		getFieldAsString(fieldOffsetCuttingTestOperator, result);
		Debug::trace("CuttingTest", msgDebug + "    Operator # " + result);
		Debug::trace("CuttingTest", msgDebug + "      Vendor # " + vendorNum);
		Debug::trace("CuttingTest", msgDebug + "     Product # " + productNum);

		getFieldAsString(fieldOffsetCuttingTestPrimalWeight, result);
		Debug::trace("CuttingTest", msgDebug + " Primal weight: " + result);

		getFieldAsString(fieldOffsetCuttingTestPrimalCost, result);
		Debug::trace("CuttingTest", msgDebug + "   Primal cost: " + result);

		getFieldAsString(fieldOffsetCuttingTestStartDateTime, result);
		Debug::trace("CuttingTest", msgDebug + "    Start date: " + result);

		getFieldAsString(fieldOffsetCuttingTestEndDateTime, result);
		Debug::trace("CuttingTest", msgDebug + "      End date: " + result);

		getFieldAsString(fieldOffsetCuttingTestLotNumber, result);
		Debug::trace("CuttingTest", msgDebug + "     LotNumber: " + result);

		getFieldAsString(fieldOffsetCuttingTestPackingDate, result);
		Debug::trace("CuttingTest", msgDebug + "  Packing Date: " + result);

		getFieldAsString(fieldOffsetCuttingTestPackingHours, result);
		Debug::trace("CuttingTest", msgDebug + " Packing Hours: " + result);

		getFieldAsString(fieldOffsetCuttingTestPackingMinutes, result);
		Debug::trace("CuttingTest", msgDebug + "PackingMinutes: " + result);

		getFieldAsString(fieldOffsetCuttingTestComments, result);
		Debug::trace("CuttingTest", msgDebug + "      Comments: " + result);

		getFieldAsString(fieldOffsetCuttingTestFinalComments, result);
		Debug::trace("CuttingTest", msgDebug + "Final Comments: " + result);

		getFieldAsString(fieldOffsetCuttingTestStatus, result);
		Debug::trace("CuttingTest", msgDebug + "        Status: " + result);

		getFieldAsString(fieldOffsetCuttingTestPercentTotalShrinkage, result);
		Debug::trace("CuttingTest", msgDebug + "Percent Total Shrinkage: " + result);

		getFieldAsString(fieldOffsetCuttingTestTotalWeightOfCutProducts, result);
		Debug::trace("CuttingTest", msgDebug + "Total Weight Of Cut Products: " + result);

		getFieldAsString(fieldOffsetCuttingTestTotalPriceOfCutProducts, result);
		Debug::trace("CuttingTest", msgDebug + "Total Price Of Cut Products: " + result);

	}
}


// static
CuttingTest *CuttingTest::find(HobRecordNum testNum, DBAccess *dbAccess)
{
	CuttingTest *readCuttingTest = 0;

	if (testNum <= 0 || !dbAccess)
		return readCuttingTest;

	DBCuttingTestsRM *dbCuttingTestsRM = dbAccess->getDBCuttingTestsRM();
	if (!dbCuttingTestsRM)
		return readCuttingTest;

	try
	{
		DBCuttingTest *dbCuttingTestRec = dbCuttingTestsRM->find(HobString(Long::toString(testNum)));
		if (dbCuttingTestRec)
		{
			readCuttingTest = new CuttingTest(dbCuttingTestRec, dbAccess);
			if (!readCuttingTest)
				delete dbCuttingTestRec;
			else
			{
				if (Debug::isTracing("CuttingTest"))
					Debug::trace("CuttingTest", HobString("    CuttingTest: ") + HobString(Hex::toString((long long) readCuttingTest)) +
													HobString("  CuttingTest number:") + HobString(Long::toString(testNum)));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "CuttingTest::find() ********** database exception **********");
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return readCuttingTest;
}


void CuttingTest::updateCutTestTotalsOption1()
{
	bool traceOn = Debug::isTracing("CuttingTest");
	HobString myDebug("updateCutTestTotalsOption1():  ");

	if (!dbAccess)
		return;

	percentTotalShrinkage = 0;
	totalWeightOfCutProducts = 0;
	totalPriceOfCutProducts = 0;

	try
	{
		DBTransactionsRM* dbTransactionsRM = dbAccess->getDBTransactionsRM();
		if ( dbTransactionsRM )
		{
			DBQuery<DBTransaction> *cutTestTotalsQuery = dbTransactionsRM->retrieveCutTestTransactionsForCutTest(Long::toString(testNum));

			// If couldn't get the query from which to access the CuttingTests, just assume there aren't any transaction records for this cut test.
			if (cutTestTotalsQuery)
			{
				DBQuery<DBTransaction>::const_iterator *transIter;
				DBTransaction *dbTransaction = 0;

				DBQuery<DBTransaction>::const_iterator tempIter = cutTestTotalsQuery->begin();
				while ( tempIter != cutTestTotalsQuery->end() )
				{
					transIter = new DBQuery<DBTransaction>::const_iterator(tempIter);
					if (transIter)
					{
						DBTransaction *trans = new DBTransaction(*(*transIter));
						if (trans)
						{
							if ( !trans->isFinalNetWeightNull() )
								totalWeightOfCutProducts += trans->getFinalNetWeight();

							if ( !trans->isFinalPriceNull() )
								totalPriceOfCutProducts += trans->getFinalPrice();

							if ( primalWeight > 0 )
							{
								int pct = (int) ( (double)(trans->getFinalNetWeight()) / primalWeight * 10000.0 );
								trans->setPercentOfPrimalWeight( pct );
							}
							if ( primalCost > 0 )
							{
								int pct = (int) ( (double)(trans->getFinalPrice()) / primalCost * 10000.0 );
								trans->setPercentOfPrimalCost( pct );
							}
							dbTransactionsRM->saveRecord(trans);

							delete trans;
						}
						delete transIter;
					}
					++tempIter;
				}
				delete cutTestTotalsQuery;
				cutTestTotalsQuery = 0;
			}
		}
	}

	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "CutTestDetailsOption1::createDetailsOption1() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}

	if ( primalWeight > 0 )
		percentTotalShrinkage = (long long) ((double)(primalWeight - totalWeightOfCutProducts) / primalWeight * 10000.0);
}
