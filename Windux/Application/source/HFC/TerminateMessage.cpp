/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

//#include "vxWorks.h"
#include "TerminateMessage.h"
#include "SessionEvent.h"
#include "HobString.h"
#include "Debug.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int TerminateMessage::dataType = 0;
std::string TerminateMessage::dataTypeName("TerminateMessage"); 
#endif


TerminateMessage::TerminateMessage(Hobart::CEvolMsgReceiver *pHobThread, bool notify) : EvolMessage(SessionDev, SnShutdownNonPrimary, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	notifyUI = notify;
}

TerminateMessage::TerminateMessage(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &notifyUI;
	for (unsigned int i = 0; i < sizeof(notifyUI); i++)
	{
		*pChar++ = buffer[j++];
	}
}

TerminateMessage::~TerminateMessage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *TerminateMessage::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &notifyUI;
	for (unsigned int i = 0; i < sizeof(notifyUI); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void TerminateMessage::show()
{
	bool isTracing = Debug::isTracing("TerminateMessage");

	if (isTracing == false)
		Debug::addTraceCategory("TerminateMessage");

	Debug::trace("TerminateMessage", HobString("TerminateMessage:"));
	EvolMessage::show();

	Debug::trace("TerminateMessage", HobString("TerminateMessage:  notifyUI: ") +
								  HobString(Bool::toString(notifyUI)));

	if (isTracing == false)
		Debug::removeTraceCategory("TerminateMessage");
}

bool TerminateMessage::isnotifyUI() const
{
	return notifyUI;
}

void TerminateMessage::setnotifyUI(bool notify)
{
	notifyUI = notify;
}
