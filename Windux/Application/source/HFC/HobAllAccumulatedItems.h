/* HLX Scale Project
 * COPYRIGHT (c) 2009.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HOBALLACCUMULATEDITEMS_H
#define HOBALLACCUMULATEDITEMS_H

#include "evolution.h"
#include "HfcObject.h"

class HobList;
class Product;
class ProductTransaction;


class HobAllAccumulatedItems : public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HobAllAccumulatedItems();
		~HobAllAccumulatedItems();

		const char *serialize(int format = BrowserFormat) const;

		void addAccumulatedItem(HobRecordNum pluNum, HobString desc, HobString byCountOrWeight, HobPrice unitPrice, HobPrice totalPrice);
		void addAccumulatedItem(const Product *prod, const ProductTransaction *trans);

		HobValue64 getTotalPrice() const;
		void setTotalPrice(const HobValue64 totalPrice);

	private:

		HobList *positionInList;
		HobList *pluNumList;
		HobList *descList;
		HobList *byCountWeightList;
		HobList *unitPriceList;
		HobList *totalPriceList;

		HobValue64 totalPrice;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HOBALLACCUMULATEDITEMS_H
