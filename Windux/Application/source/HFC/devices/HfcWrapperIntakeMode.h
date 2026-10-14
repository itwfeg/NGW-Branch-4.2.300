/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERINTAKEMODE_H
#define HFCWRAPPERINTAKEMODE_H

#include "evolution.h"
#include "HfcObject.h"
#include "WrapperIntakeMode.h"

class HfcWrapperIntakeMode : public WrapperIntakeMode, public HfcObject
{
	public:

		HfcWrapperIntakeMode(const char *data);
		HfcWrapperIntakeMode(const char *data, int len, int formatType);

		~HfcWrapperIntakeMode();

		virtual const char *serialize(int format = InternalFormat) const;


	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCWRAPPERINTAKEMODE_H
