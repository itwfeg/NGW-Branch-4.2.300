/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include <math.h>



#include "BarCodeNumber.h"
#include "Configuration.h"
#include "Product.h"
#include "ProductDefault.h"
#include "ProductTransaction.h"
#include "LabelType.h"
#include "HobDateTime.h"
#include "HfcWeigherStatus.h"

#include "Session.h"
#include "SessionManager.h"
#include "ProductTotalManager.h"

#include "DBAccess.h"
#include "DBProduct.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"

#include "drivers/HobBarCodeFormatter.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int BarCodeNumber::dataType = 0;
std::string BarCodeNumber::dataTypeName("BarCodeNumber"); 
#endif


int BarCodeNumber::EAN5WeightedProduct[10] = { 0, 5, 9, 4, 8, 3, 7, 2, 6, 1 };

//
// NOTE NOTE NOTE
// Are you updating this table?? Also update reduceGS1BarcodeString() min max table.
//
const std::string BarCodeNumber::rss14_AI_01_("(01)");         // GTIN value
const std::string BarCodeNumber::rss14_AI_15_ = "(15)";        // 'B' (Best Before)
const std::string BarCodeNumber::rss14_AI_16_ = "(16)";        // 'C' (Sell By)                // SOF-861
const std::string BarCodeNumber::rss14_AI_30_ = "(30)";        // 'Q' (By count quantity)

const std::string BarCodeNumber::rss14_AI_3922_("(3922)");     // 'P' (Price)
const std::string BarCodeNumber::rss14_AI_422_("(422)");       // Country of irigin of trade item - see https://www.gs1.org/standards/barcodes/
const std::string BarCodeNumber::rss14_AI_423_("(423)");       // Country of initial processing   - 
const std::string BarCodeNumber::rss14_AI_424_("(424)");       // Country of processing
const std::string BarCodeNumber::rss14_AI_425_("(425)");       // Country of disassembly
const std::string BarCodeNumber::rss14_AI_426_("(426)");       // Country covering full process chain
const std::string BarCodeNumber::rss14_AI_3103_("(3103)");    // 'W' (Metric Weight)
const std::string BarCodeNumber::rss14_AI_3202_("(3202)");    // 'W' (Non-Metric Weight)
const std::string BarCodeNumber::rss14_AI_3203_("(3203)");    // 'W' (Non-Metric Weight, dual-range)
const std::string BarCodeNumber::rss14_AI_8008_("(8008)");    // 'D' (Date and Time of Production)
const std::string BarCodeNumber::rss14_AI_("WWWWWWPPPPPP");;


BarCodeNumber::BarCodeNumber()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

BarCodeNumber::BarCodeNumber( const BarCodeNumber &obj ) :
	barcodeType(obj.barcodeType),
	relevantFields(obj.relevantFields),
	dbPrefix(obj.dbPrefix),
	upcPrefix(obj.upcPrefix),
	dbEanCode(obj.dbEanCode),
	dbVendorNumber(obj.dbVendorNumber),
	dbProductNumber(obj.dbProductNumber),
	rawBarCodeString(obj.rawBarCodeString), 
	mostSignificantUPCDigitReplacement(obj.mostSignificantUPCDigitReplacement),
	swappingOfEanCodeAndProductNumberAllowed(obj.swappingOfEanCodeAndProductNumberAllowed),
	isDPEnabled(obj.isDPEnabled),
	originalUnitPrice(obj.originalUnitPrice),
	currentDateTimeStr(obj.currentDateTimeStr),
	formats(obj.formats),
	barcodeTypeDeterminant(obj.barcodeTypeDeterminant)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	for (int i = 0; i < MAX_BARCODE_TYPES; i++)
	{
		formatStrings[i] = obj.getFormatString((BarcodeType)i);
		barcodeStrings[i] = obj.getBarcodeString((BarcodeType)i);
	}

}

BarCodeNumber::~BarCodeNumber()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void BarCodeNumber::init()
{
	barcodeType = UPCA;
	barcodeTypeDeterminant = UPCA;

	relevantFields = UseEanCode | UseVendorNumber | UseProductNumber; //UseNoFields;

	upcPrefix = "2";
	dbPrefix = "";
	dbEanCode = "";
	dbVendorNumber = "";
	dbProductNumber = "";

	for (int i = 0; i < MAX_BARCODE_TYPES; i++)
	{
		formatStrings[i] = "";
		barcodeStrings[i] = "";
	}

	rawBarCodeString = "";

	mostSignificantUPCDigitReplacement = "";

	swappingOfEanCodeAndProductNumberAllowed = false;
	isDPEnabled = false;

	originalUnitPrice = -1;
}


void BarCodeNumber::getFromDatabase(edb::DBProduct *dbProductRec, Product *product, DBAccess *dbAccess)
{
	if ( !product )
		return;

	Debug::trace("BarCode", "getFromDatabase: dbProductRec object is " + util_string::ptrtostring(dbProductRec) +
		" and product object is " + util_string::ptrtostring(product) + ", the barcodeNumber (before setting up) follows:");
	show();

	if (product)
	{
		//	First, get the barcode format strings for this product type
		ProductDefault *productDefault = ProductDefault::findRecord(product->getProductType(), product->getDeptNum(), dbAccess);
		if (productDefault)
		{
			setFormatStrings(productDefault);
			delete productDefault;
		}
	}

	if (dbProductRec)
	{
		rawBarCodeString = dbProductRec->isRawBarCodeNumberNull() ? "" : dbProductRec->getRawBarCodeNumber();

		dbPrefix = dbProductRec->isBarCodePrefixNull() ? "" : inttostring(dbProductRec->getBarCodePrefix());
		HobString dbString = getFormattedString(dbPrefix, 1);
		unsigned int numberSys = Integer::valueOf(dbString);
		if (product)
		{
			if ( (product->isRandomWeight() || product->isByCount()) &&  numberSys != 2 && numberSys != 4 )
			{
				numberSys = 2;
				Debug::trace("BarCode", "setUPCStrings() - forced upc number system to be 2!");
			}
			else if ( (product->isFixedWeight() || product->isFluidOunce()) && (numberSys == 3 || numberSys == 5 || numberSys < 0 || numberSys > 9) )
			{
				numberSys = 0;
				Debug::trace("BarCode", "setUPCStrings() - forced upc number system to be 0!");
			}
		}
		upcPrefix = Integer::toString(numberSys);

		// We will always be taking the right-most digits of these two fields, and since different barcodes
		// could specify a different number of digits to print, we set them up zero-filled on the left.

		dbVendorNumber = dbProductRec->isBarCodeVendorNumberNull() ? "" : inttostring(dbProductRec->getBarCodeVendorNumber());

		dbProductNumber = dbProductRec->isBarCodeProductNumberNull() ? "" : inttostring(dbProductRec->getBarCodeProductNumber());

		dbEanCode = dbProductRec->isBarCodeEANCodeNull() ? "" : dbProductRec->getBarCodeEANCode();

		determineBarcodeTypeDeterminant(product);
	}

	if ( dbProductRec && originalUnitPrice == -1 )
	{
		// original pricing
		originalUnitPrice = (HobPrice) dbProductRec->getPrice();
	}

	Debug::trace("BarCode", "getFromDatabase: after 'getting' the barcodeNumber is as follows:");
	show();
}


void BarCodeNumber::determineBarcodeTypeDeterminant(Product *product)
{
	if (product)
	{
		short printerNum = FirstPrinter;
		LabelType *myLabelType = (LabelType *)NULL;

		short bcType = RECORD_NOT_FOUND;
		while (printerNum < MaxPrinterTypes)
		{
			myLabelType = product->getLabelType(printerNum);
			if (myLabelType && myLabelType->isRecordValid() )
			{
				bcType = myLabelType->determineSpecializedBarcodeType();
				if (bcType != RECORD_NOT_FOUND)
					break;
			}
			printerNum++;
		}

		barcodeTypeDeterminant = (bcType == RECORD_NOT_FOUND) ? (short)UPCA : bcType;
		this->barcodeType = barcodeTypeDeterminant;
	}
}

void BarCodeNumber::setDiscountPriceOnBarcode(bool value)
{
	// right now, this is reset to false if the label changes.
	isDPEnabled = value;
}

void BarCodeNumber::setFormatStrings(ProductDefault* productDefault)
{
	bool isTracing = Debug::isTracing("BarCode");
	if (isTracing)
		Debug::trace("BarCode", "setFormatStrings");

	if (productDefault)
	{
		if (formatStrings[EAN13].empty())
			setFormatString(EAN13, productDefault->getFormatString(EAN13));

		if (formatStrings[EAN128C].empty())
			setFormatString(EAN128C, productDefault->getFormatString(EAN128C));

		if (formatStrings[GS1AI].empty())
			setFormatString(GS1AI, productDefault->getFormatString(GS1AI));

		if (formatStrings[GS1GTIN].empty())
			setFormatString(GS1GTIN, productDefault->getFormatString(GS1GTIN));

		if (formatStrings[QR_CODE].empty())
			setFormatString(QR_CODE, productDefault->getFormatString(QR_CODE));

		if (formatStrings[DATAMATRIX].empty())
			setFormatString(DATAMATRIX, productDefault->getFormatString(DATAMATRIX));
	}
}


HobString BarCodeNumber::getFormatString(BarcodeType bcType) const
{
	return (bcType >= UPCA && bcType < MAX_BARCODE_TYPES) ? formatStrings[bcType] : "";
}

void BarCodeNumber::setFormatString(BarcodeType bcType, std::string format)
{
	bool isTracing = Debug::isTracing("BarCode");

	switch (bcType)
	{
		case EAN13:
		{
			formatStrings[EAN13] = format;
			if (isTracing)
				Debug::trace("BarCode", "setFormatString: EAN: " + formatStrings[EAN13]);
			break;
		}
		case EAN128C:
		{
			formatStrings[EAN128C] = format;
			if (isTracing)
				Debug::trace("BarCode", "setFormatString: EAN128C: " + formatStrings[EAN128C]);
			break;
		}
		case GS1AI:
		{
			formatStrings[GS1AI] = format;
			if (isTracing)
				Debug::trace("BarCode", "setFormatString: EAN128C: " + formatStrings[GS1AI]);
			break;
		}
		case GS1GTIN:
		{
			formatStrings[GS1GTIN] = format;
			if (isTracing)
				Debug::trace("BarCode", "setFormatString: EAN128C: " + formatStrings[GS1GTIN]);
			break;
		}
		case QR_LEVELM:
		case QR_LEVELL:
		case QR_LEVELQ:
		case QR_LEVELH:
		case QR_CODE:
		{
			// All of these share the same format string, so just put it in the array for offset QR_CODE
			formatStrings[QR_CODE] = format;
			if (isTracing)
				Debug::trace("BarCode", "setFormatString: QRCode: " + formatStrings[QR_CODE]);
			break;
		}
		case DATAMATRIX:
		{
			formatStrings[DATAMATRIX] = format;
			if (isTracing)
				Debug::trace("BarCode.DataMatrix", "setFormatString: DataMatrix: " + formatStrings[DATAMATRIX]);
			break;
		}
		default:
			break;
	}
}


