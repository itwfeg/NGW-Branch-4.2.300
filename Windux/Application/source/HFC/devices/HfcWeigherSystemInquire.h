/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWEIGHERSYSTEMINQUIRE_H
#define HFCWEIGHERSYSTEMINQUIRE_H

//#include "evolution.h"
//#include "HfcObject.h"
//#include "Configuration.h"
#include "HfcWeigherSystemConfig.h"

class HfcWeigherSystemInquire : public HfcWeigherSystemConfig
{
	public:

		HfcWeigherSystemInquire();
		HfcWeigherSystemInquire(const char *data);
		HfcWeigherSystemInquire(const char *data, int len, int formatType);

		~HfcWeigherSystemInquire();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCWEIGHERSYSTEMINQUIRE_H
