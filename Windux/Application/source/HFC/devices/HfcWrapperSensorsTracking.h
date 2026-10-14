/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERSENSORSTRACKING_H
#define HFCWRAPPERSENSORSTRACKING_H

#include "evolution.h"
#include "HfcObject.h"
#include "WrapperSensorsTracking.h"

class HfcWrapperSensorsTracking : public WrapperSensorsTracking, public HfcObject
{
	public:

		HfcWrapperSensorsTracking(const char *data);
		HfcWrapperSensorsTracking(const char *data, int len, int formatType);
		HfcWrapperSensorsTracking(bool sensorsTracking);

		~HfcWrapperSensorsTracking();

		virtual const char *serialize(int format = InternalFormat) const;
		
		void setSensorsTrackingState(bool sensorsTracking);


	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCWRAPPERSENSORSTRACKING_H
