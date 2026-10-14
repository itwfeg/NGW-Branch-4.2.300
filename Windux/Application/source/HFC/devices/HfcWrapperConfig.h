/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERCONFIG_H
#define HFCWRAPPERCONFIG_H

#include "evolution.h"
#include "WrapperConfiguration.h"		//	Commented out until the file is created.
#include "HfcObject.h"
#include "Configuration.h"
#include <list>

class HfcWrapperPackCodeInfo;

class HfcWrapperConfig : public WrapperConfiguration, public HfcObject
{
	public:

		HfcWrapperConfig();
		HfcWrapperConfig(const char *data);
		HfcWrapperConfig(const char *data, int len, int formatType);

		~HfcWrapperConfig();

		void init();

		virtual const char *serialize(int format) const;

		void update(HfcWrapperPackCodeInfo *newPackCode);
		bool update(HfcWrapperConfig &wrConfig);
		void resetErrorTotals();
		void resetPackageTotals();

		bool itHasChanged();
		void clearHasChanged();

		void showPackCodeNames();

	private:
		typedef enum HfcWrapperConfigFieldOffsets
		{
			fieldOffsetWrConfigType = 1,
			fieldOffsetWrConfigPauseTimeout,
			fieldOffsetWrConfigIdleTimeout,
			fieldOffsetWrConfigFilmTension,
			fieldOffsetWrConfigPackWidth,
			fieldOffsetWrConfigPackageGrandTotal,
			fieldOffsetWrConfigWrapperCycleTotal,
			fieldOffsetWrConfigElevatorCollapseTotal,
			fieldOffsetWrConfigIntakeJamTotal,
			fieldOffsetWrConfigLengthJamTotal,
			fieldOffsetWrConfigWidthJamTotal,
			fieldOffsetWrConfigCoverOpenTotal,
			fieldOffsetWrConfigSealerBeltOpenTotal,
			fieldOffsetWrConfigPackageTooTallTotal,
			fieldOffsetWrConfigPusherHomeErrorTotal,
			fieldOffsetWrConfigFilmPullErrorTotal,
			fieldOffsetWrConfigCelsiusEnable,
			fieldOffsetWrConfigWideFilmTestEnable,
			fieldOffsetWrConfigNarrowFilmTestEnable,
			fieldOffsetWrConfigHeaterEnable,
			fieldOffsetWrConfigHeaterSetting,
			fieldOffsetWrConfigPackageLengthAdjustment,
			fieldOffsetWrConfigPackageWidthAdjustment,
			fieldOffsetWrConfigErrorTotalsDate,
			fieldOffsetWrConfigPackCodeTotalsDate,
			endHfcWrapperConfigFieldOffsets	//	MUST be last!
		} HfcWrapperConfigFieldOffsets_t;


		void checkToAdjustAdjacentPackCodes(WrPackCode packCodeNum);
		void changeGripperOnForAllPackCodes(WrPackCode packCodeNum);

		void updatePackCodeWidths(WrPackCode packCodeNum);
		void updatePackCodeLengths(WrPackCode packCodeNum);

		void updateWideFilmMax(int value);
		void updateNarrowFilmMin(int value);

		void updateLongFilmMax(int value);
		void updateMediumFilmMin(int value);
		void updateMediumFilmMax(int value);
		void updateShortFilmMin(int value);

		void adjustPackCodeWidthsLengths(WrPackCode packCodeNum);

		bool hasChanged;
		bool widthHasChanged;
		bool lengthHasChanged;
		bool gripperOnHasChanged;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCWRAPPERCONFIG_H
