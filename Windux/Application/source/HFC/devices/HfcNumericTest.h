/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCNUMERICMODE_H
#define HFCNUMERICMODE_H

#include "evolution.h"
#include "HfcObject.h"
#include "Numeric.h"


typedef enum HfcNumericTestFieldOffsets
{
	fieldOffsetNumericTest = 1,

	endHfcNumericTestFieldOffsets	//	MUST be last!
} HfcNumericTestFieldOffsets_t;

class HfcNumericTest : public HfcObject
{
	public:

		HfcNumericTest();
		HfcNumericTest(const char *data, int len, int formatType);

		~HfcNumericTest();

		NmTestVal getTest();
		void setTest(NmTestVal test);


	private:
		NmTestVal test_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//	HFCNUMERICMODE_H
