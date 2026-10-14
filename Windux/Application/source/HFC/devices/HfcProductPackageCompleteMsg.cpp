#include "HfcProductPackageCompleteMsg.h"



#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int HfcProductPackageCompleteMsg::dataType = 0;
	std::string HfcProductPackageCompleteMsg::dataTypeName("HfcProductPackageCompleteMsg"); 
#endif



HfcProductPackageCompleteMsg::HfcProductPackageCompleteMsg(const char *data, int UNUSED(len), int formatType)
	: ProductPackageCompleteMsg(data)
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
			// Nothing else to do, the ProductCompleteMsg was filled in during initialization of this object.
			break;

		case BrowserFormat:
			// Why am I receiving a ProductCompleteMsg from the Browser????  I shouldn't be!
			break;

		case SMFormat:
			// Why am I receiving a ProductCompleteMsg from the SM????  I shouldn't be!
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}


HfcProductPackageCompleteMsg::~HfcProductPackageCompleteMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

/*
const char *HfcProductPackageCompleteMsg::serialize(int format) const
{
	HfcObject::serializedData = "";

	switch (format)
	{
		case InternalFormat:
		{
			const char *data = ProductCompleteMsg::serialize();
			int datalen = ProductCompleteMsg::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
		}
			break;

		case BrowserFormat:
			HfcObject::serializedData = "Serialized HfcProductPackageCompleteMsg : browser format";
			break;

		case SMFormat:
			HfcObject::serializedData = "Serialized HfcProductPackageCompleteMsg : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
*/
