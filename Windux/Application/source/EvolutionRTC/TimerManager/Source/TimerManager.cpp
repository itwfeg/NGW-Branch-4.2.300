/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "TimerManager.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int TimerManager::dataType = 0;
std::string TimerManager::dataTypeName("TimerManager"); 
#endif

TimerManager *TimerManager::instance = (TimerManager *)NULL;

TimerManager::TimerManager( const char * name )
	: Hobart::CEvolMsgReceiver( name )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	//activeFunctionStart = true;
	Debug::trace("TimerManager", "TimerManager() is done");
	timerThread = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "TimeManager's Timer Thread" );			
	init();
}

TimerManager::~TimerManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	if (timerThread)
	{
		timerThread->StopThread();
		timerThread->WaitForCompletion();
	}
	ShutdownNicely();

	deregisterAllEvents();
}

void TimerManager::init()
{
	registerEvent( TimerDev, TimerAdd );
	Debug::trace("TimerManager", "Exiting init()");
}


DWORD TimerManager::Run() //checking all the timers timeout
{
	time_t old_ct = time(NULL); //keep track of last second, see if time may have been manually changed
	time_t ct = time(NULL); //what is the current time
	bool firstpass = true;
	bool trueVal = true;
	bool shouldNotify = true;

	do
	{
		try
		{
			//set the old_ct value to what ct was a second ago
			old_ct = ct;

			//grab current time
			time( &ct );

			eventTimersMutex.acquire();

			for ( std::list<Timer *>::iterator iter = eventTimers.begin(); iter != eventTimers.end(); iter++ )
			{
				//check if time has decreased or time difference is greater than 60 seconds to what it was previous
				//if so we are going to reset all timers to a new default time, since there appears to have been a time change
				//NOTE: if debugging, you may pass the 60 second threshold, and timers may be reset to latest time
				double timediff = difftime(ct, old_ct);
				if ( (ct < old_ct) || ((timediff > 60) && !firstpass) )
				{
					Debug::trace("TimerManager", "Detected Date/Time Change, updating timers..");

					// setup timer values again
					(*iter)->compensateForTimeChange(old_ct);
				}

				if ( ct >= ( *iter )->getEndDate().getCalendarTime() ) //signal it
				{
					shouldNotify = true;
					if ( (*iter)->getRepeatFrequency() == Timer::RepeatNO )
					{
						if ( (*iter)->isExpiredAndNotified() )
						{
							shouldNotify = false;
							Debug::trace("TimerManager", "Timer is RepeatNO, and is already expiredAndNotified, so not notifying!!");
						}
						else
							// !!! See comment in Timer.h about expiredAndNotified !!!
							(*iter)->setExpiredAndNotified(true);
					}
					else
						(*iter)->setupEndDate();

					if (shouldNotify)
					{
						Util::CSmartPtr< EvolMessage > timeoutMsg ( new Timeout( this, ( *iter )->getTimerId(), ( *iter )->getMySessionId(), ( *iter )->isSessionIdSet()) );
						dispatchEvent( timeoutMsg, ( *iter )->getMessageReceiver() );
					}
				}
			}

			eventTimersMutex.release();

			PAL_absclass::PAL_sleep( 1000 );        // Check the time once every second.
			firstpass = false;
		}
		catch(Util::CTerminateNow &)
		{
			Debug::trace("TimerManager.error", HobString("Run():  catch(Util::CTerminateNow &)"));
			return (DWORD)-1;
		}
		catch(Util::CBaseException & e)
		{
			Debug::trace("TimerManager.error", HobString("Run():  catch(Util::CBaseException & e)"));
			UTIL_REPORT_EXCEPTION(e);
			return (DWORD)-1;
		}
		catch(...)
		{
			Debug::trace("TimerManager.error", HobString("Run():  catch(...)"));
			LogCatch;
			return (DWORD)-1;
		}

	} while(trueVal);

	return 0;
}

void TimerManager::processEvent( Util::CSmartPtr< EvolMessageWrapper > & msg )
{
	try
	{
		if ( msg->getDeviceType() != TimerDev )
			Debug::trace("TimerManager", "Unknown Device");
		else
		{	
			switch ( msg->getEventType())
			{
				case TimerAdd:
				{
					Debug::trace("TimerManager", "TimerAdd message");
					break;
				}

				default:
					Debug::trace("TimerManager", "Unknown Evol Event");	
					break;
			}	 
		}	
	}
	catch (...)
	{
		Debug::trace("TimerManager.error", HobString("processEvent():  catch(...)"));
		LogCatchAndThrow;
	}		
}


