/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERINQUIRE_H
#define HFCWRAPPERINQUIRE_H

#include "HfcWrapperConfig.h"

class HfcWrapperInquire : public HfcWrapperConfig
{
	public:

		HfcWrapperInquire();
		HfcWrapperInquire(const char *data);
		HfcWrapperInquire(const char *data, int len, int formatType);

		~HfcWrapperInquire();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCWRAPPERINQUIRE_H
