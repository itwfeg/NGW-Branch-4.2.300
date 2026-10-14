#ifndef STATE_H
#define STATE_H



#include <map>
#include <exception>



#include "Action.h"
template <class StateMachine, class Event> class Transition;



template<class StateMachine, class Event>
class State
{
	public:
		// Types and subtypes
		typedef typename StateMachine::stateId_type stateId_type;
		typedef Action<StateMachine, Event> action_type;
		typedef Transition<StateMachine, Event> transition_type;
		typedef std::map<typename Event::eventId_type, transition_type* > transitionsContainer_type; 
    #if defined(LINUX_FIX)
        typedef typename transitionsContainer_type::iterator        transitionsContainer_type_iterator;
        typedef typename transitionsContainer_type::const_iterator  transitionsContainer_type_const_iterator;
    #endif

		// Constructors and destructors
		State(stateId_type id, int attribute, action_type* process, action_type* entry, action_type* exit) 
				: id_(id), attribute_(attribute), enterAction_(entry), processAction_(process), exitAction_(exit) {}
		~State();


		// Accessor methods
		stateId_type getId() const { return id_; }
		int getAttribute() const { return attribute_; }
		void setAttribute(int attribute) { attribute_ = attribute; }

		void addTransition(typename Event::eventId_type id, transition_type* transition) 
				{ transitions_.insert(std::make_pair(id, transition)); }


		// Processing methods
		const transition_type* findTransition(const Event& event) const;

		bool enterState  (StateMachine& sm, const Event& event) const { return (*enterAction_)  (sm, event); }
		bool processState(StateMachine& sm, const Event& event) const { return (*processAction_)(sm, event); }
		bool exitState   (StateMachine& sm, const Event& event) const { return (* exitAction_)  (sm, event); }


	private:
		stateId_type id_;
		int attribute_;
		transitionsContainer_type transitions_;
		action_type* enterAction_;
		action_type* processAction_;
		action_type* exitAction_;
};




template <class StateMachine, class Event>
State<StateMachine, Event>::~State()
{
	if ( enterAction_ ) 
		delete enterAction_; 

	if ( processAction_ ) 
		delete processAction_; 

	if ( exitAction_ ) 
		delete exitAction_; 

#if defined(LINUX_FIX)
    transitionsContainer_type_iterator iter = transitions_.begin();
#else
	transitionsContainer_type::iterator iter = transitions_.begin(); 
#endif
	while ( iter != transitions_.end() ) 
		delete (iter++)->second;
}


template <class StateMachine, class Event>
//const State<StateMachine, Event>::transition_type * 
const Transition<StateMachine, Event> *
State<StateMachine, Event>::findTransition(const Event& event) const
{
#if defined(LINUX_FIX)
	transitionsContainer_type_const_iterator iter = transitions_.find(event.getId());
#else
	transitionsContainer_type::const_iterator iter = transitions_.find(event.getId());
#endif
	if ( iter != transitions_.end() ) 
		return iter->second; 
	else 
		return 0;
}



#endif // STATE_H
