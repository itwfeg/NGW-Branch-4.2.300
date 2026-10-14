/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWEIGHERMODE_H
#define HFCWEIGHERMODE_H

#include "evolution.h"
#include "HfcObject.h"
#include "Wrapper.h"

typedef enum HfcWeigherModeFieldOffsets
{
	fieldOffsetWeigherMode = 1,

	endHfcWeigherModeFieldOffsets	//	MUST be last!
} HfcWeigherModeFieldOffsets_t;

class HfcWeigherMode : public HfcObject
{
	public:

		HfcWeigherMode();
		HfcWeigherMode(const char *data, int len, int formatType);

		~HfcWeigherMode();

		int getMode();
		void setMode(int mode);


	private:
		int			mode;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCWEIGHERMODE_H
