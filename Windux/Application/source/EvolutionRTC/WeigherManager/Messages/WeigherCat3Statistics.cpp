/**
 * @file WeigherCat3Statistics.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Message weigher response to request for CAT3 statistics.
 * Created on: July 1, 2016
 */
#include "WeigherCat3Statistics.h"
#include "WeigherEvent.h"
#include "Debug.h"
#include "Integer.h"

WeigherCat3Statistics::WeigherCat3Statistics( Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage(Weigher, WgCat3AuditStatistics, pHobThread)
{
	totalPages 					= 0;
	totalRecordsPerPage 		= 0;
	currentPage 				= 0;
	totalFreeRecords 			= 0;
    totalRecordedEvents 		= 0;
    totalCalibrationRecords 	= 0;
    totalConfigurationRecords 	= 0;
    gainCoefficientCounter 		= 0;
    offsetCoefficientCounter 	= 0;
    gainFactorCounter 			= 0;
    centerOfMainCounter 		= 0;
    zeroReferenceCounter 		= 0;
    weigherModelCounter 		= 0;
    minWeightPrintCounter 		= 0;
    maxWeightCounter 			= 0;
    filterSpeedCounter 			= 0;
    divisionSizeCounter 		= 0;
    weigherModeCounter			= 0;
}

WeigherCat3Statistics::WeigherCat3Statistics(const char *buffer) : EvolMessage(buffer)
{
	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &totalPages;
	for (unsigned int i = 0; i < sizeof(totalPages); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &totalRecordsPerPage;
	for (unsigned int i = 0; i < sizeof(totalRecordsPerPage); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &currentPage;
	for (unsigned int i = 0; i < sizeof(currentPage); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &totalFreeRecords;
	for (unsigned int i = 0; i < sizeof(totalFreeRecords); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &totalRecordedEvents;
	for (unsigned int i = 0; i < sizeof(totalRecordedEvents); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &totalCalibrationRecords;
	for (unsigned int i = 0; i < sizeof(totalCalibrationRecords); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &totalConfigurationRecords;
	for (unsigned int i = 0; i < sizeof(totalConfigurationRecords); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &gainCoefficientCounter;
	for (unsigned int i = 0; i < sizeof(gainCoefficientCounter); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &offsetCoefficientCounter;
	for (unsigned int i = 0; i < sizeof(offsetCoefficientCounter); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &gainFactorCounter;
	for (unsigned int i = 0; i < sizeof(gainFactorCounter); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &centerOfMainCounter;
	for (unsigned int i = 0; i < sizeof(centerOfMainCounter); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &zeroReferenceCounter;
	for (unsigned int i = 0; i < sizeof(zeroReferenceCounter); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &weigherModelCounter;
	for (unsigned int i = 0; i < sizeof(weigherModelCounter); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &minWeightPrintCounter;
	for (unsigned int i = 0; i < sizeof(minWeightPrintCounter); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &maxWeightCounter;
	for (unsigned int i = 0; i < sizeof(maxWeightCounter); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &filterSpeedCounter;
	for (unsigned int i = 0; i < sizeof(filterSpeedCounter); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &divisionSizeCounter;
	for (unsigned int i = 0; i < sizeof(divisionSizeCounter); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &weigherModeCounter;
	for (unsigned int i = 0; i < sizeof(weigherModeCounter); i++)
	{
		*pChar++ = buffer[j++];
	}

}

WeigherCat3Statistics::~WeigherCat3Statistics()
{}

const char *WeigherCat3Statistics::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &totalPages;
	for (unsigned int i = 0; i < sizeof(totalPages); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &totalRecordsPerPage;
	for (unsigned int i = 0; i < sizeof(totalRecordsPerPage); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &currentPage;
	for (unsigned int i = 0; i < sizeof(currentPage); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &totalFreeRecords;
	for (unsigned int i = 0; i < sizeof(totalFreeRecords); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &totalRecordedEvents;
	for (unsigned int i = 0; i < sizeof(totalRecordedEvents); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &totalCalibrationRecords;
	for (unsigned int i = 0; i < sizeof(totalCalibrationRecords); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &totalConfigurationRecords;
	for (unsigned int i = 0; i < sizeof(totalConfigurationRecords); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &gainCoefficientCounter;
	for (unsigned int i = 0; i < sizeof(gainCoefficientCounter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &offsetCoefficientCounter;
	for (unsigned int i = 0; i < sizeof(offsetCoefficientCounter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &gainFactorCounter;
	for (unsigned int i = 0; i < sizeof(gainFactorCounter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &centerOfMainCounter;
	for (unsigned int i = 0; i < sizeof(centerOfMainCounter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &zeroReferenceCounter;
	for (unsigned int i = 0; i < sizeof(zeroReferenceCounter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &weigherModelCounter;
	for (unsigned int i = 0; i < sizeof(weigherModelCounter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &minWeightPrintCounter;
	for (unsigned int i = 0; i < sizeof(minWeightPrintCounter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &maxWeightCounter;
	for (unsigned int i = 0; i < sizeof(maxWeightCounter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filterSpeedCounter;
	for (unsigned int i = 0; i < sizeof(filterSpeedCounter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &divisionSizeCounter;
	for (unsigned int i = 0; i < sizeof(divisionSizeCounter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &weigherModeCounter;
	for (unsigned int i = 0; i < sizeof(weigherModeCounter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WeigherCat3Statistics::show()
{

	Debug::trace("WeigherCat3Statistics", "WeigherCat3Statistics: ");
	Debug::trace("WeigherCat3Statistics", "Total number of audit trail pages: " + Integer::toString(totalPages) );
	Debug::trace("WeigherCat3Statistics", "Total number of audit records per page: " + Integer::toString(totalRecordsPerPage) );
	Debug::trace("WeigherCat3Statistics", "Audit Manager current working page: " + Integer::toString(currentPage) );
	Debug::trace("WeigherCat3Statistics", "Total number of free audit records: " + Integer::toString(totalFreeRecords) );
	Debug::trace("WeigherCat3Statistics", "Total number of recorded events: " + Integer::toString(totalRecordedEvents) );
	Debug::trace("WeigherCat3Statistics", "Total number of calibration records: " + Integer::toString(totalCalibrationRecords) );
	Debug::trace("WeigherCat3Statistics", "Total number of configuration records " + Integer::toString(totalConfigurationRecords) );
	Debug::trace("WeigherCat3Statistics", "Total number of times gainCoefficient parameter has changed: " + Integer::toString(gainCoefficientCounter) );
	Debug::trace("WeigherCat3Statistics", "Total number of times offsetCoefficient parameter has changed:" + Integer::toString(offsetCoefficientCounter) );
	Debug::trace("WeigherCat3Statistics", "Total number of times offsetCoefficient parameter has changed:" + Integer::toString(gainFactorCounter) );
	Debug::trace("WeigherCat3Statistics", "Total number of times centerOfMain parameter has changed:" + Integer::toString(centerOfMainCounter) );
	Debug::trace("WeigherCat3Statistics", "Total number of times zeroReference parameter has changed:" + Integer::toString(zeroReferenceCounter) );
	Debug::trace("WeigherCat3Statistics", "Total number of times weigherModel parameter has changed:" + Integer::toString(weigherModelCounter) );
	Debug::trace("WeigherCat3Statistics", "Total number of times minWeightPrint parameter has changed:" + Integer::toString(minWeightPrintCounter) );
	Debug::trace("WeigherCat3Statistics", "Total number of times maxWeight parameter has changed:" + Integer::toString(maxWeightCounter) );
	Debug::trace("WeigherCat3Statistics", "Total number of times filterSpeed parameter has changed:" + Integer::toString(filterSpeedCounter) );
	Debug::trace("WeigherCat3Statistics", "Total number of times divisionSize parameter has changed:" + Integer::toString(divisionSizeCounter) );
	Debug::trace("WeigherCat3Statistics", "Total number of times weigher mode parameter has changed:" + Integer::toString(weigherModeCounter) );
}


