#ifndef HFCPRINTERTEST_H
#define HFCPRINTERTEST_H
#include "HfcObject.h"
#include "PrinterTest.h"
typedef enum HfcPrinterTestFieldOffsets
{
	fieldPTOffset = 1,
	fieldPTOffsetData,
} HfcPrinterTestFieldOffsets_t;

class HfcPrinterTest : public PrinterTest, public HfcObject
{
	public:

		HfcPrinterTest( const char *data );
		HfcPrinterTest(const char *data, int len, int formatType);
		
		~HfcPrinterTest();

		virtual const char *serialize( int format = InternalFormat ) const;

};
#endif