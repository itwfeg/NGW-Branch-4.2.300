/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERLOAD_H
#define HFCWRAPPERLOAD_H

#include "evolution.h"
#include "HfcObject.h"
#include "WrapperLoad.h"

class HfcWrapperLoad : public WrapperLoad, public HfcObject
{
	public:

		HfcWrapperLoad(const char *data);
		HfcWrapperLoad(const char *data, int len, int formatType);

		~HfcWrapperLoad();

		virtual const char *serialize(int format = InternalFormat) const;


	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCWRAPPERLOAD_H
