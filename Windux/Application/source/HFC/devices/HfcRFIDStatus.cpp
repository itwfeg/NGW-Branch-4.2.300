/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcRFIDStatus.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcRFIDStatus::dataType = 0;
std::string HfcRFIDStatus::dataTypeName("HfcRFIDStatus"); 
#endif

HfcRFIDStatus::HfcRFIDStatus(Hobart::CEvolMsgReceiver *pHobThread, UINT8 status)
	: RFIDStatus(pHobThread, status)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HfcRFIDStatus::HfcRFIDStatus(const char *data, int UNUSED(len), int formatType)
	: RFIDStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case InternalFormat:
			setAllReceived(true);
			// Nothing else to do, the RFIDStatus was filled in during initialization of this object.
			break;
		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcRFIDStatus::~HfcRFIDStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcRFIDStatus::serialize(int format) const
{
	HfcObject::serializedData = "";

	switch (format)
	{
		case InternalFormat:
		{
			const char *data = RFIDStatus::serialize();
			int datalen = RFIDStatus::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
			break;
		}
		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