void BarCodeNumber::buildBarcodeStrings(ProductTransaction *productTransaction, bool withPrefix)
{
	/***********************************************************************************
	*  Determine the barcode type.  Since we now allow multiple barcodes per label
	*  we determine the barcode type as the first 1-D barcode on the label.  We
	*  do this because when determining the Product Number and Vendor Number for any
	*  barcode that allows these fields, the format strings for EAN-13 and EAN-128
	*  could be conflicting with each other, and even conflict with the method to
	*  determine these for a UPC barcode.  For example, the EAN-13 format string could
	*  specify NNNVVV, and the EAN-128 could specify NNNNNVVVVV, or even VVVVVVVNNN.
	*  These would then print different product numbers and vendor numbers if both were
	*  included in a single label.  This conflict arises from determining the
	*  Product Number and Vendor Number from the rawBarcodeData, which is received
	*  from the 'up' tag (since that is all the Quantum could send).
	*
	*  Also, with this change to allow multiple barcodes on a label, QR code and
	*  DataMatrix barcodes will only use the product number and vendor numbers that
	*  are in the PrBarProN and PrBarVenN fields in the Product database record;
	*  they will not be determined from the rawBarcodeData, 'up'.
	**********************************************************************************/

	barcodeStrings[UPCA] = buildUPCBarcodeString(productTransaction, withPrefix);

	barcodeStrings[EAN13] = buildEAN13BarcodeString(productTransaction, withPrefix);

	barcodeStrings[EAN128C] = buildEAN128CBarcodeString(productTransaction, withPrefix);

	if (!formatStrings[GS1GTIN].empty() || !formatStrings[GS1AI].empty())
		barcodeStrings[RSS14E] = buildGS1BarcodeString(productTransaction, withPrefix);

	if (!formatStrings[QR_CODE].empty())
		barcodeStrings[QR_CODE] = buildDataMatrixString(formatStrings[QR_CODE], productTransaction, withPrefix);

	if (!formatStrings[DATAMATRIX].empty())
		barcodeStrings[DATAMATRIX] = buildDataMatrixString(formatStrings[DATAMATRIX], productTransaction, withPrefix);
}

HobString BarCodeNumber::getBarcodeStringFromFormat(const HobString &format)
{
	HobBarCodeFormatter f(formats);
	return f.format(format);
}

HobString BarCodeNumber::getBarcodeString(BarcodeType type) const
{
	if (UPCA > type && type >= MAX_BARCODE_TYPES)
		return "";

	if (QR_LEVELM == type || QR_LEVELL == type || QR_LEVELQ == type || QR_LEVELH == type)
		return barcodeStrings[QR_CODE];
	else if (RSS14E == type || RSS14E_GTIN_13 == type || RSS14E_GTIN_12 == type || RSS14E_GTIN_8 == type || GS1_DATABAR_EXPANDED == type)
		return barcodeStrings[RSS14E];
	else
		return barcodeStrings[type];
}

void BarCodeNumber::setBarcodeString(BarcodeType bcType, std::string barcodeString)
{
	switch (bcType)
	{
		case USER_SUPPLIED:
		{
			barcodeStrings[USER_SUPPLIED] = barcodeString;
			Debug::trace("BarCode", "setBarcodeString: USER_SUPPLIED: " + barcodeStrings[USER_SUPPLIED]);
			break;
		}
		default:
			break;
	}
}



short BarCodeNumber::getBarcodeType() const
{
	return barcodeType;
}
void BarCodeNumber::setBarcodeType(short t)
{
	if ((UPCA <= t) && (t < MAX_BARCODE_TYPES))
		barcodeType = t;
}


HobString BarCodeNumber::getPrefix() const
{
	return dbPrefix;
}
void BarCodeNumber::setPrefix(HobString prefix)
{
	dbPrefix = prefix;
	Debug::trace("BarCode", HobString("Set Prefix to: ") + dbPrefix);
}

HobString BarCodeNumber::getEANCode() const
{
	return dbEanCode;
}
HobString BarCodeNumber::getDerivedEANCode() const
{
	if (barcodeTypeDeterminant == EAN13)
	{
		HobString eanCode, eanVendorNum, eanProductNum;
		setEANStrings(formatStrings[EAN13], eanCode, eanVendorNum, eanProductNum);
		return eanCode;
	}
	else if (barcodeTypeDeterminant == EAN128C)
	{
		HobString eanCode, eanVendorNum, eanProductNum;
		setEANStrings(formatStrings[EAN128C], eanCode, eanVendorNum, eanProductNum);
		return eanCode;
	}
	else if ( barcodeTypeDeterminant >= RSS14E && barcodeTypeDeterminant <= RSS14E_GTIN_8 || barcodeTypeDeterminant == GS1_DATABAR_EXPANDED )   // GS1
	{
		HobString gs1EanCode, gs1VendorNum, gs1ProductNum;
		setEANStrings(formatStrings[GS1GTIN], gs1EanCode, gs1VendorNum, gs1ProductNum);
		return gs1EanCode;
	}
	else
		return dbEanCode;
}
void BarCodeNumber::setEANCode(HobString eanCode)
{
	dbEanCode = eanCode;
	Debug::trace("BarCode", HobString("Set EAN Code to: ") + dbEanCode);
}

HobString BarCodeNumber::getVendorNumber() const
{
	return dbVendorNumber;
}
HobString BarCodeNumber::getDerivedVendorNumber() const
{
	if (barcodeTypeDeterminant < EAN13)  // which means it it UPC
	{
		HobString upcPrefix, upcVendorNum, upcProductNum;
		setUPCStrings(upcPrefix, upcVendorNum, upcProductNum);
		return upcVendorNum;
	}
	else if (barcodeTypeDeterminant == EAN13)
	{
		HobString eanCode, eanVendorNum, eanProductNum;
		setEANStrings(formatStrings[EAN13], eanCode, eanVendorNum, eanProductNum);
		return eanVendorNum;
	}
	else if (barcodeTypeDeterminant == EAN128C)
	{
		HobString eanCode, eanVendorNum, eanProductNum;
		setEANStrings(formatStrings[EAN128C], eanCode, eanVendorNum, eanProductNum);
		return eanVendorNum;
	}
	else if ( barcodeTypeDeterminant >= RSS14E && barcodeTypeDeterminant <= RSS14E_GTIN_8 || barcodeTypeDeterminant == GS1_DATABAR_EXPANDED )   // GS1
	{
		HobString gs1EanCode, gs1VendorNum, gs1ProductNum;
		setEANStrings(formatStrings[GS1GTIN], gs1EanCode, gs1VendorNum, gs1ProductNum);
		return gs1VendorNum;
	}
	else
		return dbVendorNumber;
}
void BarCodeNumber::setVendorNumber(HobString vendNum)
{
	dbVendorNumber = vendNum;
	Debug::trace("BarCode", HobString("Setting Vendor Number to: ") + dbVendorNumber);
}


HobString BarCodeNumber::getProductNumber() const
{
	return dbProductNumber;
}
HobString BarCodeNumber::getDerivedProductNumber() const
{
	if (barcodeTypeDeterminant < EAN13)  // which means it is UPC
	{
		HobString upcPrefix, upcVendorNum, upcProductNum;
		setUPCStrings(upcPrefix, upcVendorNum, upcProductNum);
		return upcProductNum;
	}
	else if (barcodeTypeDeterminant == EAN13)
	{
		HobString eanCode, eanVendorNum, eanProductNum;
		setEANStrings(formatStrings[EAN13], eanCode, eanVendorNum, eanProductNum);
		return eanProductNum;
	}
	else if (barcodeTypeDeterminant == EAN128C)
	{
		HobString eanCode, eanVendorNum, eanProductNum;
		setEANStrings(formatStrings[EAN128C], eanCode, eanVendorNum, eanProductNum);
		return eanProductNum;
	}
	else if ( barcodeTypeDeterminant >= RSS14E && barcodeTypeDeterminant <= RSS14E_GTIN_8 || barcodeTypeDeterminant == GS1_DATABAR_EXPANDED )   // GS1
	{
		HobString gs1EanCode, gs1VendorNum, gs1ProductNum;
		setEANStrings(formatStrings[GS1GTIN], gs1EanCode, gs1VendorNum, gs1ProductNum);
		return gs1ProductNum;
	}
	else
		return dbProductNumber;


}
void BarCodeNumber::setProductNumber(HobString prodNum)
{
	dbProductNumber = prodNum;
	Debug::trace("BarCode", HobString("Set Product Number to: ") + dbProductNumber);
}


HobString BarCodeNumber::getUPCNumberSystem() const
{
	Debug::trace("BarCode", HobString("getUPCNumberSystem(): ") + dbPrefix);
	return dbPrefix;
}
void BarCodeNumber::setUPCNumberSystem(HobString UPCNumberSystem)
{
	UPCNumberSystem.getCharsWithPadding(1);
	
	dbPrefix = UPCNumberSystem;
	Debug::trace("BarCode", HobString("Set UPCNumberSystem to: ") + dbPrefix);
}


HobString BarCodeNumber::getUPCNumber() const
{
	return getProductNumber();
}
void BarCodeNumber::setUPCNumber(HobString upcNum)
{
	upcNum.getCharsWithPadding((Configuration::getInstance()->toBoolCfg(fieldOffsetCfgPFSecondCheckDigit)) ? 5 : 6);
	setProductNumber(upcNum);
}


/*   Code prior to PJR 285 - allow multiple barcodes per label
bool BarCodeNumber::setUPCNumberFromEANCode()
{
	bool result = false;
	int maxLength = (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgPFSecondCheckDigit)) ? 5 : 6;
	
	// EAN code is stored in this BarCodeNumber class as a string, with its leading zeros;
	// it was formatted from the data in the database when the plu was loaded, based on it's product type's Product Default record.
	// Which also means that if the data in the database doesn't fit the EAN format string, it will be truncated to make it fit,
	// without any error being generated, or any notification to the operator.
	// Anyway, we have to get rid of the leading zeroes, so I convert it back to a number, then recreate a string from that,
	// to determine if it will fit into the UPC barcode.
	
	// Allow 10 digits for EAN code. For HEB Mexico demo.
	//HobRecordNum eanCodeNumber = Long::valueOf(eanCode);
	//HobString eanCodeNumberStr = Long::toString(eanCodeNumber);
	//int eanCodeSize = eanCodeNumberStr.size();
	int eanCodeSize = dbEanCode.size();

	if ( eanCodeSize <= maxLength )
	{
		// It fits, so go ahead and set it.
		setUPCNumber(dbEanCode);
		result = true;
	}
	// Don't set the UPC number if the EAN code wouldn't fit.
	return result;
}

// Returns true if we can set the EAN code from the UPC product number, based on the number of 'E's in the EAN format string.
bool BarCodeNumber::setEANCodeFromUPCNumber()
{
	bool result = false;
	if ( !formatStrings[EAN13].empty() )
	{
		int numChars = formatStrings[EAN13].countConsecutiveChars('E');
		if ( numChars > 0 )
		{
			// UPC Product number is stored in this BarCodeNumber class as a string, with its leading zeros;
			// it was formatted from the data in the database when the plu was loaded.
			// Anyway, we have to get rid of the leading zeroes, so I convert it back to a number, then recreate a string from that,
			// to determine if it will fit into the EAN code.
			HobString productNumberStr = getUPCNumber();
			HobRecordNum productNumber = Long::valueOf(productNumberStr);
			HobString productNumberStr2 = Long::toString(productNumber);
			int prodSize = productNumberStr2.size();

			if ( prodSize <= numChars )
			{
				// It fits, so go ahead and set it.
				// LAP-BARCODE
				//setEANCode(getUPCNumber(), numChars);
				setEANCode(getUPCNumber());
				result = true;
			}
		}
		// Don't set the EAN code if the UPC Product number wouldn't fit.
		else
			result = true;
	}
	else if ( !formatStrings[EAN128C].empty() )
	{
	}
	else if ( !formatStrings[GS1GTIN].empty() )
	{
	}
	else if ( !formatStrings[GS1AI].empty() )
	{
	}
	return result;
}
*/

