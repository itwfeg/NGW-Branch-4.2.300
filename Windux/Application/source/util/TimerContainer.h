#ifndef TIMERCONTAINER_H
#define TIMERCONTAINER_H



#include <map>



#include "ITimerHandler.h"


#include "CSmartPtr.h"
#include "CCriticalSection.h"



namespace Hobart
{


	class SimpleTimer;


	class TimerContainer : public ITimerHandler
	{
	public:
		/* --- Types and subtypes --- */
		typedef std::map< int, Util::CSmartPtr<SimpleTimer> > container_type;
		typedef container_type::iterator container_iterator;


		/* --- Constructors and destructor --- */
		TimerContainer();
		virtual ~TimerContainer();

		
		/* --- Instance function members --- */
		void createTimer( Util::CSmartPtr<SimpleTimer> timer, int id = 0 );
		void createTimer( int milliseconds, int id = 0, bool periodic = false );
		void cancelTimer( int id = 0 );

		void timeoutProcess( int /*timer_id*/ ) {}


	private:
		/* --- Instance data members --- */
		Util::CSmartCriticalSection  timerCS;	
		container_type container_;


		/* --- Instance function members --- */
		void cleanup();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
	};


}



#endif /* TIMERCONTAINER_H */
