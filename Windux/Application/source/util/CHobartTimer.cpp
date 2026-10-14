#include "PAL_platformheader.h"


#include "CHobartTimer.h"

#include "SimpleTimer.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace Hobart
{
#ifdef USE_ACCOUNTANT
	int CHobartTimer::dataType = 0;
	std::string CHobartTimer::dataTypeName("CHobartTimer"); 
#endif


	CHobartTimer::CHobartTimer()
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif	
	}

	CHobartTimer::~CHobartTimer()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
		timerMap.clear(); 
	}

	void CHobartTimer::createTimer( int due_time, int id, bool periodic )
	{
		try
		{
			Util::CLockCriticalSection lock( timerCS );

			cancelTimer( id  );

			Util::CSmartPtr<SimpleTimer> timer ( new SimpleTimer( id, due_time, this, periodic ) );
			timerMap[ id ] = timer;

			std::map< int, Util::CSmartPtr<SimpleTimer> >::iterator iter = timerMap.begin();
			while ( iter != timerMap.end() )
			{
				if ( iter->second->isDone() )
				{
                    // MS STL is non-standard - returns modified iter
                    // see above workaround
					iter = timerMap.erase( iter );

					continue;
				}
				++iter;
			}
		}
		catch (...)
		{
			LogCatchAndThrow;
		}
	}


	void CHobartTimer::cancelTimer( int id  )
	{
		try
		{
			Util::CLockCriticalSection lock( timerCS );

			std::map< int, Util::CSmartPtr<SimpleTimer> >::iterator iter = timerMap.begin();
			while ( iter != timerMap.end() )
			{
				if ( iter->second->getId() == id )
				{
					iter->second->stop();

					// when cancel timer invoked from within timeout process erase() will
					// deadlock... so check that we are not within timeout
					// if we are within the timeout thread then the stop() above will flag the
					// thread to terminate when it returns from timeout process
					if ( !iter->second->isTimeoutThread() )
						timerMap.erase( iter );

					return;
				}

				++iter;
			}
		}
		catch (...)
		{

		}
	}

	bool CHobartTimer::isTimerActive( int id  )
	{
		try
		{
			Util::CLockCriticalSection lock( timerCS );

			std::map< int, Util::CSmartPtr<SimpleTimer> >::iterator iter = timerMap.begin();
			while ( iter != timerMap.end() )
			{
				if ( iter->second->getId() == id )
					return !iter->second->isDone();

				++iter;
			}
		}
		catch (...)
		{
		}

		return false;
	}

	SimpleTimer * CHobartTimer::getTimer( int id  )
	{
		try
		{
			Util::CLockCriticalSection lock( timerCS );

			std::map< int, Util::CSmartPtr<SimpleTimer> >::iterator iter = timerMap.begin();
			while ( iter != timerMap.end() )
			{
				if ( iter->second->getId() == id )
				{
					return iter->second;
				}

				++iter;
			}
		}
		catch (...)
		{
		}

		return 0;
	}
}//end of namespace
