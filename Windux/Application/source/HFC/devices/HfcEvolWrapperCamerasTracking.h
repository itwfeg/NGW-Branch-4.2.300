/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERCAMERASTRACKING_H
#define HFCEVOLWRAPPERCAMERASTRACKING_H

#include "evolution.h"
#include "HfcObject.h"
#include "EvolWrapperCamerasTracking.h"

class HfcEvolWrapperCamerasTracking : public EvolWrapperCamerasTracking, public HfcObject
{
	public:

		HfcEvolWrapperCamerasTracking(const char *data);
		HfcEvolWrapperCamerasTracking(const char *data, int len, int formatType);

		~HfcEvolWrapperCamerasTracking();

		virtual const char *serialize(int format = InternalFormat)const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//	HFCEVOLWRAPPERCAMERASTRACKING_H
