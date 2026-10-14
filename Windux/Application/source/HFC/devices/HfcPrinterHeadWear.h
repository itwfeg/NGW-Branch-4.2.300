/*
 * HfcPrinterHeadWear.h
 *
 *  Created on: Feb 1, 2018
 *      Author: aaron
 */

#ifndef HFCPRINTERHEADWEAR_H
#define HFCPRINTERHEADWEAR_H
#include "HfcObject.h"
#include "PrinterHeadWear.h"

typedef enum HfcPrinterHeadWearFieldOffsets
{
	fieldOffsetHeadSize = 1,
	fieldOffsetHeadDots,
} HfcPrinterHeadWearFieldOffsets_t;

class HfcPrinterHeadWear : public PrinterHeadWear, public HfcObject
{
	public:

		HfcPrinterHeadWear( const char *data );
		~HfcPrinterHeadWear();

		virtual const char *serialize( int format = BrowserFormat ) const;

};
#endif
