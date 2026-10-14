#include "StateMachineBase.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int StateMachineBase::_dataType_ = 0;
	std::string StateMachineBase::_dataTypeName_("StateMachineBase"); 
#endif

const std::string StateMachineBase::_statenameEntering_       ("ENTERING state");
const std::string StateMachineBase::_statenameProcessing_     ("PROCESSING state");
const std::string StateMachineBase::_statenameLeaving_        ("LEAVING state");
const std::string StateMachineBase::_statenameTransitioning_  ("TRANSITIONING state");
const std::string StateMachineBase::_statenameStopped_        ("STOPPED state");
const std::string StateMachineBase::_statenameTimedout_       ("TIMED-OUT state");
const std::string StateMachineBase::_statenameError_          ("ERROR state");
const std::string StateMachineBase::_statenameUnknown_        ("UNKNOWN state");


StateMachineBase::StateMachineBase(IStateMachineController& controller, bool debug) 
				: machineState_(STOPPED_STATE), baseController_(controller), debugMode_(debug) 
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
}

StateMachineBase::~StateMachineBase()
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
}

const std::string& StateMachineBase::getStateName() const
{
	switch ( machineState_ )
	{
		case ENTERING_STATE:          return _statenameEntering_;
		case PROCESSING_STATE:        return _statenameProcessing_;
		case LEAVING_STATE:           return _statenameLeaving_;
		case TRANSITIONING_STATE:     return _statenameTransitioning_;
		case STOPPED_STATE:           return _statenameStopped_;
		case TIMEDOUT_STATE:          return _statenameTimedout_;
		case ERROR_STATE:             return _statenameError_;
		case INVALID_STATE:
		default:                      return _statenameUnknown_;
	}
}

