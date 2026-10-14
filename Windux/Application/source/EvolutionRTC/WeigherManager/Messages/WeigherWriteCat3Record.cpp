/**
 * @file WeigherWriteCat3Record.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Message instructs the weigher to write cat3 record to audit trail.
 * Created on: July 1, 2016
 */
#include "WeigherWriteCat3Record.h"
#include "WeigherEvent.h"
#include "Debug.h"
#include "Integer.h"
#include "Long.h"


WeigherWriteCat3Record::WeigherWriteCat3Record(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Weigher, WgCat3WriteRecord, pHobThread)
{
	event = CONFIGURATION;
	id = SERVICE_SWITCH_PRESSED;
	auditType = LOCAL_EVENT;
	epochTime = 0;
	newValue = 0;
	previousValue = 0;
}

WeigherWriteCat3Record::WeigherWriteCat3Record(const char *buffer) : EvolMessage(buffer)
{
	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &event;
	for (unsigned int i = 0; i < sizeof(event); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &id;
	for (unsigned int i = 0; i < sizeof(id); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &auditType;
	for (unsigned int i = 0; i < sizeof(auditType); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &epochTime;
	for (unsigned int i = 0; i < sizeof(epochTime); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &newValue;
	for (unsigned int i = 0; i < sizeof(newValue); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &previousValue;
	for (unsigned int i = 0; i < sizeof(previousValue); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WeigherWriteCat3Record::~WeigherWriteCat3Record()
{}

const char *WeigherWriteCat3Record::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &event;
	for (unsigned int i = 0; i < sizeof(event); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &id;
	for (unsigned int i = 0; i < sizeof(id); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &auditType;
	for (unsigned int i = 0; i < sizeof(auditType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &epochTime;
	for (unsigned int i = 0; i < sizeof(epochTime); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &newValue;
	for (unsigned int i = 0; i < sizeof(newValue); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &previousValue;
	for (unsigned int i = 0; i < sizeof(previousValue); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}

void WeigherWriteCat3Record::show()
{
	Debug::trace("WeigherWriteCat3Record", "WeigherWriteCat3Record: ");
	Debug::trace("WeigherWriteCat3Record", "record event: " + Integer::toString(event) );
	Debug::trace("WeigherWriteCat3Record", "record parameter: " + Integer::toString(id) );
	Debug::trace("WeigherWriteCat3Record", "record audit type: " + Integer::toString(auditType) );
	Debug::trace("WeigherWriteCat3Record", "Epoch Time: " + Long::toString(epochTime) );
	Debug::trace("WeigherWriteCat3Record", "record new value: " + Long::toString(newValue) );
	Debug::trace("WeigherWriteCat3Record", "record audit previous value: " + Long::toString(previousValue) );
}


