/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcProductUpdateMsg.h"
#include "Integer.h"
#include "Double.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcProductUpdateMsg::dataType = 0;
std::string HfcProductUpdateMsg::dataTypeName("HfcProductUpdateMsg"); 
#endif

HfcProductUpdateMsg::HfcProductUpdateMsg(Hobart::CEvolMsgReceiver *pHobThread, const ProductTransaction *pProductTransaction, bool ignoreWeigherStatus, bool ignoreWeighMode)
	: ProductUpdateMsg(pHobThread, pProductTransaction, ignoreWeigherStatus, ignoreWeighMode)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HfcProductUpdateMsg::HfcProductUpdateMsg(const char *data, int UNUSED(len), int formatType)
	: ProductUpdateMsg(data)
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
			// Nothing else to do, the ProductUpdateMsg was filled in during initialization of this object.
			break;

		case BrowserFormat:
			// Why am I receiving a ProductUpdateMsg from the Browser????  I shouldn't be!
			break;

		case SMFormat:
			// Why am I receiving a ProductUpdateMsg from the SM????  I shouldn't be!
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcProductUpdateMsg::~HfcProductUpdateMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}



const char *HfcProductUpdateMsg::serialize(int format) const
{
	HfcObject::serializedData = "";

	switch (format)
	{
		case InternalFormat:
		{
			const char *data = ProductUpdateMsg::serialize();
			int datalen = ProductUpdateMsg::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
		}
			break;

		case BrowserFormat:
			HfcObject::serializedData = "Serialized HfcProductUpdateMsg : browser format";
			break;

		case SMFormat:
			HfcObject::serializedData = "Serialized HfcProductUpdateMsg : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

