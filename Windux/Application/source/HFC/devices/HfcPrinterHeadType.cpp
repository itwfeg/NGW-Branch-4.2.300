/*
 * HfcPrinterHeadType.cpp
 *
 *  Created on: Oct 14, 2019
 *      Author: hobart
 */
#include "HfcPrinterHeadType.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterHeadType::dataType = 0;
std::string HfcPrinterHeadType::dataTypeName("HfcPrinterHeadType");
#endif

HfcPrinterHeadType::HfcPrinterHeadType(const char *data)
	: PrinterHeadType(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterHeadType::HfcPrinterHeadType(const char *data, int len, int formatType)
	: PrinterHeadType()
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
				case fieldOffType:
					setHeadType(Bool::valueOf(token));
					break;
				case fieldOffSize:
					setHeadSize(Integer::valueOf(token));
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

HfcPrinterHeadType::~HfcPrinterHeadType()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *HfcPrinterHeadType::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case InternalFormat:
		{

			msgdata = PrinterHeadType::serialize();
			msglen = PrinterHeadType::getSerializedLength();

			HfcObject::serializedData = PrinterHeadType::serializedData;
			msglen = HfcObject::getSerializedLength();

		}
		break;

		case BrowserFormat:
		{

			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcPrinterHeadTypeFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
 					case fieldOffType:
						HfcObject::serializedData.append(Integer::toString(getHeadType()) );
						break;
					case fieldOffSize:
						HfcObject::serializedData.append(Integer::toString(getHeadSize()));
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
			//HfcObject::serializedData = "Serialized Printer Cutter Status : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

