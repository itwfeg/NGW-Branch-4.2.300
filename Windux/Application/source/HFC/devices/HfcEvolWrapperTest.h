/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERTEST_H
#define HFCEVOLWRAPPERTEST_H

#include "evolution.h"
#include "HfcObject.h"

#include "EvolWrapperTest.h"

typedef enum HfcEvolWrapperTestFieldOffsets
{
	fieldOffsetEvolWrTestTest = 1,			//	This should ALWAYS be before the Data!
	fieldOffsetEvolWrTestData1,
	fieldOffsetEvolWrTestData2,

	endHfcEvolWrapperTestFieldOffsets		//	MUST be last!
} HfcEvolWrapperTestFieldOffsets_t;

class HfcEvolWrapperTest : public EvolWrapperTest, public HfcObject
{
	public:
		HfcEvolWrapperTest(Hobart::CEvolMsgReceiver *pHobThread = NULL, WrEvolTestCode wrTestCode = NoServiceTest);
		HfcEvolWrapperTest(const char *data);
		HfcEvolWrapperTest(const char *data, int len, int formatType);

		~HfcEvolWrapperTest();

		virtual const char *serialize(int format = InternalFormat)const;


	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif	//	HFCEVOLWRAPPERTEST_H
