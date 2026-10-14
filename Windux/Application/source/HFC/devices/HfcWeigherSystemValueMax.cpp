/*
 * HfcWeigherSystemValueMax.cpp
 *
 *  Created on: January 31, 2020
 *      Author: Doug Antonides
 */
#include "HfcWeigherSystemValueMax.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterHeadType::dataType = 0;
std::string HfcPrinterHeadType::dataTypeName("HfcPrinterHeadType");
#endif

HfcWeigherSystemValueMax::HfcWeigherSystemValueMax(const char *data) : WeigherSystemRequestValueMax(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWeigherSystemValueMax::HfcWeigherSystemValueMax(const char *data, int len, int formatType) : WeigherSystemRequestValueMax()
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
					case fieldOffRequestFITParams:
						setRequestFITParams(Integer::valueOf(token));
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

HfcWeigherSystemValueMax::~HfcWeigherSystemValueMax()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *HfcWeigherSystemValueMax::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case InternalFormat:
		{

			msgdata = WeigherSystemRequestValueMax::serialize();
			msglen = WeigherSystemRequestValueMax::getSerializedLength();

			HfcObject::serializedData = WeigherSystemRequestValueMax::serializedData;
			msglen = HfcObject::getSerializedLength();

		}
		break;

		case BrowserFormat:
		{

			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcWeigherSystemRequestValueMaxFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
 					case fieldOffRequestFITParams:
						HfcObject::serializedData.append(Integer::toString(getRequestFITParams()) );
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

