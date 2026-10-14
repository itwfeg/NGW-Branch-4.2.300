/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcProductChangeMsg.h"
#include "Integer.h"
#include "Double.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcProductChangeMsg::dataType = 0;
std::string HfcProductChangeMsg::dataTypeName("HfcProductChangeMsg"); 
#endif

HfcProductChangeMsg::HfcProductChangeMsg(Hobart::CEvolMsgReceiver *pHobThread, const ProductManager *pProductMgr)
	: ProductChangeMsg(pHobThread, pProductMgr)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcProductChangeMsg::HfcProductChangeMsg(Hobart::CEvolMsgReceiver *pHobThread, const Product *pProduct)
	: ProductChangeMsg(pHobThread, pProduct)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcProductChangeMsg::HfcProductChangeMsg(const char *data, int UNUSED(len), int formatType)
	: ProductChangeMsg(data)
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
			// Nothing else to do, the ProductChangeMsg was filled in during initialization of this object.
			break;

		case BrowserFormat:
			// Why am I receiving a ProductChange from the Browser????  I shouldn't be!
			break;

		case SMFormat:
			// Why am I receiving a ProductChange from the SM????  I shouldn't be!
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcProductChangeMsg::~HfcProductChangeMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}



const char *HfcProductChangeMsg::serialize(int format) const
{
	HfcObject::serializedData = "";

	switch (format)
	{
		case InternalFormat:
		{
			const char *data = ProductChangeMsg::serialize();
			int datalen = ProductChangeMsg::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
		}
			break;

		case BrowserFormat:
			HfcObject::serializedData = "Serialized HfcProductChangeMsg : browser format";
			break;

		case SMFormat:
			HfcObject::serializedData = "Serialized HfcProductChangeMsg : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

