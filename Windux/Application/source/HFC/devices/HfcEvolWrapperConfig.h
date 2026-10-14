/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERCONFIG_H
#define HFCEVOLWRAPPERCONFIG_H

#include "evolution.h"
#include "HfcObject.h"
#include "EvolWrapperConfiguration.h"

typedef enum HfcEvolWrapperConfigFieldOffsets
{
	fieldOffsetEvolWrConfigUpperRollWidth = 1,
	fieldOffsetEvolWrConfigLowerRollWidth,
	fieldOffsetEvolWrConfigSealerBeltSetting,
	fieldOffsetEvolWrConfigSideDischargeType,
	fieldOffsetEvolWrConfigPriPlacementOffset,
	fieldOffsetEvolWrConfigPriRotationCalibOffset,
	fieldOffsetEvolWrConfigSecondCLAAttached,
	fieldOffsetEvolWrConfigWrapperOperation,
	fieldOffsetEvolWrConfigHardwareAddOn,
	fieldOffsetEvolWrConfigSecPlacementOffset,
	fieldOffsetEvolWrConfigAutoRotation,			//	Does the EWS have automatic rotation?
	fieldOffsetEvolWrConfigPriWandCalibOffset,
	fieldOffsetEvolWrConfigSecWandCalibOffset,
	fieldOffsetEvolWrConfigSealerBeltPackageSensorType,
	fieldOffsetEvolWrConfigUpperFilmPullTension,
	fieldOffsetEvolWrConfigLowerFilmPullTension,
	fieldOffsetEvolWrConfigHeaterIsUsedOffet,
	fieldOffsetEvolWrConfigPriRotationCalibOffset_90,
	fieldOffsetEvolWrConfigPriRotationCalibOffset_180,
	fieldOffsetEvolWrConfigPriRotationCalibOffset_270,
	fieldOffsetEvolWrConfigSecRotationCalibOffset_90,
	fieldOffsetEvolWrConfigSecRotationCalibOffset_180,
	fieldOffsetEvolWrConfigSecRotationCalibOffset_270,
	fieldOffsetEvolWrConfigSecRotationCalibOffset,		//0 degrees
	fieldOffsetEvolWrConfigPriApplierHandAdjust,
	fieldOffsetEvolWrConfigSecApplierHandAdjust,
	endHfcEvolWrConfigFieldOffsets	//	MUST be last!
} HfcEvolWrapperConfigFieldOffsets_t;

class HfcEvolWrapperControl;
class HfcEvolWrapperConfigLabelWidthUpdate;

class HfcEvolWrapperConfig : public EvolWrapperConfiguration, public HfcObject
{
	public:
		HfcEvolWrapperConfig(const char *data);
		HfcEvolWrapperConfig(const char *data, int len, int formatType = BrowserFormat);

		~HfcEvolWrapperConfig();

		virtual const char *serialize(int format = BrowserFormat)const;
		
		bool update(HfcEvolWrapperConfig *wrConfig);
		bool update(HfcEvolWrapperControl *wrControl);
		bool update(HfcEvolWrapperConfigLabelWidthUpdate *pWrWidthUpdate);	

		bool isSealerBeltEnabled() const;
		void setIsSealerBeltEnabled(bool value = true);
		void clearIsSealerBeltEnabled();

		bool wasSealerBeltTempValid() const;

		WrHardwareAddOn getHardwareAddOn() const;
		
		void setPriLabelWidth(LabelWidth size) {getLabelApplierConfig().setPriLabelWidth(size);}
		void setSecLabelWidth(LabelWidth size) {getLabelApplierConfig().setSecLabelWidth(size);}

	private:
		bool	sealerBeltEnabled;
		bool	sealerBeltTempWasValid;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//	HFCEVOLWRAPPERCONFIG_H
