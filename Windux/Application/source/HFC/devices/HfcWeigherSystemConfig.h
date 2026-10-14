/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWEIGHERSYSTEMCONFIG_H
#define HFCWEIGHERSYSTEMCONFIG_H

#include "HfcSystemConfig.h"

class HfcWeigherSystemConfig : public HfcSystemConfig
{
	public:

		HfcWeigherSystemConfig();
		HfcWeigherSystemConfig(const char *data);
		HfcWeigherSystemConfig(const char *data, int len, int formatType);

		~HfcWeigherSystemConfig();


	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCWEIGHERSYSTEMCONFIG_H
