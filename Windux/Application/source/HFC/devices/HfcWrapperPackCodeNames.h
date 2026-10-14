/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERPACKCODENAMES_H
#define HFCWRAPPERPACKCODENAMES_H

#include "HfcWrapperConfig.h"

class HobList;

class HfcWrapperPackCodeNames : public HfcObject
{
	public:

		HfcWrapperPackCodeNames(HfcWrapperConfig *wrapConfig);

		~HfcWrapperPackCodeNames();

		const char *serialize(int format) const;

	private:
		HfcWrapperConfig *wrapperConfig;
		HobList			 *packCodeNameList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCWRAPPERPACKCODENAMES_H
