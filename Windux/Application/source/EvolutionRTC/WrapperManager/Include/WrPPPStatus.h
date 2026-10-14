/**
 * @file WRPPPSTATUS.h
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate position status message
 * Created on: Mar 26, 2021
 */
#ifndef WRPPPSTATUS_H
#define WRPPPSTATUS_H
#include "WrapperGeneric.h"

class PPPStatus : public WrapperGeneric
{
	public:
    PPPStatus( Hobart::CEvolMsgReceiver	*pHobThread	= NULL );
    PPPStatus( const char *buffer );
    ~PPPStatus();

    typedef enum
    {
        RUN,
        CAL
    }PPPMode;

    typedef enum
    {
        OPERATIONAL,								// no error to remove 
        PRI_SENSOR_CAL_NEEDED,						// removed error
        SEC_SENSOR_CAL_NEEDED,						// removed error
        BOTH_SENSORS_CAL_NEEDED,					// removed error
        PRI_SENSOR_INTERNAL_CAL_FAILED,				// removed error
        SEC_SENSOR_INTERNAL_CAL_FAILED,				// removed error
        PRI_SENSOR_MEASUREMENT_START_FAILURE,		// removed error
        SEC_SENSOR_MEASUREMENT_START_FAILURE,		// removed error
        SENSOR_COMM_FAILURE,						// removed error
        SEC_SENSOR_ADDRESS_FAILURE,					// removed error
        PRI_TEMP_SENSOR_CONFIG_FAILURE,				// removed error
        PRI_TEMP_SENSOR_ADDRESS_FAILURE,			// removed error
        SEC_TEMP_SENSOR_CONFIG_FAILURE,				// removed error
        SEC_TEMP_SENSOR_ADDRESS_FAILURE,			// removed error
    }SensorStatus;
    
    PPPMode getMode() const { return mode_; }
    void setMode( PPPMode mode ) { mode_ = mode; }

    SensorStatus getSensorStatus() const { return sensorStatus_; }
    void setSensorStatus( SensorStatus status ) { sensorStatus_ = status; }

    const char *serialize() const;
    
    void show();

	private:
    PPPMode mode_;
    SensorStatus sensorStatus_;
};
#endif

