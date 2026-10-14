/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef BARCODENUMBER_H
#define BARCODENUMBER_H

#include "evolution.h"
#include "Barcode.h"
#include "HobString.h"

#include <map>

//#include "Product.h"
//#include "ProductTransaction.h"

class Product;
class ProductDefault;

namespace edb 
{
class DBProduct;
}
class ProductTransaction;

class DBAccess;


/**
 * DPS-5.0.0.B
 * Bar Codes supported include:
 * UPC-E, EAN, RS14??
 */

//message Status 
typedef enum RelevantFields
{
	UseNoFields = 0x00,
	UseProductNumber = 0x01,
	UseVendorNumber = 0x02,
	UseEanCode = 0x04
}   RelevantFields_t;


class BarCodeNumber
{
	public:
		BarCodeNumber(const BarCodeNumber&);
		BarCodeNumber();
		~BarCodeNumber();

		short getBarcodeType() const;
		void setBarcodeType(short barcodeType);

		//HobString getBarcodeString() const;
		HobString getBarcodeString(BarcodeType type) const;
		HobString getBarcodeStringFromFormat(const HobString &format);
		void setBarcodeString(BarcodeType bcType, std::string formatString);  // Needed for USER_SUPPLIED (Walmart Cake Label project)

		void getFromDatabase(edb::DBProduct *productRec, Product *product, DBAccess *dbAccess);

		bool isEanCodeFromRaw() const { return dbEanCode.empty() ? true : false; }   // { return eanCodeFromRaw; }
		bool isVendorNumberFromRaw() const { return dbVendorNumber.empty() ? true : false; }   // { return vendorNumberFromRaw; }
		bool isProductNumberFromRaw() const { return dbProductNumber.empty() ? true : false; }   // { return productNumberFromRaw; }


		HobString getPrefix() const;
		void setPrefix(HobString prefix);

		HobString getEANCode() const;
		HobString getDerivedEANCode() const;
		void setEANCode(HobString EANCode);

		HobString getVendorNumber() const;
		HobString getDerivedVendorNumber() const;
		void setVendorNumber(HobString vendNum);

		HobString getProductNumber() const;
		HobString getDerivedProductNumber() const;
		void setProductNumber(HobString prodNum);

		HobString getUPCNumberSystem() const;
		void setUPCNumberSystem(HobString UPCNumberSystem);

		HobString getUPCNumber() const;
		void setUPCNumber(HobString upcNum);

		//bool setUPCNumberFromEANCode();		// LAP-BARCODE
		//bool setEANCodeFromUPCNumber();		// LAP-BARCODE

		HobString getFormatString(BarcodeType bcType) const;
		void setFormatString(BarcodeType bcType, std::string formatString);
		void setFormatStrings(ProductDefault* productDefault = NULL);


		bool valueIsTooBig(int chars, long value);

		HobString UPCAPriceCheck(HobString priceString);
		HobString ComputeUPCAChecksumDigit(HobString barcodeString);
		HobString ComputeEANChecksumDigit(HobString barcodeString);
		HobString ComputeGS1ChecksumDigit(HobString barcodeString);
		HobString EAN4DigitValueCheck(HobString value);
		HobString EAN5DigitValueCheck(HobString value);

		void buildBarcodeStrings(ProductTransaction *productTransaction, bool withPrefix = true);

		int getRelevantFields() const;

		std::string getMostSignificantUPCDigit();
		void replaceMostSignificantUPCDigit(HobString digit);

		void allowswappingOfEanCodeAndProductNumber(bool allow) { swappingOfEanCodeAndProductNumberAllowed = allow; }

		void operator=(const BarCodeNumber& barcode);

		void show() const;
		void showBarcodeStrings() const;

		void determineBarcodeTypeDeterminant(Product *product);

		void setDiscountPriceOnBarcode(bool value);

	private:

		/** DPS-5.0.0.A.
		 * The following number systems will be supported:
		 * 0, 2, 4, 6, 7.
		 *
		 * There are additional business rules that will have to be mined from the current Ultima software.
		 */
		short		barcodeType;
		short		barcodeTypeDeterminant;

		HobString	barcodeStrings[MAX_BARCODE_TYPES];

