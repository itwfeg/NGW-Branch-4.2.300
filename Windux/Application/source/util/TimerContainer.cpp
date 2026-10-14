#include "PAL_platformheader.h"


#include "TimerContainer.h"

#include "SimpleTimer.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace Hobart
{
#ifdef USE_ACCOUNTANT
	int TimerContainer::dataType = 0;
	std::string TimerContainer::dataTypeName("TimerContainer"); 
#endif

	TimerContainer::TimerContainer() 
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif	
	}

	TimerContainer::~TimerContainer()
	{ 
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
		container_.clear(); 
	}

	void TimerContainer::createTimer( Util::CSmartPtr<SimpleTimer> timer, int id )
	{
		try
		{
			Util::CLockCriticalSection lock( timerCS );

			cancelTimer( id  );

			container_[ id ] = timer;

			cleanup();
		}
		catch (...)
		{
			LogCatchAndThrow;
		}
	}


	void TimerContainer::createTimer( int due_time, int id, bool periodic )
	{
		try
		{
			Util::CLockCriticalSection lock( timerCS );

			cancelTimer( id  );

			Util::CSmartPtr<SimpleTimer> timer ( new SimpleTimer( id, due_time, this, periodic ) );
			container_[ id ] = timer;

			container_iterator iter = container_.begin();

			cleanup();
		}
		catch (...)
		{
			LogCatchAndThrow;
		}
	}


	void TimerContainer::cancelTimer( int id  )
	{
		try
		{
			Util::CLockCriticalSection lock( timerCS );

			container_iterator iter = container_.begin();
			while ( iter != container_.end() )
			{
				if ( iter->second->getId() == id )
				{
					container_.erase( iter );

					return;
				}

				++iter;
			}
		}
		catch (...)
		{
		}
	}


	void TimerContainer::cleanup()
	{
		container_iterator iter = container_.begin();
		while ( iter != container_.end() )
		{
        #if defined(LINUX_FIX)
            container_iterator iter_to_erase;
            iter_to_erase = iter;
            if ( iter_to_erase->second->isDone() )
            {
                // iter = container_.erase( iter );
                container_.erase( iter_to_erase );
        
                continue;
            }
        #else
			if ( iter->second->isDone() )
			{
				iter = container_.erase( iter );

				continue;
			}
        #endif

			++iter;
		}
	}


}//end of namespace
