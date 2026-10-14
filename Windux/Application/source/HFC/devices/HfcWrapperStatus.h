/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERSTATUS_H
#define HFCWRAPPERSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "WrapperStatus.h"

class Session;

typedef enum HfcWrapperStatusFieldOffsets
{
	fieldOffsetWrStFilmWidth = 1,
	fieldOffsetWrStFilmTension,
	fieldOffsetWrStBeltOnOff,
	fieldOffsetWrStBeltTempSetting,
	fieldOffsetWrStBeltTemperature,
	fieldOffsetWrStRate,
	fieldOffsetWrStMainMotor,

	endHfcWrapperStatusFieldOffsets		//	MUST be last!
} HfcWrapperStatusFieldOffsets_t;

class HfcWrapperStatus : public WrapperStatus, public HfcObject
{
	public:
		HfcWrapperStatus(const char *data);
		~HfcWrapperStatus();

		virtual const char *serialize(int format = InternalFormat) const;

		bool getInvalidPackSize();
		bool isInvalidPackSize();
		void setInvalidPackSize(bool value = true);

	private:

		Session	*mySession;

		bool invalidPackSize;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCWRAPPERSTATUS_H
