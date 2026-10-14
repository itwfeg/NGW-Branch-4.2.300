/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERTESTCOMPLETE_H
#define HFCEVOLWRAPPERTESTCOMPLETE_H

#include "evolution.h"
#include "HfcObject.h"

#include "EvolWrapperTestComplete.h"

typedef enum HfcEvolWrapperTestCompleteFieldOffsets
{
	fieldOffsetEvolWrTestCompTest = 1,
	fieldOffsetEvolWrTestCompSubTest,
	fieldOffsetEvolWrTestCompTestPassed,

	endHfcEvolWrapperTestCompleteFieldOffsets		//	MUST be last!
} HfcEvolWrapperTestCompleteFieldOffsets_t;

class HfcEvolWrapperTestComplete : public EvolWrapperTestComplete, public HfcObject
{
	public:
		HfcEvolWrapperTestComplete(const char *data);
		~HfcEvolWrapperTestComplete();

		virtual const char *serialize(int format = BrowserFormat)const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif	//	HFCEVOLWRAPPERTESTCOMPLETE_H
