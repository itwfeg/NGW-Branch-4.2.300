/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperLoad.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperLoad::dataType = 0;
std::string HfcEvolWrapperLoad::dataTypeName("HfcEvolWrapperLoad"); 
#endif

HfcEvolWrapperLoad::HfcEvolWrapperLoad(const char *data) : EvolWrapperLoad(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcEvolWrapperLoad::HfcEvolWrapperLoad(const char *data, int len, int formatType) : EvolWrapperLoad()
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
			if (len != 0)
				setFilmWidthSelection((WrFilmWidthSelection) Integer::valueOf(temp.c_str()));
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcEvolWrapperLoad::~HfcEvolWrapperLoad()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcEvolWrapperLoad::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			EvolWrapperLoad::serialize();
			HfcObject::serializedData = EvolWrapperLoad::serializedData;
		}
		break;

		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getFilmWidthSelection()));
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