void BarCodeNumber::setUPCStrings(HobString &upcNumberSys, HobString &upcVendorNum, HobString &upcProductNum) const
{
	bool traceOn = Debug::isTracing("BarCode");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "setUPCStrings():   ";
		Debug::trace("BarCode", msgDebug + "Barcode type is UPC");
	}

	unsigned int numSys, numChars = 1;
	HobString dbString;

        upcNumberSys = upcPrefix;
        numSys = Integer::valueOf(upcPrefix);

	if ((numSys == 2) || (numSys == 4))
	{
		// This gets set to an empty string by default for number system 2 or 4, since the vendor number is irrelevant.
		upcVendorNum = "";

		if (dbProductNumber.empty())
		{
			if (swappingOfEanCodeAndProductNumberAllowed && !dbEanCode.empty())
				dbString = dbEanCode;
			else
				dbString = rawBarCodeString;
		}
		else
			dbString = dbProductNumber;

		if (traceOn)
			Debug::trace("BarCode", msgDebug + " numberSys is 2 or 4, determining product number from: " + dbString);

		if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgPFSecondCheckDigit))
		{
			numChars = 5;
			if (dbString.length() < numChars)
				dbString.insert(0, "00000", numChars - dbString.length());
			else if (dbString.length() > numChars)
				dbString = dbString.substr(0, numChars);
		}
		else
		{
			numChars = 6;
			if (dbString.length() < numChars)
				dbString.insert(0, "000000", numChars - dbString.length());
			else if (dbString.length() > numChars)
				dbString = dbString.substr(0, numChars);
		}

		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append(" numberSys is 2 or 4, determined product number to be:  ").append(dbString);
			msgDebug2.append("  numChars = ").append(Integer::toString(numChars));
			Debug::trace("BarCode", msgDebug2);
		}
		upcProductNum = dbString;
	}
	else if ((numSys == 0) || (numSys == 1) || (numSys == 6) || (numSys == 7) || (numSys == 8) || (numSys == 9))
	{
		numChars = 5;

		HobString venNum, prodNum;
		HobString raw(rawBarCodeString);

		HobString productNumberToUse = dbProductNumber;
		if (productNumberToUse.empty() && swappingOfEanCodeAndProductNumberAllowed && !dbEanCode.empty())
			productNumberToUse = dbEanCode;

		if (dbVendorNumber.empty() && productNumberToUse.empty() && !raw.empty())
		{
			unsigned int rbcLen = (unsigned int)raw.size();
			int maxUpcLen = numChars * 2;
			if (rbcLen < (unsigned int)maxUpcLen)
			{
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("Warning, rawBarCodeString '").append(raw).append("' is less than ").append(inttostring(maxUpcLen));
					msgDebug2.append(" characters long! Zero-filling on left");
					Debug::trace("BarCode", msgDebug2);
				}

				raw.insert(0, "0000000000", maxUpcLen - rbcLen);
				// SOF-2751 - Also fixed a bug where product number and vendor number are determined from raw barcode data,
				//            and if the raw barcode data is only 5 digits (so vendor number comes out as '00000'),
				//            product number won't get set to be the raw barcode data unless we re-setup the rbcLen variable.
				rbcLen = (unsigned int)raw.size();

				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("New rawBarCodeString '").append(raw).append("'");
					Debug::trace("BarCode", msgDebug2);
				}
			}

			venNum = raw.substr(0, numChars);
			if (traceOn)
				Debug::trace("BarCode", msgDebug + "venNum [" + venNum + "]");
			if (rbcLen - venNum.size())
			{
				prodNum = raw.substr(venNum.size(), numChars);
				if (traceOn)
					Debug::trace("BarCode", msgDebug + "prodNum [" + prodNum + "]");
			}
		}

		if (dbVendorNumber.empty())
		{
			if (!venNum.empty())
				upcVendorNum = getFormattedString(venNum, numChars);
		}
		else
		{
			upcVendorNum = getFormattedString(dbVendorNumber, numChars);
		}

		if (productNumberToUse.empty())
		{
			if (!prodNum.empty())
				upcProductNum = getFormattedString(prodNum, numChars);
		}
		else
		{
			upcProductNum = getFormattedString(productNumberToUse, numChars);
		}
	}
}


void BarCodeNumber::setEANStrings(HobString formatString, HobString &eanCode, HobString &eanVendorNum, HobString &eanProductNum) const
{
	bool traceOn = Debug::isTracing("BarCode");
	HobString msgDebug;
	if (traceOn)
		msgDebug = "setEANStrings():   ";

	eanCode = eanVendorNum = eanProductNum = "";

	if (formatString.empty())
		return;

	if (traceOn)
		Debug::trace("BarCode", msgDebug + " EAN FormatString = " + formatString);

	HobString dbString;
	int pos = 0;

	//	Find the number of consecutive characters for the User enterable EANCode field
	int numChars = formatString.countConsecutiveChars('E');
	if (numChars)
	{
		if (dbEanCode.empty())
		{

			if (swappingOfEanCodeAndProductNumberAllowed && !dbProductNumber.empty())
				dbString = dbProductNumber;
			else
			{
				if (rawBarCodeString.empty())
					dbString = "";
				else
					dbString = HobString(rawBarCodeString, 0, numChars);
			}
		}
		else
			// SOF-4082: make sure we keep the front numChar characters, not the back.
			// If we don't cut it here, it gets cut on the other end, leading to
			// inconsistent truncation depending on the label type.
			// (was "dbString = dbEanCode;").
			dbString = HobString(dbEanCode, 0, numChars);

		eanCode = getFormattedString(dbString, numChars);

		if (traceOn)
			Debug::trace("BarCode", msgDebug + " UseEanCode  dbString = " + dbString + " resulting eanCode = " + eanCode);
	}
	else
	{
		int vChars = formatString.countConsecutiveChars('V');
		int nChars = formatString.countConsecutiveChars('N');
		int vPos = (int) formatString.find('V');
		int nPos = (int) formatString.find('N');
		pos = 0;

		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("vChars = ").append(Integer::toString(vChars));
			msgDebug2.append("  vPos = ").append(Integer::toString(vPos));
			msgDebug2.append("  nChars = ").append(Integer::toString(nChars));
			msgDebug2.append("  nPos = ").append(Integer::toString(nPos));
			msgDebug2.append("  pos = ").append(Integer::toString(pos));
			Debug::trace("BarCode", msgDebug2);
		}
		if (vChars)
		{
			if (dbVendorNumber.empty())
			{
				if (rawBarCodeString.empty())
					dbString = "";
				else
				{
					if ((vPos > nPos) && nChars)				//	Product number before Vendor number
					{
						dbString = HobString(rawBarCodeString, nChars, vChars);
						pos = 0;		//	Next position in the RawBarCode for possible Vendor chars
					}
					else
					{
						dbString = HobString(rawBarCodeString, pos, vChars);
						pos = vChars;		//	Next position in the RawBarCode for possible Vendor chars
					}
				}
			}
			else
				dbString = dbVendorNumber;

			eanVendorNum = getFormattedString(dbString, vChars);

			if (traceOn)
				Debug::trace("BarCode", msgDebug + " UseVendorNumber  dbString = " + dbString + " resulting eanVendorNum = " + eanVendorNum);
		}

		if (nChars)
		{
			if (dbProductNumber.empty())
			{
				if (rawBarCodeString.empty())
					dbString = "";
				else
					dbString = HobString(rawBarCodeString, pos, nChars);
			}
			else
				dbString = dbProductNumber;

			eanProductNum = getFormattedString(dbString, nChars);

			if (traceOn)
				Debug::trace("BarCode", msgDebug + " UseProductNumber  dbString = " + dbString + " resulting eanProductNum = " + eanProductNum);
		}
	}
}


void BarCodeNumber::setGS1GTINStrings(HobString formatString, HobString &gs1EanCode) const
{
	bool traceOn = Debug::isTracing("BarCode");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "setGS1GTINStrings():   ";
		Debug::trace("BarCode", msgDebug + "Barcode type is RSS14E");
	}

	gs1EanCode = "";

	if (formatString.empty())
		return;

	if (traceOn)
		Debug::trace("BarCode", msgDebug + " formatString = " + formatString);

	HobString dbString;

	//	Find the number of consecutive characters for the User enterable eanCode field
	int numChars = formatString.countConsecutiveChars('E');
	if (numChars)			// have E
	{
		if (dbEanCode.empty())
		{
			if (rawBarCodeString.empty())
				dbString = "";
			else
				dbString = HobString(rawBarCodeString, 0, numChars);
		}
		else
			// Allow 10 digits for EAN code. For HEB Mexico demo.
			dbString = dbEanCode;

		gs1EanCode = getFormattedString(dbString, numChars);

		if (traceOn)
			Debug::trace("BarCode", msgDebug + " UseEanCode  dbString = " + dbString + " resulting eanCode = " + gs1EanCode);
	}
}


bool BarCodeNumber::specialRulesForWalmartGS1InEffect(HobString formatString) const
{
	bool traceOn = Debug::isTracing("BarCode");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "specialRulesForWalmartGS1InEffect():   ";
		Debug::trace("BarCode", msgDebug);
	}

	// Joe's comment from when he implemented this in 'branch walmart-3.1.0', which ended up in 3.3.0
	// Special Case - Need to find out if we need to swap in UPC rules for the GS1 Rules
	// we are only going to apply this case to UPC Types of 0,2,4
	// TEST 1: we have some Es to deal with
	// TEST 2: EANCode stored in the item record is null or empty is first clue - no E vals, so we might be in play
	// TEST 3: GTIN only - !GS1GTINFormatString.empty() tested up above this already
	// TEST 4: What is the prefix. Has to be 0 or 2.
	// TEST 5: if prefix 0, do we have vendor and product numbers
	// TEST 6: if prefix 2 or 4, do we have have product number


	// If no format string is set up, then the Walmart rules don't apply
	if (formatString.empty())
		return false;

	if (traceOn)
		Debug::trace("BarCode", msgDebug + " formatString = " + formatString);

	// Find the number of consecutive characters for the User enterable eanCode field
	// If there are no E's in the format string, then the Walmart rules don't apply
	int numChars = formatString.countConsecutiveChars('E');
	if (numChars == 0)
		return false;

	// If there is a specific EAN Code in the product record, then the Walmart rules don't apply
	if (!dbEanCode.empty())
		return false;

	int prefix = Integer::valueOf(dbPrefix);
	// If prefix is 0 and the product record contains explicit vendor and product numbers (essentially we received pn and vn),
	// Or if prefix is 2 or 4 (I added the check for '4') and the product record contains an explicit product number (received pn)
	// then the Walmart rules DO apply
	if ((0 == prefix && !dbVendorNumber.empty() && !dbProductNumber.empty()) || ((2 == prefix || 4 == prefix) && !dbProductNumber.empty()))
	{
		if (traceOn)
			Debug::trace("BarCode", msgDebug + " is TRUE - prefix=" + dbPrefix + "  dbVendorNumber=" + dbVendorNumber + "  dbProductNumber=" + dbProductNumber);
		return true;
	}

	return false;
}


bool BarCodeNumber::valueIsTooBig(int chars, long value)
{
	bool error;
	long maxValue = 10L;
	for (int i=1; i<chars; i++)
		maxValue *= 10;

	if ( value < maxValue )
		error = false;                            
	else
		error = true;                            
	return (error);
}    


