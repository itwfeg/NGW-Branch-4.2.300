#ifndef STATEMACHINEBASE_H
#define STATEMACHINEBASE_H



#include <string>



#include "StateMachineController.h"



class StateMachineBase
{
	public:
		// Types and subtypes
		typedef enum 
		{
			ENTERING_STATE,
			PROCESSING_STATE,
			LEAVING_STATE,
			TRANSITIONING_STATE,
			STOPPED_STATE,
			TIMEDOUT_STATE,
			ERROR_STATE,
			INVALID_STATE
		} StateMachineState;

		// Constructor and destructor
		StateMachineBase(IStateMachineController& controller, bool debug); 
		virtual ~StateMachineBase();
		// Not defining a virtual destructor because this class is not to be used in polimorphism


		// Accessor functions
		StateMachineState getState() const { return machineState_; }

		const std::string& getStateName() const;

		bool getDebugMode() const { return debugMode_; }
		void setDebugMode(bool debug) { debugMode_ = debug; }

		StateMachineState operator()() const { return getState(); }


	protected:
		StateMachineState machineState_;
		IStateMachineController& baseController_;
		bool debugMode_;

		void debug(const std::string& category, int level, const std::string& message) const
				{ if ( debugMode_ /*|| true*/ ) baseController_.debug(category, level, message); }


	private:
		/* --- Function members --- */
		/* Operators */
		// Operator asignment is not available
		StateMachineBase& operator = (const StateMachineBase& rhs);

		static const std::string _statenameEntering_;
		static const std::string _statenameProcessing_;
		static const std::string _statenameLeaving_;
		static const std::string _statenameTransitioning_;
		static const std::string _statenameStopped_;
		static const std::string _statenameTimedout_;
		static const std::string _statenameError_;
		static const std::string _statenameUnknown_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif // STATEMACHINEBASE_H
