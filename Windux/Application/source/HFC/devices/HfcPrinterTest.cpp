#include "HfcPrinterTest.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
HfcPrinterTest::HfcPrinterTest(const char *data)
	: PrinterTest(data)
{

}

HfcPrinterTest::HfcPrinterTest(const char *data, int len, int formatType) : PrinterTest()
{
		switch(formatType) 
		{
			case BrowserFormat: {
				HobString temp(data, len);
				HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
				HobString token;

				short loopCnt = 1;
				while (tok.next(&token) != -1)
				{
					switch (loopCnt)
					{
						case fieldPTOffset:
							setData( Integer::valueOf( token.c_str() ) );
							break;
						default:
							break;
					}
					loopCnt++;
				}
				break;
			}
		}
}
HfcPrinterTest::~HfcPrinterTest()
{


}

const char *HfcPrinterTest::serialize(int format) const
{
	
	HfcObject::serializedData = "";

	switch (format)
	{
		case InternalFormat: {
			const char *msgdata;
			msgdata = PrinterTest::serialize();
			int msglen;
			msglen = PrinterTest::getSerializedLength();

			HfcObject::serializedData = PrinterTest::serializedData;
			msglen = HfcObject::getSerializedLength();
            
            break;
        }
		default:
			break;
	}
	return HfcObject::serializedData.data();
}




