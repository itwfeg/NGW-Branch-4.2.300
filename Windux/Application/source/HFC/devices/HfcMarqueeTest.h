/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCMARQUEEMODE_H
#define HFCMARQUEEMODE_H

#include "evolution.h"
#include "HfcObject.h"
#include "Marquee.h"


typedef enum HfcMarqueeTestFieldOffsets
{
	fieldOffsetMarqueeTest = 1,

	endHfcMarqueeTestFieldOffsets	//	MUST be last!
} HfcMarqueeTestFieldOffsets_t;

class HfcMarqueeTest : public HfcObject
{
	public:

		HfcMarqueeTest();
		HfcMarqueeTest(const char *data, int len, int formatType);

		~HfcMarqueeTest();

		MqTestVal getTest();
		void setTest(MqTestVal test);


	private:
		MqTestVal test_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//	HFCMARQUEEMODE_H
