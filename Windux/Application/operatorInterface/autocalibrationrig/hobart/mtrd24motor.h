/**
 * @file mtrd24Motor.h
 * @copyright Copyright (c) 2018 Hobart Corportation
 * 
 * @{
 */
#ifndef mtrd24Motor_h
#define mtrd24Motor_h

#include "hobart/rs485/calrigrs485.h"

#if defined ( __linux__ )		// Windows 
#include <linux/serial.h>
#include <termios.h>
#endif // __linux__


#include <QObject>

typedef enum
{
    NO_ERROR,

    //Motor Drive Status Errors
    MOTOR_DRIVE_ERROR_PRESENT,

    //Motor drive sensor errors
    INVALID_HOME_SENSOR_STATE,

    //Motor Drive move Errors
    MOTOR_STALL, //encoder pulses dont match commanded motor steps


    //Motor Drive Info Errors
    WRONG_MOTOR_DRIVE_MODE,
    WRONG_IO_CONFIGURATION,

    //Motor Drive Command Errors
    INVALID_DRIVE_COMMAND,

    //RS485 Send Errors
    RS485_SEND_ERROR,

    //RS485 Receive Errors
    RECEIVE_HEADER_ERROR,
    NO_ACK_RECEIVED_ERROR,
    RS485_RECEIVE_TIMEOUT_ERROR,
    RS485_RECEIVE_ERROR,


}ERROR_CODE;

typedef enum
{
    NO_COMMAND,
    RESET_DRIVE,
    GET_FIRMWARE_VERSION,
    GET_MODEL_REVISION,
    GET_COMMAND_MODE,
    SET_COMMAND_MODE,
    GET_POWER_UP_MODE,
    SET_POWER_UP_MODE,
    GET_DRIVE_STATUS,
    GET_IO_CONFIG,
    GET_IO_STATUS,
    SET_ACCL_COMMAND,
    SET_DECEL_COMMAND,
    SET_DISTANCE_COMMAND,
    SET_VELOCITY_COMMAND,
    EXECUTE_FEED_TO_SENSOR_COMMAND,
    EXECUTE_FEED_TO_DISTANCE_COMMAND,
    EXECUTE_FEED_TO_POSITION_COMMAND,
    SET_TRAVEL_LIMITS_COMMAND,
    EXECUTE_SEEK_HOME_COMMAND,
    SET_ENCODER_POSITION,
    SET_MOTOR_POSITION,
    GET_ENCODER_POSITION,
    GET_MOTOR_POSITION,

}COMMAND_TYPE;

typedef enum
{
    UNDEFINED_IO_NUMBER,
    HOME_INPUT,             //1
    UPPER_LIMIT_INPUT,      //2
    LOWER_LIMIT_INPUT,      //3
    INPUT_4                 //4
}IO_INPUT_NUMBER;

typedef enum
{
    LOW,
    HIGH,
    FALLING_EDGE,
    RISING_EDGE

}SENSE_CONDITION;


#define HOME_SENSOR_INDEX           11
#define UPPER_LIMIT_SENSOR_INDEX    10
#define LOWER_LIMIT_SENSOR_INDEX    9

#define MTRD24_STEPS_PER_REV        20000
#define LEAD_SCREW_INCHES_PER_REV   .5

#define DISTANCE_INCHES_TO_DROP_FIRST_5KG_AT_TILT       2.9//2.8
#define DISTANCE_INCHES_TO_DROP_FIRST_5KG_AT_LEVEL      3.6
#define DISTANCE_INCHES_TO_DROP_NEXT_5KG    .5
#define DISTANCE_INCHES_TO_SECOND_AND_THIRD_WEIGHT_DROP .5


#define DISTANCE_STEPS_TO_DROP_FIRST_5KG_AT_TILT    (DISTANCE_INCHES_TO_DROP_FIRST_5KG_AT_TILT / LEAD_SCREW_INCHES_PER_REV) * MTRD24_STEPS_PER_REV
#define DISTANCE_STEPS_TO_DROP_FIRST_5KG_AT_LEVEL   (DISTANCE_INCHES_TO_DROP_FIRST_5KG_AT_LEVEL / LEAD_SCREW_INCHES_PER_REV) * MTRD24_STEPS_PER_REV
#define DISTANCE_STEPS_TO_DROP_NEXT_5KG             (DISTANCE_INCHES_TO_DROP_NEXT_5KG / LEAD_SCREW_INCHES_PER_REV) * MTRD24_STEPS_PER_REV

//encoder
#define MTRD24_ENCODER_PULSES_PER_REV   4000
#define MTRD24_ENCODER_INCHES_PER_PULSE .000125
#define MTRD24_ENCODER_PULSE_TOLERANCE  10 //equals one thousands of an inch
#define ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT    (DISTANCE_INCHES_TO_DROP_FIRST_5KG_AT_TILT / LEAD_SCREW_INCHES_PER_REV) * MTRD24_ENCODER_PULSES_PER_REV
#define ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_LEVEL   (DISTANCE_INCHES_TO_DROP_FIRST_5KG_AT_LEVEL / LEAD_SCREW_INCHES_PER_REV) * MTRD24_ENCODER_PULSES_PER_REV
#define ENCODER_PULSES_TO_DROP_NEXT_5KG             (DISTANCE_INCHES_TO_DROP_NEXT_5KG / LEAD_SCREW_INCHES_PER_REV) * MTRD24_ENCODER_PULSES_PER_REV


