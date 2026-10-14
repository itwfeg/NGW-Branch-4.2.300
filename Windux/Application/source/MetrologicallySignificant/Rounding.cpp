/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "evolution.h"
#include "Rounding.h"
#include <math.h>
#include "Power.h"
#include "HobString.h"
#include "Debug.h"
#include "Double.h"
#include "Long.h"



// SOF-5333 - New method - Changed to W&M rounding, instead of Standard Conventional rounding that we had been using
// static
HobValue Rounding::roundPerWMPriceWeight(HobValue val, short pricePrecision, short weightPrecision, short returnPrecision)
{
	return (HobValue)Rounding::roundPerWMPriceWeight((HobValue64)val, pricePrecision, weightPrecision, returnPrecision);
}

// SOF-5333 - New method - Changed to W&M rounding, instead of Standard Conventional rounding that we had been using
HobValue64 Rounding::roundPerWMPriceWeight(HobValue64 val, short pricePrecision, short weightPrecision, short returnPrecision)
{
	bool traceOn = Debug::isTracing("Rounding.roundPerWM");
	if (traceOn)
		Debug::trace("Rounding.roundPerWM", "roundPerWMPriceWeight: 1) valueToRound = " + util_string::longlongtostring(val));

	short decimalPlaces = pricePrecision + weightPrecision - returnPrecision;
	if (decimalPlaces <= 0)
		return val;

	long ldivisor = Power::power(10, decimalPlaces);
	double value = (double)val / ldivisor;

	if (traceOn)
		Debug::trace("Rounding.roundPerWM", "roundPerWMPriceWeight: 2) double valueToRound = " + util_string::doubletostring(value, 20, 20));
	HobValue64 roundedValue = Rounding::roundPerWM64(value);

	if (traceOn)
		Debug::trace("Rounding.roundPerWM", "roundPerWMPriceWeight: 3) returning = " + util_string::longlongtostring(roundedValue));
	return roundedValue;
}

// static
HobValue Rounding::roundConventionalPriceWeight(HobValue val, short pricePrecision, short weightPrecision, short returnPrecision)
{
	short decimalPlaces = pricePrecision + weightPrecision - returnPrecision;
	if (decimalPlaces <= 0)
		return val;

	long ldivisor = Power::power(10, decimalPlaces);
	return (HobValue)( (val + (5 * (ldivisor / 10))) / ldivisor );
}

HobValue64 Rounding::roundConventionalPriceWeight(HobValue64 val, short pricePrecision, short weightPrecision, short returnPrecision)
{
	short decimalPlaces = pricePrecision + weightPrecision - returnPrecision;
	if (decimalPlaces <= 0)
		return val;

	long ldivisor = Power::power(10, decimalPlaces);
	return (HobValue64)( (val + (5 * (ldivisor / 10))) / ldivisor );
}

// static
HobValue Rounding::roundUpPriceWeight(HobValue val, short pricePrecision, short weightPrecision, short returnPrecision)
{
	short decimalPlaces = pricePrecision + weightPrecision - returnPrecision;
	if (decimalPlaces <= 0)
		return val;

	long ldivisor = Power::power(10, decimalPlaces);
	return (HobValue)( (val + (9 * (ldivisor / 10))) / ldivisor );
}

HobValue64 Rounding::roundUpPriceWeight(HobValue64 val, short pricePrecision, short weightPrecision, short returnPrecision)
{
	short decimalPlaces = pricePrecision + weightPrecision - returnPrecision;
	if (decimalPlaces <= 0)
		return val;

	long ldivisor = Power::power(10, decimalPlaces);
	return (HobValue64)( (val + (9 * (ldivisor / 10))) / ldivisor );
}

// static
HobValue Rounding::roundDownPriceWeight(HobValue val, short pricePrecision, short weightPrecision, short returnPrecision)
{
	short decimalPlaces = pricePrecision + weightPrecision - returnPrecision;
	if (decimalPlaces <= 0)
		return val;

	long ldivisor = Power::power(10, decimalPlaces);
	return (HobValue)( val / ldivisor );
}

HobValue64 Rounding::roundDownPriceWeight(HobValue64 val, short pricePrecision, short weightPrecision, short returnPrecision)
{
	short decimalPlaces = pricePrecision + weightPrecision - returnPrecision;
	if (decimalPlaces <= 0)
		return val;

	long ldivisor = Power::power(10, decimalPlaces);
	return (HobValue64)( val / ldivisor );
}

// static
HobValue Rounding::roundPriceWeight(HobValue val, short pricePrecision, short weightPrecision, short returnPrecision, short roundingMethod)
{
	if (roundingMethod == ROUND_UP)
		return roundUpPriceWeight(val, pricePrecision, weightPrecision, returnPrecision);
	else if (roundingMethod == ROUND_DOWN)
		return roundDownPriceWeight(val, pricePrecision, weightPrecision, returnPrecision);
	else
		return roundConventionalPriceWeight(val, pricePrecision, weightPrecision, returnPrecision);
}

HobValue64 Rounding::roundPriceWeight(HobValue64 val, short pricePrecision, short weightPrecision, short returnPrecision, short roundingMethod)
{
	if (roundingMethod == ROUND_UP)
		return roundUpPriceWeight(val, pricePrecision, weightPrecision, returnPrecision);
	else if (roundingMethod == ROUND_DOWN)
		return roundDownPriceWeight(val, pricePrecision, weightPrecision, returnPrecision);
	else
		return roundConventionalPriceWeight(val, pricePrecision, weightPrecision, returnPrecision);
}

// static
/* These rules for the roundPerWM method are from Handbook 44 (2008), Appendix A, Section 10.2
	titled "General Rules"

	rule (a)
	"When the figure next beyond the last figure or place to be retained is
	less than 5, the figure in the last place retained is to be kept unchanged."

	rule (b)
	"When the figure next beyond the last figure or place to be retained is
	greater than 5, the figure in the last place retained is to be increased by 1."

	rule (c)
	"When figure next beyond the last figure to be retained is 5 followed 
	by any figures other than zero(s), treat as in (b) above; that is, 
	the figure in the last place retained is to be increased by 1."

	rule (d)
	"When the figure next beyond the last figure to be retained is 5 and there
	are no figures, or only zeroes, beyond this 5, the figure in the last place
	to be retained is to be left unchanged if it is even (0, 2, 4, 6, or 8) and
	is to be increased by 1 if it is odd (1, 3, 5, 7, or 9).  This is the odd
	and even rule, and may be stated as follows:  "If odd, then add."
*/
HobValue Rounding::roundPerWM(double valueToRound)
{
	return (HobValue)Rounding::roundPerWM64(valueToRound);
}

HobValue64 Rounding::roundPerWM64(double valueToRound)
{
	bool traceOn = Debug::isTracing("Rounding.roundPerWM");

	HobValue64 lValueToRound = (HobValue64)valueToRound;
	//if (traceOn)
	//	Debug::trace("Rounding.roundPerWM", "2) lValueToRound = " + util_string::longlongtostring(lValueToRound));

	double valueFraction = valueToRound - lValueToRound;
	//if (traceOn)
	//	Debug::trace("Rounding.roundPerWM", "3) valueFraction = " + HobString(Double::toString(valueFraction,20,20)));

	if (valueFraction > 0.5)
	{
		lValueToRound += 1;
		if (traceOn)
			Debug::trace("Rounding.roundPerWM", "valueFraction > .5, increasing by 1 - valueToRound: " + util_string::doubletostring(valueToRound, 20, 20) + "  returning : " + util_string::longlongtostring(lValueToRound));
	}
	else if (valueFraction == 0.5)
	{
		bool traceOn2 = Debug::isTracing("Rounding.roundPerWM*");
		if (lValueToRound % 2)
		{
			lValueToRound += 1;
			if (traceOn)
				Debug::trace("Rounding.roundPerWM", "valueFraction = .5, odd, increasing by 1 - valueToRound: " + util_string::doubletostring(valueToRound, 20, 20) + "  returning: " + util_string::longlongtostring(lValueToRound));
			else if (traceOn2)
			{
				Debug::trace("Rounding.roundPerWM*", "************************************************************************************************************************************************************");
				Debug::trace("Rounding.roundPerWM*", "valueFraction = .5, odd, increasing by 1 - valueToRound: " + util_string::doubletostring(valueToRound, 20, 20) + "  returning: " + util_string::longlongtostring(lValueToRound) );
				Debug::trace("Rounding.roundPerWM*", "************************************************************************************************************************************************************");
			}
		}
		else
		{
			if (traceOn)
				Debug::trace("Rounding.roundPerWM", "valueFraction = .5, even, leaving the same - valueToRound: " + util_string::doubletostring(valueToRound, 20, 20) + "  returning : " + util_string::longlongtostring(lValueToRound));
			else if (traceOn2)
			{
				Debug::trace("Rounding.roundPerWM*", "************************************************************************************************************************************************************");
				Debug::trace("Rounding.roundPerWM*", "valueFraction = .5, even, leaving the same - valueToRound: " + util_string::doubletostring(valueToRound, 20, 20) + "  returning : " + util_string::longlongtostring(lValueToRound) );
				Debug::trace("Rounding.roundPerWM*", "************************************************************************************************************************************************************");
			}
		}
	}
	else
		if (traceOn)
			Debug::trace("Rounding.roundPerWM", "valueFraction < .5, leaving the same - valueToRound: " + util_string::doubletostring(valueToRound, 20, 20) + "  returning : " + util_string::longlongtostring(lValueToRound));

	return lValueToRound;
}


//  This used to be in Application/source/util/HobartMath.cpp, but was moved to here during the MetSig project
double Rounding::hobround(double value)
{
	int i = (int)(value * 10);
	if (i % 10 >= 5)
		i += 10;
	return (i / 10);
}


// Inline code converted to method
// used by AutoWeighEvolWrapperMode
HobWeight Rounding::roundWeightToTwoImpliedDecimalPlaces(HobWeight weight)
{
	//	Round to 2 decimal places
	if (0 <= weight)
		return (weight + 5) / 10;
	else
		return (weight - 5) / 10;
}


// private
Rounding::Rounding()
{
}
