/**
 * @file KpcSystemErrors.h
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Aaron Swift
 * @brief Kpc engingeering debug system errors
 * Created on: Jan 8, 2019
 *
 **/
#ifndef KPCSYSTEMERRORS_H
#define KPCSYSTEMERRORS_H
#include <string>
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"
class KpcSystemErrors : public EvolMessage
{
	public:

	//Pulled for KPC Printer/Weigher firmware
	typedef enum
	{
	    _SENSOR_THREAD_CREATION,
	    _UNKNOWN_MODEL_CONFIGURATION,
	    _WEIGHER_CNT_QUEUE,
	    _WEIGHER_THREAD_CREATION,
	    _WEIGHER_THREAD_PREPACK_THREAD_CREATION,
	    _PRINTER_THREAD_CREATION,
	    _PRINTER_PREPACK_THREAD_CREATION,
	    _HEART_BEAT_CREATION,
	    _HEART_BEAT_STARTUP,
	    _HEAP_FAILURE,
	    _ADC_CALIBRATION_FAILURE,
	    _DMA_SUBMISSION_FAILURE_SENSORS,
	    _I2C_READ_FAILURE_VALUEMAX_ASSY,
	    _I2C_WRITE_FAILURE_VALUEMAX_ASSY,
	    _I2C_READ_FAILURE_VALUEMAX_VER,
	    _I2C_WRITE_FAILURE_VALUEMAX_VER,
	    _I2C_READ_FAILURE_VALUEMAX_SER,
	    _I2C_WRITE_FAILURE_VALUEMAX_SER,
	    _I2C_READ_FAILURE_VALUEMAX_DATE,
	    _I2C_WRITE_FAILURE_VALUEMAX_DATE,
	    _I2C_READ_FAILURE_VALUEMAX_XGAIN,
	    _I2C_WRITE_FAILURE_VALUEMAX_XGAIN,
	    _I2C_READ_FAILURE_VALUEMAX_YGAIN,
	    _I2C_WRITE_FAILURE_VALUEMAX_YGAIN,
	    _I2C_READ_FAILURE_VALUEMAX_ZGAIN,
	    _I2C_WRITE_FAILURE_VALUEMAX_ZGAIN,
	    _I2C_READ_FAILURE_VALUEMAX_CAL,
	    _I2C_WRITE_FAILURE_VALUEMAX_CAL,
	    _VALUEMAX_UNKNOWN_COMMAND,
	    _VALUEMAX_MANAGER_NULL,
	    _DS1050_FAILED_SET_DUTY_CYCLE,
	    _DS1050_FAILED_SHUTDOWN,
	    _DS1050_FAILED_WAKEUP,
	    _RS485_LABEL_IMAGE_QUEUE_NULL,
	    _RS485_IMAGE_BUFFER_NULL,
	    _RS485_INTERFACE_OPEN_FAILURE,
	    _RS485_DMA_FAILED_RESTART,
	    _SYSTEM_TIMER_FAILED_MUTEX,
	    _SYSTEM_TIMER_CREATE_FAILED,
	    _KPC_SYSTEM_UNKNOWN
	}KpcSystemError;

	KpcSystemErrors( Hobart::CEvolMsgReceiver *pHobThread = NULL, KpcSystemError err = _KPC_SYSTEM_UNKNOWN );

	KpcSystemErrors( const char *buffer );

	~KpcSystemErrors();

	const char *serialize()const;

	KpcSystemError getError() { return error; }
	std::string getErrorDescription();

	private:
	KpcSystemError error;

};
#endif