// static
TimerManager *TimerManager::getInstance()
{
	if (!instance)
		instance = new TimerManager();

	return instance;
}


// If the timer with the corresponding timerId is in the list, return it.
Timer *TimerManager::getTimer(const int timerId)
{
	Timer *timer = (Timer *)NULL;

	if (Debug::isTracing("TimerManager"))
		Debug::trace("TimerManager", HobString("getTimer() - trying to find timer: ") +
												HobString("  timerId=") + HobString(Long::toString((long)timerId)));

	eventTimersMutex.acquire();

	if (timerId > 0 && !eventTimers.empty())
	{
		std::list<Timer *>::iterator i = eventTimers.begin();
		while (i != eventTimers.end())
		{
			if ((*i)->getTimerId() == timerId)
			{
				if (Debug::isTracing("TimerManager"))
					Debug::trace("TimerManager", HobString("found it!!  timerId=") + HobString(Long::toString((long)(*i)->getTimerId())));

				timer = *i;
				break;
			}
			i++;
		}
	}

	eventTimersMutex.release();

	return timer;
}


// If the timer with the corresponding sessionId and timerPurpose is in the list, return it.
Timer *TimerManager::getTimer(const TimedFunction timerPurpose)
{
	Timer *timer = (Timer *)NULL;

	if (Debug::isTracing("TimerManager"))
		Debug::trace("TimerManager", HobString("getTimer() - trying to find timer for non-specified Session: ") +
												HobString("  timerPurpose=") + HobString(Integer::toString((int)timerPurpose)));

	eventTimersMutex.acquire();

	if (!eventTimers.empty())
	{
		std::list<Timer *>::iterator i = eventTimers.begin();
		while (i != eventTimers.end())
		{
			if (!(*i)->isSessionIdSet() && (*i)->getTimerPurpose() == timerPurpose)
			{
				if (Debug::isTracing("TimerManager"))
					Debug::trace("TimerManager", HobString("found it!!  timerId=") + HobString(Long::toString((long)(*i)->getTimerId())));

				timer = *i;
				break;
			}
			i++;
		}
	}

	eventTimersMutex.release();

	return timer;
}

// If the timer with the corresponding sessionId and timerPurpose is in the list, return it.
Timer *TimerManager::getTimer(const unsigned int mySessionId, const TimedFunction timerPurpose)
{
	Timer *timer = (Timer *)NULL;

	if (Debug::isTracing("TimerManager"))
		Debug::trace("TimerManager", HobString("getTimer() - trying to find timer: ") +
												HobString("  mySessionId=") + HobString(Integer::toString(mySessionId)) +
												HobString("  timerPurpose=") + HobString(Integer::toString((int)timerPurpose)));

	eventTimersMutex.acquire();

	if (!eventTimers.empty())
	{
		std::list<Timer *>::iterator i = eventTimers.begin();
		while (i != eventTimers.end())
		{
			if ((*i)->isSessionIdSet() && (*i)->getMySessionId() == mySessionId &&
				 (*i)->getTimerPurpose() == timerPurpose)
			{
				if (Debug::isTracing("TimerManager"))
					Debug::trace("TimerManager", HobString("found it!!  timerId=") + HobString(Long::toString((long)(*i)->getTimerId())));

				timer = *i;
				break;
			}
			i++;
		}
	}

	eventTimersMutex.release();

	return timer;
}


int TimerManager::addTimer(const unsigned int signalTime, const unsigned int repeatFrequency, const TimedFunction timerPurpose)
{
	if (Debug::isTracing("TimerManager"))
		Debug::trace("TimerManager", HobString("addTimer() - ") +
												HobString("  signalTime=") + HobString(Integer::toString(signalTime)) + 
												HobString("  repeatFrequency=") + HobString(Integer::toString(repeatFrequency)) +
												HobString("  timerPurpose=") + HobString(Integer::toString((int)timerPurpose)));

	int createReturn = (int)NULL;
	Timer *newTimer = new Timer();
	if (newTimer)
	{
		newTimer->setSignalTime((unsigned short)signalTime);
		newTimer->setRepeatFrequency((short)repeatFrequency);
		newTimer->setTimerPurpose(timerPurpose);
		newTimer->setupEndDate();

		createReturn = addTimer(newTimer);
		if (!createReturn)
			delete newTimer;
	}
	return (createReturn);
}

