/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCLABELAPPLIERMODE_H
#define HFCLABELAPPLIERMODE_H

#include "HfcObject.h"
#include "LabelApplierMode.h"

typedef enum HfcLaModeFieldOffsets
{
	fieldOffsetLaMode = 1,
	endHfcLaModeFieldOffsets	//	MUST be last!
} HfcLaModeFieldOffsets_t;

class HfcLabelApplierMode : public LabelApplierMode, public HfcObject
{
	public:
		HfcLabelApplierMode(const char *data, int len, int formatType);
		~HfcLabelApplierMode();

		virtual const char *serialize(int format = InternalFormat) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCLABELAPPLIERMODE_H
