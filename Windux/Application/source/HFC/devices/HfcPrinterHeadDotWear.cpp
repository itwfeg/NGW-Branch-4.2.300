/*HeadDot Scale Project
 * COPYRIGHT (c) 2019.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterHeadDotWear.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"
#include "UInteger.h"
#include "HobList.h"
#include <vector>
#include <HobString.h>
#include <Long.h>
#include <Debug.h>
#include "Integer.h"
#include <Hex.h>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterHeadDotWear::dataType = 0;
std::string HfcPrinterHeadDotWear::dataTypeName("HfcPrinterHeadDotWear"); 
#endif

HfcPrinterHeadDotWear::HfcPrinterHeadDotWear(const char *data)
	: PrinterHeadWear(data)
{
    Debug::trace("PrinterHeadDotWear", "HfcPrinterHeadDotWear() *data START");

    std::string data_test((char*) data);
    Debug::trace("PrinterHeadDotWear", "HfcPrinterHeadDotWear() data_test: " + data_test);

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

        Debug::trace("PrinterHeadDotWear", "HfcPrinterHeadDotWear() *data END");
}

HfcPrinterHeadDotWear::HfcPrinterHeadDotWear(const char *data, int len, int formatType)
	: PrinterHeadWear()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

        Debug::trace("PrinterHeadDotWear", "HfcPrinterHeadDotWear() data len format START");

	switch (formatType)
	{
		case InternalFormat:
                        Debug::trace("PrinterHeadDotWear", "HfcPrinterHeadDotWear() data len format InsternalFormat");
			break;

		case BrowserFormat:
		{
                        Debug::trace("PrinterHeadDotWear", "HfcPrinterHeadDotWear() data len format BrowserFormat");
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);

                                // dont really need these
				switch (loopCnt)
				{
                                    case fieldOffsetHeadDotWearSize:
                                        Debug::trace("PrinterHeadDotWear", "HfcPrinterHeadDotWear() data len format case fieldOffsetHeadDotWearSize");
                                        //setHeadSize(Integer::valueOf(token));
                                    break;
                                    case fieldOffsetHeadDotWearStatus:
                                        Debug::trace("PrinterHeadDotWear", "HfcPrinterHeadDotWear() data len format case fieldOffsetHeadDotWearStatus");
                                        //setHeadDotStatus(Integer::valueOf(token));
                                        break;
                                    case fieldOffsetHeadDotWearCounts:
                                        Debug::trace("PrinterHeadDotWear", "HfcPrinterHeadDotWear() data len format case fieldOffsetHeadDotWearCounts");
                                        //setHeadDotCounts(Integer::valueOf(token));
                                        break;
                                    default:
                                        Debug::trace("PrinterHeadDotWear", "HfcPrinterHeadDotWear() data len format case default");
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

        Debug::trace("PrinterHeadDotWear", "HfcPrinterHeadDotWear() data len format END");
}

HfcPrinterHeadDotWear::~HfcPrinterHeadDotWear()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *HfcPrinterHeadDotWear::serialize(int format) const
{
        Debug::trace("PrinterHeadDotWear", "serialize() START");

	const char *msgdata = NULL;
	int msglen = 0;

        HobString wearStatusStr;
        HobString wearCountsStr;

        size_t status_len = headDotStatus_.size();
        size_t counts_len = headDotCounts_.size();

        Debug::trace("PrinterHeadDotWear", "serialize() headDotSize" + Integer::toString( getHeadSize() ) );
        Debug::trace("PrinterHeadDotWear", "serialize() dont care but headDotStatus_.size" + Integer::toString(status_len) );
        Debug::trace("PrinterHeadDotWear", "serialize() headDotCounts_.size" + Integer::toString(counts_len) );

        for (size_t j = 0; j < status_len; j++)
        {
            wearStatusStr.append( Integer::toString(headDotStatus_[j]) );
        }

        for (size_t k = 0; k < counts_len; k++)
        {
            wearCountsStr.append( Integer::toString(headDotCounts_[k]) );
        }

	switch (format)
	{
		case InternalFormat:
		{
                        Debug::trace("PrinterHeadDotWear", "serialize() InternalFormat");
			msgdata = PrinterHeadWear::serialize();
			msglen = PrinterHeadWear::getSerializedLength();

			HfcObject::serializedData = PrinterHeadWear::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
		{
                        Debug::trace("PrinterHeadDotWear", "serialize() BrowserFormat");

			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcPrinterHeadDotWearFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetHeadDotWearSize:
						Debug::trace("PrinterHeadDotWear", "serialize() case fieldOffsetHeadDotWearSize");
                                                HfcObject::serializedData.append(Integer::toString( getHeadSize() ));
						break;

					case fieldOffsetHeadDotWearStatus:
						Debug::trace("PrinterHeadDotWear", "serialize() case fieldOffsetHeadDotWearStatus");
                                                HfcObject::serializedData.append(wearStatusStr);
						break;

                                        case fieldOffsetHeadDotWearCounts:
                                                Debug::trace("PrinterHeadDotWear", "serialize() case fieldOffsetHeadDotWearCounts");
                                                HfcObject::serializedData.append(wearCountsStr);
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
			HfcObject::serializedData = "Serialized Printer HeadDot Wear : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}

        Debug::trace("PrinterHeadDotWear", "serialize() END");

	return HfcObject::serializedData.data();
}