HobString BarCodeNumber::buildUPCBarcodeString(ProductTransaction *productTransaction, bool withPrefix, bool replaceMostSignificantUPCDigit, bool withFinalCheckDigit)
{
	HobString upcPrefix, upcVendorNum, upcProductNum;
	setUPCStrings(upcPrefix, upcVendorNum, upcProductNum);

	HobString upcBarcodeString = "";

	switch (Integer::valueOf(upcPrefix))
	{
		case 0:
		case 1:
		case 6:
		case 7:
		case 8:
		case 9:
		{
			upcBarcodeString.append(upcVendorNum);
			upcBarcodeString.append(upcProductNum);
			break;
		}

		case 2:
		case 4:
		{
			//	Now append the productNumber/UPC number.
			upcBarcodeString.append(upcProductNum);

			//	Now append the Total price.
			short priceChars = 4;
			HobString priceString;

			if ( !productTransaction )
			{
				isDPEnabled = false;						// set to false with every plu change
				priceString = HobString( priceChars, '0' );	//	Put Zeros in the price field - there's no data for it.
				Debug::trace("BarCode.DiscountPrice", "BarcodeNumber::buildUPCBarcodeString() NO productTransaction so set isDPEnabled to false: " + booltostring(isDPEnabled));
			}
			else
			{
				Product *prod = productTransaction->getProduct();
				long price = productTransaction->getTotalPrice();

				// SOF-4682 - Raley's embed FS price in barcode
				// This is the preferred method, just use a flag - but we are running out of space.
				HobPrice priceDiscount = productTransaction->getDiscTotalPrice1();			// not testing dp2 or dp3 at this time

				if( !isDPEnabled )
					isDPEnabled = isDiscountPricePriceInBarcode(prod);
				
				Debug::trace("BarCode.DiscountPrice", "BarcodeNumber::buildUPCBarcodeString() check flag isDPEnabled " + booltostring(isDPEnabled) );
				
				if (( prod ) && ( Product::PRINTING_RUN_TOTALS_LABEL == prod->getLabelBeingPrinted() ))
				{
					Session *session = SessionManager::getInstance()->getPrimaryConsoleSession();
					ProductRunTotal *productRunTotal = session ? session->getProductTotalManager()->getCurrentProductRunTotal() : ((ProductRunTotal*) NULL);

					if ( productRunTotal )
						price = productRunTotal->getPrice();
				}

				//	Make sure the Price will fit in the Price field
				if ( valueIsTooBig( priceChars, price ) )
				{
					Debug::trace("BarCode.DiscountPrice", "BarcodeNumber::buildUPCBarcodeString() valueIsTooBig is TRUE " );
					priceString = HobString( priceChars, '0' );	//	Put Zeros in the price field - it's too big to fit.
				}
				else
				{
					// SOF-4682 - Raley's embed FS price in barcode
					// This is the preferred method, just use a flag - but we are running out of space.
					if (isDPEnabled)
					{
						if( 0 == priceDiscount)
							priceString = Long::toString(price);
						else
							priceString = Long::toString(priceDiscount);

						Debug::trace("BarCode.DiscountPrice", "BarcodeNumber::buildUPCBarcodeString() use priceDiscount in the priceString as: " + priceString);
					}
				    else
				    {
					    priceString = Long::toString( price );
						Debug::trace("BarCode.DiscountPrice", "BarcodeNumber::buildUPCBarcodeString() use price in the priceString as: " + priceString);
					}

					priceString.getCharsWithPadding( priceChars );
				}
			}

			if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgPFSecondCheckDigit) )	//	We need a Price Check digit.
				upcBarcodeString.append( UPCAPriceCheck(priceString) );

			upcBarcodeString.append(priceString);
			break;
		}

		default:
			break;
	}

	if ( replaceMostSignificantUPCDigit && !mostSignificantUPCDigitReplacement.empty() )
		upcBarcodeString.replace(0, 1, mostSignificantUPCDigitReplacement);

	barcodeStrings[GS1UPCBARCODE] = upcBarcodeString;

	//	The Prefix/Number system is the first digit of the barcode.
	if (withPrefix)
		upcBarcodeString.insert(0, upcPrefix);

	if (withFinalCheckDigit)
		upcBarcodeString.append( ComputeUPCAChecksumDigit(upcBarcodeString) );

	return upcBarcodeString;
}

HobString BarCodeNumber::UPCAPriceCheck(HobString priceString)
{
	int sum = 0, tempvalue;

	if (priceString.length() == 4)
	{
		HobString tempString(priceString.c_str());
		tempString.makeBinary();

		std::string::iterator iter = tempString.begin();

		tempvalue = (*iter * 2);
		sum = (tempvalue - (tempvalue / 10)) % 10;
		iter++;
		tempvalue = (*iter * 2);
		sum += (tempvalue - (tempvalue / 10)) % 10;
		iter++;
		sum += (*iter * 3) % 10;
		iter++;
		tempvalue = (*iter * 5);
		sum += (tempvalue - (tempvalue / 10) % 10);
	}

	return Integer::toString(((sum * 3) % 10));
}

HobString BarCodeNumber::ComputeUPCAChecksumDigit(HobString barcodeString)
{
	int i, oddSum = 0, evenSum = 0, checkSum = 0;

	if (barcodeString.length() == 11)
	{
		HobString tempString(barcodeString.c_str());
		tempString.makeBinary();
		std::string::iterator iter = tempString.begin();

		for (i = 0; i < 11; i++, iter++)
		{
			if (i % 2)
			{
				oddSum += *iter;
			}
			else
			{
				evenSum += *iter;
			}
		}
		checkSum = (10 - (((3 * evenSum) + oddSum) % 10)) % 10;
	}
	return (Integer::toString(checkSum));
}



HobString BarCodeNumber::buildEAN13BarcodeString(ProductTransaction *productTransaction, bool withPrefix)
{
	if (formatStrings[EAN13].empty())
		return "";

	HobString result = buildEANBarcodeString(formatStrings[EAN13], productTransaction, withPrefix);
	if (result.length() == 12)
		result.append(ComputeEANChecksumDigit(result));
	return result;
}

HobString BarCodeNumber::buildEAN128CBarcodeString(ProductTransaction *productTransaction, bool withPrefix)
{
	if (formatStrings[EAN128C].empty())
		return "";

	HobString result = buildEANBarcodeString(formatStrings[EAN128C], productTransaction, withPrefix);
	result.append(ComputeEANChecksumDigit(result));	// complete to 18 characters to calculate checksum digits
	return result;
}

