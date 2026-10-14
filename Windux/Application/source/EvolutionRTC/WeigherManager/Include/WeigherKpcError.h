/**
 * @file WeigherKpcError.h
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Aaron Swift
 * @brief Kpc engingeering debug Weigher errors
 * Created on: Jan 8, 2019
 *
 **/
#ifndef WEIGHERKPCERROR_H
#define WEIGHERKPCERROR_H
#include <string>
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class WeigherKpcError : public EvolMessage
{
	public:

	//Pulled for KPC Weigher firmware
	typedef enum
	{
	    _WG_READ_CHKSUM_PAGE_FAILURE,
	    _WG_READ_CFG_PAGE_FAILURE,
	    _WG_WAKETIMER_CREATE_FAILURE,
	    _WG_TIMER_DELETION_FAILURE,
	    _WG_MSG_QUEUE_FAILURE,
	    _WG_I2C_QUEUE_FAILURE,
	    _WG_UNKNOWN_MSG_FAILURE,
	    _WG_ADC_CLOCK_INIT_FAILURE,
	    _WG_ADC_CLOCK_PWM_FAILURE,
	    _WG_CNTS_FAILURED_TO_QUEUE,
	    _WG_TIMER_CREATE_FAILURE,
	    _WG_CFG_ZERO_TIME_FAILURE,
	    _WG_CAT3_AUDIT_MGR_INIT_FAILURE,
	    _WG_CAT3_AUDIT_MGR_READ_FAILURE,
	    _WG_CAT3_AUDIT_MGR_WRITE_FAILURE,
	    _WG_CAT3_LOG_CAL_EVENT_FAILURE,
	    _WG_CAT3_LOG_CFG_EVENT_FAILURE,
	    _WG_CAT3_AUDIT_MGR_WRITE_RO_FAILURE,
	    _WG_CAT3_AUDIT_MGR_ERASE_RO_FAILURE,
	    _WG_CAT3_PAGE_ERASE_FAILURE,
	    _WG_ACCEL_CMD_BUFFER_NULL,
	    _WG_UNKNOWN
	}WgKpcWeigherErrors;

	WeigherKpcError( Hobart::CEvolMsgReceiver *pHobThread = NULL, WgKpcWeigherErrors err = _WG_UNKNOWN );

	WeigherKpcError( const char *buffer );

	~WeigherKpcError();

	const char *serialize()const;

	WgKpcWeigherErrors getError() { return error; }
	std::string getErrorDescription();

	private:
	WgKpcWeigherErrors error;

};
#endif
