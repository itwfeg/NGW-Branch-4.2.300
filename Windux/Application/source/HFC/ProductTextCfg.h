/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTTEXTCFG_H
#define PRODUCTTEXTCFG_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

#include "ProductText.h"

#include "DBProductTextCfg.h"

class DBAccess;


typedef enum ProductTextCfgFieldOffsets
{
	fieldOffsetPTCTextType = 1,
	fieldOffsetPTCPrinterNum,
	fieldOffsetPTCLabelNum,
	endProductTextCfgFieldOffsets
}   ProductTextCfgFieldOffsets_t;

typedef enum ProductTextPrinterChoices
{
	PrinterChoice_PrimaryPrinter,
	PrinterChoice_SecondPrinter,
	PrinterChoice_ThirdPrinter,
	PromptForPrinter = 9,
	UnspecifiedPrinter
}   ProductTextPrinterChoices_t;


class ProductTextCfg : public HfcObject
{
	public:

		ProductTextCfg(ProdSupportTextType textType);
		ProductTextCfg(const char *data, int len, int formatType);
		ProductTextCfg(edb::DBProductTextCfg *dbProductTextCfgRec, DBAccess *access = (DBAccess *)NULL);

		virtual ~ProductTextCfg();


		ProdSupportTextType getTextType() const { return textType_; }
		void setTextType(ProdSupportTextType type) { textType_ = type; }

		ProductTextPrinterChoices getPrinterNum() const { return printerNum_; }
		void setPrinterNum(ProductTextPrinterChoices num) { printerNum_ = num; }

		HobRecordNum getLabelNum() const { return labelNum_; }
		void setLabelNum(HobRecordNum num) { labelNum_ = num; }

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode code);


		void setDBAccess(DBAccess *dbAccess) { dbAccess_ = dbAccess; }

		edb::DBProductTextCfg *getMyDbRecord() { return myDbRecord_; }

		bool isRecordValid() { return true; }

		static ProductTextCfg *findProductTextCfg(ProdSupportTextType type, DBAccess *dbAccess);
		virtual void updateProductTextCfg(ProductTextCfg *newText);
		void saveRecord();
		int deleteRecord();

		const char *serialize(int format) const;

		
		static void initFieldList();

		static void verifyDefaultRecords(DBAccess *dbAccess);
		static void verifyDefaultRecord(ProdSupportTextType type, HobRecordNum labelNum, DBAccess *dbAccess);

		static bool validateDefaultRecords(DBAccess *dbAccess);
		static bool validateDefaultRecord(ProdSupportTextType type, bool secondPrinterIsValid, bool thirdPrinterIsValid, DBAccess *dbAccess);


	private:

		ProdSupportTextType       textType_;
		ProductTextPrinterChoices printerNum_;
		HobRecordNum              labelNum_;

		HobErrorCode errorCode_;

		DBAccess *dbAccess_;

		edb::DBProductTextCfg *myDbRecord_;

		static std::list<ProductTextCfgFieldOffsets> fieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //PRODUCTTEXTCFG_H


