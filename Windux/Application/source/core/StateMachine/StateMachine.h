#ifndef STATEMACHINE_H
#define STATEMACHINE_H



#include <map>
#include <list>
#include <stdexcept>

#include "StateMachineBase.h"

template <class Controller, class Event> class State;
template <class Controller, class Event> class Transition;
template <class Controller, class Event> class EventAction;


template <class Controller>
class StateMachine : public StateMachineBase
{
	public:
		/* --- Types and subtypes --- */
		typedef typename Controller::stateId_type stateId_type;
		typedef typename Controller::event_type event_type;
		typedef typename event_type::eventId_type eventId_type;
		typedef State<Controller, event_type> state_type;
		typedef Transition<Controller, event_type> transition_type;
		typedef typename EventAction<Controller, event_type>::eventFunction_type eventFunction_type;
		typedef std::map<stateId_type, state_type*> stateContainer_type;
		typedef std::list<event_type*> eventContainer_type;
		typedef std::map<eventId_type, transition_type* > transitionContainer_type;
    #if defined(LINUX_FIX)
        typedef typename stateContainer_type::iterator              stateContainer_type_iterator;
        typedef typename stateContainer_type::const_iterator        stateContainer_type_const_iterator;
        typedef typename eventContainer_type::iterator              eventContainer_type_iterator;
        typedef typename transitionContainer_type::iterator         transitionContainer_type_iterator;
        typedef typename transitionContainer_type::const_iterator   transitionContainer_type_const_iterator;
    #endif

		/* --- Constructors and destructor --- */
		StateMachine(Controller& controller, bool notifyTopOfLoop = false, bool debug = false);
		virtual ~StateMachine();


		/* --- Accessor methods --- */
		const state_type* getInitialState() const { return initialState_; }

		const state_type* getPreviousState() const { return previousState_; }

		const state_type* getCurrentState() const { return currentState_; }

		const event_type* getCurrentEvent() const { return currentEvent_; }

		const transition_type* getCurrentTransition() const { return currentTransition_; }

		bool isLimitedExecutionTime() const { return limitedExecutionTime_; }

		int getMaxExecutionTime() const { return maxExecutionTime_; }
		void setMaxExecutionTime(int maxExecutionTimeMilli)
				{ maxExecutionTime_ = maxExecutionTimeMilli; limitedExecutionTime_ = (maxExecutionTime_ != 0); }

		void setNotifyTopOfLoop(bool notify = true) { notifyTopOfLoop_ = notify; }


		/* --- Instance function members --- */
		// When adding a new state to the StateMachine you may specify an attribute
		// (which can be used if a processing function needs to store "state" info),
		// an entry function, a processing function, and an exit function.
		void addState(stateId_type state, eventFunction_type process = 0,
				eventFunction_type entry = 0, eventFunction_type exit = 0);
		void addState(stateId_type state, int attribute, eventFunction_type process = 0,
				eventFunction_type entry = 0, eventFunction_type exit = 0);

		// When adding a new transition to the StateMachine you must specify a starting
		// state and an event and you may specify a destination state (if not specified
		// the end is assumed) and a function to execute while transitioning.
    #if defined( WIN32 )
		void addTransition(stateId_type state, eventId_type event,
				eventFunction_type function = 0) throw(.../*std::domain_error*/);
		void addTransition(stateId_type state, eventId_type event, stateId_type toState,
				eventFunction_type function = 0) throw(.../*std::domain_error*/);

		// A default transition can be initiated from any state.
		void addDefaultTransition(eventId_type event, eventFunction_type function = 0) throw(.../*std::domain_error*/);
		void addDefaultTransition(eventId_type event, stateId_type toState,
				eventFunction_type function = 0) throw(.../*std::domain_error*/);
    #elif   defined( __linux__ )
		void addTransition(stateId_type state, eventId_type event,
				eventFunction_type function = 0) throw(/*std::domain_error*/);
		void addTransition(stateId_type state, eventId_type event, stateId_type toState,
				eventFunction_type function = 0) throw(/*std::domain_error*/);

		// A default transition can be initiated from any state.
		void addDefaultTransition(eventId_type event, eventFunction_type function = 0) throw(/*std::domain_error*/);
		void addDefaultTransition(eventId_type event, stateId_type toState,
				eventFunction_type function = 0) throw(/*std::domain_error*/);
    #else
        #error "Unsupported system"
    #endif

