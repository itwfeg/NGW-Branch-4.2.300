#ifndef ACTION_H
#define ACTION_H



template <class StateMachine, class Event>
class Action
{
	public:
		virtual ~Action() {}


		virtual bool operator()(StateMachine& sm, const Event& event) = 0;
};



template <class StateMachine, class Event>
class NoAction : public Action<StateMachine, Event>
{
	public:
		bool operator()(StateMachine& /*sm*/, const Event& /*event*/) { return true; }
};



template <class StateMachine, class Event>
class EventAction : public Action<StateMachine, Event>
{
	public:
		// Types and subtypes
		typedef bool (StateMachine::* eventFunction_type)(const Event& event);


		// Constructors and destructor
		EventAction(eventFunction_type function) : function_(function) {}


		bool operator()(StateMachine& sm, const Event& event) { return (sm.*function_)(event); }


	private:
		eventFunction_type function_;
};



#endif // ACTION_H
