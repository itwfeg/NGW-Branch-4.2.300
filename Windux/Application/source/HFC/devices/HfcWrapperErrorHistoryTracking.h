/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERERRORHISTORYTRACKING_H
#define HFCWRAPPERERRORHISTORYTRACKING_H

#include "evolution.h"
#include "HfcObject.h"
#include "WrapperErrorHistoryTracking.h"

class HfcWrapperErrorHistoryTracking : public WrapperErrorHistoryTracking, public HfcObject
{
	public:

		HfcWrapperErrorHistoryTracking(const char *data);
		HfcWrapperErrorHistoryTracking(const char *data, int len, int formatType);
		HfcWrapperErrorHistoryTracking(bool sensorsTracking);

		~HfcWrapperErrorHistoryTracking();

		virtual const char *serialize(int format = InternalFormat) const;
		
		void setErrorHistoryTrackingState(bool sensorsTracking);


	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCWRAPPERERRORHISTORYTRACKING_H
