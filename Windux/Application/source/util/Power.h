/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef POWER_H
#define POWER_H

#include "evolution.h"
#include "Double.h"

#include "Debug.h"


class Power
{
	public:

		//	Note:	Use power(...) in place of the vxWorks function pow(...)
		//			which does not always return the value you expect.
		//			(Clare - Jan 14, 2003)
    #if defined(LINUX_FIX)
		static double power(double base, double powerOf)
    #else
		static double Power::power(double base, double powerOf)
    #endif
		{
			Debug::trace("Power", "Power(double, double) = double"); 
			double	result = 1;
			int		x;

			if (powerOf < 0) { for (x = -1; x >= powerOf; x--) { result /= base; } }
			else if (powerOf > 0) {	for (x = 1; x <= powerOf; x++) { result *= base; } }
			return result;
		};

    #if defined(LINUX_FIX)
		static long power(int base, int powerOf)
    #else
		static long Power::power(int base, int powerOf)
    #endif
		{
			Debug::trace("Power", "Power(int, int) = long"); 
			long	result = 1;
			int		x;

			if (powerOf < 0) { for (x = -1; x >= powerOf; x--) { result /= base; } }
			else if (powerOf > 0) {	for (x = 1; x <= powerOf; x++) { result *= base; } }
			return result;
		};
};

#endif	//	POWER_H
