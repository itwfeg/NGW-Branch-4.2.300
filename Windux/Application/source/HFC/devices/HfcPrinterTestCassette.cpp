/**
 * @file HfcPrinterTestCassette.cpp
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for reporting results of print roller validation test.
 * Created on: Aug 28, 2020
 */
#include "HfcPrinterTestCassette.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterTestCassette::dataType = 0;
std::string HfcPrinterTestCassette::dataTypeName("HfcPrinterTestCassette"); 
#endif
HfcPrinterTestCassette::HfcPrinterTestCassette(const char *data) : PrinterTestCassette(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterTestCassette::HfcPrinterTestCassette(const char *data, int len, int formatType) : PrinterTestCassette()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case InternalFormat:
			break;

		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetMeasured:
						setMeasuredSize( Integer::valueOf( token.c_str() ) );
						break;
					case fieldOffsetActual:
						setActualSize( Integer::valueOf( token.c_str() ) );
						break;
					case fieldOffsetTestResult:
						setResult( Bool::valueOf( token.c_str() ) );
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;
		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcPrinterTestCassette::~HfcPrinterTestCassette()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcPrinterTestCassette::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case InternalFormat:
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcPrinterTestCassetteFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
 					case fieldOffsetMeasured:
						HfcObject::serializedData.append( Integer::toString( getMeasuredSize() ) );
						break;
					case fieldOffsetActual:
						HfcObject::serializedData.append( Integer::toString( getActualSize() ) );
						break;
					case fieldOffsetTestResult:
						HfcObject::serializedData.append( Bool::toString( getResult() ) );
					default:
						break;
				}
				loopCnt++;
			}

			msgdata = HfcObject::serializedData.data();
			msglen = HfcObject::getSerializedLength();
		}
		break;

		default:
			break;
	}
	return HfcObject::serializedData.data();
}
