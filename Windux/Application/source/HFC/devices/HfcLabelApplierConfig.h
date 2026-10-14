/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCLABELAPPLIERCONFIG_H
#define HFCLABELAPPLIERCONFIG_H

#include "evolution.h"
//#include "LabelApplierConfiguration.h"
#include "CompactLabelApplierConfiguration.h"
#include "HfcObject.h"
#include "Configuration.h"
												
typedef enum HfcLabelApplierConfigFieldOffsets
{
	fieldOffsetWeigherStationAttached = 1,
	fieldOffsetPrimaryStationAttached,
	fieldOffsetSecondaryStationAttached,
	fieldOffsetLeftToRight,
	fieldOffsetFrontDischarge,
	//fieldOffsetWeigherConveyorSensitivity,
	//fieldOffsetPrimaryPrinterConveyorSensitivity,
	//fieldOffsetSecondaryPrinterConveyorSensitivity,
	fieldOffsetPrimaryPrinterLabelOffset,
	//fieldOffsetPrimaryPrinterMaxJustificationDistance,
	fieldOffsetSecondaryPrinterLabelOffset,
	fieldOffsetLabelPlacement,
	fieldOffsetOverdrive,//Alex Zavala 11/7
	//fieldOffsetMiniStationLabelPlacement,
	//fieldOffsetDiagonalLabelPlacement,
	//fieldOffsetLabelPosition,
	endHfcLabelApplierConfigFieldOffsets	//	MUST be last!
} HfcLabelApplierConfigFieldOffsets_t;

class HfcLabelApplierConfig : public CompactLabelApplierConfiguration, public HfcObject//Changed to Compact
{
	public:

		HfcLabelApplierConfig();
		HfcLabelApplierConfig(HfcLabelApplierConfig *pHfcLaConfig);
		HfcLabelApplierConfig(const char *data);
		HfcLabelApplierConfig(const char *data, int len, int formatType);

		~HfcLabelApplierConfig();

		virtual const char *serialize(int format = BrowserFormat)const;

		bool update(HfcLabelApplierConfig &hfcLaConfig);

		LaPosLoc getLabelPosition();
		void setLabelPosition(LaPosLoc value);

	private:

		LaPosLoc	labelPosition_;	//	Left or Right side of the package. Based on leftToRight AND miniStationLabelPlacement
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCLABELAPPLIERCONFIG_H
