/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcILASpeed.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcILASpeed::dataType = 0;
std::string HfcILASpeed::dataTypeName("HfcILASpeed"); 
#endif

HfcILASpeed::HfcILASpeed()
	: ILASpeed(),
	  cycleCount(0)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcILASpeed::HfcILASpeed(const char *data)
	: ILASpeed(data),
	  cycleCount(0)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HfcILASpeed::HfcILASpeed(const char *UNUSED(data), int UNUSED(len), int UNUSED(formatType))
	: ILASpeed(),
	  cycleCount(0)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

/* This case should never happen, we use the above constructor (with the data buffer)
 * when internal-formatted data is received.
 */
}

HfcILASpeed::~HfcILASpeed()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

int HfcILASpeed::getCycleCount() const
{
	return cycleCount;
}
void HfcILASpeed::setCycleCount(int cycleCount)
{
	this->cycleCount = cycleCount;
}
void HfcILASpeed::incrementCycleCount(int inc)
{
	cycleCount += inc;
}

const char *HfcILASpeed::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			const char *data = ILASpeed::serialize();
			int datalen = ILASpeed::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
		}
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcILASpeedFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetILASpeed:
						HfcObject::serializedData.append(Integer::toString(getSpeed()));
						break;
					case fieldOffsetILACycleCount:
						HfcObject::serializedData.append(Integer::toString(cycleCount));
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		case SMFormat:
			HfcObject::serializedData = "Serialized ILA Speed : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
