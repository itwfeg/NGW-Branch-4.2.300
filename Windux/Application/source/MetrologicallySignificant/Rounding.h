/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ROUNDING_H
#define ROUNDING_H

#include "evolution.h"

class Rounding
{
	public:
		typedef enum
		{
			RoundingMethodInvalid   = -1,

			CONVENTIONAL_ROUNDING   = 0,   //  >= 0.5, ADD 1       (10.8653 = 10.87; 5.294 = 5.29)
			ROUND_DOWN              = 1,   //  Truncate            (10.8653 = 10.86; 5.294 = 5.29)
			ROUND_UP                = 2,   //  Any fraction, ADD 1 (10.8653 = 10.87; 5.294 = 5.30)
			UNCONVENTIONAL_ROUNDING = 3,   //  > 0.5, ADD 1        (10.8653 = 10.86; 5.294 = 5.29)

			RoundingMethodLast
		} Method;

		// SOF-5333 - 2 new methods - Changed to W&M rounding, instead of Standard Conventional rounding that we had been using
		static HobValue roundPerWMPriceWeight(HobValue val, short pricePrecision, short weightPrecision, short returnPrecision);
		static HobValue64 roundPerWMPriceWeight(HobValue64 val, short pricePrecision, short weightPrecision, short returnPrecision);

		static HobValue roundConventionalPriceWeight(HobValue val, short pricePrecision, short weightPrecision, short returnPrecision);
		static HobValue roundUpPriceWeight(HobValue val, short pricePrecision, short weightPrecision, short returnPrecision);
		static HobValue roundDownPriceWeight(HobValue val, short pricePrecision, short weightPrecision, short returnPrecision);
		static HobValue roundPriceWeight(HobValue val, short pricePrecision, short weightPrecision, short returnPrecision, short roundingMethod);

		static HobValue64 roundConventionalPriceWeight(HobValue64 val, short pricePrecision, short weightPrecision, short returnPrecision);
		static HobValue64 roundUpPriceWeight(HobValue64 val, short pricePrecision, short weightPrecision, short returnPrecision);
		static HobValue64 roundDownPriceWeight(HobValue64 val, short pricePrecision, short weightPrecision, short returnPrecision);
		static HobValue64 roundPriceWeight(HobValue64 val, short pricePrecision, short weightPrecision, short returnPrecision, short roundingMethod);

		static HobValue roundPerWM(double valueToRound);
		// SOF-5333 - New method - Changed to W&M rounding, instead of Standard Conventional rounding that we had been using
		static HobValue64 roundPerWM64(double valueToRound);

		// Originally was in Application/source/util/HobartMath.cpp, but was moved to here during the MetSig project
		static double hobround(double value);

		// Originally in-line code that I converted to a method during the MetSig project
		static HobWeight roundWeightToTwoImpliedDecimalPlaces(HobWeight weight);


	private:
		Rounding();
};

#endif //ROUNDING_H