typedef struct
{
    bool alarm_code_present;
    bool drive_disabled;
    bool drive_faulted;
    bool homing;
    bool jogging;
    bool in_motion_from_feed_jog_command;
    bool in_motion_from_any_command;
    bool enabled_and_ready;
    bool stopping_a_motion;
    bool wait_time;
    bool wait_input;

}DRIVE_STATUS;

typedef struct
{
    unsigned int    accl_rate;  //revs/sec positive numbers only
    unsigned int    decel_rate; //revs/sec positive numbers only
    long            distance;   //distance in steps Pos and Neg numbers
    unsigned int    velocity;   //Positive numbers only
    IO_INPUT_NUMBER input_no;   //1-4
    SENSE_CONDITION input_condition;
    SENSE_CONDITION travel_limits_input_condition;

}TRAJECTORY;

typedef struct
{
    COMMAND_TYPE    current_command;
    //unsigned char   command_buffer[RS485_MAX_LINE_LENGTH];
    QByteArray      command_buffer;
    unsigned char  *command_buffer_ptr;
    unsigned int    command_length;
    bool            command_sent;//was sent on 485
}COMMAND;

typedef struct
{
    unsigned char   response_buffer[RS485_MAX_LINE_LENGTH];
    unsigned int    response_length;
    bool            response_received;
    ERROR_CODE      response_rx_error;//Timeout or -1
}RESPONSE;

/**
 * @class mtrd24Motor
 * @brief Calibration Rig Motor Control
 * 
 */
class mtrd24MotorDrive: public RS485Manager{
    Q_OBJECT

    public slots:
        void handleDataReady( const QString &buffer, int totalBytes);

    public:      
        mtrd24MotorDrive();
        
        ERROR_CODE initializeWeightStack(void);
        ERROR_CODE Mtrd24FindHome(void);
        ERROR_CODE Mtrd24ReturnHome(void);
        ERROR_CODE Mtrd24DropFirst5KgAtTilt(void);
        ERROR_CODE Mtrd24DropFirst5KgAtLevel(void);
        ERROR_CODE Mtrd24DropSecond5Kg(void);
        ERROR_CODE Mtrd24DropThird5Kg(void);
        ERROR_CODE Mtrd24DropSecondAndThird5Kg(void);
        ERROR_CODE Mtrd24Drop15KgAtLevel(void);
        ERROR_CODE Mtrd24Drop15KgAtTilt(void);
        ERROR_CODE Mtrd24TestDrive(void);
        bool isMtrd24HomePositionDetermined(void);


        /**
         * @brief Destructor 
         * 
         */
        ~mtrd24MotorDrive() {}

    private:
        QString         firmware_version;
        QString         drive_command_mode;
        DRIVE_STATUS    drive_status;
        QString         drive_io_config;
        COMMAND         command;
        RESPONSE        response;
        bool            ack_received;
        bool            home_position_found;
        ERROR_CODE buildCommand(COMMAND_TYPE Command, TRAJECTORY parameters);
        ERROR_CODE sendCommand(void);
        ERROR_CODE receiveResponse(QString *receiveData);
        ERROR_CODE setHomingTrajectoryParameters(void);
        ERROR_CODE setMtrd24ZeroPosition(void);
        ERROR_CODE setMtrd24TravelLimits(SENSE_CONDITION condition);
        ERROR_CODE setMtrd24AcclRate(unsigned int revs_per_sec_rate);
        ERROR_CODE setMtrd24DecelRate(unsigned int revs_per_sec_rate);
        ERROR_CODE setMtrd24MoveDistance(long distance_in_steps);
        ERROR_CODE setMtrd24Velocity(unsigned int revs_per_sec);
        ERROR_CODE Mtrd24WaitUntilMotionComplete(void);

        ERROR_CODE getMtrd24MotorModel(QString *model);
        QString getMtrd24FirmwareVersion(void);
        ERROR_CODE requestMtrd24FirmwareVersion(QString *version);
        ERROR_CODE requestMtrd24CommandMode(QString *mode);
        ERROR_CODE requestMtrd24Status(QString *status);
        ERROR_CODE requestMtrd24IOConfig(QString *io_config);
        ERROR_CODE requestMtrd24IOStatus(QString *io_status);
        ERROR_CODE requestMtrd24EncoderPosition(QString *position);
        ERROR_CODE requestMtrd24MotorPosition(QString *position);
        ERROR_CODE isMtrd24AtHome(bool *home);

        //motor movements
        ERROR_CODE Mtrd24FeedToSensor(IO_INPUT_NUMBER sensor_num, SENSE_CONDITION condition);
        ERROR_CODE Mtrd24SeekHome(IO_INPUT_NUMBER sensor_num, SENSE_CONDITION condition);
        ERROR_CODE Mtrd24FeedToDistance(long distance_in_steps);
        ERROR_CODE Mtrd24FeedToPosition(long position_in_steps);
        
        QString getMyChar(int index, const QString &localStr);
        bool gotAck(void);
        DRIVE_STATUS decodeMtrd24StatusMsg(QString statusMsg);


};


/** @} */
#endif //calRigrs485manager_h
