/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcTimeout.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcTimeout::dataType = 0;
std::string HfcTimeout::dataTypeName("HfcTimeout"); 
#endif

HfcTimeout::HfcTimeout()
	: Timeout()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcTimeout::HfcTimeout(const char *data)
	: Timeout(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcTimeout::HfcTimeout(const char *UNUSED(data), int UNUSED(len), int UNUSED(formatType))
	: Timeout()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcTimeout::~HfcTimeout()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcTimeout::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			Timeout::serialize();
			HfcObject::serializedData = Timeout::serializedData;
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
