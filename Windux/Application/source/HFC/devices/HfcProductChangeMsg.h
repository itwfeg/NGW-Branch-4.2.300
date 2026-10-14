/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRODUCTCHANGEMSG_H
#define HFCPRODUCTCHANGEMSG_H

#include "evolution.h"
#include "ProductChangeMsg.h"
#include "HfcObject.h"


class HfcProductChangeMsg : public ProductChangeMsg, public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HfcProductChangeMsg(Hobart::CEvolMsgReceiver *pHobThread = NULL, const Product *pProduct = NULL);
		HfcProductChangeMsg(Hobart::CEvolMsgReceiver *pHobThread, const ProductManager *pProductMgr);
		HfcProductChangeMsg(const char *data, int len, int formatType);

		~HfcProductChangeMsg();

		virtual const char *serialize(int format) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCPRODUCTCHANGEMSG_H
