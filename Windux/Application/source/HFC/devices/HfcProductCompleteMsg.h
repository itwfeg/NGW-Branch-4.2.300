/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRODUCTCOMPLETEMSG_H
#define HFCPRODUCTCOMPLETEMSG_H

#include "evolution.h"
#include "ProductCompleteMsg.h"
#include "HfcObject.h"
#include "Product.h"
#include "CEvolMsgReceiver.h"

class HfcProductCompleteMsg : public ProductCompleteMsg, public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HfcProductCompleteMsg(Hobart::CEvolMsgReceiver *pHobThread = NULL, const ProductTransaction *pProductTransaction = NULL);
		HfcProductCompleteMsg(const char *data, int len, int formatType);

		~HfcProductCompleteMsg();

		virtual const char *serialize(int format) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif //HFCPRODUCTCOMPLETEMSG_H
