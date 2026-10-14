/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductTextCfg.h"
#include "ProductTextCfgTags.h"
#include "Configuration.h"
#include "Device.h"
#include "Printer.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "util/e_string/utilstring.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBProductTextCfgsRM.h"


using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductTextCfg::dataType = 0;
string ProductTextCfg::dataTypeName("ProductTextCfg"); 
#endif

std::list<ProductTextCfgFieldOffsets> ProductTextCfg::fieldList;      // list<ProductTextCfgFieldOffsets> ProductTextCfg::fieldList;

ProductTextCfg::ProductTextCfg(ProdSupportTextType type)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	textType_ = type;
}

ProductTextCfg::ProductTextCfg(const char *data, int len, int formatType)
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
					case fieldOffsetPTCTextType:
						setTextType((ProdSupportTextType)stringtoint(token));
						break;
					case fieldOffsetPTCPrinterNum:
						setPrinterNum((ProductTextPrinterChoices)stringtoint(token));
						break;
					case fieldOffsetPTCLabelNum:
						setLabelNum((HobRecordNum)stringtolong(token));
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
			ProductTextCfgTags productTextCfgTags;
			
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
					offset = (temp.size()) ? productTextCfgTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						temp = token->getData();
						switch (offset)
						{
							case fieldOffsetPTCTextType:
							  	setTextType((ProdSupportTextType)stringtoint(temp));
								break;
							case fieldOffsetPTCPrinterNum:
							  	setPrinterNum((ProductTextPrinterChoices)stringtoint(temp));
								break;
							case fieldOffsetPTCLabelNum:
							  	setLabelNum((HobRecordNum)stringtolong(temp));
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

ProductTextCfg::ProductTextCfg(edb::DBProductTextCfg *dbProductTextCfgRec, DBAccess *access)
{
	init();
	dbAccess_ = access;

	if (dbProductTextCfgRec)
	{
		myDbRecord_ = dbProductTextCfgRec;

		textType_ = (ProdSupportTextType) stringtoint(dbProductTextCfgRec->getTextType());
		printerNum_ = (ProductTextPrinterChoices) stringtoint(dbProductTextCfgRec->getPrinterNum());
		labelNum_ = (HobRecordNum) stringtoint(dbProductTextCfgRec->getLabelNum());

		errorCode_ = RECORD_OK;
	}
	else
		Debug::trace("ProductTextCfg", HobString("Constructor passing in a db - dbrec IS NULL"));
}


ProductTextCfg::~ProductTextCfg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord_)
		delete myDbRecord_;

	if (Debug::isTracing("ProductTextCfg"))
		Debug::trace("ProductTextCfg", string(" Text: ") + ptrtostring(this) + string("  Text type:") + inttostring((int)textType_));
}


void ProductTextCfg::init()
{
	textType_ = psttUnknown;
	printerNum_ = UnspecifiedPrinter;
	labelNum_ = Configuration::getInstance()->toIntCfgI(CfgInternal::PRODUCTSUPPORTTEXTLABELNUM);
	dbAccess_ = 0;
	myDbRecord_ = 0;

	errorCode_ = RECORD_NOT_FOUND;

	initFieldList();
}


