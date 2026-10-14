/**
 * @file WeigherKpcError.cpp
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Aaron Swift
 * @brief Kpc engingeering debug Weigher errors
 * Created on: Jan 8, 2019
 *
 **/
#include "WeigherKpcError.h"
#include "WeigherEvent.h"
WeigherKpcError::WeigherKpcError( Hobart::CEvolMsgReceiver *pHobThread, WgKpcWeigherErrors err ) : EvolMessage( Weigher, WgKPCError, pHobThread )
{
	error =  err;
}

WeigherKpcError::WeigherKpcError( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &error;
	for (unsigned int i = 0; i < sizeof(error); i++)
	{
		*pChar++ = buffer[j++];
	}

}

WeigherKpcError::~WeigherKpcError()
{
}

const char *WeigherKpcError::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &error;
	for (unsigned int i = 0; i < sizeof(error); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}

std::string WeigherKpcError::getErrorDescription()
{
	std::string description;

	switch(error)
	{
		case _WG_READ_CHKSUM_PAGE_FAILURE: {
			description = "Failed to read serial flash page where checksums are stored!";
			break;
		}
		case _WG_READ_CFG_PAGE_FAILURE: {
			description = "Failed to read serial flash page where configuration is stored!";
			break;
		}
		case _WG_WAKETIMER_CREATE_FAILURE: {
			description = "Failed to create timer for Weigher wakeup message!";
			break;
		}
		case _WG_TIMER_DELETION_FAILURE: {
			description = "Failed to delete timer for Weigher wakeup message!";
			break;
		}
		case _WG_MSG_QUEUE_FAILURE: {
			description = "Failed to get Weigher message from message queue!";
			break;
		}
		case _WG_I2C_QUEUE_FAILURE: {
			description = "Failed to get I2C transaction from I2C queue!";
			break;
		}
		case _WG_UNKNOWN_MSG_FAILURE: {
			description = "Unknown Weigher message type!";
			break;
		}
		case _WG_ADC_CLOCK_INIT_FAILURE: {
			description = "Failed to initialize clock timer for external ADC!";
			break;
		}
		case _WG_ADC_CLOCK_PWM_FAILURE: {
			description = "Failed to PWM value for clock to external ADC!";
			break;
		}
		case _WG_CNTS_FAILURED_TO_QUEUE: {
			description = "Failed to add weight count to counts queue!";
			break;
		}
		case _WG_TIMER_CREATE_FAILURE: {
			description = "Failed to create one shot timer for initialization of zero!";
			break;
		}
		case _WG_CFG_ZERO_TIME_FAILURE: {
			description = "Failed to configure one shot timer for initialization of zero!";
			break;
		}
		case _WG_CAT3_AUDIT_MGR_INIT_FAILURE: {
			description = "Failed to initialize CAT3 audit manager from serial flash!";
			break;
		}
		case _WG_CAT3_AUDIT_MGR_READ_FAILURE: {
			description = "Failed to read CAT3 audit manager from serial flash!";
			break;
		}
		case _WG_CAT3_AUDIT_MGR_WRITE_FAILURE: {
			description = "Failed to write CAT3 audit manager to serial flash!";
			break;
		}
		case _WG_CAT3_LOG_CAL_EVENT_FAILURE: {
			description = "Failed to log CAT3 calibration event to serial flash!";
			break;
		}
		case _WG_CAT3_LOG_CFG_EVENT_FAILURE: {
			description = "Failed to log CAT3 configuration event to serial flash!";
			break;
		}
		case _WG_CAT3_AUDIT_MGR_WRITE_RO_FAILURE: {
			description = "Failed to write cat3 audit manager page to serial flash after a roll over!";
			break;
		}
		case _WG_CAT3_AUDIT_MGR_ERASE_RO_FAILURE: {
			description = "Failed to erase cat3 audit manager page in serial flash after a roll over!";
			break;
		}
		case _WG_CAT3_PAGE_ERASE_FAILURE: {
			description = "Failed to erase cat3 audit page in serial flash!";
			break;
		}
		case _WG_ACCEL_CMD_BUFFER_NULL: {
			description = "Accelerometer command buffer is null!";
			break;
		}
	}
	return description;
}


