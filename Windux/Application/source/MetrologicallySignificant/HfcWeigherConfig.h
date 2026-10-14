/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWEIGHERCONFIG_H
#define HFCWEIGHERCONFIG_H

#include "evolution.h"
#include "WeigherConfiguration.h"
#include "HfcObject.h"
#include "Configuration.h"

#include <list>

typedef enum HfcWeigherConfigFieldOffsets
{
	fieldOffsetFilterSpeed = 1,
	fieldOffsetScaleWeighMode,
	fieldOffsetScaleFactor,
	fieldOffsetMaxScaleWeight,
	fieldOffsetMinWeightToPrint,
	fieldOffsetCalibrationDate,
	fieldOffsetCalibrationTimes,
	fieldOffsetConfigurationDate,
	fieldOffsetConfigurationTimes,
	fieldOffsetWeigherModel,
	fieldOffsetUsingNetWeightDualRangeLimitCheck,
	fieldOffsetDualRangeLimit,
	fieldOffsetScaleValuemax,
	endHfcWeigherConfigFieldOffsets	//	MUST be last!
} HfcWeigherConfigFieldOffsets_t;


class HfcWeigherConfig : public WeigherConfiguration, public HfcObject
{
	public:

		HfcWeigherConfig();
		HfcWeigherConfig(const char *data);
		HfcWeigherConfig(const char *data, int len, int formatType);
		HfcWeigherConfig(/*const*/ HfcWeigherConfig *cfg);

		~HfcWeigherConfig();

		void init();

		virtual const char *serialize(int format = InternalFormat) const;
		
		bool update(HfcWeigherConfig *weigherConfig);

		void updateConfigurationDateAndTimes();
		void updateCalibrationDateAndTimes();

		bool configChanged(HfcWeigherConfig *cfg);

		HobWeight getMaxScaleWeight() const;
		void setMaxScaleWeight(HobWeight weight);

		bool isValid();

		static void initFieldList();


	private:

		HobWeight maxScaleWeight;		//	Maximum Weight the scale can weigh.

		static std::list<HfcWeigherConfigFieldOffsets> fieldList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCWEIGHERCONFIG_H
