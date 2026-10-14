#ifndef TRANSITION_H
#define TRANSITION_H



#include "Action.h"



template <class StateMachine, class Event> class State;



template <class StateMachine, class Event>
class Transition
{
	public:
		typedef State<StateMachine, Event> state_type;
		typedef Action<StateMachine, Event> action_type;


		Transition(Action<StateMachine, Event>* action, const state_type* state = 0)
				: action_(action), nextState_(state) {}
		~Transition() { if ( action_ ) delete action_; }


		bool processTransition(StateMachine& sm, const Event& event) const { return (*action_)(sm, event); }

		const state_type* operator()() const { return nextState_; }


	private:
		action_type* action_;
		const state_type* nextState_;
};



#endif // TRANSITION_H
