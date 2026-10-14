/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRODUCTLOOKUPMSG_H
#define HFCPRODUCTLOOKUPMSG_H

#include "evolution.h"
#include "ProductLookupMsg.h"
#include "HfcObject.h"
#include "CEvolMsgReceiver.h"

class HfcProductLookupMsg : public ProductLookupMsg, public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HfcProductLookupMsg(Hobart::CEvolMsgReceiver *pHobThread = NULL, const long pluNum = 0);
		HfcProductLookupMsg(const char *data, int len, int formatType);

		~HfcProductLookupMsg();

		virtual const char *serialize(int format) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif //HFCPRODUCTLOOKUPMSG_H
