/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcProductStartPrintForTransactionMsg.h"
#include "Integer.h"
#include "Double.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcProductStartPrintForTransactionMsg::dataType = 0;
std::string HfcProductStartPrintForTransactionMsg::dataTypeName("HfcProductStartPrintForTransactionMsg"); 
#endif

HfcProductStartPrintForTransactionMsg::HfcProductStartPrintForTransactionMsg(Hobart::CEvolMsgReceiver *pHobThread, const ProductTransaction *pProductTransaction)
	: ProductStartPrintForTransactionMsg(pHobThread, pProductTransaction)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcProductStartPrintForTransactionMsg::HfcProductStartPrintForTransactionMsg(const char *data, int UNUSED(len), int formatType)
	: ProductStartPrintForTransactionMsg(data)
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
			// Nothing else to do, the ProductStartPrintForTransactionMsg was filled in during initialization of this object.
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcProductStartPrintForTransactionMsg::~HfcProductStartPrintForTransactionMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}



const char *HfcProductStartPrintForTransactionMsg::serialize(int format) const
{
	HfcObject::serializedData = "";

	switch (format)
	{
		case InternalFormat:
		{
			const char *data = ProductStartPrintForTransactionMsg::serialize();
			int datalen = ProductStartPrintForTransactionMsg::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
		}
			break;

		default:
			HfcObject::serializedData = HobString("HfcProductStartPrintForTransactionMsg::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

