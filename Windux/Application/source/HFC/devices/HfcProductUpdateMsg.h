/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRODUCTUPDATEMSG_H
#define HFCPRODUCTUPDATEMSG_H

#include "evolution.h"
#include "ProductUpdateMsg.h"
#include "HfcObject.h"


class HfcProductUpdateMsg : public ProductUpdateMsg, public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HfcProductUpdateMsg(Hobart::CEvolMsgReceiver *pHobThread = NULL, const ProductTransaction *pProductTransaction = NULL, bool ignoreWeigherStatus = false, bool ignoreWeighMode = false);
		HfcProductUpdateMsg(const char *data, int len, int formatType);

		~HfcProductUpdateMsg();

		virtual const char *serialize(int format) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCPRODUCTUPDATEMSG_H

