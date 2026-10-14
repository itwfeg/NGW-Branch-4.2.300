#include "EvolWrapperDiagnosticsTest.h"

#include "WrapperEvent.h"
#include "HobString.h"
#include "Long.h"
#include "Debug.h"
#include "Hex.h"



#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperDiagnosticsTest::_dataType_ = 0;
std::string EvolWrapperDiagnosticsTest::_dataTypeName_("EvolWrapperDiagnosticsTest");
#endif


EvolWrapperDiagnosticsTest::EvolWrapperDiagnosticsTest(int test, Hobart::CEvolMsgReceiver *pHobThread)
		: WrapperGeneric(WrEvolDiagnosticsTest, pHobThread), test_(test)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	show();
}


EvolWrapperDiagnosticsTest::EvolWrapperDiagnosticsTest(const char* buffer)
		: WrapperGeneric(buffer), test_(0)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	EvolMessage::serialize();
	int bufCounter = EvolMessage::getSerializedLength();


	// Now, for the derived class items

	// Read the type of timings to request
	char* pChar = (char*)&test_;
	for (unsigned int i = 0; i < sizeof(test_); i++)
	{
		*pChar++ = buffer[bufCounter++];
	}
}


EvolWrapperDiagnosticsTest::~EvolWrapperDiagnosticsTest()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char* EvolWrapperDiagnosticsTest::serialize()const
{
	Debug::trace("EvolWrapperDiagnosticsTest", HobString("serialize()"));

	WrapperGeneric::serialize();

	// Now, for the derived class items

	// Write the type of timings to request
	const char* pChar = (const char*)&test_;
	for ( unsigned int i = 0; i < sizeof(test_); i++ )
		serializedData.append(1, pChar[i]);


	Debug::trace("EvolWrapperDiagnosticsTest", HobString("serialize: ") +
										HobString(Hex::toString(serializedData.data(), (int) serializedData.length())));

	return serializedData.data();
}


void EvolWrapperDiagnosticsTest::show() const
{
//	Debug::addTraceCategory("EvolWrapperDiagnosticsTest");

	Debug::trace("EvolWrapperDiagnosticsTest", "******* EvolWrapperDiagnosticsTest: ");
	Debug::trace("EvolWrapperDiagnosticsTest", "      test = " + Long::toString(test_));
	Debug::trace("EvolWrapperDiagnosticsTest", "******************************************");

//	Debug::removeTraceCategory("EvolWrapperDiagnosticsTest");
}
