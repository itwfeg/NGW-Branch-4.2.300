#include "EvolWrapperDiagnosticsTestResult.h"

#include "WrapperEvent.h"
#include "EvolWrapperErrorInfo.h"

#include <HobString.h>
#include <Long.h>
#include <Debug.h>
#include <Hex.h>



#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperDiagnosticsTestResult::_dataType_ = 0;
std::string EvolWrapperDiagnosticsTestResult::_dataTypeName_("EvolWrapperDiagnosticsTestResult");
#endif


EvolWrapperDiagnosticsTestResult::EvolWrapperDiagnosticsTestResult(Hobart::CEvolMsgReceiver *pHobThread)
		: WrapperGeneric(WrEvolDiagnosticsTestResult, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	initialize();
	show();
}


EvolWrapperDiagnosticsTestResult::EvolWrapperDiagnosticsTestResult(const char* buffer)
		: WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	EvolMessage::serialize();
	int bufCounter = EvolMessage::getSerializedLength();

	// Now, for the derived class items
	memberPairList vars( getMembers() );

	for ( memberPairList::iterator i( vars.begin() ); i != vars.end(); ++i )
	{
		char *pChar( i->first );
		size_t sz( i->second );

		while ( sz-- )
			*pChar++ = buffer[bufCounter++];
	}
}


EvolWrapperDiagnosticsTestResult::~EvolWrapperDiagnosticsTestResult()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char* EvolWrapperDiagnosticsTestResult::serialize()const
{
	Debug::trace("EvolWrapperDiagnosticsTestResult", HobString("serialize()"));

	WrapperGeneric::serialize();

	// Now, for the derived class items
	memberPairList vars( getMembers() );

	for ( memberPairList::iterator i( vars.begin() ); i != vars.end(); ++i )
		serializedData.append( i->first, i->second );

	Debug::trace( "EvolWrapperDiagnosticsTestResult", HobString("serialize: ") +
			HobString(Hex::toString(serializedData.data(), (int) serializedData.length())));

	return serializedData.data();
}


void EvolWrapperDiagnosticsTestResult::show() const
{
//	Debug::addTraceCategory("EvolWrapperDiagnosticsTestResult");

	Debug::trace("EvolWrapperDiagnosticsTestResult", "******* EvolWrapperDiagnosticsTestResult: ");
	Debug::trace("EvolWrapperDiagnosticsTestResult", "               test = " + Long::toString(test_));
	Debug::trace("EvolWrapperDiagnosticsTestResult", "         testResult = " + Long::toString(testResult_));
	Debug::trace("EvolWrapperDiagnosticsTestResult", "   openLoopOverride = " + Long::toString(openLoopOverride_));
	Debug::trace("EvolWrapperDiagnosticsTestResult", "               diag = " + Long::toString(diag_));
	Debug::trace("EvolWrapperDiagnosticsTestResult", " diagExpectedResult = " + Long::toString(diagExpectedResult_));
	Debug::trace("EvolWrapperDiagnosticsTestResult", "   diagActualResult = " + Long::toString(diagActualResult_));
	Debug::trace("EvolWrapperDiagnosticsTestResult", "          errorCode = " + Long::toString(errorCode_));
	Debug::trace("EvolWrapperDiagnosticsTestResult", "******************************************");

//	Debug::removeTraceCategory("EvolWrapperDiagnosticsTestResult");
}

void EvolWrapperDiagnosticsTestResult::setTest(int value)
{
	test_ = value;

	if ( EvolWrapperErrorInfo::NoWrapperError != value )
	{
		// offset wrapper test by EvolWrapperErrorOffset
		test_ += EvolWrapperErrorInfo::EvolWrapperCommLost;

		// add wrapper device offset
		test_ += (ErrWrapper * DEVICE_ERROR_OFFSET);
    }
}

void EvolWrapperDiagnosticsTestResult::setErrorCode(int value)
{
	errorCode_ = value;

	if ( EvolWrapperErrorInfo::NoWrapperError != value )
	{
		// offset wrapper error code by EvolWrapperErrorOffset
		errorCode_ += EvolWrapperErrorInfo::EvolWrapperCommLost;

		// add wrapper device offset
		errorCode_ += (ErrWrapper * DEVICE_ERROR_OFFSET);
    }
}

void EvolWrapperDiagnosticsTestResult::initialize()
{
	test_ = 0;

	testResult_ = false;
	openLoopOverride_ = false;

	diag_ = 0;
	diagExpectedResult_ = 0;
	diagActualResult_ = 0;

	errorCode_ = 0;
}

EvolWrapperDiagnosticsTestResult::memberPairList EvolWrapperDiagnosticsTestResult::getMembers() const
{
	memberPairList result;
	result.push_back( memberPairList::value_type( (char*) &test_, sizeof(test_) ) );
	result.push_back( memberPairList::value_type( (char*) &testResult_, sizeof(testResult_) ) );
	result.push_back( memberPairList::value_type( (char*) &openLoopOverride_, sizeof(openLoopOverride_) ) );
	result.push_back( memberPairList::value_type( (char*) &diag_, sizeof(diag_) ) );
	result.push_back( memberPairList::value_type( (char*) &diagExpectedResult_, sizeof(diagExpectedResult_) ) );
	result.push_back( memberPairList::value_type( (char*) &diagActualResult_, sizeof(diagActualResult_) ) );
	result.push_back( memberPairList::value_type( (char*) &errorCode_, sizeof(errorCode_) ) );

	return result;
}

