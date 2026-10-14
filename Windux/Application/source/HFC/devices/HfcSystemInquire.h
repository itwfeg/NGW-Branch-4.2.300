/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCSYSTEMINQUIRE_H
#define HFCSYSTEMINQUIRE_H

#include "evolution.h"
#include "HfcObject.h"
#include "Configuration.h"
#include "HfcSystemConfig.h"

class HfcSystemInquire : public HfcSystemConfig
{
	public:

		HfcSystemInquire();
		HfcSystemInquire(const char *data);
		HfcSystemInquire(const char *data, int len, int formatType);

		~HfcSystemInquire();

private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCSYSTEMINQUIRE_H
