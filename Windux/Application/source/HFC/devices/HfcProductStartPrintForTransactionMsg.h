/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRODUCTSTARTPRINTFORTRANSACTIONMSG_H
#define HFCPRODUCTSTARTPRINTFORTRANSACTIONMSG_H

#include "evolution.h"
#include "ProductStartPrintForTransactionMsg.h"
#include "HfcObject.h"


class HfcProductStartPrintForTransactionMsg : public ProductStartPrintForTransactionMsg, public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HfcProductStartPrintForTransactionMsg(Hobart::CEvolMsgReceiver *pHobThread = NULL, const ProductTransaction *pProductTransaction = NULL);
		HfcProductStartPrintForTransactionMsg(const char *data, int len, int formatType);

		~HfcProductStartPrintForTransactionMsg();

		virtual const char *serialize(int format) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCPRODUCTSTARTPRINTFORTRANSACTIONMSG_H