    #if defined( WIN32 )
		void start(event_type* event, stateId_type id, int maxExecutionTimeMilli = 0) throw(.../*std::domain_error*/);
    #elif   defined( __linux__ )
		void start(event_type* event, stateId_type id, int maxExecutionTimeMilli = 0) throw(/*std::domain_error*/);
    #else
        #error "Unsupported system"
    #endif

		void postEvent(event_type* event) { events_.push_back(event); }
		void processSingleAction();
		void runStateMachine();

															

	private:
		Controller& controller_;
		stateContainer_type states_;
		eventContainer_type events_;
		transitionContainer_type defaultTransitions_;
		const state_type* initialState_;
		const state_type* previousState_;
		const state_type* currentState_;
		const event_type* currentEvent_;
		const transition_type* currentTransition_;
		bool limitedExecutionTime_;
		int maxExecutionTime_;
		bool notifyTopOfLoop_;


		void setupNextState(const state_type* nextState);
		void setupNextEvent();
		void setupNextTransition();
};


#include "Action.h"
#include "State.h"
#include "Transition.h"
#include "util/e_string/utilstring.h"


#ifndef NO_NAMESPACES
using namespace util_string;
#endif



template <class Controller>
StateMachine<Controller>::StateMachine(Controller& controller, bool notifyTopOfLoop, bool debug)
		: StateMachineBase(controller, debug), controller_(controller), states_(), events_(), defaultTransitions_(),
		initialState_(0), previousState_(0), currentState_(0), currentEvent_(0), currentTransition_(0),
		limitedExecutionTime_(false), maxExecutionTime_(0), notifyTopOfLoop_(notifyTopOfLoop)
{
}



template <class Controller>
StateMachine<Controller>::~StateMachine()
{
#if defined(LINUX_FIX)
	stateContainer_type_iterator iter1 = states_.begin();
#else
	stateContainer_type::iterator iter1 = states_.begin();
#endif
	while ( iter1 != states_.end() )
	{
		delete iter1->second;
		++iter1;
	}

#if defined(LINUX_FIX)
	transitionContainer_type_iterator iter2 = defaultTransitions_.begin();
#else
	transitionContainer_type::iterator iter2 = defaultTransitions_.begin();
#endif
	while ( iter2 != defaultTransitions_.end() )
	{
		delete iter2->second;
		++iter2;
	}

	if ( events_.size() )
	{
		debug("StateMachine.destructor", 10, "Found " + inttostring(events_.size()) + " unprocessed events");

    #if defined(LINUX_FIX)
        eventContainer_type_iterator iter3 = events_.begin();
    #else
		eventContainer_type::iterator iter3 = events_.begin();
    #endif
		while ( iter3 != events_.end() )
		{
			delete *iter3;
			++iter3;
		}
	}
}


template <class Controller>
void StateMachine<Controller>::addState(stateId_type state, eventFunction_type process, 
		eventFunction_type entry, eventFunction_type exit)
{
	states_.insert(
		std::make_pair(state, 
			new state_type(state, 0,
				process ? new EventAction<Controller, event_type>(process) : ( EventAction<Controller, event_type> * ) ( new NoAction<Controller, event_type>()),
				entry   ? new EventAction<Controller, event_type>(entry)   : ( EventAction<Controller, event_type> * ) ( new NoAction<Controller, event_type>()),
				exit    ? new EventAction<Controller, event_type>(exit)    : ( EventAction<Controller, event_type> * ) ( new NoAction<Controller, event_type>())) ) );
}


template <class Controller>
void StateMachine<Controller>::addState(stateId_type state, int attribute, eventFunction_type process, 
		eventFunction_type entry, eventFunction_type exit)
{
	states_.insert(
		std::make_pair(state, 
			new state_type(state, attribute,
				process ? new EventAction<Controller, event_type>(process) : ( EventAction<Controller, event_type> * ) ( new NoAction<Controller, event_type>() ),
				entry   ? new EventAction<Controller, event_type>(entry)   : ( EventAction<Controller, event_type> * ) ( new NoAction<Controller, event_type>() ),
				exit    ? new EventAction<Controller, event_type>(exit)    : ( EventAction<Controller, event_type> * ) ( new NoAction<Controller, event_type>() ) ) ) );
}


template <class Controller>
void StateMachine<Controller>::addTransition(stateId_type state, eventId_type event,
		eventFunction_type function)
    #if defined( WIN32 )
        throw(.../*std::domain_error*/)
    #elif   defined( __linux__ )
        throw(/*std::domain_error*/)
    #endif
{
#if defined(LINUX_FIX)
	stateContainer_type_iterator originState = states_.find(state);
#else
	stateContainer_type::iterator originState = states_.find(state);
#endif
	if ( originState != states_.end() )
	{
		originState->second->addTransition(event, 
			new transition_type(
				function ? new EventAction<Controller, event_type>(function) : ( EventAction<Controller, event_type> * )( new NoAction<Controller, event_type>() ) ) );

		return;
	}

	throw std::domain_error("unable to create transition for inexisting state");
}


