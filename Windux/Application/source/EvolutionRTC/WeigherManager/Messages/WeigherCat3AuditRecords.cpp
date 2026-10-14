/**
 * @file WeigherCat3AuditRecords.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Message weigher response to page request or next records request.
 * 		  List of up to 10 records.
 * Created on: July 1, 2016
 */
#include "WeigherCat3AuditRecords.h"
#include "WeigherEvent.h"
#include "Debug.h"
#include "Integer.h"
#include "Hex.h"

WeigherCat3AuditRecords::WeigherCat3AuditRecords( Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage(Weigher, WgCat3Records, pHobThread)
{
	pageNumber = 0;
	pageIndex = 0;

	for(int i = 0; i < MAX_BUFFER_SIZE; i++)
	{
		buffer[i] = 0;
	}
}

WeigherCat3AuditRecords::WeigherCat3AuditRecords(const char *buff) : EvolMessage(buff)
{
	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &pageNumber;
	for (unsigned int i = 0; i < sizeof(pageNumber); i++)
	{
		*pChar++ = buff[j++];
	}
	pChar = (char *) &pageIndex;
	for (unsigned int i = 0; i < sizeof(pageIndex); i++)
	{
		*pChar++ = buff[j++];
	}
	pChar = (char *) &buffer[0];
	for (unsigned int i = 0; i < MAX_BUFFER_SIZE; i++)
	{
		*pChar++ = buff[j++];
	}

	if (Debug::isTracing("WeigherCat3AuditRecords"))
		Debug::trace("WeigherCat3AuditRecords", std::string("constructor buffer: ") + Hex::toString((char *)buffer, MAX_BUFFER_SIZE));
}

WeigherCat3AuditRecords::~WeigherCat3AuditRecords()
{}

const char *WeigherCat3AuditRecords::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &pageNumber;
	for (unsigned int i = 0; i < sizeof(pageNumber); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &pageIndex;
	for (unsigned int i = 0; i < sizeof(pageIndex); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	if (Debug::isTracing("WeigherCat3AuditRecords"))
		Debug::trace("WeigherCat3AuditRecords", std::string("serialize buffer: ") + Hex::toString((char *)buffer, MAX_BUFFER_SIZE));

	pChar = (char *) &buffer[0];
	for (unsigned int i = 0; i < MAX_BUFFER_SIZE; i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

// Returns true if there is a record at that index, false if not.
bool WeigherCat3AuditRecords::getRecord(int index, Cat3AuditRecord *pRecord)
{
	if ( index < 0 || index >= NUMBER_OF_RECORDS )
		return false;

	int i = ( index * RECORD_SIZE );

	i++;	//skip pad0
	pRecord->event  = buffer[i++];

	pRecord->parameterId = buffer[i++];
    i++;	//skip pad1
	pRecord->epochTime =  buffer[i + 3];
	pRecord->epochTime <<= 8;
	pRecord->epochTime |=  buffer[i + 2];
	pRecord->epochTime <<= 8;
	pRecord->epochTime |=  buffer[i + 1];
	pRecord->epochTime <<= 8;
	pRecord->epochTime |=  buffer[i];
	i += 4;

	// This is how we will determine if we are reading a valid record;
	// If the epochTime is 0, then this record is empty, and thus not valid.
	if (pRecord->epochTime == 0)
		return false;

	pRecord->eventType = buffer[i++];
	i += 10; //skip unused + 2 pads

	pRecord->oldParamValue = buffer[i + 3];
	pRecord->oldParamValue <<= 8;
	pRecord->oldParamValue |=  buffer[i + 2];
	pRecord->oldParamValue <<= 8;
	pRecord->oldParamValue |=  buffer[i];
	pRecord->oldParamValue <<= 8;
	pRecord->oldParamValue |=  buffer[i + 1];
	i += 4;

	i++; //skip pad
	pRecord->newParamValue = buffer[i + 3];
	pRecord->newParamValue <<= 8;
	pRecord->newParamValue |=  buffer[i + 2];
	pRecord->newParamValue <<= 8;
	pRecord->newParamValue |=  buffer[i + 1];
	pRecord->newParamValue <<= 8;
	pRecord->newParamValue |=  buffer[i];
	i += 4;
	return true;
}

void WeigherCat3AuditRecords::setRecordBuffer(const unsigned char *pBuffer)
{
	for (unsigned int i = 0; i < MAX_BUFFER_SIZE; i++)
	{
		buffer[i] = *pBuffer++;
	}

}

void WeigherCat3AuditRecords::show()
{

	Debug::trace("WeigherCat3AuditRecords", "WeigherCat3AuditRecords: ");
	Debug::trace("WeigherCat3AuditRecords", "Page Number: " + Integer::toString(pageNumber) );
	Debug::trace("WeigherCat3AuditRecords", "Page Index: " + Integer::toString(pageIndex) );



	int i = 0;
	for( int j = 0; j < NUMBER_OF_RECORDS;  j++ )
	{

		Cat3AuditRecord record;
		getRecord( j, &record );

		Debug::trace("WeigherCat3AuditRecords", "Event: " + Hex::toString(record.event) );
		Debug::trace("WeigherCat3AuditRecords", "ParameterId: " + Hex::toString(record.parameterId) );
		Debug::trace("WeigherCat3AuditRecords", "Epoch time: " + Hex::toString(record.epochTime) );
		Debug::trace("WeigherCat3AuditRecords", "EventType: " + Hex::toString(record.eventType) );
		Debug::trace("WeigherCat3AuditRecords", "Previous value: " + Hex::toString(record.oldParamValue) );
		Debug::trace("WeigherCat3AuditRecords", "Current value: " + Hex::toString(record.newParamValue) );
		Debug::trace("WeigherCat3AuditRecords", "Current Index: " + Integer::toString( pageIndex + j ) );
	}
}