HobString BarCodeNumber::buildEANBarcodeString(HobString formatString, ProductTransaction *productTransaction, bool withPrefix)
{
	unsigned int i;
	short numChars = 0, priceChars = 0, weightChars = 0, transNumChars = 0, scaleIdChars = 0;
	bool load_error = false;
	char character;

	HobString priceStr("");
	HobString discPriceStr("");				// New dicount price in barcode
	HobString weightStr("");
	HobString transNumStr("");
	HobString scaleIdStr("");

	bool traceOn = Debug::isTracing("BarCode.EAN");
	HobString msgDebug;
	if (traceOn)
		msgDebug = "buildEANBarcodeString():   ";


	HobString eanCode, eanVendorNum, eanProductNum;
	setEANStrings(formatString, eanCode, eanVendorNum, eanProductNum);


	/* Clear the buffer */
	HobString eanBarcodeString = "";

	if ( !formatString.empty() )
	{
		if ( traceOn )
			Debug::trace( "BarCode.EAN", msgDebug + "Setting up the priceStr & weightStr" );

		//	Find the number of digits for the Price field
		if ( 0 < (priceChars = formatString.countConsecutiveChars( 'P' )) )
		{
			if ( !productTransaction )
			{
				isDPEnabled = false;
				priceStr = HobString( priceChars, '0' );	//	Put Zeros in the price field - there's no data for it.
				Debug::trace("BarCode.DiscountPrice", "BarcodeNumber::buildEANBarcodeString() NO productTransaction so set isDPEnabled to false: " + booltostring(isDPEnabled));
			}
			else
			{
				Product *prod = productTransaction->getProduct();
				long price = productTransaction->getTotalPrice();

				// SOF-4682 - Raley's embed FS price in barcode
				// This is the preferred method, just use a flag - but we are running out of space.
				long priceDiscount = productTransaction->getDiscTotalPrice1();			// not testing dp2 or dp3 at this time

				if (!isDPEnabled)
					isDPEnabled = isDiscountPricePriceInBarcode(prod);

				Debug::trace("BarCode.DiscountPrice", "BarcodeNumber::buildEANBarcodeString() check flag isDPEnabled " + booltostring(isDPEnabled));

				if (( prod ) && ( Product::PRINTING_RUN_TOTALS_LABEL == prod->getLabelBeingPrinted() ))
				{
					Session *session = SessionManager::getInstance()->getPrimaryConsoleSession();
					ProductRunTotal *productRunTotal = session ? session->getProductTotalManager()->getCurrentProductRunTotal() : ((ProductRunTotal*) NULL);

					if ( productRunTotal )
						price = productRunTotal->getPrice();
				}

				//	Make sure the Price (or discount price) will fit in the Price field
				if ( valueIsTooBig( priceChars, price ) )
				{
					Debug::trace("BarCode.DiscountPrice", "BarcodeNumber::buildEANBarcodeString() valueIsTooBig is TRUE ");
					priceStr = HobString( priceChars, '0' );	//	Put Zeros in the price field - it's too big to fit.
				}
				else
				{
					// SOF-4682 - Raley's embed FS price in barcode
					// This is the preferred method, just use a flag - but we are running out of space.
					if (isDPEnabled)
					{
						if (0 == priceDiscount)
							priceStr = Long::toString(price);
						else
							priceStr = Long::toString(priceDiscount);

						Debug::trace("BarCode.DiscountPrice", "BarcodeNumber::buildUPCBarcodeString() use priceStr in the priceString as: " + priceStr);
					}
				    else
				    {
					    priceStr = Long::toString( price );
				        Debug::trace("BarCode.DiscountPrice", "BarcodeNumber::buildUPCBarcodeString() use price in the priceString as: " + priceStr);
				    }

					priceStr.getCharsWithPadding( priceChars );
				}
			}
		}

		//	Find the number of digits for the Weight field
		if ( 0 < (weightChars = formatString.countConsecutiveChars( 'W' )) )
		{
			Configuration *pConfig = Configuration::getInstance();

			if ( !productTransaction )
				weightStr = HobString( weightChars, '0' );	//	Put Zeros in the weight field - there's no data for it.
			else
			{
				Product *prod = productTransaction->getProduct();
				HobWeight wght = productTransaction->getTotalNetWeight();

				if (( prod ) && ( Product::PRINTING_RUN_TOTALS_LABEL == prod->getLabelBeingPrinted() ))
				{
					Session *session = SessionManager::getInstance()->getPrimaryConsoleSession();
					ProductRunTotal *productRunTotal = session ? session->getProductTotalManager()->getCurrentProductRunTotal() : ((ProductRunTotal*) NULL);

					if ( productRunTotal )
						wght = productRunTotal->getWeight();
				}

                if (((prod) && !prod->hasKbdNetWeightLabel()) && ((MetSig::isScaleAvoir()) && (MetSig::isWeigherConfiguredForSingleRange())))
                {
                    //Debug::trace("BarCode.EAN", string("buildGS1BarcodeStringPart() TRIM DIGIT W: ") + Integer::toString(wght) );
                    wght = MetSig::divideWeightForSingleRange(wght);
                    //Debug::trace("BarCode.EAN", string("buildGS1BarcodeStringPart() AFTER TRIM DIGIT W: ") + Integer::toString(wght));
                }

				//	Make sure the Weight will fit in the Weight field
				if ( valueIsTooBig( weightChars, wght ) )
					weightStr = HobString( weightChars, '0' );	//	Put Zeros in the weight field - it's too big to fit.
				else
				{
					weightStr = Long::toString( wght );
					weightStr.getCharsWithPadding( weightChars );
				}
			}
		}
		
		//	Find the number of digits for the Transaction Number field
		transNumChars = formatString.countConsecutiveChars('T');
		if (transNumChars > 0)
		{
			if (!productTransaction)
				transNumStr = HobString(transNumChars, '0');	//	Put Zeros in the trans num field - there's no data for it.
			else
			{
				//	Make sure the Transaction Number will fit in the trans num field
				UINT32 transNum = productTransaction->getTransactionNumberForBarcode();
				long ldivisor = Power::power(10, transNumChars);
				UINT32 transNumFit = transNum % ldivisor;
				transNumStr = Long::toString(transNumFit);
				transNumStr.getCharsWithPadding(transNumChars);
			}
		}

		//	Find the number of digits for the Scale Id field
		scaleIdChars = formatString.countConsecutiveChars('S');
		if (scaleIdChars > 0)
		{
			//	Make sure the Scale Id will fit in the Scale Id field
			load_error = valueIsTooBig(scaleIdChars, Configuration::getInstance()->toIntCfg(fieldOffsetCfgAccumItemsScaleId));
			if (load_error)
				scaleIdStr = HobString(scaleIdChars, '0');	//	Put Zeros in the Scale Id field - it's too big to fit.
			else
			{
				scaleIdStr = Integer::toString(Configuration::getInstance()->toIntCfg(fieldOffsetCfgAccumItemsScaleId));
				scaleIdStr.getCharsWithPadding(scaleIdChars);
			}
		}
	}

	/* Now switch out the remaining data */
	/* for the characters in the format string */
	for (i = 0; i < formatString.length(); )
	{
		character = formatString.at(i);
		if (character == 'E')
		{   
			/* Find the number of digits for the EAN Code  */
			numChars = formatString.countConsecutiveChars('E', i);
			eanBarcodeString.append(eanCode);

			i += numChars;
		}
		else if (character == 'V')
		{   
			/* Find the number of digits for the Vendor number  */
			numChars = formatString.countConsecutiveChars('V', i);
			eanBarcodeString.append(eanVendorNum);

			i += numChars;
		}
		else if (character == 'N')
		{   
			/* Find the number of digits for the Product Number  */
			numChars = formatString.countConsecutiveChars('N', i);
			eanBarcodeString.append(eanProductNum);

			i += numChars;
		}
		else if (character == 'P')
		{   
			eanBarcodeString.append(priceStr);
			i += priceChars;
		}
		else if (character == 'p')
		{   
			//	Find the number of digits for the Price check field
			numChars = formatString.countConsecutiveChars('p', i);
			if((numChars != 1) || (priceChars < 4) || (priceChars > 5) || priceStr.empty())
				eanBarcodeString.append(numChars, '0');	//	Put Zeros in the price check field - it's too big to fit.
			else
			{
				HobString tempStr("");
				if(priceChars == 4)
					tempStr = EAN4DigitValueCheck(priceStr);
				else
					tempStr = EAN5DigitValueCheck(priceStr);
				eanBarcodeString.append(tempStr);
			}
			i += numChars;
		}
		else if (character == 'W')
		{   
			eanBarcodeString.append(weightStr);
			i += weightChars;
		}
		else if (character == 'w')
		{   
			//	Find the number of digits for the Weight check field
			numChars = formatString.countConsecutiveChars('w', i);
			if((numChars != 1) || (weightChars < 4) || (weightChars > 5) || weightStr.empty())
				eanBarcodeString.append(numChars, '0');	//	Put Zeros in the Weight check field - it's too big to fit.
			else
			{
				HobString tempStr("");
				if(weightChars == 4)
					tempStr = EAN4DigitValueCheck(weightStr);
				else
					tempStr = EAN5DigitValueCheck(weightStr);
				eanBarcodeString.append(tempStr);
			}
			i += numChars;
		}
		else if (character == 'Q')
		{   
			HobString byCountStr("");

			//	Find the number of digits for the By Count field
			numChars = formatString.countConsecutiveChars('Q', i);
			if(!productTransaction)
				byCountStr = HobString(numChars, '0');	//	Put Zeros in the By Count field - there's no data for it.
			else
			{
				//	Make sure the By Count quantity will fit in the By Count field
				load_error = valueIsTooBig(numChars, productTransaction->getByCount());
				if(load_error)
					byCountStr = HobString(numChars, '0');	//	Put Zeros in the By Count field - it's too big to fit.
				else
				{
					byCountStr = Long::toString(productTransaction->getByCount());
					byCountStr.getCharsWithPadding(numChars);
				}
			}
			//	Append the entire string
			eanBarcodeString.append(byCountStr);
			i += numChars;
		}
		else if (character == 'X')
		{   
			//	Find the number of digits for the Prefix field
			numChars = formatString.countConsecutiveChars('X', i);
			if (withPrefix)
				eanBarcodeString.append(getFormattedString(dbPrefix, numChars));

			i += numChars;
		}
		else if (character == 'T')
		{
			eanBarcodeString.append(transNumStr);
			i += transNumChars;
		}
		else if (character == 'S')
		{   
			eanBarcodeString.append(scaleIdStr);
			i += scaleIdChars;
		}
		else if (character == 'L')
		{
			numChars = formatString.countConsecutiveChars('L', i);
			eanBarcodeString.append(getFormattedString(Long::toString(productTransaction->getProductNum()), numChars));

			i += numChars;
		}
		else if ((character >= '0') && (character <= '9'))
		{
			eanBarcodeString.append(1, character);
			++i;
		}
		else
		{
			++i;
		}
	}
	return eanBarcodeString;
}

HobString BarCodeNumber::EAN4DigitValueCheck(HobString value)
{
	int sum = 0, tempValue = 0;
	HobString tempString(value.c_str());

	tempString.makeBinary();

	std::string::iterator iter = tempString.begin();

	tempValue = ((*iter) * 2);
	sum = (tempValue - (tempValue / 10)) % 10;		//	Digit 1 - weighted by 2-
	iter++;
	tempValue = ((*iter) * 2);
	sum += (tempValue - (tempValue / 10)) % 10;		//	Digit 2 - weighted by 2-
	iter++;
	sum += ((*iter) * 3) % 10;						//	Digit 3 - weighted by 3
	iter++;
	tempValue = ((*iter) * 5);
	sum += (tempValue - (tempValue / 10)) % 10;		//	Digit 4 - weighted by 5-

	sum = (sum * 3) % 10;
	return (Integer::toString(sum));     // Converted to a HobString
}

HobString BarCodeNumber::EAN5DigitValueCheck(HobString value)
{
	int sum = 0, tempValue = 0;
	HobString tempString(value.c_str());

	tempString.makeBinary();

	std::string::iterator iter = tempString.begin();

	tempValue = ((*iter) * 5);
	sum = (tempValue / 10) + (tempValue % 10);			//	Digit 1 - weighted by 5+
	iter++;
	tempValue = ((*iter) * 2);
	sum += (tempValue - (tempValue / 10)) % 10;			//	Digit 2 - weighted by 2-
	iter++;
	tempValue = ((*iter) * 5);
	sum += 10 - ((tempValue / 10) + (tempValue % 10));	//	Digit 3 - weighted by 5-
	iter++;
	tempValue = ((*iter) * 5);
	sum += (tempValue / 10) + (tempValue % 10);			//	Digit 4 - weighted by 5+
	iter++;
	tempValue = ((*iter) * 2);
	sum += (tempValue - (tempValue / 10)) % 10;			//	Digit 5 - weighted by 2-

	sum = (10 - (sum % 10)) % 10;
	//	sum = 10 - (sum % 10);

	//	tempValue = (10 - (((sum * 5) / 10) + ((sum * 5) % 10))) % 10;
	for (tempValue = 0; tempValue < 10; tempValue++)
	{
		if (sum == EAN5WeightedProduct[tempValue])
			break;
	}

	return (Integer::toString(tempValue));     // Converted to a HobString
}

HobString BarCodeNumber::ComputeEANChecksumDigit(HobString barcodeString)
{
	int i = 0, oddSum = 0, evenSum = 0, checkSum = 0;
	
	// SOF-5728
	// if odd number of digits in the string, prepend a '0' to make it even length,
	// so that we consider the correct digits as the odd digits, for when we multiply by 3.
	// Really, the right-most digit is considered the first odd digit, but the algorithm
	// below starts at the left, so we need to massage it to get the correct even and odd digits.
	// At this point, I don't want to rewrite this to go from right to left, even if it would be straight-forward.
	if (barcodeString.length() % 2 == 1)
		barcodeString.insert(0, "0");

	int bcLen = (int) barcodeString.length();

	if (bcLen >= 1)
	{
		HobString tempString(barcodeString.c_str());
		tempString.makeBinary();
		std::string::iterator iter = tempString.begin();

		for (i = 0; i < bcLen; i++, iter++)
		{
			if (i % 2)
			{
				oddSum += *iter;
			}
			else
			{
				evenSum += *iter;
			}
		}
		checkSum = (10 - (((3 * oddSum) + evenSum) % 10)) % 10;
	}
	return (Integer::toString(checkSum));
}



HobString BarCodeNumber::buildGS1BarcodeString(ProductTransaction *productTransaction, bool withPrefix)
{
	HobString checkDigit("");

	bool traceOn = Debug::isTracing("BarCode.GS1");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "buildGS1BarcodeString():   ";
		Debug::trace("BarCode.GS1", "\nSTART buildGS1BarcodeString()");
	}

	// First things first, do we have a U in the string?
	if ( formatStrings[GS1GTIN].empty() )
		Debug::trace("BarCode.GS1", "GS1GTINFormatString -- Error: GS1GTIN format string is empty!!!");

	HobString gs1BarcodeString = "";				// clear the buffer

												/*add first AI */
	gs1BarcodeString.append(rss14_AI_01_);

	if ( formatStrings[GS1GTIN].empty() )
		Debug::trace("BarCode.GS1", "GS1GTIN formatString is empty!!!");
	else
	{
		Debug::trace("BarCode.GS1", string("buildGS1BarcodeString() GS1GTIN formatString: ") + formatStrings[GS1GTIN]);

		string gs1GTIN = buildGS1BarcodeStringPart(formatStrings[GS1GTIN], productTransaction, withPrefix, false);
		Debug::trace("BarCode.GS1", string("buildGS1BarcodeString() gs1BarcodeString: ") + gs1BarcodeString);
		Debug::trace("BarCode.GS1", string("buildGS1BarcodeString() gs1GTIN: ") + gs1GTIN);

		gs1BarcodeString += gs1GTIN;
	}

	// NOTE:  This is where the check digit is calculated for the GTIN portion of the GS1 barcode.  This code pre-dated the change for the special rules for Walmart GS1 barcodes,
	//        and thus when building the GTIN portion with the special Walmart rules, we don't want to put on the check digit for the GTIN part, because it's gonig to be put on here!
	//        Additionally, if Walmart ever puts anything but 'X' and 'E' and fixed digit '0' in the GTIN format string, this check digit that will be calculated next will likely not be
	//        the check digit that would normally be calculated for the UPC barcode string they want in the E's of the format string.  So there you go!
	checkDigit += ComputeGS1ChecksumDigit(gs1BarcodeString);
	gs1BarcodeString += checkDigit;
	
	if ( formatStrings[GS1AI].empty() )
		Debug::trace("BarCode.GS1", "GS1AI formatString is empty!!!");
	else
	{
		const bool reduceGS1Length(Configuration::getInstance()->toBoolCfg(fieldOffsetCfgReduceGS1Length));

		Debug::trace("BarCode.GS1", string("buildGS1BarcodeString() GS1AI formatString: ") + formatStrings[GS1AI]);
		Debug::trace("BarCode.GS1", string("buildGS1BarcodeString() reduceGS1Length: ") + Integer::toString(reduceGS1Length));

		string gs1AI = buildGS1BarcodeStringPart(formatStrings[GS1AI], productTransaction, withPrefix, true);

		if (reduceGS1Length)
		{
			Debug::trace("BarCode.GS1", string("GS1AI dataString before reduction: ") + gs1AI);
			gs1AI = reduceGS1BarcodeString(gs1AI);
		}
		else
		{
			Debug::trace("BarCode.GS1", string("GS1AI dataString before reduction: ") + gs1AI);
			gs1AI = reduceGS1BarcodeString(gs1AI, false);
		}

		Debug::trace("BarCode.GS1", string("GS1AIdataString: ") + gs1AI);
		gs1BarcodeString += gs1AI;
	}

	Debug::trace("BarCode.GS1", "GS1 Barcode string finished: " + gs1BarcodeString);
	return gs1BarcodeString;
}

