/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERTEST_H
#define HFCWRAPPERTEST_H

#include "evolution.h"
#include "HfcObject.h"
#include "WrapperTest.h"

class HfcWrapperTest : public WrapperTest, public HfcObject
{
	public:

		HfcWrapperTest(const char *data);
		HfcWrapperTest(const char *data, int len, int formatType);

		~HfcWrapperTest();

		virtual const char *serialize(int format = InternalFormat) const;


	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCWRAPPERTEST_H