int TimerManager::addTimer(const unsigned int mySessionId, const unsigned int signalTime, const unsigned int repeatFrequency, const TimedFunction timerPurpose)
{
	if (Debug::isTracing("TimerManager"))
		Debug::trace("TimerManager", HobString("addTimer() - ") +
												HobString("  mySessionId=") + HobString(Integer::toString(mySessionId)) +
												HobString("  signalTime=") + HobString(Integer::toString(signalTime)) + 
												HobString("  repeatFrequency=") + HobString(Integer::toString(repeatFrequency)) +
												HobString("  timerPurpose=") + HobString(Integer::toString((int)timerPurpose)));

	int createReturn = (int)NULL;
	Timer *newTimer = new Timer();
	if (newTimer)
	{
		newTimer->setMySessionId(mySessionId);
		newTimer->setSignalTime(signalTime);
		newTimer->setRepeatFrequency((short)repeatFrequency);
		newTimer->setTimerPurpose(timerPurpose);
		newTimer->setupEndDate();

		createReturn = addTimer(newTimer);
		if (!createReturn)
			delete newTimer;
	}
	return (createReturn);
}

int TimerManager::addTimer(Hobart::CEvolMsgReceiver* receiver, const unsigned int signalTime, const unsigned int repeatFrequency, const TimedFunction timerPurpose)
{
	if (Debug::isTracing("TimerManager"))
		Debug::trace("TimerManager", HobString("addTimer() - ") +
												HobString("  MsgReceiver=") + HobString(Hex::toString((long long)receiver)) +
												HobString("  signalTime=") + HobString(Integer::toString(signalTime)) + 
												HobString("  repeatFrequency=") + HobString(Integer::toString(repeatFrequency)) +
												HobString("  timerPurpose=") + HobString(Integer::toString((int)timerPurpose)));

	int createReturn = (int)NULL;
	Timer *newTimer = new Timer();
	if (newTimer)
	{
		newTimer->setMessageReceiver(receiver);
		newTimer->setSignalTime(signalTime);
		newTimer->setRepeatFrequency((short)repeatFrequency);
		newTimer->setTimerPurpose(timerPurpose);
		newTimer->setupEndDate();

		createReturn = addTimer(newTimer);
		if (!createReturn)
			delete newTimer;
	}
	return (createReturn);
}

int TimerManager::addTimer(Timer *newTimer)
{
	if (!newTimer)
		return ((int)NULL);

	// If this timer doesn't repeat, then push it on the front of the list, since it will expire, and then likely will be
	// removed from the list once it expires, so pushing on the front should make it quicker to find.
	eventTimersMutex.acquire();
	if (newTimer->getRepeatFrequency() == Timer::RepeatNO)
		eventTimers.push_front(newTimer);
	else
		eventTimers.push_back(newTimer);
	eventTimersMutex.release();

	//	Then arm the timer.
	Debug::trace("TimerManager", HobString("calling Timer.setTimer()"));
	if (newTimer->setTimer() == ERROR_CODE)
	{
		Debug::trace("TimerManager", HobString("addTimer() : timer NOT connected - sessionId:") + HobString(Integer::toString(newTimer->getMySessionId())) +
												HobString("  timerPurpose:") + HobString(Integer::toString(newTimer->getTimerPurpose())));
		removeTimer(newTimer->getTimerId());
		return ((int)NULL);
	}

	displayAllTimers();

	Debug::trace("TimerManager", HobString("addTimer() : timerAdded - timerId:") + HobString(Long::toString((long)newTimer->getTimerId())));
	return newTimer->getTimerId();
}


/* If passing in a newSignalTime, then the length of the timer is (could be) changing.
 * So set the signal time, before resetting the timer.
 * Note: Can't set the signal time to 0, since 0 is the default value if none is passed in.
 * Whether or not the signal time is changing, start the timer over.
 */
void TimerManager::resetTimer(const int timerId, const int newSignalTime)
{
	if (!timerId)
		return;

	Timer *timer = getTimer(timerId);
	if (timer)
	{
		Debug::trace("TimerManager", HobString("resetTimer() : timerId:") + HobString(Long::toString((long)timer->getTimerId())) +
												HobString("  newSignalTime:") + HobString(Integer::toString(newSignalTime)));
		if (newSignalTime)
			timer->setSignalTime(newSignalTime);
		timer->setupEndDate();
		timer->setTimer();
	}
}