string BarCodeNumber::reduceGS1BarcodeString( const HobString &formatString, bool compressFields) const
{
	typedef std::pair<int, int> minMaxLengthValue;
	typedef std::map<int, minMaxLengthValue> minMaxMap;

	typedef std::list<std::string> stringList;

	typedef std::pair<int, std::string> aiValue;
	typedef std::list<aiValue> aiValueList;

	// create table of ai min/max values
	minMaxMap aiLength;
	aiLength[15] = minMaxLengthValue( 6, 6 );
	aiLength[16] = minMaxLengthValue( 6, 6 );
	aiLength[30] = minMaxLengthValue( 1, 8 );
	aiLength[422] = minMaxLengthValue( 3, 3 );
	aiLength[423] = minMaxLengthValue( 3, 15 );
	aiLength[424] = minMaxLengthValue( 3, 3 );
	aiLength[425] = minMaxLengthValue( 3, 15 );
	aiLength[426] = minMaxLengthValue( 3, 3 );
	aiLength[3102] = minMaxLengthValue( 6, 6 );
	aiLength[3103] = minMaxLengthValue( 6, 6 );
	aiLength[3202] = minMaxLengthValue( 6, 6 );
	aiLength[3203] = minMaxLengthValue( 6, 6 );
	aiLength[3922] = minMaxLengthValue( 1, 15 );
	aiLength[8008] = minMaxLengthValue( 8, 12 );

	// parse out ai strings
	stringList strings;

	int pos( 0 );
	int i( 1 );

	while ( i < (int) formatString.length() )
	{
		if ( '(' == formatString[i] )
		{
			strings.push_back( formatString.substr( pos, i - pos ) );
			pos = i;

			Debug::trace( "BarCode.GS1.reduce", HobString("found ai string ") + strings.back() );
		}

		++i;
	}

	strings.push_back( formatString.substr( pos, i - pos ) );

	Debug::trace( "BarCode.GS1.reduce", HobString("found ai string ") + strings.back() );

	// create list of ai values
	aiValueList ais;

	for ( stringList::const_iterator s( strings.begin() ); s != strings.end(); ++s )
	{
		const size_t split( s->find( ')' ) );

		if ( std::string::npos != split )
			if (( 1 < split ) && ( split < (s->length() - 1) ))
			{
				const std::string id( s->substr( 1, split - 1 ) );
				const std::string value( s->substr( split + 1 ) );

				Debug::trace( "BarCode.GS1.reduce", HobString("have ai ") + id + HobString(" ") + value );

				ais.push_back( aiValueList::value_type( util_string::stringtoint( id ), value ) );
			}
	}

	// reduce ai string values
	int numFields( 0 );

	for ( aiValueList::iterator ai( ais.begin() ); ai != ais.end(); ++ai )
	{
		const int id( ai->first );
		std::string value( ai->second );

		minMaxMap::const_iterator aiMinMax( aiLength.find( id ) );

		if ( aiMinMax == aiLength.end() )
			Debug::trace( "BarCode.GS1.reduce", HobString("missing ai ") + Integer::toString(id) );
		else
		{
			const int tmpMin( aiMinMax->second.first );
			const int tmpMax( aiMinMax->second.second );

			Debug::trace( "BarCode.GS1.reduce", HobString("ai tmpMin tmpMax: ") + Integer::toString(id) +
				HobString(" ") + Integer::toString(tmpMin) + HobString(" ") + Integer::toString(tmpMax) );

			Debug::trace( "BarCode.GS1.reduce", HobString("before: ") + value );

			while ( (int) value.length() < tmpMin)
				value.insert( 0, "0" );

			if (compressFields)
			{
				while ((tmpMin < (int)value.length()) && ('0' == value[0]))
					value.erase(0, 1);
			}

			Debug::trace( "BarCode.GS1.reduce", HobString("after: ") + value );

			// replace value
			ai->second = value;
		}

		++numFields;
	}

	// build ai string back up
	std::string result;

	for ( aiValueList::const_iterator ai( ais.begin() ); ai != ais.end(); ++ai )
	{
		const int id( ai->first );
		const std::string value( ai->second );

		--numFields;

		// append ai id
		result.push_back( '(' );
		result += Integer::toString(id);
		result.push_back( ')' );

		// append ai value
		result += value;

		minMaxMap::const_iterator aiMinMax( aiLength.find( id ) );

		if ( aiMinMax != aiLength.end() )
		{
			const int tmpMax( aiMinMax->second.second );

			// append terminator (FNC1) for variable length fields
			if (( (int) value.length() < tmpMax) && ( 0 < numFields ))
				result.push_back( '#' );
		}
	}

	Debug::trace( "BarCode.GS1.reduce", HobString("done! ") + result );
	return result;
}

