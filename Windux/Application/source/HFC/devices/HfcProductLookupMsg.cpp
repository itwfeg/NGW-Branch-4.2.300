#include "HfcProductLookupMsg.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcProductLookupMsg::dataType = 0;
std::string HfcProductLookupMsg::dataTypeName("HfcProductLookupMsg"); 
#endif

HfcProductLookupMsg::HfcProductLookupMsg(Hobart::CEvolMsgReceiver *pHobThread, const long pluNum)
	: ProductLookupMsg(pHobThread, pluNum)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HfcProductLookupMsg::HfcProductLookupMsg(const char *data, int UNUSED(len), int formatType)	: ProductLookupMsg(data)
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
			// Nothing else to do, the ProductLookupMsg was filled in during initialization of this object.
			break;

		case BrowserFormat:
			// Why am I receiving a ProductLookupMsg from the Browser????  I shouldn't be!
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcProductLookupMsg::~HfcProductLookupMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}



const char *HfcProductLookupMsg::serialize(int format) const
{
	HfcObject::serializedData = "";

	switch (format)
	{
		case InternalFormat:
		{
			const char *data = ProductLookupMsg::serialize();
			int datalen = ProductLookupMsg::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
		}
			break;

		case BrowserFormat:
			HfcObject::serializedData = "Serialized HfcProductCompleteMsg : browser format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

