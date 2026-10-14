#ifndef HFCLABELAPPLIERSETUP_H
#define HFCLABELAPPLIERSETUP_H
#include "HfcObject.h"
#include "LabelApplierSetup.h"

typedef enum HfcLabelApplierSetupOffsets
{
	fieldOffsetPrimaryLabelSize = 1,
	fieldOffsetPrimaryLabelRotation,
	fieldOffsetPrimaryLabelPlacement,
	fieldOffsetSecondaryLabelSize,
	fieldOffsetSecondaryLabelRotation,
	fieldOffsetSecondaryLabelPlacement,
	endHfcLabelApplierSetupOffsets
} HfcLabelApplierSetupOffsets_t;

class HfcLabelApplierSetup : public LabelApplierSetup, public HfcObject
{
	public:

		HfcLabelApplierSetup(const char *data);
		HfcLabelApplierSetup(const char *data, int len, int formatType);

		~HfcLabelApplierSetup();

		virtual const char *serialize(int format = InternalFormat) const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif
