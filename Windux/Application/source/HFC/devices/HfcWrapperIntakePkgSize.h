/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERINTAKEPKGSIZE_H
#define HFCWRAPPERINTAKEPKGSIZE_H

#include "HfcWrapperStatus.h"
class WrapperPackageInfo;
class HfcWrapperConfig;

typedef enum HfcWrapperIntakePkgSizeFieldOffsets
{
	fieldOffsetWrIntakePkgWidth = 1,
	fieldOffsetWrIntakePkgLength,

	endHfcWrapperIntakePkgSizeFieldOffsets	//	MUST be last!
} HfcWrapperIntakePkgSizeFieldOffsets_t;

class HfcWrapperIntakePkgSize : public WrapperPackageInfo, public HfcObject
{
	public:

		HfcWrapperIntakePkgSize(HfcWrapperStatus *wrapStatus);
		HfcWrapperIntakePkgSize(HfcWrapperStatus *wrapStatus, HfcWrapperConfig *wrapperConfig, WrPackCode packCode);

		~HfcWrapperIntakePkgSize();

		void setIntakePkgSize(HfcWrapperStatus *wrapStatus);
		bool thePackageFits();

		const char *serialize(int format) const;

	private:
		bool	packageFits;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCWRAPPERINTAKEPKGSIZE_H
