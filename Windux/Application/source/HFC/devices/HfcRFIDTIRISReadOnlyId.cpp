/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcRFIDTIRISReadOnlyId.h"
#include "Integer.h"
#include "Hex.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcRFIDTIRISReadOnlyId::dataType = 0;
std::string HfcRFIDTIRISReadOnlyId::dataTypeName("HfcRFIDTIRISReadOnlyId"); 
#endif

// Added the RFID module sofware version to this message, FHA 3/13
//HfcRFIDTIRISReadOnlyId::HfcRFIDTIRISReadOnlyId(Hobart::CEvolMsgReceiver *pHobThread, UINT8 *currentId, UINT32 size)
//	: TIRISReadOnlyId(pHobThread, currentId, size)
HfcRFIDTIRISReadOnlyId::HfcRFIDTIRISReadOnlyId(Hobart::CEvolMsgReceiver *pHobThread, UINT8 *currentId, UINT32 size,
											   UINT8 swVersionRFID)
	: TIRISReadOnlyId(pHobThread, currentId, size, swVersionRFID)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HfcRFIDTIRISReadOnlyId::HfcRFIDTIRISReadOnlyId(Hobart::CEvolMsgReceiver *pHobThread, std::string currentId)
	: TIRISReadOnlyId(pHobThread, currentId)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HfcRFIDTIRISReadOnlyId::HfcRFIDTIRISReadOnlyId(const char *data, int UNUSED(len), int formatType)
	: TIRISReadOnlyId(data)
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
			// Nothing else to do, the TIRISReadOnlyId was filled in during initialization of this object.
			break;
		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcRFIDTIRISReadOnlyId::~HfcRFIDTIRISReadOnlyId()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *HfcRFIDTIRISReadOnlyId::serialize(int format) const
{
	HfcObject::serializedData = "";

	switch (format)
	{
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);

			std::string theId = getCurrentId();
			theId = Hex::toString(theId.c_str(), (int)theId.size(), false);
			HfcObject::serializedData.append(theId);
			
			// Added the RFID modules software version, FHA 3/13
			HfcObject::serializedData.append(BrowserFormatSeparator);
			UINT8 version = getSoftwareVersion();
			HfcObject::serializedData.append(Hex::toString((const char *)&version, 1, false));
			break;
		}
		case InternalFormat:
		{
			const char *data = TIRISReadOnlyId::serialize();
			int datalen = TIRISReadOnlyId::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
			break;
		}
		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
