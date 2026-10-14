/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H

#include "evolution.h"
#include "EvolMessage.h"

#include "Timer.h"
#include "Timeout.h"
#include <list>

#include "CEvolMsgReceiver.h"

#include <util/thread/mutex.h>

#define MaxEvents endOfTimedFunction

class Timer;

class TimerManager : public Hobart::CEvolMsgReceiver, Util::CRunnableClass
{
	public:
		virtual ~TimerManager();

		Timer *getTimer(const int timerId);
		Timer *getTimer(const TimedFunction timerPurpose);
		Timer *getTimer(const unsigned int mySessionId, const TimedFunction timerPurpose);

		int addTimer(const unsigned int signalTime, const unsigned int repeatFrequency = Timer::RepeatNO, const TimedFunction timerFunction = timedFuncNone);
		int addTimer(const unsigned int mySessionId, const unsigned int signalTime, const unsigned int repeatFrequency = Timer::RepeatNO, const TimedFunction timerFunction = timedFuncNone);
		int addTimer(Hobart::CEvolMsgReceiver* receiver, const unsigned int signalTime, const unsigned int repeatFrequency = Timer::RepeatNO, const TimedFunction timerFunction = timedFuncNone);
		int addTimer(Timer *timer);

		void resetTimer(const int timerId, const int newSignalTime = 0);

		void removeTimer(const int timerId);
		void removeAllEventTimersForSession(const unsigned int sessionId);

		void displayAllTimers();

		static void processTimerSignal(const int expiredTimerId, const int timerIndex);

		static TimerManager *getInstance();

		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

	private:
		virtual DWORD Run();	
			
	private:
		Util::CSmartPtr<Util::CTLSThread> timerThread;

		mutable util_thread::Mutex<> eventTimersMutex; // SOF-1683
		std::list<Timer *> eventTimers;

		TimerManager( const char * name = "TimerManager" ); // need to private for singleton pattern.
		static TimerManager *instance; // only way to get access to object is via getInstance()

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //TIMERMANAGER_H
