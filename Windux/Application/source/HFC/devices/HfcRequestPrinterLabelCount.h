/**
 * @file HfcRequestPrinterLabelCount.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief Fastrack personality module manager message to request label count
 * Created on: Feb 17, 2017
 */

#ifndef HFCREQUESTPRINTERLABELCOUNT_H_
#define HFCREQUESTPRINTERLABELCOUNT_H_

#include "EvolMessage.h"
#include "HfcObject.h"
#include "FpmEvent.h"
#include "Printer.h"


typedef enum HfcRequestPrinterCountOffsets
{
	fieldOffset_type = 1,
	endHfcRequestPrinterCountOffsets
}HfcRequestPrinterCountOffsets_t;

class HfcRequestPrinterLabelCount : public EvolMessage, public HfcObject
{

	public:
		HfcRequestPrinterLabelCount( const char* data, int len, int formatType );
		~HfcRequestPrinterLabelCount();
		PrinterType getPrinterType() { return type_; }

	private:
		PrinterType type_;

};
#endif