string BarCodeNumber::buildGS1BarcodeStringPart(HobString formatString, ProductTransaction *productTransaction, bool withPrefix, bool withAI)
{
	unsigned int i;
	string barcode_string;
	short numChars = 0, priceChars = 0, weightChars = 0;
	bool load_error = false;
	char character;
	HobString priceStr("");
	HobString weightStr("");
	HobString transNumStr("");
	HobString scaleIdStr("");
	HobString bestBeforeStr("");
	HobString sellByStr("");
	HobString dateTimeStr("");
	HobString checkDigit("");

	bool traceOn = Debug::isTracing("BarCode.GS1");
	HobString msgDebug;
	if (traceOn)
	{
		Debug::trace("BarCode.GS1", "\nSTART buildGS1BarcodeStringPART()");
		msgDebug = "buildGS1BarcodeStringPart():   ";
	}
	

	if (formatString.empty())
	{
		Debug::trace("BarCode.GS1", "formatString -- Error: string is empty!!!");
		return barcode_string;
	}

	if (traceOn)
		Debug::trace("BarCode.GS1", msgDebug + "Setting up the priceStr & weightStr");



	HobString upcBarcodeString;

	bool walmartSpecial = specialRulesForWalmartGS1InEffect(formatString);
	if (walmartSpecial)
		upcBarcodeString = buildUPCBarcodeString(productTransaction, false, true, false);

	HobString gs1EanCode, gs1VendorNum, gs1ProductNum;
	// Determine the formatted EAN Code
	// setGS1GTINStrings(formatString, gs1EanCode);
	setEANStrings(formatString, gs1EanCode, gs1VendorNum, gs1ProductNum);


	//	Find the number of digits for the Price field
	if ( 0 < (priceChars = formatString.countConsecutiveChars( 'P' )) )
	{
		if ( !productTransaction )
			priceStr = HobString( priceChars, '0' );	//	Put Zeros in the price field - there's no data for it.
		else
		{
			Product *prod = productTransaction->getProduct();
			long price = productTransaction->getTotalPrice();

			if (( prod ) && ( Product::PRINTING_RUN_TOTALS_LABEL == prod->getLabelBeingPrinted() ))
			{
				Session *session = SessionManager::getInstance()->getPrimaryConsoleSession();
				ProductRunTotal *productRunTotal = session ? session->getProductTotalManager()->getCurrentProductRunTotal() : ((ProductRunTotal*) NULL);

				if ( productRunTotal )
					price = productRunTotal->getPrice();
			}

			//	Make sure the Price will fit in the Price field
			if ( valueIsTooBig( priceChars, price ) )
				priceStr = HobString( priceChars, '0' );	//	Put Zeros in the price field - it's too big to fit.
			else
			{
				priceStr = Long::toString( price );
				priceStr.getCharsWithPadding( priceChars );
			}
		}
	}

	//	Find the number of digits for the Weight field
	if ( 0 < (weightChars = formatString.countConsecutiveChars( 'W' )) )
	{
		Configuration *pConfig = Configuration::getInstance();

		if ( !productTransaction )
			weightStr = HobString( weightChars, '0' );	//	Put Zeros in the weight field - there's no data for it.
		else
		{
			Product *prod = productTransaction->getProduct();
			HobWeight wght = productTransaction->getTotalNetWeight();

			if (( prod ) && ( Product::PRINTING_RUN_TOTALS_LABEL == prod->getLabelBeingPrinted() ))
			{
				Session *session = SessionManager::getInstance()->getPrimaryConsoleSession();
				ProductRunTotal *productRunTotal = session ? session->getProductTotalManager()->getCurrentProductRunTotal() : ((ProductRunTotal*) NULL);

				if ( productRunTotal )
					wght = productRunTotal->getWeight();
			}

            if (((prod) && !prod->hasKbdNetWeightLabel()) && ((MetSig::isScaleAvoir()) && (MetSig::isWeigherConfiguredForSingleRange())))
            {
                //Debug::trace("BarCode.GS1", string("buildGS1BarcodeStringPart() TRIM DIGIT W: ") + Integer::toString(wght) );
                wght = MetSig::divideWeightForSingleRange(wght);
                //Debug::trace("BarCode.GS1", string("buildGS1BarcodeStringPart() AFTER TRIM DIGIT W: ") + Integer::toString(wght));
            }

			//	Make sure the Weight will fit in the Weight field
			if ( valueIsTooBig( weightChars, wght ) )
				weightStr = HobString( weightChars, '0' );	//	Put Zeros in the weight field - it's too big to fit.
			else
			{
				weightStr = Long::toString( wght );
				weightStr.getCharsWithPadding( weightChars );
			}
		}
	}

	for ( i = 0; i < formatString.length(); )
	{
		character = formatString.at(i);
		if (character == 'E')
		{
			Debug::trace("BarCode.GS1", string("buildGS1BarcodeStringPart() char=E barcode_string: ") + barcode_string);

			/* Find the number of digits for the EAN Code  */
			numChars = formatString.countConsecutiveChars('E', i);
				
			if (walmartSpecial)
				// Not going to worry about how many characters are in the upcBarcodeString vs the number of 'E' characters in the format string.
				// The format string was always going to have 10 E's, and the upcBarcodeString should always have 10 digits.
				barcode_string.append(upcBarcodeString);  
			else
				barcode_string.append(gs1EanCode);

			i += numChars;
		}
		else
		if (character == 'V')
		{
			/* Find the number of digits for the Vendor number  */
			numChars = formatString.countConsecutiveChars('V', i);
			barcode_string.append(gs1VendorNum);
			i += numChars;
		}
		else
		if (character == 'N')
		{
			/* Find the number of digits for the Product Number  */
			numChars = formatString.countConsecutiveChars('N', i);
			barcode_string.append(gs1ProductNum);
			i += numChars;
		}
		else
		if (character == 'P')
		{
			if (withAI)
				barcode_string.append(rss14_AI_3922_);
			barcode_string.append(priceStr);
			i += priceChars;
		}
		else
		if (character == 'p')
		{
			//	Find the number of digits for the Price check field
			numChars = formatString.countConsecutiveChars('p', i);
			if ((numChars != 1) || (priceChars < 4) || (priceChars > 5) || priceStr.empty())
				barcode_string.append(numChars, '0');	//	Put Zeros in the price check field - it's too big to fit.
			else
			{
				HobString tempStr("");
				if (priceChars == 4)
					tempStr = EAN4DigitValueCheck(priceStr);
				else
					tempStr = EAN5DigitValueCheck(priceStr);

				barcode_string.append(tempStr);
			}
			i += numChars;
		}
		else
		if (character == 'W')
		{
			if ( MetSig::isScaleMetric() )
			{
				if (withAI)
					barcode_string.append(rss14_AI_3103_);
			}
			else
			{
				bool isDualRange = false;
				if ( productTransaction && productTransaction->getWeigherStatus() && productTransaction->getWeigherStatus()->isDualRangeEnabled() )
					isDualRange = true;
				if (withAI)
					barcode_string.append((isDualRange) ? rss14_AI_3203_ : rss14_AI_3202_);
			}
			barcode_string.append(weightStr);
			i += weightChars;
		}
		else
		if (character == 'w')
		{
			//	Find the number of digits for the Weight check field
			numChars = formatString.countConsecutiveChars('w', i);
			if ((numChars != 1) || (weightChars < 4) || (weightChars > 6) || weightStr.empty())
				barcode_string.append(numChars, '0');	//	Put Zeros in the Weight check field - it's too big to fit.
			else
			{
				HobString tempStr("");
				if (weightChars == 4)
					tempStr = EAN4DigitValueCheck(weightStr);
				else
					tempStr = EAN5DigitValueCheck(weightStr);
				barcode_string.append(tempStr);
			}
			i += numChars;
		}
		else
		if (character == 'Q')
		{
			HobString byCountStr("");

			//	Find the number of digits for the By Count field
			numChars = formatString.countConsecutiveChars('Q', i);
			if (!productTransaction)
				byCountStr = HobString(numChars, '0');	//	Put Zeros in the By Count field - there's no data for it.
			else
			{
				//	Make sure the By Count quantity will fit in the By Count field
				load_error = valueIsTooBig(numChars, productTransaction->getByCount());
				if (load_error)
					byCountStr = HobString(numChars, '0');	//	Put Zeros in the By Count field - it's too big to fit.
				else
				{
					byCountStr = Long::toString(productTransaction->getByCount());
					byCountStr.getCharsWithPadding(numChars);
				}
			}
			if (withAI)
				barcode_string.append(rss14_AI_30_);

			//	Append the entire string
			barcode_string.append(byCountStr);
			i += numChars;
		}
		else
		if (character == 'X')
		{
			//	Find the number of digits for the Prefix field
			numChars = formatString.countConsecutiveChars('X', i);
			if ( withPrefix )
				barcode_string.append(getFormattedString(dbPrefix, numChars));

			i += numChars;
		}
		else
		if (character == 'B')
		{
			//Find the number of digits for best before date
			numChars = formatString.countConsecutiveChars('B', i);

			HobString str = ((productTransaction->getProduct())->getProductLife()).formatDate(FormatDate_YYMMDD_NoDelimiter);
			load_error = valueIsTooBig(numChars, (long) str.size());
			if (load_error)
				bestBeforeStr = HobString(numChars, '0');	//	Put Zeros in the By Count field - it's too big to fit.
			else
			{
				bestBeforeStr = str;
				bestBeforeStr.getCharsWithPadding(numChars);
			}
			if (withAI)
				barcode_string.append(rss14_AI_15_);

			//	Append the entire string
			barcode_string.append(bestBeforeStr);

			Debug::trace("BarCode.GS1", "Best before: " + bestBeforeStr);
			i += numChars;
		}
		else
		if (character == 'C')
		{
			numChars = formatString.countConsecutiveChars('C', i);

			HobString str = ((productTransaction->getProduct())->getShelfLife()).formatDate(FormatDate_YYMMDD_NoDelimiter);
			Debug::trace("BarCode.GS1", "shelf life: " + str);

			load_error = valueIsTooBig(numChars, (long) str.size());
			if (load_error)
				sellByStr = HobString(numChars, '0');	//	Put Zeros in the By Count field - it's too big to fit.
			else
			{
				sellByStr = str;
				sellByStr.getCharsWithPadding(numChars);
			}
			if (withAI)
				barcode_string.append(rss14_AI_16_); // SOF-861

			//	Append the entire string
			barcode_string.append(sellByStr);

			Debug::trace("BarCode.GS1", "Sell by String: " + sellByStr);
			i += numChars;

		}
		else
		if (character == 'D')
		{
			numChars = formatString.countConsecutiveChars('D', i);

			HobDateTime time;

			//std::string str = time.formatDate(FormatDate_MMDDYY_NoDelimiter);
			std::string str = time.formatDate(FormatDate_YYMMDD_NoDelimiter);
			str += time.formatTime(FormatTime_HHMMSS_NoDelimiter, HourFormat_MILITARY);
			int strSize = (int) str.size();
			Debug::trace("BarCode.GS1", "Data time: " + str + " Num D characters: " + Integer::toString(numChars));

			if (strSize > numChars)
			{
				dateTimeStr = HobString(numChars, '0');	//	Put Zeros in the By Count field - it's too big to fit.
			}
			else
			{
				dateTimeStr = str;
				dateTimeStr.getCharsWithPadding(numChars);
			}
			if (withAI)
				barcode_string.append(rss14_AI_8008_);

			//	Append the entire string
			barcode_string.append(dateTimeStr);

			Debug::trace("BarCode.GS1", "Data time: " + dateTimeStr);
			i += numChars;
		}
		else
		if ((character >= '0') && (character <= '9'))
		{
			barcode_string.append(1, character);
			++i;
		}
		else
		{
			++i;
		}

		Debug::trace("BarCode.GS1.loop", string("buildGS1BarcodeStringPart() barcode_string: ") + barcode_string);
	}

	return barcode_string;
}

HobString BarCodeNumber::ComputeGS1ChecksumDigit(HobString barcodeString)
{
	int i, oddSum = 0, evenSum = 0, checkSum = 0;
	HobString tempString("");

	/*strip tag from our data before computing the checksum */
	std::size_t tagPos = barcodeString.find(')');
	if (tagPos != std::string::npos)
	{
		tempString += barcodeString.substr((tagPos + 1));
	}
	else
	{
		tempString += barcodeString;
	}
	Debug::trace("BarCode.GS1", "tempString: " + tempString);
	Debug::trace("BarCode.GS1", "tempString.length: " + Integer::toString(tempString.length()));

	if (tempString.length() > 0 && tempString.length() <= 13)
	{
		Debug::trace("BarCode.GS1", "Compute");
		tempString.makeBinary();
		std::string::iterator iter = tempString.begin();

		for (i = 0; iter != tempString.end(); i++, iter++)
		{
			if (i % 2)
			{
				oddSum += *iter;
			}
			else
			{
				evenSum += *iter;
			}
		}
		checkSum = (10 - (((3 * evenSum) + oddSum) % 10)) % 10;
	}
	Debug::trace("BarCode.GS1", "Computed checksum: " + Integer::toString(checkSum));
	return (Integer::toString(checkSum));
}



HobString BarCodeNumber::buildDataMatrixString(HobString formatString, ProductTransaction *productTransaction, bool withPrefix)
{
	if ( formatString.empty() )
		return "";

	Configuration *config(Configuration::getInstance());
	Product *product(productTransaction ? productTransaction->getProduct() : NULL);

	HobString dataMatrixBarcodeString = "";

	HobString formattedUPCA = buildUPCBarcodeString(productTransaction);
	HobString formattedEAN13 = buildEAN13BarcodeString(productTransaction);

	formats['E'] = dbEanCode;
	formats['N'] = dbProductNumber;
	formats['V'] = dbVendorNumber;
	formats['X'] = dbPrefix;

	formats['u'] = formattedUPCA;
	formats['e'] = formattedEAN13;

	//
	// setup format fields
	//

	HobString field;

	if (product)
	{
		//if (formattedDate.length() > 0)
		//	formats['D'] = formattedDate;
		//else
		//	formats['D'] = "";

		formats['B'] = product->getProductLife().formatDate(FormatDate_YYMMDD_NoDelimiter);
		formats['C'] = product->getShelfLife().formatDate(FormatDate_YYMMDD_NoDelimiter);

		field = product->getDescription();
		field.stripFontSequences();

		formats['I'] = field;
		formats['L'] = Integer::toString(product->getProductNum());
		formats['o'] = Integer::toString(originalUnitPrice);
		formats['Y'] = Integer::toString(product->getProductType());
	}

	if ( productTransaction )
	{
		char buffer[256];

		Product *prod = productTransaction->getProduct();
		long price = productTransaction->getTotalPrice();
		long wght = productTransaction->getTotalNetWeight();

		if (( prod ) && ( Product::PRINTING_RUN_TOTALS_LABEL == prod->getLabelBeingPrinted() ))
		{
			Session *session = SessionManager::getInstance()->getPrimaryConsoleSession();
			ProductRunTotal *productRunTotal = session ? session->getProductTotalManager()->getCurrentProductRunTotal() : ((ProductRunTotal*) NULL);

			if ( productRunTotal )
			{
				price = productRunTotal->getPrice();
				wght = productRunTotal->getWeight();
			}
		}

		formats['F'] = getFormattedWeightStr(productTransaction);
		formats['f'] = getFormattedTareStr(productTransaction);
		formats['O'] = Integer::toString(productTransaction->getUnitPrice());
		formats['P'] = Integer::toString(price);

		sprintf(buffer, "%04ld", price);
		field = buffer;

		if (4 == field.length())
			formats['p'] = EAN4DigitValueCheck(field);

		if (field.length() <= 5)
		{
			while (field.length() <= 5)
				field.insert(0, 1, '0');

			formats['K'] = EAN5DigitValueCheck(field);
		}

		formats['Q'] = Integer::toString(productTransaction->getByCount());
		formats['R'] = Integer::toString(productTransaction->getTotalTare());
		formats['T'] = Integer::toString(productTransaction->getTransactionNumberForBarcode());
		formats['W'] = Integer::toString(wght);

		sprintf(buffer, "%04ld", wght);
		field = buffer;

		if (4 == field.length())
			formats['w'] = EAN4DigitValueCheck(field);

		if (field.length() <= 5)
		{
			while (field.length() <= 5)
				field.insert(0, 1, '0');

			formats['k'] = EAN5DigitValueCheck(field);
		}
	}

	formats['d'] = config->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
	formats['M'] = Integer::toString(config->getPricePrecision());
	formats['S'] = config->toStringCfg(fieldOffsetCfgAccumItemsScaleId);

	if (MetSig::isScaleMetric())
		formats['Z'] = "3";
	else
	{
		bool isDualRange(false);

		if (productTransaction)
			if ((productTransaction->getWeigherStatus()) && (productTransaction->getWeigherStatus()->isDualRangeEnabled()))
				isDualRange = true;

		formats['Z'] = (isDualRange) ? "3" : "2";
	}

	if (MetSig::isScaleMetric())
		formats['z'] = "0";
	else
		formats['z'] = "1";

	/// FNC1
	formats['^'] = 1;		// dont know what this means

	HobString testerStr = "";
	// log out formats
	for (formatMapping::const_iterator i(formats.begin()); i != formats.end(); ++i)
	{
		if ('^' == i->first)
			continue;			// ok, so nothing to add - end of var length item

		char c[16];
		c[0] = i->first;
		c[1] = 0;

		if (formatString.find(c[0]) != std::string::npos)
		{
			char buffer[256];
			sprintf(buffer, "%s %-128.128s", c, i->second.c_str());		// copy a formatted string into the buffer
			dataMatrixBarcodeString += i->second.c_str();
		}
	}
	Debug::trace("BarCode.DataMatrix", dataMatrixBarcodeString);
	return dataMatrixBarcodeString;
}


