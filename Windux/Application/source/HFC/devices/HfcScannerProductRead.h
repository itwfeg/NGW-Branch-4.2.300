/**
 * @file HfcScannerProductRead.h
 * @brief HfcScannerProductRead tracks all barcodes that have been scanned by the usb scanner.  Currently the
 * Scanner is intended to be attached to an elevated self service device. The customer will have
 * the ability to walk up to a elevated self service scale with their produce item and hold it under
 * the scanner and the barcode sticker attached to the item will be used to look up a PLU which the
 * customer can then print a label for purchase.
 *
 * @copyright Copyright (c) 2015, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup HfcScannerProductReadCsu
 * @{
*/

#ifndef HFCSCANNERPRODUCTREAD
#define HFCSCANNERPRODUCTREAD

#include "evolution.h"
#include "../../EvolutionRTC/ScannerManager/Include/ScannerProductRead.h"
#include "HfcObject.h"
#include <list>

#include "BarcodeScannerEvent.h"

#include "DBBarcodeScanner.h"

class DBAccess;


typedef enum BarcodeScannerFieldOffsets
{
	fieldOffsetBCScannerCode = 1,
	fieldOffsetBCScannerProductNum,
	fieldOffsetBCScannerDescription,
	endBarcodeScannerFieldOffsets
}   BarcodeScannerFieldOffsets_t;


class HfcScannerProductRead : public ScannerProductRead, public HfcObject
{
	public:

		HfcScannerProductRead(const HfcScannerProductRead&);
		HfcScannerProductRead();

		HfcScannerProductRead(HobString scannerCode);
		HfcScannerProductRead(const char *data);
		HfcScannerProductRead(const char *data, int len, int formatType);
		HfcScannerProductRead(edb::DBBarcodeScanner *dbBarcodeScannerRec, DBAccess *access = (DBAccess *)NULL);
		
		HfcScannerProductRead(const HfcScannerProductRead *hspr);

		virtual ~HfcScannerProductRead();

		HobString getScannerCode() const;
		void setScannerCode(HobString code);

		HobRecordNum getScannerProductNum() const;
		void setScannerProductNum(HobRecordNum pluNum);

		HobString getScannerDescription() const;
		void setScannerDescription(HobString desc);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode code);

		void setDBAccess(DBAccess *dbAccess) { dbAccess_ = dbAccess; }

		edb::DBBarcodeScanner *getMyDbRecord() { return myDbRecord_; }

		bool isRecordValid() { return true; }

		static HfcScannerProductRead *findBarcodeScanner(HobString scannerCode, DBAccess *dbAccess);
		virtual void updateBarcodeScanner(HfcScannerProductRead *newText);
		void saveRecord();
		int deleteRecord();

		const char *serialize(int format) const;
		
		static void initFieldList();

	private:

		mutable HobString codeDisplay;

		HobString scancode_;
		HobRecordNum scanProductNum_;
		HobString scanDescription_;

		HobErrorCode errorCode_;

		DBAccess *dbAccess_;

		edb::DBBarcodeScanner *myDbRecord_;

		static std::list<BarcodeScannerFieldOffsets> fieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCSCANNERPRODUCTREAD_H
