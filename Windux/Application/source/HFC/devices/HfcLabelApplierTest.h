/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCLABELAPPLIERTEST_H
#define HFCLABELAPPLIERTEST_H

#include "HfcObject.h"
#include "LabelApplierTest.h"

typedef enum HfcLabelApplierTestFieldOffsets
{
	fieldOffsetTestApplierStationId = 1,
	fieldOffsetTestApplierTestId,
	fieldOffsetTestApplierParm1,
	fieldOffsetTestApplierParm2,
	endHfcLabelTestApplierFieldOffsets	//	MUST be last!
} HfcLabelApplierTestFieldOffsets_t;

class HfcLabelApplierTest : public LabelApplierTest, public HfcObject
{
	public:

		HfcLabelApplierTest(const char *data);
		HfcLabelApplierTest(const char *data, int len, int formatType);

		~HfcLabelApplierTest();

		virtual const char *serialize(int format = InternalFormat) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	// HFCLABELAPPLIERTEST_H