template <class Controller>
void StateMachine<Controller>::addTransition(stateId_type state, eventId_type event, stateId_type toState, 
		eventFunction_type function)
    #if defined( WIN32 )
        throw(.../*std::domain_error*/)
    #elif   defined( __linux__ )
        throw(/*std::domain_error*/)
    #endif
{
#if defined(LINUX_FIX)
	stateContainer_type_iterator originState = states_.find(state);
	stateContainer_type_iterator destinationState = states_.find(toState);
#else
	stateContainer_type::iterator originState = states_.find(state);
	stateContainer_type::iterator destinationState = states_.find(toState);
#endif

	if ( originState != states_.end() && destinationState != states_.end() )
	{
		originState->second->addTransition(event, 
			new transition_type(
				function ? new EventAction<Controller, event_type>(function) : ( EventAction<Controller, event_type> * ) ( new NoAction<Controller, event_type>() ), 
				destinationState->second) );

		return;
	}

	throw std::domain_error("unable to create transition for inexisting states");
}


template <class Controller>
void StateMachine<Controller>::addDefaultTransition(eventId_type event, eventFunction_type function)
    #if defined( WIN32 )
		throw(.../*std::domain_error*/)
    #elif   defined( __linux__ )
        throw(/*std::domain_error*/)
    #endif
{
	defaultTransitions_.insert(
		make_pair(event, 
			new transition_type(
				function ? new EventAction<Controller, event_type>(function) : ( EventAction<Controller, event_type> * ) ( new NoAction<Controller, event_type>() ) ) ) );
}


template <class Controller>
void StateMachine<Controller>::addDefaultTransition(eventId_type event, stateId_type toState,
		eventFunction_type function)
    #if defined( WIN32 )
        throw(.../*std::domain_error*/)
    #elif   defined( __linux__ )
        throw(/*std::domain_error*/)
    #endif
{
#if defined(LINUX_FIX)
	stateContainer_type_iterator destinationState = states_.find(toState);
#else
	stateContainer_type::iterator destinationState = states_.find(toState);
#endif
	if ( destinationState != states_.end() )
	{
		defaultTransitions_.insert(
			std::make_pair(event, 
				new transition_type(
					function ? new EventAction<Controller, event_type>(function) : ( EventAction<Controller, event_type> * ) ( new NoAction<Controller, event_type>() ), 
					destinationState->second) ) );

		return;
	}

	throw std::domain_error("unable to create transition for inexisting destination state");
}


template <class Controller>
void StateMachine<Controller>::start(event_type* event, stateId_type id, int maxExecutionTimeMilli)
#if defined( WIN32 )
    throw(.../*std::domain_error*/)
#elif   defined( __linux__ )
    throw(/*std::domain_error*/)
#endif
{
	switch ( machineState_ )
	{
		case STOPPED_STATE:
		case ERROR_STATE:
		case TIMEDOUT_STATE:
		{
        #if defined(LINUX_FIX)
			stateContainer_type_const_iterator iter = states_.find(id);
        #else
			stateContainer_type::const_iterator iter = states_.find(id);
        #endif
			if ( iter != states_.end() )
			{
				// Set the initial state
				initialState_ = iter->second;

				// Set the first event
				postEvent(event);

				setMaxExecutionTime(maxExecutionTimeMilli);

				// Transition onto the initial state
				currentEvent_ = event;
				machineState_ = TRANSITIONING_STATE;

				setupNextState(initialState_);

				return;
			}

			throw std::domain_error("cannot start state machine with a non-existing state");
		}
		default:
		{
			// Do not start (do nothing)
			return;
		}
	}
}


