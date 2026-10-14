/**
 * @file KpcSystemErrors.cpp
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Aaron Swift
 * @brief Kpc engingeering debug system errors
 * Created on: Jan 8, 2019
 *
 **/
#include "KpcSystemErrors.h"
#include "KpcSystemEvents.h"

KpcSystemErrors::KpcSystemErrors( Hobart::CEvolMsgReceiver *pHobThread, KpcSystemError err ) : EvolMessage( KPCSystem, KpcError, pHobThread )
{
	error =  err;
}

KpcSystemErrors::KpcSystemErrors( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &error;
	for (unsigned int i = 0; i < sizeof(error); i++)
	{
		*pChar++ = buffer[j++];
	}

}

KpcSystemErrors::~KpcSystemErrors()
{
}

const char *KpcSystemErrors::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &error;
	for (unsigned int i = 0; i < sizeof(error); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}

std::string KpcSystemErrors::getErrorDescription()
{
	std::string description;

	switch(error)
	{
		case _SENSOR_THREAD_CREATION: {
			description = "Failed to create freeRTOS thread for sensors!";
			break;
		}
		case _UNKNOWN_MODEL_CONFIGURATION: {
			description = "ADC counts out of range for supported model types!";
			break;
		}
		case _WEIGHER_CNT_QUEUE: {
			description = "Failed to create weigher load cell counts queue!";
			break;
		}
		case _WEIGHER_THREAD_CREATION: {
			description = "Failed to create freeRTOS thread for KPC service scale weigher!";
			break;
		}
		case _WEIGHER_THREAD_PREPACK_THREAD_CREATION: {
			description = "Failed to create freeRTOS thread for prepack weigher!";
			break;
		}
		case _PRINTER_THREAD_CREATION: {
			description = "Failed to create freeRTOS thread for KPC service scale printer!";
			break;
		}
		case _PRINTER_PREPACK_THREAD_CREATION: {
			description = "Failed to create freeRTOS thread for prepack printer!";
			break;
		}
		case _HEART_BEAT_CREATION: {
			description = "Failed to create system heart beat timer!";
			break;
		}
		case _HEART_BEAT_STARTUP: {
			description = "Failed to start system heart beat timer!";
			break;
		}
		case _HEAP_FAILURE: {
			description = "System heap failure!";
			break;
		}
		case _ADC_CALIBRATION_FAILURE: {
			description = "Failed to calibration ADC for sensors!";
			break;
		}
		case _DMA_SUBMISSION_FAILURE_SENSORS: {
			description = "DMA channel submission failure for ADC sensors!";
			break;
		}
		case _I2C_READ_FAILURE_VALUEMAX_ASSY: {
			description = "Failed to read I2C Value Max assembly number value!";
			break;
		}
		case _I2C_WRITE_FAILURE_VALUEMAX_ASSY: {
			description = "Failed to write I2C Value Max assembly number value!";
			break;
		}
		case _I2C_READ_FAILURE_VALUEMAX_VER: {
			description = "Failed to read I2C Value Max version number value!";
			break;
		}
		case _I2C_WRITE_FAILURE_VALUEMAX_VER: {
			description = "Failed to write I2C Value Max version number value!";
			break;
		}
		case _I2C_READ_FAILURE_VALUEMAX_SER: {
			description = "Failed to read I2C Value Max serial number value!";
			break;
		}
		case _I2C_WRITE_FAILURE_VALUEMAX_SER: {
			description = "Failed to write I2C Value Max serial number value!";
			break;
		}
		case _I2C_READ_FAILURE_VALUEMAX_DATE: {
			description = "Failed to read I2C Value Max date value!";
			break;
		}
		case _I2C_WRITE_FAILURE_VALUEMAX_DATE: {
			description = "Failed to write I2C Value Max date value!";
			break;
		}
		case _I2C_READ_FAILURE_VALUEMAX_XGAIN: {
			description = "Failed to read I2C Value Max X Gain value!";
			break;
		}
		case _I2C_WRITE_FAILURE_VALUEMAX_XGAIN: {
			description = "Failed to write I2C Value Max X Gain value!";
			break;
		}
		case _I2C_READ_FAILURE_VALUEMAX_YGAIN: {
			description = "Failed to read I2C Value Max Y Gain value!";
			break;
		}
		case _I2C_WRITE_FAILURE_VALUEMAX_YGAIN: {
			description = "Failed to write I2C Value Max Y Gain value!";
			break;
		}
		case _I2C_READ_FAILURE_VALUEMAX_ZGAIN: {
			description = "Failed to read I2C Value Max Z Gain value!";
			break;
		}
		case _I2C_WRITE_FAILURE_VALUEMAX_ZGAIN: {
			description = "Failed to write I2C Value Max Z Gain value!";
			break;
		}
		case _I2C_READ_FAILURE_VALUEMAX_CAL: {
			description = "Failed to read I2C Value Max calibration!";
			break;
		}
		case _I2C_WRITE_FAILURE_VALUEMAX_CAL: {
			description = "Failed to write I2C Value Max calibration command!";
			break;
		}
		case _VALUEMAX_UNKNOWN_COMMAND: {
			description = "Unknown Value Max command!";
			break;
		}
		case _VALUEMAX_MANAGER_NULL: {
			description = "Value Max command buffer is null!";
			break;
		}
		case _DS1050_FAILED_SET_DUTY_CYCLE: {
			description = "Failed to setup PWM for media sensor or label taken sensor!";
			break;
		}
		case _DS1050_FAILED_SHUTDOWN: {
			description = "Failed to shutdown PWM for media sensor or label taken sensor!";
			break;
		}
		case _DS1050_FAILED_WAKEUP: {
			description = "Failed to wake up PWM for media sensor or label taken sensor!";
			break;
		}
		case _RS485_LABEL_IMAGE_QUEUE_NULL: {
			description = "Failed to create label image queue!";
			break;
		}
		case _RS485_IMAGE_BUFFER_NULL: {
			description = "Label image buffer is null!";
			break;
		}
		case _RS485_INTERFACE_OPEN_FAILURE: {
			description = "Failed to open 485 interface for label data transfer!";
			break;
		}
		case _RS485_DMA_FAILED_RESTART: {
			description = "Failed to restart DMA channel for label data transfer!";
			break;
		}
		case _SYSTEM_TIMER_FAILED_MUTEX: {
			description = "Failed to create mutex for system timer!";
			break;
		}
		case _SYSTEM_TIMER_CREATE_FAILED: {
			description = "Failed to create system timer!";
			break;
		}
	}
	return description;
}





