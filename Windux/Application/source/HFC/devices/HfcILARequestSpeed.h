/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCILAREQUESTSPEED_H
#define HFCILAREQUESTSPEED_H

#include "evolution.h"
#include "HfcObject.h"
#include "ILARequestSpeed.h"

typedef enum HfcILARequestSpeedFieldOffsets
{
	fieldOffsetILASpeedState = 1,
	endHfcILARequestSpeedFieldOffsets		//	MUST be last!
} HfcILARequestSpeedFieldOffsets_t;

class HfcILARequestSpeed : public ILARequestSpeed, public HfcObject
{
	public:

		HfcILARequestSpeed(const char *data);
		HfcILARequestSpeed(const char *data, int len, int formatType);

		~HfcILARequestSpeed();

		short getSpeedState() const;
		void setSpeedState(short speedState);

		virtual const char *serialize(int format = InternalFormat) const;

	private:

		int speedState;  // 0 = Off, 1 = On

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCILAREQUESTSPEED_H
