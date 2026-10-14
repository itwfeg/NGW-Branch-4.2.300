/*
 * HfcPrinterHeadType.h
 *
 *  Created on: Oct 14, 2019
 *      Author: hobart
 */

#ifndef HFCPRINTERHEADTYPE_H_
#define HFCPRINTERHEADTYPE_H_

#include "evolution.h"
#include "HfcObject.h"
#include "PrinterHeadType.h"

typedef enum HfcPrinterHeadTypeFieldOffsets
{
	fieldOffType = 1,
	fieldOffSize,
	endHfcPrinterHeadTypeFieldOffsets
} HfcPrinterHeadTypeFieldOffsets_t;

class HfcPrinterHeadType : public PrinterHeadType, public HfcObject
{
	public:

		HfcPrinterHeadType(const char *data);
		HfcPrinterHeadType(const char *data, int len, int formatType);

		~HfcPrinterHeadType();

		virtual const char *serialize(int format = InternalFormat) const;


private:
#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif

};

#endif /* HFCPRINTERHEADTYPE_H_ */


