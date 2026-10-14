/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRODUCTUPDATEVALUESMSG_H
#define HFCPRODUCTUPDATEVALUESMSG_H

#include "evolution.h"
#include "ProductTransaction.h"
#include "HfcObject.h"


class HfcProductUpdateValuesMsg : public HfcObject
{
	public:

		static const std::string _invalidTotalPriceDisplay_;

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HfcProductUpdateValuesMsg(const ProductTransaction *pProductTransaction);
		HfcProductUpdateValuesMsg(const char *data, int len, int formatType);

		~HfcProductUpdateValuesMsg();

		const ProductTransaction *getProductTransaction() const;

		HobPrice getTotalPrice() const;

		const HobString &gettotalTareDisplay() const;
		const HobString &getNetWeightDisplay() const;
		const HobString &getUnitPriceDisplay() const;
		const HobString &getTotalPriceDisplay() const;
		const HobString &getFsUnitPriceDisplay() const;
		const HobString &getFsTotalPriceDisplay() const;
		const HobString &getFsTotalSavingsDisplay() const;
		const HobString &getPoundsForDisplay() const;
		const HobString &getFractionalPricingForDisplay() const;

		bool isZeroIndicator() const;

		long GetTareWeight() { return tareWeight; }

		void determineDisplayValues(int format) const;

		virtual const char *serialize(int format) const;

	private:

		const ProductTransaction *myProductTransaction;
		HobPrice totalPrice;

		mutable HobString totalTareDisplay;
		mutable HobString netWeightDisplay;
		mutable HobString unitPriceDisplay;
		mutable HobString totalPriceDisplay;
		mutable HobString fsUnitPriceDisplay;
		mutable HobString fsTotalPriceDisplay;
		mutable HobString fsTotalSavingsDisplay;
		mutable HobString dualRangeIndDisplay;
		mutable HobString poundsForDisplay;
		mutable HobString fractionalPricingForDisplay;

		mutable long tareWeight;

		mutable bool zeroIndicator;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCPRODUCTUPDATEVALUESMSG_H

