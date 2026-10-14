/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERSTATUSTRACKING_H
#define HFCWRAPPERSTATUSTRACKING_H

#include "evolution.h"
#include "HfcObject.h"
#include "WrapperStatusTracking.h"

class HfcWrapperStatusTracking : public WrapperStatusTracking, public HfcObject
{
	public:

		HfcWrapperStatusTracking(bool isTracking);
		HfcWrapperStatusTracking(const char *data);
		HfcWrapperStatusTracking(const char *data, int len, int formatType);

		~HfcWrapperStatusTracking();

		virtual const char *serialize(int format = InternalFormat) const;
		
		void setStatusTrackingState(bool statusTracking);


	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCWRAPPERSTATUSTRACKING_H
