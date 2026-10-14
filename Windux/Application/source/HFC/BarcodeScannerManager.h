/**
 * @file BarcodeScannerManager.h
 * @brief BarcodeScannerManager tracks all barcodes that have been scanned by the usb scanner.  Currently the
 * Scanner is intended to be attached to an elevated self service device. The customer will have
 * the ability to walk up to a elevated self service scale with their produce item and hold it under
 * the scanner and the barcode sticker attached to the item will be used to look up a PLU which the
 * customer can then print a label for purchase.
 *
 * @copyright Copyright (c) 2015, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup BarcodeScannerManagerCsu
 * @{
*/

#ifndef BARCODESCANNERMANAGER_H
#define BARCODESCANNERMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "BarcodeScannerEvent.h"
#include "HfcScannerProductRead.h"

class SessionMessage;
class DBSession;

#include "DBBarcodeScannersRM.h"
#include "DBBarcodeScanner.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}

class BarcodeScannerManager : public DataManager
{
	public:

	    BarcodeScannerManager();
	    BarcodeScannerManager(hfc::HFCManagerContainer *container);
	    virtual ~BarcodeScannerManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		HfcScannerProductRead* getFirstRecord() { return firstBarcodeScanner(); }
		HfcScannerProductRead* getNextRecord()  { return nextBarcodeScanner(); }
		EvolDeviceType getDevice() const { return BarcodeScannerDev; }

		HfcScannerProductRead *firstBarcodeScanner();
		HfcScannerProductRead *nextBarcodeScanner();
		int countAllBarcodeScanners();

		HfcScannerProductRead *getCurrentBarcodeScanner();

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();
		void handleSaveRecord(HfcScannerProductRead *recordToSave);

		void boop();

		// jhc - Dont know what type tp pass right now
		HfcScannerProductRead *createNewBarcodeScanner(HobString codeNum);

		void destroyQueriesAndIters();
	
	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		HfcScannerProductRead *currentBarcodeScanner;

		edb::DBQuery<edb::DBBarcodeScanner> *allBarcodeScannersQuery;
		edb::DBQuery<edb::DBBarcodeScanner>::const_iterator *allBarcodeScannersIter;

		bool receivedBarcodeScannerRestore;

		void init();

		void handleRead(HfcScannerProductRead *msgRec);
		void handleReadAll();
		void handleReadScannedCode(HfcScannerProductRead *msgRec);
		void handleWrite(HfcScannerProductRead *modifiedRec);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(HfcScannerProductRead *modifiedRec = 0);
		void handleRestore(SessionMessage *msg);
		void handleCreate(HfcScannerProductRead *msgRec);
		void setCurrentBarcodeScanner(HfcScannerProductRead *msgRec);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif

};

#endif //BARCODESCANNERMANAGER_H
