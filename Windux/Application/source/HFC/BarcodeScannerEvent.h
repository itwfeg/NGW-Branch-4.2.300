#ifndef BARCODESCANNEREVENT_H
#define BARCODESCANNEREVENT_H

	// This file contains all the events that occur in the Hobart Scales Projects
	#include "Event.h"
	#include "DataTypeEvent.h"

	// Device Generated Events - BarcodeScannerDev

	#define MAX_SCANNED_CODE_STRING			16
    #define MAX_SCANNED_HEADER_STRING		13

	typedef enum BarcodeScannerEvent
	{
		BarcodeScannerEnable =		20,
		BarcodeScannerDisable,
        BarcodeReadScannerCode,
		BarcodeScannerPlu
	}   BarcodeScannerEvent_t;

	typedef enum ScannedBarcodeFieldOffsets
	{
		fieldOffsetScannedCode = 1,
		fieldOffsetScannedPluNum,
		fieldOffsetScannedDesc,
		endScannedBarcodeFieldOffsets
	}   ScannedBarcodeFieldOffsets_t;


	typedef struct 
	{
	    UINT8	bsEvent;
	    UINT8	pad0;
	    UINT8	code;
		//UINT8	pad1;
	} BsPluReadMsg;

	typedef enum BarcodeScannerDevError
	{
		BarcodeScannerErrUnknownError = BarcodeScannerDev * DEVICE_ERROR_OFFSET,
		BarcodeScannerErrRecordNotFound,			// from app, received a read 1 from ui, and cant find it in the db. cant really happen tho bc we sent the item to the ui initially anyway
		BarcodeScannerErrInvalidPLU,				// from UI, cant add&save current scan code bc plu field is < 0 (how can this happen???)
		BarcodeScannerErrEnterPLUNumber,			// from UI, cant add&save current scan code bc plu field is blank
		BarcodeScannerErrBlankBarcodeNumber,		// from UI, cant delete or add&save an item because there is no scan code bc no row selected or item scanned
		BarcodeScannerErrBlankPLUNumber,			// from UI, cant delete item because there is no plu in the plu field			
		BarcodeScannerErrSelectTableRow,			// from UI, cant delete anything when a row is not yet selected
		BarcodeScannerErrItemAlreadyExists,			// from UI, on a save & add on a code and PLU and the combo already exists
		BarcodeScannerErrScanCodeNotFound,			// from app, on scan code lookup not found
		BarcodeScannerErrScanItemAlreadyExists,		// from app, on a scan in supervisor tell user a record already exists for it
		BarcodeScannerErrPluDoesNotExist			// from app, telling user the row they clicked on in the scale contains a non existant PLU

	}   BarcodeScannerDevError_t;

#endif // BARCODESCANNEREVENT_H