void BarCodeNumber::operator=(const BarCodeNumber& barcode)
{
	Debug::trace("BarCode","---- Assigning barcodenumbers THIS barcode");
	show();
	Debug::trace("BarCode","---- Assigning barcodenumbers RHS");
	barcode.show();

	setBarcodeType(barcode.getBarcodeType());

	// SOF-3834
	//setPrefix(barcode.getPrefix());
	//setUPCNumberSystem(barcode.getUPCNumberSystem());
	dbPrefix = barcode.dbPrefix;
	upcPrefix = barcode.upcPrefix;

	setEANCode(barcode.getEANCode());
	dbVendorNumber = barcode.dbVendorNumber;
	dbProductNumber = barcode.dbProductNumber;
	rawBarCodeString = barcode.rawBarCodeString;
	mostSignificantUPCDigitReplacement = barcode.mostSignificantUPCDigitReplacement;
	swappingOfEanCodeAndProductNumberAllowed = barcode.swappingOfEanCodeAndProductNumberAllowed;

	for (int i = 0; i < MAX_BARCODE_TYPES; i++)
	{
		formatStrings[i] = barcode.getFormatString((BarcodeType)i);
		barcodeStrings[i] = barcode.getBarcodeString((BarcodeType)i);
	}

	originalUnitPrice = barcode.originalUnitPrice;
	currentDateTimeStr = barcode.currentDateTimeStr;

	formats = barcode.formats;

	relevantFields = barcode.getRelevantFields();

	Debug::trace("BarCode", "---- After assignment THIS barcode");
	show();
}


int BarCodeNumber::getRelevantFields() const
{
	Debug::trace("BarCode", HobString("getRelevantFields()  relevantFields = ") + HobString(Integer::toString(relevantFields)));
	return relevantFields;
}


std::string BarCodeNumber::getMostSignificantUPCDigit()
{
	HobString upcBarcodeString = buildUPCBarcodeString(0, false, false, false);
	if (!upcBarcodeString.empty())
		return upcBarcodeString.substr(0, 1);
	return "";
}

void BarCodeNumber::replaceMostSignificantUPCDigit(HobString digit)
{
	if (digit.length() == 1)
	{
		mostSignificantUPCDigitReplacement = digit;
		Debug::trace("BarCode", string("replaceMostSignificantUPCDigit to ") + digit);
	}
}


std::string BarCodeNumber::getFormattedWeightStr( ProductTransaction *productTransaction ) const
{
	bool useDefaultDecimalSeparator( false );

	Configuration *config( Configuration::getInstance() );
	Product *product( productTransaction ? productTransaction->getProduct() : NULL );

	if ( !product )
		return "";

	HobWeight wght = productTransaction->getTotalNetWeight();

	if (( product ) && ( Product::PRINTING_RUN_TOTALS_LABEL == product->getLabelBeingPrinted() ))
	{
		Session *session = SessionManager::getInstance()->getPrimaryConsoleSession();
		ProductRunTotal *productRunTotal = session ? session->getProductTotalManager()->getCurrentProductRunTotal() : ((ProductRunTotal*) NULL);

		if ( productRunTotal )
			wght = productRunTotal->getWeight();
	}

	HobString netWeightString;

	if ( product->isByCount() )
	{
		if ( 1 == productTransaction->getByCount() )
		{
			if ( config->toBoolCfg( fieldOffsetCfgPFOneOnByCountPrinted ) )
				netWeightString = "1";
			else if (( config->toBoolCfg( fieldOffsetCfgPFWeightOnByCountPrinted ) ) && ( wght > 0 ))
			{
				short formatIndicator( AppendWeightText );

				if ( productTransaction->getWeigherStatus() )
				{
					const HfcWeigherStatus *weight( productTransaction->getWeigherStatus() );

					if (( weight->isDualRangeEnabled() ) && ( weight->getDualRangeDirection() == DualRangeDown ))
						formatIndicator |= DualRangeModeDown;
				}

				config->formatWeight( wght, netWeightString, useDefaultDecimalSeparator, formatIndicator );
			}
		}
		else if ( 1 < productTransaction->getByCount() )
		{
			netWeightString = Integer::toString( productTransaction->getByCount() );
		}
	}
	else if ( product->isRandomWeight() )
	{
		short formatIndicator( AppendWeightText );

		if ( productTransaction->getWeigherStatus() )
		{
			const HfcWeigherStatus *weight( productTransaction->getWeigherStatus() );

			if (( weight->isDualRangeEnabled() ) && ( weight->getDualRangeDirection() == DualRangeDown ))
				formatIndicator |= DualRangeModeDown;

			// FT-1695 - Don't print the weight for a side item for BuildAMeal that is random weight,
			// since we aren't actually weighing the item.  Previously, this call to formatWeight was below the brace that immediately follows it.
			config->formatWeight( wght, netWeightString, useDefaultDecimalSeparator, formatIndicator );
		}
	}

	return netWeightString;
}

std::string BarCodeNumber::getFormattedTareStr( ProductTransaction *productTransaction ) const
{
	bool useDefaultDecimalSeparator( false );

	Configuration *config( Configuration::getInstance() );
	Product *product( productTransaction ? productTransaction->getProduct() : NULL );

	if ( !product )
		return "";

	HobString totalTareString;

	if (( product->isRandomWeight() ) ||
			(( product->isByCount() ) && ( productTransaction->getByCount() == 1 ) && ( config->toBoolCfg( fieldOffsetCfgPFWeightOnByCountPrinted ) )))
	{
		short formatIndicator( AppendWeightText );

		if ( productTransaction->getWeigherStatus() )
		{
			const HfcWeigherStatus *weight( productTransaction->getWeigherStatus() );

			if (( weight->isDualRangeEnabled() ) && ( weight->getDualRangeDirection() == DualRangeDown ))
				formatIndicator |= DualRangeModeDown;
		}

		config->formatWeight( productTransaction->getTotalTare(), totalTareString, useDefaultDecimalSeparator, formatIndicator );
	}

	return totalTareString;
}

void BarCodeNumber::show() const
{
	if (Debug::isTracing("BarCode"))
	{
		Debug::trace("BarCode", "---- Showing data elements for BarCode object " + util_string::ptrtostring(this));

		Debug::trace("BarCode", "     RelevantFields   = " + util_string::inttostring(relevantFields));
		Debug::trace("BarCode", "     BarcodeType      = " + util_string::inttostring(barcodeType));
		Debug::trace("BarCode", "     dbPrefix         = " + dbPrefix);
		Debug::trace("BarCode", "     upcPrefix        = " + upcPrefix);
		Debug::trace("BarCode", "     dbVendorNumber   = " + dbVendorNumber);
		Debug::trace("BarCode", "     dbProductNumber  = " + dbProductNumber);
		Debug::trace("BarCode", "     dbEANCode        = " + dbEanCode);
		Debug::trace("BarCode", "     rawBarCodeString = " + rawBarCodeString);
		Debug::trace("BarCode", "     mostSignificantUPCDigitReplacement = " + mostSignificantUPCDigitReplacement);
		Debug::trace("BarCode", string("     swappingOfEanCodeAndProductNumberAllowed = ") + Bool::toString(swappingOfEanCodeAndProductNumberAllowed));
		Debug::trace("BarCode", "     EANFormatString        = " + formatStrings[EAN13]);
		Debug::trace("BarCode", "     EAN128CFormatString    = " + formatStrings[EAN128C]);
		Debug::trace("BarCode", "	  GS1GTINFormatString    = " + formatStrings[GS1GTIN]);
		Debug::trace("BarCode", "	  GS1AIFormatString      = " + formatStrings[GS1AI]);
		Debug::trace("BarCode", "	  QRCodeFormatString     = " + formatStrings[QR_CODE]);
		Debug::trace("BarCode", "	  DataMatrixFormatString = " + formatStrings[DATAMATRIX]);
		showBarcodeStrings();
		Debug::trace("BarCode", "---- Ended showing data elements for BarCode object " + util_string::ptrtostring(this));
	}
}

void BarCodeNumber::showBarcodeStrings() const
{
	if (Debug::isTracing("BarCode"))
	{
		Debug::trace("BarCode", "---- Showing barcodeStrings  for BarCode object " + util_string::ptrtostring(this));

		Debug::trace("BarCode", "     UPCA barcodeString       = " + barcodeStrings[UPCA]);
		Debug::trace("BarCode", "     EAN barcodeString        = " + barcodeStrings[EAN13]);
		Debug::trace("BarCode", "     EAN128C barcodeString    = " + barcodeStrings[EAN128C]);
		Debug::trace("BarCode", "	  GS1GTIN barcodeString    = " + barcodeStrings[GS1GTIN]);
		Debug::trace("BarCode", "	  GS1AI barcodeString      = " + barcodeStrings[GS1AI]);
		Debug::trace("BarCode", "	  QRCode barcodeString     = " + barcodeStrings[QR_CODE]);
		Debug::trace("BarCode", "	  DataMatrix barcodeString = " + barcodeStrings[DATAMATRIX]);
		Debug::trace("BarCode", "---- Ended showing data elements for BarCode object " + util_string::ptrtostring(this));
	}
}


HobString BarCodeNumber::getFormattedString(const HobString source, int numChars) const
{
	HobString result = source;
	result.getCharsWithPadding(numChars);
	return result;
}

bool BarCodeNumber::isDiscountPricePriceInBarcode(Product *prod)
{
	Debug::trace("BarCode.DiscountPrice", "BarcodeNumber::isDiscountPricePriceInBarcode() START " );

	bool foundFS = false;
	bool foundDiscountFlagOn = false;
	int printerNumber = FirstPrinter;

	while (printerNumber < MaxPrinterTypes && !foundFS)
	{
		LabelType *myLabelType(prod ? prod->getLabelType(printerNumber) : NULL);
		if( myLabelType && myLabelType->labelHasFrequentShopper() )
		{
			foundFS = true;
			Debug::trace("BarCode.DiscountPrice", "BarcodeNumber::isDiscountPricePriceInBarcode() FOUND Frequent Shopper field in the label for printer " + Integer::toString(printerNumber));
			// Now see if this label type for this printer, has the flag turned on
			if(  myLabelType->labelHasDiscountPriceInBarcode() )
			{
				foundDiscountFlagOn = true;
				Debug::trace("BarCode.DiscountPrice", "BarcodeNumber::isDiscountPricePriceInBarcode() FOUND Frequent Shopper flag ENABLED for the barcode for printer " + Integer::toString(printerNumber));
			}
		}
		
		++printerNumber;
	}

	Debug::trace("BarCode.DiscountPrice", "isDiscountPricePriceInBarcode() Returning FLAG foundDiscountFlagOn: " + booltostring(foundDiscountFlagOn));

	return foundDiscountFlagOn;
}
