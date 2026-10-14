/* Evolution Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "BackupRestoreEvent.h"
#include "BackupRestoreInitiateReadAllMsg.h"
#include "Integer.h"
#include "Hex.h"

#include "Debug.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int BackupRestoreInitiateReadAllMsg::dataType = 0;
std::string BackupRestoreInitiateReadAllMsg::dataTypeName("BackupRestoreInitiateReadAllMsg"); 
#endif


BackupRestoreInitiateReadAllMsg::BackupRestoreInitiateReadAllMsg(Hobart::CEvolMsgReceiver *pHobThread, EvolDeviceType dev, UINT8 ev, CommMechanism *pUi) : EvolMessage(BackupRestore, InitiateReadAll, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	readAllDevice = dev;
	readAllEvent = ev;
	uiClient = pUi;
}

BackupRestoreInitiateReadAllMsg::BackupRestoreInitiateReadAllMsg(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	const HobString m = buffer;
	HobString h = HobString(Hex::toString(m.data(), (int)m.size()));
	Debug::trace("BackupRestoreReadAll", HobString("constructor(buf): input: ") + h);

	EvolMessage::serialize();

	int j = EvolMessage::getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &readAllDevice;
	for (unsigned int i = 0; i < sizeof(readAllDevice); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &readAllEvent;
	for (unsigned int i = 0; i < sizeof(readAllEvent); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &uiClient;
	for (unsigned int i = 0; i < sizeof(uiClient); i++)
	{
		*pChar++ = buffer[j++];
	}

	show();
}

BackupRestoreInitiateReadAllMsg::~BackupRestoreInitiateReadAllMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *BackupRestoreInitiateReadAllMsg::serialize() const
{
	return serialize(InternalFormat);
}
const char *BackupRestoreInitiateReadAllMsg::serialize(int /* format */) const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &readAllDevice;
	for (unsigned int i = 0; i < sizeof(readAllDevice); i++)
	{
		EvolMessage::serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &readAllEvent;
	for (unsigned int i = 0; i < sizeof(readAllEvent); i++)
	{
		EvolMessage::serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &uiClient;
	for (unsigned int i = 0; i < sizeof(uiClient); i++)
	{
		EvolMessage::serializedData.append(1, pChar[i]);
	}

	const HobString m = EvolMessage::serializedData;
	HobString h = HobString(Hex::toString(m.data(), (int)m.size()));
	Debug::trace("BackupRestoreReadAll", HobString("serialize: ") + h);

	return EvolMessage::serializedData.data();
}

EvolDeviceType BackupRestoreInitiateReadAllMsg::getReadAllDevice()
{
	return readAllDevice;
}
void BackupRestoreInitiateReadAllMsg::setReadAllDevice(EvolDeviceType dev)
{
	readAllDevice = dev;
}

UINT8 BackupRestoreInitiateReadAllMsg::getReadAllEvent()
{
	return readAllEvent;
}
void BackupRestoreInitiateReadAllMsg::setReadAllEvent(UINT8 ev)
{
	readAllEvent = ev;
}

CommMechanism *BackupRestoreInitiateReadAllMsg::getUiClient()
{
	return uiClient;
}
void BackupRestoreInitiateReadAllMsg::setCommMechanism(CommMechanism *ui)
{
	uiClient = ui;
}

void BackupRestoreInitiateReadAllMsg::show()
{
	bool isTracing = Debug::isTracing("BackupRestoreReadAll");

	if (isTracing == false)
	{
		Debug::addTraceCategory("BackupRestoreReadAll");
	}

	Debug::trace("BackupRestoreReadAll", "BackupRestoreReadAll:show() ");

	EvolMessage::show();

	Debug::trace("BackupRestoreReadAll", HobString("readAllDevice: ") +
								  HobString(Integer::toString((int)readAllDevice)));
	Debug::trace("BackupRestoreReadAll", HobString("readAllEvent: ") +
								  HobString(Integer::toString((int)readAllEvent)));

	if (isTracing == false)
	{
		Debug::removeTraceCategory("BackupRestoreReadAll");
	}
}

