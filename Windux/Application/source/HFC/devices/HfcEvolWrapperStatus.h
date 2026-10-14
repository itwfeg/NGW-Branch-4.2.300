/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERSTATUS_H
#define HFCEVOLWRAPPERSTATUS_H

#include "evolution.h"
#include "EvolWrapperStatus.h"
#include "HfcObject.h"

class HfcEvolWrapperStatus : public EvolWrapperStatus, public HfcObject
{
	public:
		HfcEvolWrapperStatus();
		HfcEvolWrapperStatus(const char *data);

		~HfcEvolWrapperStatus();

		const char *serialize(int format = BrowserFormat) const;

	private:
		typedef enum HfcEvolWrapperStatusFieldOffsets
		{
			fieldOffsetEvolWrSealerBeltTemperature = 1,
			fieldOffsetEvolWrFilmSupplyTemperature,
			fieldOffsetEvolWrAmbientTemperature,
			fieldOffsetEvolWrSealerBeltPower,
			fieldOffsetEvolWrMainPower,
			fieldOffsetEvolWrBuildingPressure,

			endHfcEvolWrapperStatusFieldOffsets		//	MUST be last!
		} HfcEvolWrapperStatusFieldOffsets_t;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCEVOLWRAPPERSTATUS_H
