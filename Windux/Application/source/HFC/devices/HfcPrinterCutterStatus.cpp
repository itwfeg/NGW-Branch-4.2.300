/*Cutter Scale Project
 * COPYRIGHT (c) 2019.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterCutterStatus.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterCutterStatus::dataType = 0;
std::string HfcPrinterCutterStatus::dataTypeName("HfcPrinterCutterStatus"); 
#endif

HfcPrinterCutterStatus::HfcPrinterCutterStatus(const char *data)
	: PrinterCutterStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterCutterStatus::HfcPrinterCutterStatus(const char *data, int len, int formatType)
	: PrinterCutterStatus()
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
				case fieldOffsetHome:
					setHome(Bool::valueOf(token));
					break;
				case fieldOffsetInstalled:
					setInstalled(Bool::valueOf(token));
					break;
				case fieldOffsetInterlock:
					setInterlock(Bool::valueOf(token));
					break;
				case fieldOffsetJammed:
					setJammed(Bool::valueOf(token));
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

HfcPrinterCutterStatus::~HfcPrinterCutterStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *HfcPrinterCutterStatus::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case InternalFormat:
		{
			msgdata = PrinterCutterStatus::serialize();
			msglen = PrinterCutterStatus::getSerializedLength();

			HfcObject::serializedData = PrinterCutterStatus::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcPrinterCutterStatusFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
 					case fieldOffsetHome:
						HfcObject::serializedData.append(Bool::toString(getHome()) );
						break;
					case fieldOffsetInstalled:
						HfcObject::serializedData.append(Bool::toString(getInstalled()));
						break;
					case fieldOffsetInterlock:
						HfcObject::serializedData.append(Bool::toString(getInterlock()));
						break;
					case fieldOffsetJammed:
						HfcObject::serializedData.append(Bool::toString(getJammed()));
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
			HfcObject::serializedData = "Serialized Printer Cutter Status : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
