/*HeadDot Scale Project
 * COPYRIGHT (c) 2019.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterHeadDotStatus.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"
#include "UInteger.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterHeadDotStatus::dataType = 0;
std::string HfcPrinterHeadDotStatus::dataTypeName("HfcPrinterHeadDotStatus"); 
#endif

HfcPrinterHeadDotStatus::HfcPrinterHeadDotStatus(const char *data)
	: PrinterHeadDotStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterHeadDotStatus::HfcPrinterHeadDotStatus(const char *data, int len, int formatType)
	: PrinterHeadDotStatus()
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
					case fieldOffsetHeadDotSource:
						setSource(Integer::valueOf(token));
						break;
					case fieldOffsetHeadDotDestination:
						setDestination(Integer::valueOf(token));
						break;
					case fieldOffsetHeadDotStatus:
						setStatus(Integer::valueOf(token));
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		case SMFormat:
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcPrinterHeadDotStatus::~HfcPrinterHeadDotStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *HfcPrinterHeadDotStatus::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case InternalFormat:
		{
			msgdata = PrinterHeadDotStatus::serialize();
			msglen = PrinterHeadDotStatus::getSerializedLength();

			HfcObject::serializedData = PrinterHeadDotStatus::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcPrinterHeadDotStatusFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetHeadDotSource:
						HfcObject::serializedData.append(UInteger::toString(getSource()));
						break;

					case fieldOffsetHeadDotDestination:
						HfcObject::serializedData.append(UInteger::toString(getDestination()));
						break;

 					case fieldOffsetHeadDotStatus:
						HfcObject::serializedData.append(UInteger::toString(getStatus()) );
						break;
		
					default:
						break;
				}
				loopCnt++;
			}

			msgdata = HfcObject::serializedData.data();
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case SMFormat:
			HfcObject::serializedData = "Serialized Printer HeadDot Status : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
