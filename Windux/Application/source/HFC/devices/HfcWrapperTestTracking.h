/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERTESTTRACKING_H
#define HFCWRAPPERTESTTRACKING_H

#include "evolution.h"
#include "HfcObject.h"
#include "HfcWrapperSensorsTracking.h"

class HfcWrapperTestTracking : public HfcWrapperSensorsTracking
{
	public:

		HfcWrapperTestTracking(const char *data);
		HfcWrapperTestTracking(const char *data, int len, int formatType);

		~HfcWrapperTestTracking();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCWRAPPERTESTTRACKING_H
