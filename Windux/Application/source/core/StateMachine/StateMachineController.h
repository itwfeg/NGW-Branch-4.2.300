#ifndef STATEMACHINECONTROLLER_H
#define STATEMACHINECONTROLLER_H



#include <string>



class IStateMachineController
{
	public:
		// debug method that the state machine uses in debug mode
		virtual void debug(const std::string& /*category*/, int /*level*/, const std::string& /*message*/) const {}
		// Method to be notified of being at the top of the state machine loop (returning false means stop state machine)
		virtual bool atTopOfLoop() { return true; }
};



#endif // STATEMACHINECONTROLLER_H