void TimerManager::removeTimer(const int timerId)
{
	Timer *timer = getTimer(timerId);
	if (timer)
	{
		if (Debug::isTracing("TimerManager"))
			Debug::trace("TimerManager", HobString("removeTimer() - ") +
													HobString("  timerId=") + HobString(Long::toString((long)timer->getTimerId())) +
													HobString("  mySessionId=") + HobString(Integer::toString(timer->getMySessionId())) +
													HobString("  signalTime=") + HobString(Integer::toString(timer->getSignalTime())) + 
													HobString("  repeatFrequency=") + HobString(Integer::toString(timer->getRepeatFrequency())) +
													HobString("  timerPurpose=") + HobString(Integer::toString((int)timer->getTimerPurpose())));

		eventTimersMutex.acquire();

		delete timer;
		eventTimers.remove(timer);

		eventTimersMutex.release();
	}

	displayAllTimers();
}


void TimerManager::removeAllEventTimersForSession(const unsigned int sessionId)
{
	Debug::trace("TimerManager", HobString("removeAllEventTimersForSession() - sessionId:") + HobString(Integer::toString(sessionId)));

	eventTimersMutex.acquire();

	if (!eventTimers.empty())
	{
		std::list<Timer *>::iterator i = eventTimers.begin();
		while (i != eventTimers.end())
		{
			if ((*i)->isSessionIdSet() && (*i)->getMySessionId() == sessionId)
			{
				Timer *timer = *i;

				if (Debug::isTracing("TimerManager"))
					Debug::trace("TimerManager", HobString("removeAllEventTimersForSession() - ") +
															HobString("  timerId=") + HobString(Long::toString((long)timer->getTimerId())) +
															HobString("  mySessionId=") + HobString(Integer::toString(timer->getMySessionId())) +
															HobString("  signalTime=") + HobString(Integer::toString(timer->getSignalTime())) + 
															HobString("  repeatFrequency=") + HobString(Integer::toString(timer->getRepeatFrequency())) +
															HobString("  timerPurpose=") + HobString(Integer::toString((int)timer->getTimerPurpose())));

				delete timer;
				i = eventTimers.erase(i);
			}
			else
				i++;
		}
	}

	eventTimersMutex.release();

	displayAllTimers();
}


void TimerManager::displayAllTimers()
{
	if (Debug::isTracing("TimerManager"))
	{
		Debug::trace("TimerManager", "Displaying all timers ...");

		eventTimersMutex.acquire();

		std::list<Timer *>::iterator i = eventTimers.begin();
		while (i != eventTimers.end())
		{
			(*i)->show();
			i++;
		}

		eventTimersMutex.release();
	}		
}


// static
void TimerManager::processTimerSignal(const int expiredTimerId, const int timerPurpose)
{
	if (Debug::isTracing("TimerManager"))
		Debug::trace("TimerManager", HobString("processTimerSignal() : expiredTimerId = ") + HobString(Long::toString((long)expiredTimerId)) +
												HobString(" : timerPurpose arg = ") + HobString(Integer::toString(timerPurpose)));

	TimerManager *timerManager = TimerManager::getInstance();
	if (timerManager)
	{
		Timer *timer = timerManager->getTimer(expiredTimerId);
		if (timer)
		{
			if (Debug::isTracing("TimerManager"))
			{
				HobDateTime currentTime;
				HobDateTime endTime = timer->getEndDate();

				Debug::trace("TimerManager", HobString("processTimerSignal() : found the expired timer - ") +
														HobString("  mySessionId: ") + HobString(Long::toString(timer->getMySessionId())) +
														HobString("  isSessionIdSet: ") + HobString(Bool::toString(timer->isSessionIdSet())) +
														HobString("  signalTime: ") + HobString(Integer::toString(timer->getSignalTime())) +
														HobString("  timerPurpose: ") + HobString(Integer::toString((int)timer->getTimerPurpose())) +
														HobString("  endtime: ") + HobString(Long::toString((long)endTime.getCalendarTime())) +
														HobString("  currenttime: ") + HobString(Long::toString((long)currentTime.getCalendarTime())));
			}

			Util::CSmartPtr< EvolMessage > timeoutMsg ( new Timeout(timerManager, timer->getTimerId(), timer->getMySessionId(), timer->isSessionIdSet()) );
			timerManager->dispatchEvent( timeoutMsg, timerManager);
		}
	}
}
