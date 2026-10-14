/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Timeout.h"
#include "Timer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Timeout::dataType = 0;
std::string Timeout::dataTypeName("Timeout"); 
#endif

Timeout::Timeout( Hobart::CEvolMsgReceiver * pReceiver, const int tId, unsigned int sId, bool sIdSet)
	: EvolMessage(TimerDev, TimerExpired, pReceiver )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	timerId = tId;
	mySessionId = sId;
	sessionIdSet = sIdSet;
}

Timeout::Timeout(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &timerId;
	for (unsigned int i = 0; i < sizeof(timerId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &mySessionId;
	for (unsigned int i = 0; i < sizeof(mySessionId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sessionIdSet;
	for (unsigned int i = 0; i < sizeof(sessionIdSet); i++)
	{
		*pChar++ = buffer[j++];
	}
}

Timeout::~Timeout()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

#if defined(LINUX_FIX)
int Timeout::getTimerId() const
#else
const int Timeout::getTimerId() const
#endif
{
	return timerId;
}

#if	defined(LINUX_FIX)
unsigned int Timeout::getMySessionId() const
#else
const unsigned int Timeout::getMySessionId() const
#endif
{
	return mySessionId;
}

bool Timeout::isSessionIdSet() const
{
	return sessionIdSet;
}


void Timeout::operator=(const Timeout &timeout)
{
	timerId = timeout.getTimerId();
	mySessionId = timeout.getMySessionId();
	sessionIdSet = timeout.isSessionIdSet();
}


const char *Timeout::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &timerId;
	for (unsigned int i = 0; i < sizeof(timerId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &mySessionId;
	for (unsigned int i = 0; i < sizeof(mySessionId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sessionIdSet;
	for (unsigned int i = 0; i < sizeof(sessionIdSet); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}
