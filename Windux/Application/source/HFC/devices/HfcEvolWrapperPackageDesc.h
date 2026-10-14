/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERPACKAGEDESC_H
#define HFCEVOLWRAPPERPACKAGEDESC_H

#include "evolution.h"
#include "EvolWrapperPackageDesc.h"

#include "HfcObject.h"

typedef enum HfcEvolWrapperPackageDescFieldOffsets
{
	fieldOffsetEvolWrPDId = 1,
	fieldOffsetEvolWrPDLength,
	fieldOffsetEvolWrPDWidth,
	fieldOffsetEvolWrPDHeight,
	fieldOffsetEvolWrPDLip,
	fieldOffsetEvolWrPDFilmWidthUsedToWrap,
	fieldOffsetEvolWrPDWrapBoxIndex,
	fieldOffsetEvolWrPDHeightBoxIndex,
	fieldOffsetEvolWrPDTrayName,
	fieldOffsetEvolWrPDUsedWrapBoxBestFit,
	fieldOffsetEvolWrPDModifiedNetStretch,
	endHfcEvolWrPackageDescFieldOffsets	//	MUST be last!
} HfcEvolWrapperPackageDescFieldOffsets_t;

class HfcEvolWrapperPackageDesc : public EvolWrapperPackageDesc, public HfcObject
{
	public:
		HfcEvolWrapperPackageDesc();
		HfcEvolWrapperPackageDesc(const char *data);
		HfcEvolWrapperPackageDesc(EvolWrapperPackageDesc *ewpd);

		~HfcEvolWrapperPackageDesc();

		void show();

		virtual const char *serialize(int format = BrowserFormat)const;


	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCEVOLWRAPPERPACKAGEDESC_H
