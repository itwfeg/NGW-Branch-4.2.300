/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef TIMER_H
#define TIMER_H

#include "evolution.h"
#include "HfcObject.h"
#include "TimerEvent.h"
#include "HobDateTime.h"
#include <time.h>


namespace Hobart
{
	class CEvolMsgReceiver;
}


typedef void (*VOIDFUNCPTR)();

class Timer : public HfcObject
{
	public:
		Timer();
		Timer(Hobart::CEvolMsgReceiver* receiver, unsigned int signalTime, TimedFunction timerPurpose, short repeatFreq = RepeatNO);
		Timer(const char *buffer);

		~Timer();

    #if defined(LINUX_FIX)
		int getTimerId() const;
    #else
		const int getTimerId() const;
    #endif
		void setTimerId(const int timerId);

    #if defined(LINUX_FIX)
		unsigned int getMySessionId() const;
    #else
		const unsigned int getMySessionId() const;
    #endif
		void setMySessionId(const unsigned int	mySessionId);

		bool isSessionIdSet() const;

		Hobart::CEvolMsgReceiver* getMessageReceiver() const { return receiver; }
		void setMessageReceiver(Hobart::CEvolMsgReceiver* rec) { receiver = rec; }

    #if defined(LINUX_FIX)
		unsigned int getSignalTime() const;
    #else
		const unsigned int getMySessionId() const;
    #endif
		void setSignalTime(const unsigned int signalTime);

    #if defined(LINUX_FIX)
		short getRepeatFrequency() const;
    #else
		const short getRepeatFrequency() const;
    #endif
		void setRepeatFrequency(const short repeatFrequency);

    #if defined(LINUX_FIX)
		TimedFunction getTimerPurpose() const;
    #else
		const TimedFunction getTimerPurpose() const;
    #endif
		void setTimerPurpose(const TimedFunction timerPurpose);

		const HobDateTime &getStartDate() const;

		const HobDateTime &getEndDate() const;
		void setEndDate(const HobDateTime &endDate);

		VOIDFUNCPTR getFunction();
		void setFunction(VOIDFUNCPTR function);

		const HobString &getFunctionParams() const;
		void setFunctionParams(const HobString &params);

		bool isExpiredAndNotified() const { return expiredAndNotified; }
		void setExpiredAndNotified(bool exp) { expiredAndNotified = exp; }


		int createTimer();
		int connectTimer();
		int setTimer();

		time_t setupEndDate();
		
		void compensateForTimeChange(time_t old_current_time);

		bool operator==(const Timer &timer) const;

		void operator=(const Timer &timer);

		const char *serialize(int format) const;

		void show();


	private:

		int			timerId;
		unsigned int	mySessionId;
		Hobart::CEvolMsgReceiver* receiver;

		// read-only, internally managed.
		bool				sessionIdSet;

		unsigned int	signalTime;
		short			repeatFrequency;
		TimedFunction	timerPurpose;

		HobDateTime		startDate;
		HobDateTime		endDate;

		VOIDFUNCPTR		function;
		HobString		functionParams;

		// I created this attribute because of the following situation.  The attribute 'repeatFrequency' indicates
		// if this timer should repeat after it expires.  How often it expires, the timeout time, is based on the
		// attribute 'signalTime'.  If the 'repeatFrequency' is RepeatNO, when the timer expires, the timer's creator
		// is notified that it expired, and the timer should never expire again and notify the creator.  As it turns out,
		// the timer's creator has to remove the timer so it doesn't fire again; the TimerManager doesn't remove it.  The
		// timer remains in the TimerManager's list of timers (it's 'eventTimers' attribute).  One second later, the
		// TimerManager will again see that this timer is expired, and notify the creator again.  The timer's creator set
		// it up so that it wouldn't repeat, so it shouldn't be notified a second time.  Session.cpp, a class that uses
		// timers, removes a timer set up as RepeatNO after it handles the expired timer.  SessionManager.cpp also uses timers,
		// but didn't remove one set up as RepeatNO after it expired, and so SessionManager kept getting messages that this
		// timer has expired, when it really only wanted the first notification of the timer expiring.  I added the code
		// that removes timers set up as RepeatNO after handling its expiration to SessionManager.  However, if the timer's
		// creator takes more than a second to process the expired timer, the TimerManager will send out multiple notices
		// of the timer's expiration before the timer's creator gets to removing it.  So I'm adding this attribute to not 
		// send out multiple notifications for timers set up as RepeatNO.  It is still the responsibility of the timer's
		// creator to remove the timer, and it will remain in the TimerManager's 'eventTimers' list, but now RepeatNO will
		// truly mean, "don't repeat this timer".
		bool expiredAndNotified;

		void init();
		static int getId();


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

	public:
		static const short RepeatNO			= 0x00;
		static const short RepeatInMinutes	= 0x01;
		static const short RepeatInHours	= 0x02;
		static const short RepeatInDays		= 0x04;
		static const short RepeatInWeeks	= 0x08;
		static const short RepeatInMonths	= 0x10;
		static const short RepeatInYears	= 0x20;

		static const short EveryDayNO		= 0x00;
		static const short EverySunday		= 0x01;
		static const short EveryMonday		= 0x02;
		static const short EveryTuesday		= 0x04;
		static const short EveryWednesday	= 0x08;
		static const short EveryThursday	= 0x10;
		static const short EveryFriday		= 0x20;
		static const short EverySaturday	= 0x40;
};

#endif //TIMER_H