		/* This attribute is used to indicate which of the fields,
		 * product number, vendor number, or ean code is relevant for 
		 * the label type selected for the product.
		 * The methods setEAN13Strings() and setUPCStrings() set up
		 * this field.  The relevant fields are then sent to the Browser
		 * so it knows which to display, and allow the user to update.
		 */
		int relevantFields;

		typedef enum
		{ RSS14_GTIN12_RW_Format, RSS14_GTIN12_FW_Format, RSS14_GTIN12_BC_Format,
		  RSS14_GTIN13_RW_Format, RSS14_GTIN13_FW_Format, RSS14_GTIN13_BC_Format,
		  RSS14_RW_Format, RSS14_FW_Format, RSS14_BC_Format } RSS14BarcodeType_;

		//HobString	prefix;
		HobString	upcPrefix;
		HobString	dbPrefix;

		HobString	dbEanCode;
		HobString	dbVendorNumber;
		HobString	dbProductNumber;
		HobString	rawBarCodeString;

		HobString	formatStrings[MAX_BARCODE_TYPES];

		HobString	mostSignificantUPCDigitReplacement;

		HobPrice	originalUnitPrice;
		HobString   currentDateTimeStr;

		typedef std::map<char, HobString> formatMapping;
		formatMapping formats;

		bool swappingOfEanCodeAndProductNumberAllowed;
		bool isDPEnabled;

		static int EAN5WeightedProduct[10];

		// GS1
		static const std::string rss14_AI_01_ ;			//= "(01)";
		static const std::string rss14_AI_15_;			//= "(15)";  'B' (Best Before)
		static const std::string rss14_AI_16_;          //= "(16)";  'C' (Sell By)                // SOF-861
		static const std::string rss14_AI_30_; 			//= "(30)";  'Q' (By count quantity)

		static const std::string rss14_AI_3922_;		//= "(3922)";'P' (Price)
		static const std::string rss14_AI_422_; 		//= "(422)";
		static const std::string rss14_AI_423_; 		//= "(423)";
		static const std::string rss14_AI_424_; 		//= "(424)";
		static const std::string rss14_AI_425_; 		//= "(425)";
		static const std::string rss14_AI_426_;			//= "(426)";
		static const std::string rss14_AI_3103_; 		//= "(3103)";'W' (Metric Weight)
		static const std::string rss14_AI_3202_; 		//= "(3202)";'W' (Non-Metric Weight)
		static const std::string rss14_AI_3203_; 		//= "(3203)";'W' (Non-Metric Weight, dual-range)
		static const std::string rss14_AI_8008_; 		//= "(8008)";'D' (Date and Time of Production)
		static const std::string rss14_AI_;		 		//= "WWWWWWPPPPPP";


		void init();

		void setUPCStrings(HobString &upcPrefix, HobString &upcVendorNum, HobString &upcProductNum) const;
		HobString buildUPCBarcodeString(ProductTransaction *productTransaction, bool withPrefix = true, bool replaceMostSignificantUPCDigit = true, bool withFinalCheckDigit = true);

		void setEANStrings(HobString formatString, HobString &eanCode, HobString &eanVendorNum, HobString &eanProductNum) const;
		HobString buildEAN13BarcodeString(ProductTransaction *productTransaction, bool withPrefix = true);
		HobString buildEAN128CBarcodeString(ProductTransaction *productTransaction, bool withPrefix = true);
		HobString buildEANBarcodeString(HobString formatString, ProductTransaction *productTransaction, bool withPrefix = true);

		bool specialRulesForWalmartGS1InEffect(HobString formatString) const;

		void setGS1GTINStrings(HobString formatString, HobString &gs1EanCode) const;
		HobString buildGS1BarcodeString(ProductTransaction *productTransaction, bool withPrefix = true);
		std::string buildGS1BarcodeStringPart(HobString formatString, ProductTransaction *productTransaction, bool withPrefix, bool withAI);
		std::string reduceGS1BarcodeString(const HobString &formatString, bool compressFields = true) const;

		HobString buildDataMatrixString(HobString formatString, ProductTransaction *productTransaction, bool withPrefix = true);

		std::string getFormattedWeightStr( ProductTransaction *productTransaction ) const;
		std::string getFormattedTareStr( ProductTransaction *productTransaction ) const;

		HobString getFormattedString(const HobString source, int numChars) const;

		bool isDiscountPricePriceInBarcode(Product *prod);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //BARCODENUMBER_H


