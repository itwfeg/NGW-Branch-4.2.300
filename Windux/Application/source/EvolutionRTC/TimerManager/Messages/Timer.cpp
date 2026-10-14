/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Timer.h"
#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Timer::dataType = 0;
std::string Timer::dataTypeName("Timer"); 
#endif

Timer::Timer()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	
	init();

	timerId = getId();	
}

Timer::Timer(Hobart::CEvolMsgReceiver* receiver, unsigned int signalTime, TimedFunction timerPurpose, short repeatFreq)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	
	init();

	this->timerId = getId();	
	this->receiver = receiver;
	this->signalTime = signalTime;
	this->repeatFrequency = repeatFreq;
	this->timerPurpose = timerPurpose;
}

Timer::Timer(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	int j = 0;

	char *pChar = (char *) &mySessionId;
	for (unsigned int i = 0; i < sizeof(mySessionId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &receiver;
	for (unsigned int i = 0; i < sizeof(receiver); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &signalTime;
	for (unsigned int i = 0; i < sizeof(signalTime); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &repeatFrequency;
	for (unsigned int i = 0; i < sizeof(repeatFrequency); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &timerPurpose;
	for (unsigned int i = 0; i < sizeof(timerPurpose); i++)
	{
		*pChar++ = buffer[j++];
	}

	time_t end;
	pChar = (char *) &end;
	for (unsigned int i = 0; i < sizeof(end); i++)
	{
		*pChar++ = buffer[j++];
	}
	endDate.setCalendarTime(end);
}

Timer::~Timer()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void Timer::init()
{
	timerId = 0;
	mySessionId = 0;
	receiver = 0;
	sessionIdSet = false;
	signalTime = 0;
	repeatFrequency = RepeatNO;
	timerPurpose = timedFuncNone;
	endDate = startDate;

	/*timerValues.it_interval.tv_sec = 0;
	timerValues.it_interval.tv_nsec = 0;
	timerValues.it_value.tv_sec = 0;
	timerValues.it_value.tv_nsec = 0;*/
	function = (VOIDFUNCPTR) 0;
	functionParams = "";

	expiredAndNotified = false;
}

#if defined(LINUX_FIX)
int Timer::getTimerId() const
#else
const int Timer::getTimerId() const
#endif
{
	return timerId;
}
void Timer::setTimerId(const int timerId)
{
	this->timerId = timerId;
}

#if defined(LINUX_FIX)
unsigned int Timer::getMySessionId() const
#else
const unsigned int Timer::getMySessionId() const
#endif
{
	return mySessionId;
}
void Timer::setMySessionId(const unsigned int id)
{
	mySessionId = id;
	sessionIdSet = true;
}

bool Timer::isSessionIdSet() const
{
	return sessionIdSet;
}

#if defined(LINUX_FIX)
unsigned int Timer::getSignalTime() const
#else
const unsigned int Timer::getSignalTime() const
#endif
{
	return signalTime;
}
void Timer::setSignalTime(const unsigned int signalTime)
{
	this->signalTime = signalTime;
}

#if defined(LINUX_FIX)
short Timer::getRepeatFrequency() const
#else
const short Timer::getRepeatFrequency() const
#endif
{
	return repeatFrequency;
}
void Timer::setRepeatFrequency(const short repeatFrequency)
{
	this->repeatFrequency = repeatFrequency;
}

#if defined(LINUX_FIX)
TimedFunction Timer::getTimerPurpose() const
#else
const TimedFunction Timer::getTimerPurpose() const
#endif
{
	return timerPurpose;
}
void Timer::setTimerPurpose(const TimedFunction timerPurpose)
{
	this->timerPurpose = timerPurpose;
}

const HobDateTime &Timer::getStartDate() const
{
	return startDate;
}

const HobDateTime &Timer::getEndDate() const
{
	return endDate;
}
void Timer::setEndDate(const HobDateTime &endDate)
{
	this->endDate = endDate;
}

VOIDFUNCPTR Timer::getFunction()
{
	return function;
}
void Timer::setFunction(VOIDFUNCPTR function)
{
	this->function = function;
}

const HobString &Timer::getFunctionParams() const
{
	return functionParams;
}
void Timer::setFunctionParams(const HobString &params)
{
	this->functionParams = params;
}


int Timer::createTimer()
{
	/*int value = timer_create(CLOCK_REALTIME, (sigevent *) 0, &timerId);
	return value;*/
	return 0;
}

int Timer::connectTimer()
{
	/*int value = timer_connect(timerId, function, timerPurpose);
	return value;*/
	return 0;
}

int Timer::setTimer()
{
	 /*if (Debug::isTracing("Timer"))
	{
		Debug::trace("Timer", HobString("setTimer() : timerPurpose = ") + HobString(Integer::toString(timerPurpose)) +
										HobString(" : signalTime = ") + HobString(Integer::toString(signalTime)) +
										HobString(" : repeatFrequency = ") + HobString(Integer::toString(repeatFrequency)));
		startDate.show();
		endDate.show();
		Debug::trace("Timer", HobString("setTimer() : starttime = ") + HobString(Long::toString((long)startDate.getCalendarTime())));
		Debug::trace("Timer", HobString("setTimer() :   endtime = ") + HobString(Long::toString((long)endDate.getCalendarTime())));
	}

	timerValues.it_value.tv_sec = endDate.getCalendarTime();		// Timer expiration

	if (getRepeatFrequency() != (unsigned int)RepeatNO)
		timerValues.it_interval.tv_sec = startDate.timeDiff(endDate);

	if (Debug::isTracing("Timer"))
		Debug::trace("Timer", HobString("Alarm Calendar time = ") + HobString(Long::toString(timerValues.it_value.tv_sec)) +
									HobString("\n Repeat interval = ") + HobString(Long::toString(timerValues.it_interval.tv_sec)));

	int value = timer_settime(timerId, TIMER_ABSTIME, &timerValues, (struct itimerspec *) 0);
	return value;*/
	return 0; //
}


time_t Timer::setupEndDate()
{
	bool isTracing = Debug::isTracing("Timer");
	if (isTracing)
		Debug::trace("Timer", HobString("setupEndDate() :  before setting, endtime = ") + HobString(Long::toString((long)endDate.getCalendarTime())));

	expiredAndNotified = false;
	startDate.setToCurrent();
	endDate.setToCurrent();
	endDate.incrementSeconds(signalTime);
	time_t endTime = endDate.getCalendarTime();

	if (isTracing)
		Debug::trace("Timer", HobString("setupEndDate() :  after  setting, endtime = ") + HobString(Long::toString((long)endTime)));

	return endTime;
}

void Timer::compensateForTimeChange(time_t old_current_time)
{
	if (Debug::isTracing("Timer"))
		Debug::trace("Timer", HobString("compensateForTimeChange(), resetting timer time"));  
	
	if ( repeatFrequency == Timer::RepeatNO && expiredAndNotified )
		// Already expired, and notified creator, so don't recalculate new start and end time.
		return;

	//grab what old end date should have been
	time_t old_end_date_time = endDate.getCalendarTime();
	
	double secs_remaining = difftime(old_end_date_time, old_current_time);
	secs_remaining = secs_remaining - 1; //since old_current_time was obtained the last time TimerManger::Run() ran
	
	expiredAndNotified = false;
	startDate.setToCurrent();
	endDate.setToCurrent();
	
	if (secs_remaining < signalTime)
		endDate.incrementSeconds(secs_remaining);
	else
		endDate.incrementSeconds(signalTime);
}

bool Timer::operator==(const Timer &timer) const
{
	return (timerId == timer.getTimerId()) ? true : false;
}


void Timer::operator=(const Timer &timer)
{
	timerId = timer.getTimerId();
	mySessionId = timer.getMySessionId();
	receiver = timer.getMessageReceiver();
	sessionIdSet = timer.isSessionIdSet();
	signalTime = timer.getSignalTime();
	repeatFrequency = timer.getRepeatFrequency();
	timerPurpose = timer.getTimerPurpose();
	startDate = timer.getStartDate();
	endDate = timer.getEndDate();
	//itimerspec		timerValues;
	//function = timer.getFunction();
	functionParams = timer.getFunctionParams();
	expiredAndNotified = timer.isExpiredAndNotified();
}


const char *Timer::serialize(int /*format*/) const
{
	serializedData = "";

	// Now, for the derived class items
	char *pChar = (char *) &mySessionId;
	for (unsigned int i = 0; i < sizeof(mySessionId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &receiver;
	for (unsigned int i = 0; i < sizeof(receiver); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &signalTime;
	for (unsigned int i = 0; i < sizeof(signalTime); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &repeatFrequency;
	for (unsigned int i = 0; i < sizeof(repeatFrequency); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &timerPurpose;
	for (unsigned int i = 0; i < sizeof(timerPurpose); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	time_t end = endDate.getCalendarTime();
	pChar = (char *) &end;
	for (unsigned int i = 0; i < sizeof(end); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void Timer::show()
{
	if (Debug::isTracing("Timer.show"))
	{
		HobString h1 = Long::toString((long)timerId);
		HobString h2 = Integer::toString(mySessionId);
		HobString h3 = Bool::toString(sessionIdSet);
		HobString h34 = Hex::toString((long long)receiver);
		HobString h4 = Integer::toString(signalTime);
		HobString h5 = Integer::toString(repeatFrequency);
		HobString h6 = Integer::toString(timerPurpose);
		HobString h7 = Long::toString((long)endDate.getCalendarTime());
		HobString h8 = Bool::toString(expiredAndNotified);

		Debug::trace("Timer.show", HobString("timerId: ") + h1 + HobString("  ssn: ") + h2 + HobString("  ssnSet: ") + h3 +
				HobString("  receiver: ") + h34 + HobString("  signal: ") + h4 + HobString("  freq: ") + h5 +
				HobString("  purpose: ") + h6 + HobString("  end: ") + h7 + HobString("  expiredAndNotified: ") + h8);
		//startDate.show();
		endDate.show();
	}
}

int Timer::getId()
{
	static int id = 0;
	return ++id;
}
