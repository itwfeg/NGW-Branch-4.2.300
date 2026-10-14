/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERINQUIRE_H
#define HFCEVOLWRAPPERINQUIRE_H

#include "HfcEvolWrapperConfig.h"

class HfcEvolWrapperInquire : public HfcEvolWrapperConfig
{
	public:

		HfcEvolWrapperInquire(const char *data);

		~HfcEvolWrapperInquire();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCEVOLWRAPPERINQUIRE_H
