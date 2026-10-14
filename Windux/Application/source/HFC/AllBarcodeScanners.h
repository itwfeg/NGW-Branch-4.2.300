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
 * @addtogroup AllBarcodeScannersCsu
 * @{
*/

#ifndef ALLBARCODESCANNERS_H
#define ALLBARCODESCANNERS_H


#include "evolution.h"
#include "HfcObject.h"

class HobList;
class DBAccess;

class AllBarcodeScanners : public HfcObject
{
	public:

		AllBarcodeScanners();
		~AllBarcodeScanners();

		HobList *getProductScannerProductNumList();
		HobList *getProductScannerCodeList();
		HobList *getProductScannerDescriptionList();

		const char *serialize(int format = BrowserFormat) const;

		void findAllBarcodeScanners(DBAccess *dbAccess);


	private:

		HobList *productScannerProductNumList;
		HobList *productScannerCodeList;
		HobList *productScannerDescriptionList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLBARCODESCANNERS_H