ProductTextCfg *ProductTextCfg::findProductTextCfg(ProdSupportTextType textType, DBAccess *dbAccess)
{
	ProductTextCfg *foundRec = 0;

	if (textType <= psttUnknown || textType >= endProdSupportTextType || !dbAccess)
		return foundRec;

	DBProductTextCfgsRM	*myRM = dbAccess->getDBProductTextCfgsRM();
	if (!myRM)
		return foundRec;

	try
	{
		DBProductTextCfg *dbRec = myRM->findNumber(textType);
		if (dbRec)
		{
			if (Debug::isTracing("ProductTextCfg"))
				Debug::trace("ProductTextCfg", "findProductTextCfg() - read the record from the db, textType:" + dbRec->getTextType());
			foundRec = new ProductTextCfg(dbRec, dbAccess);
			if (!foundRec)
				delete dbRec;
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("ProductTextCfg::findProductTextCfg() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return foundRec;
}


void ProductTextCfg::saveRecord()
{
	if (textType_ > psttUnknown && textType_ < endProdSupportTextType && dbAccess_)   //No need to save to the Database if the number is 0
	{
		DBProductTextCfgsRM *myRM = dbAccess_->getDBProductTextCfgsRM();
		if (!myRM)
			return;

		if (!myDbRecord_)
		{
			//	This must be a new record for the Database
			Debug::trace("ProductTextCfg", HobString("saveRecord() - don't have a db record, creating it...  Text type:") + inttostring((int)textType_));
			myDbRecord_ = myRM->createRecord(textType_);
		}
		
		if (myDbRecord_)
		{
			myDbRecord_->setTextType(inttostring((int)textType_));
			myDbRecord_->setPrinterNum(inttostring((int)printerNum_));
			myDbRecord_->setLabelNum(longtostring(labelNum_));

			Debug::trace("ProductTextCfg", HobString("Saving  textType:") + inttostring((int)textType_));

			try
			{
				myRM->saveRecord(myDbRecord_);
				Debug::trace("ProductTextCfg", "Saved in the database");
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "ProductTextCfg::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + inttostring(t) + HobString("  info:") + dbe.info());
			}
		}
	}
}


int ProductTextCfg::deleteRecord()
{
	int result = 0;
	if (myDbRecord_ && dbAccess_)   //No need to delete if there is no record
	{
		DBProductTextCfgsRM *myRM = dbAccess_->getDBProductTextCfgsRM();
		if (myRM)
		{
			Debug::trace("ProductText", HobString("Deleting ProductTextCfg: ") + ptrtostring(this) + string("  textType:") + inttostring((int)textType_));
			result = myRM->deleteRecord(myDbRecord_);
		}
	}
	return result;
}


void ProductTextCfg::updateProductTextCfg(ProductTextCfg *newRec)
{
	if (!newRec)
		return;

	short loopCnt = 1;
	while (loopCnt < endProductTextCfgFieldOffsets)
	{
		if (newRec->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetPTCTextType:
					setTextType(newRec->getTextType());
					break;
				case fieldOffsetPTCPrinterNum:
					setPrinterNum(newRec->getPrinterNum());
					break;
				case fieldOffsetPTCLabelNum:
					setLabelNum(newRec->getLabelNum());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


const char *ProductTextCfg::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
			for (int loopCnt = 1; loopCnt < endTextFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetPTCTextType:
						serializedData.append(inttostring((int)textType_));
						break;
					case fieldOffsetPTCPrinterNum:
						serializedData.append(inttostring((int)printerNum_));
						break;
					case fieldOffsetPTCLabelNum:
						serializedData.append(longtostring(labelNum_));
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
			ProductTextCfgTags productTextCfgTags;
			HobString tag = "";
			HobString fieldData = "";

			std::list<ProductTextCfgFieldOffsets>::iterator fieldIter = fieldList.begin();
			while (fieldIter != fieldList.end())
			{
				tag = productTextCfgTags.getTag(*fieldIter);
				switch (*fieldIter)
				{
					case fieldOffsetPTCTextType:
						fieldData = inttostring((int)textType_);
						break;
					case fieldOffsetPTCPrinterNum:
						fieldData = inttostring((int)printerNum_);
						break;
					case fieldOffsetPTCLabelNum:
						fieldData = longtostring(labelNum_);
						break;
					default:
					{
						tag = "";
						fieldData = "";
						break;
					}
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
			serializedData = HobString("ProductTextCfg::serialize - unknown format:") + inttostring(format);
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


// static
void ProductTextCfg::initFieldList()
{
	if (fieldList.empty())
	{
		fieldList.push_back(fieldOffsetPTCTextType);	
		fieldList.push_back(fieldOffsetPTCPrinterNum);
		fieldList.push_back(fieldOffsetPTCLabelNum);

		ProductTextCfgTags productTextCfgTags((FieldOffsetsList *)&fieldList);
	}
}

// static
void ProductTextCfg::verifyDefaultRecords(DBAccess *dbAccess)
{
	if (!dbAccess)
	{
		Debug::trace("ProductTextCfg", "verifyDefaultRecords - no dbAccess passed in!");
		return;
	}

	Debug::trace("ProductTextCfg", "verifying all default ProductTextCfgs ...");

	HobRecordNum defaultLabelNum = (HobRecordNum)Configuration::getInstance()->toIntCfgI(CfgInternal::PRODUCTSUPPORTTEXTLABELNUM);
	verifyDefaultRecord(psttExpandedText, defaultLabelNum, dbAccess);
	verifyDefaultRecord(psttSpecialMessage, defaultLabelNum, dbAccess);
	verifyDefaultRecord(psttProductNote, defaultLabelNum, dbAccess);
	verifyDefaultRecord(psttMarqueeText, defaultLabelNum, dbAccess);
	verifyDefaultRecord(psttProductText5, defaultLabelNum, dbAccess);
	verifyDefaultRecord(psttProductText6, defaultLabelNum, dbAccess);
	verifyDefaultRecord(psttProductText7, defaultLabelNum, dbAccess);
	verifyDefaultRecord(psttProductText8, defaultLabelNum, dbAccess);
	verifyDefaultRecord(psttProductText9, defaultLabelNum, dbAccess);
	defaultLabelNum = (HobRecordNum)Configuration::getInstance()->toIntCfgI(CfgInternal::PRODUCTUINUTRILABELNUM);
	verifyDefaultRecord(psttNutrifacts, defaultLabelNum, dbAccess);
}


void ProductTextCfg::verifyDefaultRecord(ProdSupportTextType type, HobRecordNum labelNum, DBAccess *dbAccess)
{
	ProductTextCfg *defaultPTC = 0;

	defaultPTC = findProductTextCfg(type, dbAccess);
	if (!defaultPTC)
	{
		Debug::trace("ProductTextCfg", "creating default ProductTextCfg - type:" + inttostring((int)type));
		defaultPTC = new ProductTextCfg(type);
		if (defaultPTC)
		{
			defaultPTC->setLabelNum(labelNum);
			defaultPTC->setPrinterNum(PrinterChoice_PrimaryPrinter);
			defaultPTC->setDBAccess(dbAccess);
			defaultPTC->saveRecord();
		}
	}
	if (defaultPTC)
	{
		delete defaultPTC;
		defaultPTC = 0;
	}
}


//static
// This method checks each of the default ProductTextCfg records to make sure they specify a valid printer number.
// If an external printer gets deleted, or it becomes a network report printer (network printer other than Zebra),
// then a ProductTextCfg record cannot be set to this printer number.  So automatically set it back to the primary printer.
//
// Returns true if any ProductTextCfg record was changed as a result of the validation.

bool ProductTextCfg::validateDefaultRecords(DBAccess *dbAccess)
{
	bool secondPrinterIsValid = true, thirdPrinterIsValid = true;

	Device *currentDev = Device::findDevice((int)Printer, SecondPrinter, dbAccess);
	if ( !currentDev || ( !currentDev->isARealPrinter() || currentDev->isANetworkReportPrinter() ) )
		secondPrinterIsValid = false;
	if (currentDev)
	{
		delete currentDev;
		currentDev = 0;
	}

	currentDev = Device::findDevice((int)Printer, ThirdPrinter, dbAccess);
	if ( !currentDev || ( !currentDev->isARealPrinter() || currentDev->isANetworkReportPrinter() ) )
		thirdPrinterIsValid = false;
	if (currentDev)
	{
		delete currentDev;
		currentDev = 0;
	}

	if ( secondPrinterIsValid && thirdPrinterIsValid )
		// No need to check anything if both external printers are of the type that can print the Product Text Support records.
		return false;
	else
	{
		bool result = false;

		if ( validateDefaultRecord(psttExpandedText,   secondPrinterIsValid, thirdPrinterIsValid, dbAccess) ) result = true;
		if ( validateDefaultRecord(psttSpecialMessage, secondPrinterIsValid, thirdPrinterIsValid, dbAccess) ) result = true;
		if ( validateDefaultRecord(psttProductNote,    secondPrinterIsValid, thirdPrinterIsValid, dbAccess) ) result = true;
		if ( validateDefaultRecord(psttMarqueeText,    secondPrinterIsValid, thirdPrinterIsValid, dbAccess) ) result = true;
		if ( validateDefaultRecord(psttProductText5,   secondPrinterIsValid, thirdPrinterIsValid, dbAccess) ) result = true;
		if ( validateDefaultRecord(psttProductText6,   secondPrinterIsValid, thirdPrinterIsValid, dbAccess) ) result = true;
		if ( validateDefaultRecord(psttProductText7,   secondPrinterIsValid, thirdPrinterIsValid, dbAccess) ) result = true;
		if ( validateDefaultRecord(psttProductText8,   secondPrinterIsValid, thirdPrinterIsValid, dbAccess) ) result = true;
		if ( validateDefaultRecord(psttProductText9,   secondPrinterIsValid, thirdPrinterIsValid, dbAccess) ) result = true;
		if ( validateDefaultRecord(psttNutrifacts,     secondPrinterIsValid, thirdPrinterIsValid, dbAccess) ) result = true;

		return result;
	}
}

bool ProductTextCfg::validateDefaultRecord(ProdSupportTextType type, bool secondPrinterIsValid, bool thirdPrinterIsValid, DBAccess *dbAccess)
{
	bool result = false;
	ProductTextCfg *defaultPTC = findProductTextCfg(type, dbAccess);
	if (defaultPTC)
	{
		Debug::trace("ProductTextCfg", "validating default ProductTextCfg - type:" + inttostring((int)type));
		if ( ( !secondPrinterIsValid && defaultPTC->getPrinterNum() == PrinterChoice_SecondPrinter ) ||
			 ( !thirdPrinterIsValid && defaultPTC->getPrinterNum() == PrinterChoice_ThirdPrinter ) )
		{
			Debug::trace("ProductTextCfg", "printer number is invalid, setting to Primary Printer");
			defaultPTC->setPrinterNum(PrinterChoice_PrimaryPrinter);
			defaultPTC->saveRecord();
			result = true;
		}
		delete defaultPTC;
		defaultPTC = 0;
	}
	return result;
}


HobErrorCode ProductTextCfg::getErrorCode() const
{
	return errorCode_;
}
void ProductTextCfg::setErrorCode(HobErrorCode code)
{
	errorCode_ = code;
}