template <class Controller>
void StateMachine<Controller>::processSingleAction()
{
	switch ( machineState_ )
	{
		case ENTERING_STATE:
		{
			// When the State Machine enters a state, the first thing it does is executed
			// the state's enter action. When this completes, then it moves onto PROCESSING_STATE
			if ( currentState_->enterState(controller_, *currentEvent_) )
				machineState_ = PROCESSING_STATE;

			break;
		}
		case PROCESSING_STATE:
		{
			// When the State Machine is in a state, it runs its process action. When this 
			// completes, the State Machine expects to find new events in order to transition
			// to another state.
			if ( currentState_->processState(controller_, *currentEvent_) )
			{
				while ( currentEvent_ )
				{
					// Find the next event
					setupNextEvent();
					// Find the transition associated with this new event
					setupNextTransition();
					// If there is no such a transition, log an error rejecting the event.
					// Then move on to the next event (next iteration of the loop).
					if ( !currentTransition_ )
					{
					}
					else
						break;
				}

				// If we have an event (and therefore a transition), leave the current state.
				if ( currentEvent_ )
					machineState_ = LEAVING_STATE;
				else
					machineState_ = ERROR_STATE;
			}

			break;
		}
		case LEAVING_STATE:
		{
			// When the State Machine is going to leave a state, it executes its exit action.
			// When this completes, the State Machine moves onto TRANSITIONING_STATE
			if ( currentState_->exitState(controller_, *currentEvent_) )
				machineState_ = TRANSITIONING_STATE;

			break;
		}
		case TRANSITIONING_STATE:
		{
			// When the State Machine is transitioning to another state, it executes the
			// transition's action. When this completes, the State Machine establishes
			// the new current state (the transition's target state) and moves onto
			// ENTERING_STATE. If this was the last transition, it moves onto STOPPED_STATE.
			if ( currentTransition_->processTransition(controller_, *currentEvent_) )
			{
				const state_type* nextState = (*currentTransition_)();
				if ( nextState )
					setupNextState(nextState);
				else
					machineState_ = STOPPED_STATE;
			}

			break;
		}
		default:
		{
			// Do not process anything (do nohing)
			break;
		}
	}
}


template <class Controller>
void StateMachine<Controller>::runStateMachine()
{
	bool loop = true;
	// Set up time marker to determine if the state machine has been running longer than allowed
#if defined(LINUX_FIX)
	util_time::DateTime startTime;
#else
    DateTime startTime;
#endif

	while ( loop )
	{
		switch ( machineState_ )
		{
			case STOPPED_STATE:
			case ERROR_STATE:
			case TIMEDOUT_STATE:
			{
				loop = false;
				break;
			}
			default:
			{
				if ( limitedExecutionTime_ )
				{
					// Check if the alloted time has passed
                #if defined(LINUX_FIX)
					if ( (util_time::DateTime() - startTime).getTotalMilliseconds() >= maxExecutionTime_ )
                #else
					if ( (DateTime() - startTime).getTotalMilliseconds() >= maxExecutionTime_ )
                #endif
					{
						machineState_ = TIMEDOUT_STATE;
						loop = false;
						break;
					}
				}

				if ( notifyTopOfLoop_ )
				{
					if ( !baseController_.atTopOfLoop() )
						return;
				}

				processSingleAction();

				// Give other threads (tasks) a chance to do something else
            #if defined(LINUX_FIX)
                PAL_absclass::PAL_sleep( 0 );
            #else
				Sleep( 0 );
            #endif
			}
		}
	}
}


template <class Controller>
void StateMachine<Controller>::setupNextState(const state_type* nextState)
{
	if ( machineState_ == TRANSITIONING_STATE )
	{
		previousState_ = currentState_;
		currentState_ = nextState;
		machineState_ = ENTERING_STATE;
	}
}


template <class Controller>
void StateMachine<Controller>::setupNextEvent()
{
	if ( machineState_ == PROCESSING_STATE )
	{
		// Remove top event
		if (currentEvent_ != 0)
		{
			event_type* pEvent = events_.front();
			events_.pop_front();
			delete pEvent;
		}

		//get next event
		if ( events_.size() )
			currentEvent_ = events_.front();
		else 
			currentEvent_ = 0;
	}
}


template <class Controller>
void StateMachine<Controller>::setupNextTransition()
{
	if ( machineState_ == PROCESSING_STATE )
	{
		if ( currentEvent_ )
		{
			currentTransition_ = currentState_->findTransition(*currentEvent_);
			if ( !currentTransition_ )
			{
				// If we could not find the next transition from the current state, check
				// among the default transitions for one associated with the current event
            #if defined(LINUX_FIX)
				transitionContainer_type_const_iterator iter = defaultTransitions_.find(currentEvent_->getId());
            #else
				transitionContainer_type::const_iterator iter = defaultTransitions_.find(currentEvent_->getId());
            #endif
				if ( iter != defaultTransitions_.end() ) 
					currentTransition_ = iter->second;
				else 
					currentTransition_ = 0;
			}
		}
		else
			currentTransition_ = 0;
	}
}



#endif // STATEMACHINE_H
