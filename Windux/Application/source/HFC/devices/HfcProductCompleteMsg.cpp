/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcProductCompleteMsg.h"
#include "ProductTransaction.h"
#include "Integer.h"
#include "Double.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcProductCompleteMsg::dataType = 0;
std::string HfcProductCompleteMsg::dataTypeName("HfcProductCompleteMsg"); 
#endif

HfcProductCompleteMsg::HfcProductCompleteMsg(Hobart::CEvolMsgReceiver *pHobThread, const ProductTransaction *pProductTransaction)
	: ProductCompleteMsg(pHobThread, -1, pProductTransaction)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (pProductTransaction != NULL)
	{
		setRecordNumber(pProductTransaction->getTransactionNum());
	}
}

HfcProductCompleteMsg::HfcProductCompleteMsg(const char *data, int UNUSED(len), int formatType)	: ProductCompleteMsg(data)
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

HfcProductCompleteMsg::~HfcProductCompleteMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}



const char *HfcProductCompleteMsg::serialize(int format) const
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
			HfcObject::serializedData = "Serialized HfcProductCompleteMsg : browser format";
			break;

		case SMFormat:
			HfcObject::serializedData = "Serialized HfcProductCompleteMsg : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

