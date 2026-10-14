#ifndef CHOBARTTIMER_H
#define CHOBARTTIMER_H



#include "ITimerHandler.h"

#include "SimpleTimer.h"

#include "CTLSThread.h"
#include "CSmartPtr.h"



namespace Hobart
{


	class SimpleTimer;


	class CHobartTimer : public ITimerHandler
	{
	public:
		/* --- Types and subtypes --- */
		static const bool PERIODIC_TIMER = true;


		/* --- Constructors and destructor --- */
		CHobartTimer();
		virtual ~CHobartTimer();

		
		void createTimer( int due_time,  int id = 0, bool periodic = false );
		void cancelTimer( int id = 0 );
		bool isTimerActive(int id );
		SimpleTimer * getTimer( int id  );
		
		void shutdownTimerThreads()
		{
			std::map< int, Util::CSmartPtr<SimpleTimer> >::iterator it;
			for (it = timerMap.begin(); it != timerMap.end(); ++ it)
				it->second->shutdownThread();
		}
							 			
	private:
		Util::CSmartCriticalSection  timerCS;	
		std::map< int, Util::CSmartPtr<SimpleTimer> > timerMap;	

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

	};


}



#endif /* CHOBARTTIMER_H */
