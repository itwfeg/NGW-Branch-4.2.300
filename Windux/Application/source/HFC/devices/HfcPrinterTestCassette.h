/**
 * @file HfcPrinterTestCassette.h
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for reporting results of print roller validation test.
 * Created on: Aug 28, 2020
 */

#ifndef HFCPRINTERTESTCASSETTE_H
#define HFCPRINTERTESTCASSETTE_H
#include "evolution.h"
#include "HfcObject.h"
#include "PrinterTestCassette.h"


typedef enum HfcPrinterTestCassetteFieldOffsets
{
	fieldOffsetMeasured = 1,
	fieldOffsetActual,
	fieldOffsetTestResult,
	endHfcPrinterTestCassetteFieldOffsets		//	MUST be last!
} HfcPrinterTestCassetteFieldOffsets_t;
class HfcPrinterTestCassette : public PrinterTestCassette, public HfcObject
{
	public:

		HfcPrinterTestCassette(const char *data);
		HfcPrinterTestCassette(const char *data, int len, int formatType);

		~HfcPrinterTestCassette();

		virtual const char *serialize(int format = InternalFormat) const;


private:
#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif

};
#endif
