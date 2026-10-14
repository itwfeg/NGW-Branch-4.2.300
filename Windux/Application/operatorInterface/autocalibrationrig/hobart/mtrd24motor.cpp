/**
 * @file RS485Manager.cpp
 * @copyright Copyright (c) 2018 Hobart Corportation
 * @author Aaron Swift
 * @brief Responsible for sending label data over RS485
 * @addtogroup CommManagers
 * @{
 */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>


#if defined ( __linux__ )		// Windows
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#include <QObject>
#include <QEventLoop>
#include <QTimer>
#include "common.h"
#include <string.h>

#include <math.h>
#include <stdint.h>

#include "hobart/mtrd24motor.h"
#include "readThread.h"



mtrd24MotorDrive::mtrd24MotorDrive()
{

    
}

void mtrd24MotorDrive::handleDataReady( const QString &buffer, int totalBytes)
{
    unsigned char mychar;
    unsigned char mycharArray[20];
    const QString my_timeout_string = "Rx Timeout\n\r";

    /*
    *   look for timeout error from read(). read() will signal a timeout error
    *   if it hasn't received any data with in 5sec. This will happen even when
    *   setting idle. So only pass error if we are expecting data back.
    */
    if(my_timeout_string == buffer && ((command.current_command == NO_COMMAND) || (command.command_sent == false)))
    {
        return;
        
    }


    if(totalBytes <= RS485_MAX_LINE_LENGTH)
    {
        //load response
        memcpy( (void*)&response.response_buffer[0] , buffer.toStdString().c_str(), totalBytes );
        response.response_length = totalBytes;
    }
    
    response.response_received = true;

    if(my_timeout_string == buffer)
    {
        std::cerr << "Received Data Ready. Timeout Error!!: " << totalBytes << std::endl;
        response.response_rx_error = RS485_RECEIVE_TIMEOUT_ERROR;
    }

    //std::cerr << "Received Data Ready. totalBytes: " << totalBytes << std::endl;

#if 0
    for(int i=0; i<totalBytes; i++)
    {
        std::cerr << "Data[ " << i << "] = " << response.response_buffer[i] << std::endl;
    }
#endif    

}



/*
 * 
 *  1. Get Drive info
 *  2. Get Command Mode
 *  3. Get Ready Status 
 *  Assumption: rs485 interface is open 
 */
ERROR_CODE mtrd24MotorDrive::initializeWeightStack()
{
    ERROR_CODE response_code;
    QString response_string;

    /*
    *   Get Firmware Version
    */
    response_code = requestMtrd24FirmwareVersion(&response_string);

    if( response_code == NO_ERROR)
    {
        std::cerr << "requestMtrd24FirmwareVersion = " << response_string.toStdString() << std::endl;
        firmware_version = response_string;
    }
    else
    {
        std::cerr << "ERROR Getting Firmware Version " << std::endl;
        return response_code;
    }

    /*
    *   Get Command Mode
    */
    response_code = requestMtrd24CommandMode(&response_string);

    if( response_code == NO_ERROR)
    {
        std::cerr << "requestMtrd24CommandMode = " << response_string.toStdString() << std::endl;
        drive_command_mode = response_string;
    }
    else
    {
        std::cerr << "ERROR Getting Command Mode " << std::endl;
        return response_code;
    }

    /*
    *   Get I/O input configuration
    */
    response_code = requestMtrd24IOConfig(&response_string);

    if( response_code == NO_ERROR)
    {
        QString valid_io_config = "1SD=00001111\r";

        std::cerr << "requestMtrd24IOConfig = " << response_string.toStdString() << std::endl;
        drive_io_config = response_string;

        //ensure that IO is all inputs.
        if(drive_io_config != valid_io_config)
        {
            std::cerr << "ERROR Wrong IO Config " << std::endl;
            return WRONG_IO_CONFIGURATION;
        }
    }
    else
    {
        std::cerr << "ERROR Getting IO Config " << std::endl;
        return response_code;
    }

    /*  
    *   Set Over Travel Limits
    */
    std::cerr << "setMtrd24TravelLimits(LOW)  " << std::endl;
    response_code = setMtrd24TravelLimits(LOW);
    if(response_code != NO_ERROR)
    {
        return response_code;
    }

    
    /*
    *   Get Ready Status
    */
    response_code = requestMtrd24Status(&response_string);

    if( response_code == NO_ERROR)
    {
        std::cerr << "requestMtrd24Status = " << response_string.toStdString() << std::endl;
        //drive_status = response_string;
    }
    else
    {
        std::cerr << "ERROR Getting Status " << std::endl;
        //return response_code;
    }

    //force a homing routine
    home_position_found = false;

    return response_code;

}

/*
*   ERROR_CODE isMtrd24AtHome(bool *home)
*
*   Brief:  Requests Motor's IO status via RS485 and looks at Home input value.
*   Return: Response code indicating request RS485 result. If no error,
*           boolean indicating if motor is at home position.
*
*/
ERROR_CODE mtrd24MotorDrive::isMtrd24AtHome(bool *home)
{
    QString io;
    QString input_state;
    QString high = "1";
    QString low = "0";
    ERROR_CODE result;

    result = requestMtrd24IOStatus(&io);

    std::cerr << "requestMtrd24IOStatus = " << io.toStdString() << std::endl;

    if(result == NO_ERROR)
    {
        /* response format: 1IS=0000XXXX */
        input_state = getMyChar(HOME_SENSOR_INDEX, io);

        if(input_state == high)//High means open sensor
        {
            *home = false;
            std::cerr << "Home sensor = 1" << std::endl;
        }
        else if(input_state == low)//low means closed sensor
        {
            *home = true;
            std::cerr << "Home sensor = 0" << std::endl;
        }
        else
        {
            result = INVALID_HOME_SENSOR_STATE;
        }
    }

    return result;
}


/*
*   Look at response buffer and validate data
*/
ERROR_CODE mtrd24MotorDrive::receiveResponse(QString *receiveData)
{
    QEventLoop myeventLoop;
    QTimer mytimer;
    

    //mytimer.setInterval(500);
    mytimer.setInterval(100);
    mytimer.setSingleShot(false);
    mytimer.start();
    QObject::connect( &mytimer, SIGNAL(timeout()), &myeventLoop, SLOT(quit() ));
    
    while(1)
    {
       
        switch(command.current_command)
        {

            case RESET_DRIVE:
            {
                /*
                *   Drive just returns ack 
                */
                if(gotAck() == true)
                {
                    return NO_ERROR;
                }
                else
                {
                    return NO_ERROR;
                }
            }
            break;

            case GET_FIRMWARE_VERSION:
            {
                std::cerr << "checking response" << std::endl;
                
                if(response.response_received == true)
                {
                    //clear current command
                    command.current_command = NO_COMMAND;
                    
                    *receiveData = (const char *) &response.response_buffer;

                    //look for errors
                    if(response.response_rx_error == NO_ERROR)
                    {   
                        if(response.response_buffer[0] == '1'   &&
                           response.response_buffer[1] == 'R'   &&
                           response.response_buffer[2] == 'V'   &&
                           response.response_buffer[3] == '=')
                        {
                            return NO_ERROR;
                        }
                        else
                        {
                            return RECEIVE_HEADER_ERROR;
                        }
                        
                    }
                    else
                    {
                        return response.response_rx_error;
                    }
                } 
                
            }
            break;

            case GET_COMMAND_MODE:
            {
                std::cerr << "checking response" << std::endl;
                /* Drive should respond with 1CM=21 (point-to-point SCL Mode)*/
                if(response.response_received == true)
                {
                    //clear current command
                    command.current_command = NO_COMMAND;
                    
                    *receiveData = (const char *) &response.response_buffer;
                        
                    if( response.response_rx_error == NO_ERROR)
                    {
                        
                        if( response.response_buffer[0] == '1'   &&
                            response.response_buffer[1] == 'C'   &&
                            response.response_buffer[2] == 'M'   &&
                            response.response_buffer[3] == '=')
                        {           
                            return NO_ERROR;
                        }
                        else
                        {
                            return RECEIVE_HEADER_ERROR;
                        }

                    }
                    else
                    {
                        return response.response_rx_error;
                    }
                } 
                
            }
            break;   

            case GET_DRIVE_STATUS:
            {
                std::cerr << "checking response" << std::endl;
                /* Drive should respond with 1RS=R (point-to-point SCL Mode)*/
                if(response.response_received == true)
                {
                    //clear current command
                    command.current_command = NO_COMMAND;
                    
                    *receiveData = (const char *) &response.response_buffer;

                    if(response.response_rx_error == NO_ERROR)
                    {
                        if( response.response_buffer[0] == '1'   &&
                            response.response_buffer[1] == 'R'   &&
                            response.response_buffer[2] == 'S'   &&
                            response.response_buffer[3] == '=')
                        {
                            //drive_status = (const char *) &response.response_buffer;
                            return NO_ERROR;
                        }
                        else
                        {
                            //drive_status = (const char *) &response.response_buffer;
                            return RECEIVE_HEADER_ERROR;
                        }

                    }
                    else
                    {
                        return response.response_rx_error;
                    }
                } 
                
            }
            break;    

            case GET_IO_CONFIG:
            {
                std::cerr << "checking response" << std::endl;
                /* Drive should respond with 1SD=00001111*/
                if(response.response_received == true)
                {
                    //clear current command
                    command.current_command = NO_COMMAND;

                    *receiveData = (const char *) &response.response_buffer;

                    if(response.response_rx_error == NO_ERROR)
                    {
                        if( response.response_buffer[0] == '1'   &&
                            response.response_buffer[1] == 'S'   &&
                            response.response_buffer[2] == 'D'   &&
                            response.response_buffer[3] == '=')
                        {
                            return NO_ERROR;
                        }
                        else
                        {
                            return RECEIVE_HEADER_ERROR;
                        }

                    }
                    else
                    {
                        return response.response_rx_error;
                    }
                } 
                
            }
            break;   

            case GET_IO_STATUS:
            {
                std::cerr << "checking response" << std::endl;
                /* Drive should respond with 1IS=0000XXXX*/
                if(response.response_received == true)
                {
                    //clear current command
                    command.current_command = NO_COMMAND;

                    *receiveData = (const char *) &response.response_buffer;

                    if(response.response_rx_error == NO_ERROR)
                    {
                        if( response.response_buffer[0] == '1'   &&
                            response.response_buffer[1] == 'I'   &&
                            response.response_buffer[2] == 'S'   &&
                            response.response_buffer[3] == '=')
                        {
                            return NO_ERROR;
                        }
                        else
                        {
                            return RECEIVE_HEADER_ERROR;
                        }

                    }
                    else
                    {
                        return response.response_rx_error;
                    }
                } 
                
            }
            break;   

            case GET_ENCODER_POSITION:
            {
                std::cerr << "checking response" << std::endl;
                /* Drive should respond with 1EP=XXXXXXXX*/
                if(response.response_received == true)
                {
                    //clear current command
                    command.current_command = NO_COMMAND;

                    *receiveData = (const char *) &response.response_buffer;

                    if(response.response_rx_error == NO_ERROR)
                    {
                        if( response.response_buffer[0] == '1'   &&
                            response.response_buffer[1] == 'E'   &&
                            response.response_buffer[2] == 'P'   &&
                            response.response_buffer[3] == '=')
                        {
                            return NO_ERROR;
                        }
                        else
                        {
                            return RECEIVE_HEADER_ERROR;
                        }

                    }
                    else
                    {
                        return response.response_rx_error;
                    }
                } 
                
            }
            break;

            case GET_MOTOR_POSITION:
            {
                std::cerr << "checking response" << std::endl;
                /* Drive should respond with 1SP=XXXXXXXX*/
                if(response.response_received == true)
                {
                    //clear current command
                    command.current_command = NO_COMMAND;

                    *receiveData = (const char *) &response.response_buffer;

                    if(response.response_rx_error == NO_ERROR)
                    {
                        if( response.response_buffer[0] == '1'   &&
                            response.response_buffer[1] == 'S'   &&
                            response.response_buffer[2] == 'P'   &&
                            response.response_buffer[3] == '=')
                        {
                            return NO_ERROR;
                        }
                        else
                        {
                            return RECEIVE_HEADER_ERROR;
                        }

                    }
                    else
                    {
                        return response.response_rx_error;
                    }
                } 
                
            }
            break;   

            case SET_ACCL_COMMAND:
            case SET_DECEL_COMMAND:
            case SET_DISTANCE_COMMAND:
            case SET_VELOCITY_COMMAND:
            case EXECUTE_FEED_TO_SENSOR_COMMAND:
            case EXECUTE_FEED_TO_DISTANCE_COMMAND:
            case EXECUTE_FEED_TO_POSITION_COMMAND:
            case SET_TRAVEL_LIMITS_COMMAND:
            case EXECUTE_SEEK_HOME_COMMAND:
            case SET_ENCODER_POSITION:
            case SET_MOTOR_POSITION:
            {
                std::cerr << "checking response" << std::endl;
                /* Drive should respond with ack '%'*/
                if(response.response_received == true)
                {
                    //clear current command
                    command.current_command = NO_COMMAND;

                    *receiveData = (const char *) &response.response_buffer;

                    if(response.response_rx_error == NO_ERROR)
                    {
                        if( response.response_buffer[0] == '1' && response.response_buffer[1] == '%' )
                        {
                            return NO_ERROR;
                        }
                        else
                        {
                            return NO_ACK_RECEIVED_ERROR;
                        }

                    }
                    else
                    {
                        return response.response_rx_error;
                    }
                } 
                
            }
            break;

            
        }

        myeventLoop.exec(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);

    }

}
/*
*   Look at response buffer and see if '%' ack 
*   character is present.
*/
bool mtrd24MotorDrive::gotAck(void)
{
    bool found              = false;
    int  chars_in_buff      = response.response_length;
    int  i                  = 0;


    while(i < (chars_in_buff -1))
    {
        if(response.response_buffer[i] == '%')
        {
            found = true;
            std::cerr << "ACK detected" << std::endl;
            break;
        }

        i++;
    }

    return found;

}

ERROR_CODE mtrd24MotorDrive::buildCommand(COMMAND_TYPE Command, TRAJECTORY parameters)
{
    ERROR_CODE result = NO_ERROR;
    /* clear command buffer */
    //memset(&command.command_buffer, 0, sizeof(command));

    //remember command
    command.current_command = Command;
    command.command_sent    = false;//set to true in sendCommand() to prevent race condition

    switch(Command)
    {
        case RESET_DRIVE:
        {
            QString reset = "1RE";

            reset.append("\r\n");
            
            command.command_buffer      = reset.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();             
        }
        break;

        case GET_FIRMWARE_VERSION:
        {
            QString firmware_rev = "1RV";
            
            firmware_rev.append("\r\n");
            
            command.command_buffer      = firmware_rev.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();               
        }
        break;

        case GET_MODEL_REVISION:
        {
            QString model_rev = "1MV";
            
            model_rev.append("\r\n");
            
            command.command_buffer      = model_rev.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();              
        }
        break;
        
        case GET_COMMAND_MODE:
        {
            QString cmd_mode = "1CM";
            
            cmd_mode.append("\r\n");
            
            command.command_buffer      = cmd_mode.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();                       
        }
        break;

        case GET_DRIVE_STATUS:
        {
            QString drive_status = "1RS";
            
            drive_status.append("\r\n");
            
            command.command_buffer      = drive_status.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();            
        }
        break;
        
        case GET_IO_CONFIG:
        {
            QString io_config = "1SD";
            
            io_config.append("\r\n");
            
            command.command_buffer      = io_config.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();
            
        }
        break;

        case GET_IO_STATUS:
        {
            QString io_status = "1IS";
            
            io_status.append("\r\n");
            
            command.command_buffer      = io_status.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();

        }
        break;

        case SET_ACCL_COMMAND:
        {
            QString accl_command    = "1AC";

            accl_command.append(QString::number(parameters.accl_rate));
            accl_command.append("\r\n");
            
            command.command_buffer      = accl_command.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();
        }
        break;

        case SET_DECEL_COMMAND:
        {
            QString accl_command    = "1DE";

            accl_command.append(QString::number(parameters.decel_rate));
            accl_command.append("\r\n");
            
            command.command_buffer      = accl_command.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();
        }
        break;

        case SET_DISTANCE_COMMAND:
        {
            QString accl_command    = "1DI";

            accl_command.append(QString::number(parameters.distance));
            accl_command.append("\r\n");
            
            command.command_buffer      = accl_command.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();
        }
        break;

        case SET_VELOCITY_COMMAND:
        {
            QString accl_command    = "1VE";

            accl_command.append(QString::number(parameters.velocity));
            accl_command.append("\r\n");
            
            command.command_buffer      = accl_command.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();
        }
        break;

        case EXECUTE_FEED_TO_SENSOR_COMMAND:
        {
            QString accl_command    = "1FS";

            accl_command.append(QString::number((unsigned int)parameters.input_no));
            
            if(parameters.input_condition == LOW)
            {
                accl_command.append('L');
            }
            else if(parameters.input_condition == HIGH)
            {
                accl_command.append('H');
            }
            else if(parameters.input_condition == FALLING_EDGE)
            {
                accl_command.append('F');
            }
            else if(parameters.input_condition == RISING_EDGE
            )
            {
                accl_command.append('R');
            }

            accl_command.append("\r\n");
            
            command.command_buffer      = accl_command.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();
        }
        break;

        case EXECUTE_FEED_TO_DISTANCE_COMMAND:
        case EXECUTE_FEED_TO_POSITION_COMMAND:
        {
            QString feed_command;   

            if(Command == EXECUTE_FEED_TO_DISTANCE_COMMAND)
            {
                feed_command = "1FL";
            }
            else//Position
            {
                feed_command = "1FP";
            }
            

            //if(parameters.distance != 0)
            //{
                feed_command.append(QString::number(parameters.distance));
            //}
            
            feed_command.append("\r\n");
            
            command.command_buffer      = feed_command.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();
        }
        break;

        case SET_TRAVEL_LIMITS_COMMAND:
        {
            QString limits_command    = "1DL";

            if(parameters.travel_limits_input_condition == LOW)
            {
                limits_command.append('1');
            }
            else if(parameters.travel_limits_input_condition == HIGH)
            {
                limits_command.append('2');
            }
            
            limits_command.append("\r\n");
            
            command.command_buffer      = limits_command.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();
        }
        break;

        case EXECUTE_SEEK_HOME_COMMAND:
        {
            QString sh_command    = "1SH";

            sh_command.append(QString::number((unsigned int)parameters.input_no));
            
            if(parameters.input_condition == LOW)
            {
                sh_command.append('L');
            }
            else if(parameters.input_condition == HIGH)
            {
                sh_command.append('H');
            }
            else if(parameters.input_condition == FALLING_EDGE)
            {
                sh_command.append('F');
            }
            else if(parameters.input_condition == RISING_EDGE
            )
            {
                sh_command.append('R');
            }

            sh_command.append("\r\n");
            
            command.command_buffer      = sh_command.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();
        }
        break;

        case SET_ENCODER_POSITION:
        case SET_MOTOR_POSITION:
        {
            QString position_command;    

            if(Command == SET_ENCODER_POSITION)
            {
                position_command = "1EP";
            }
            else//Motor position
            {
                position_command = "1SP";
            }
            

            position_command.append(QString::number(parameters.distance));
            
            position_command.append("\r\n");
            
            command.command_buffer      = position_command.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();

        }
        break;

        case GET_ENCODER_POSITION:
        case GET_MOTOR_POSITION:
        {
            QString req_position_command;    

            if(Command == GET_ENCODER_POSITION)
            {
                req_position_command = "1EP";
            }
            else//Motor position
            {
                req_position_command = "1SP";
            }
            
            
            req_position_command.append("\r\n");
            
            command.command_buffer      = req_position_command.toLatin1();
            command.command_buffer_ptr  = (unsigned char *)command.command_buffer.data();
            command.command_length      = command.command_buffer.length();

        }
        break;

        default:
        {
            result = INVALID_DRIVE_COMMAND;
        }
    }
    
}

/*
*   Send Drive command and Wait for response
*/
ERROR_CODE mtrd24MotorDrive::sendCommand(void)
{
    int result;
    
    ack_received = false;

    //clear response data
    memset(&response, 0, sizeof(response));

    result = RS485Send((unsigned char*)command.command_buffer_ptr, (int)command.command_length);

    //did we send all our data    
    if(result == command.command_length)
    {
        return NO_ERROR;
        command.command_sent = true;

    }
    else
    {
        std::cerr << "ERROR " <<  "RS485Send((unsigned char*)&command.command_buffer, (int)command.command_length)" << std::endl;
        return RS485_SEND_ERROR;
    }

}

/*
 *  getMtrd24MotorModel(QString *)
 *  Request Revison model from Motor.
 */
ERROR_CODE mtrd24MotorDrive::getMtrd24MotorModel(QString *model)
{
    ERROR_CODE response_code;
    ERROR_CODE send_code;
    TRAJECTORY parameters;
    /*
    *   For some unknown reason the motor responds back with 
    *   model information immmediately. It's configured to
    *   wait 5ms after its received a command to respond. This
    *   is to prevent race conditions with the RS485 transiver.
    */
    //Get Motor Info
    buildCommand(GET_MODEL_REVISION, parameters);
    
    send_code = sendCommand();

    if(send_code != NO_ERROR)
    {
        return send_code;
    }


    /*
    *   Get Motor's response
    */
    response_code = receiveResponse(model);

    return response_code;

}


/*
*   QString getMtrd24FirmwareVersion();
*   Gets Firmware version from class. 
*
*/
QString mtrd24MotorDrive::getMtrd24FirmwareVersion(void)
{
    return firmware_version;
}

/*
*   ERROR_CODE getMtrd24FirmwareVersion();
*
*   Brief:  Requests Motor's Firmware version via RS485. 
*   Return: Response code indicating request result. If no error,
*           version string will contain Firmware version.
*
*/
ERROR_CODE mtrd24MotorDrive::requestMtrd24FirmwareVersion(QString *version)
{
    ERROR_CODE response_code;
    ERROR_CODE send_code;
    TRAJECTORY parameters;

    //Build command to Get Motor Firmware Info
    buildCommand(GET_FIRMWARE_VERSION, parameters);
    send_code = sendCommand();

    if(send_code != NO_ERROR)
    {
        return send_code;
    }


    /*
    *   Get Motor's response
    */
    response_code = receiveResponse(version);

    return response_code;
}

/*
*   ERROR_CODE requestMtrd24CommandMode();
*
*   Brief:  Requests Motor's Command Mode via RS485. 
*   Return: Response code indicating request result. If no error,
*           version string will contain Command Mode.
*
*/
ERROR_CODE mtrd24MotorDrive::requestMtrd24CommandMode(QString *mode)
{
    ERROR_CODE response_code;
    ERROR_CODE send_code;
    TRAJECTORY parameters;

    //Build command to Get Motor Command Mode
    buildCommand(GET_COMMAND_MODE, parameters);
    send_code = sendCommand();

    if(send_code != NO_ERROR)
    {
        return send_code;
    }


    /*
    *   Get Motor's response
    */
    response_code = receiveResponse(mode);

    return response_code;
}


/*
*   ERROR_CODE requestMtrd24Status(QString *status)
*
*   Brief:  Requests Motor's Status via RS485. 
*   Return: Response code indicating request result. If no error,
*           version string will contain Motor Status.
*
*/
ERROR_CODE mtrd24MotorDrive::requestMtrd24Status(QString *status)
{
    ERROR_CODE response_code;
    ERROR_CODE send_code;
    TRAJECTORY parameters;

    //Build command to Get Motor Status
    buildCommand(GET_DRIVE_STATUS, parameters);
    send_code = sendCommand();

    if(send_code != NO_ERROR)
    {
        return send_code;
    }


    /*
    *   Get Motor's response
    */
    response_code = receiveResponse(status);
    
    std::cerr << "buildCommand(GET_DRIVE_STATUS, parameters); = " << status->toStdString() << std::endl;

    return response_code;
}


/*
*   ERROR_CODE requestMtrd24IOConfig(QString *mode)
*
*   Brief:  Requests Motor's IO config via RS485. 
*   Return: Response code indicating request result. If no error,
*           version string will contain IO Config.
*
*/
ERROR_CODE mtrd24MotorDrive::requestMtrd24IOConfig(QString *io_config)
{
    ERROR_CODE response_code;
    ERROR_CODE send_code;
    TRAJECTORY parameters;

    //Build command to Get Motor Status
    buildCommand(GET_IO_CONFIG, parameters);
    send_code = sendCommand();

    if(send_code != NO_ERROR)
    {
        return send_code;
    }


    /*
    *   Get Motor's response
    */
    response_code = receiveResponse(io_config);

    return response_code;
}

/*
*   ERROR_CODE requestMtrd24IOStatus(QString *io_status)
*
*   Brief:  Requests Motor's IO status via RS485. 
*   Return: Response code indicating request result. If no error,
*           version string will contain IO status.
*
*/
ERROR_CODE mtrd24MotorDrive::requestMtrd24IOStatus(QString *io_status)
{
    ERROR_CODE response_code;
    ERROR_CODE send_code;
    TRAJECTORY parameters;

    //Build command to Get Motor Status
    buildCommand(GET_IO_STATUS, parameters);
    send_code = sendCommand();

    if(send_code != NO_ERROR)
    {
        return send_code;
    }


    /*
    *   Get Motor's response
    */
    response_code = receiveResponse(io_status);

    return response_code;
}

/*
// Get the index at location 5
Int myIndex = 5;
QString myString = “Hobarts”;
QString myChar = getMyChar(myIndex, myString)
*/

// Calls this
QString mtrd24MotorDrive::getMyChar(int index, const QString& localStr )
{
    // note when I pass it in I make it a const and a reference (so I don’t mess it up and a reference)
    // but when I call this method, I’m just passing a straight QString in
    // I think QString c here can also be a QChar c but I tend to stay away from chars unless I’m doing a lot character parsing.
    
    QString c = localStr.at(index);                 // where index is any int
    return c;                         // returns ‘t’
}

/*
*   DRIVE_STATUS decodeMtrd24StatusMsg(QString statusMsg)
*
*   Brief:  Function will parse status message received from Drive and  
*           set or clear respective fields in DRIVE_STATUS structure.
*   Return: Updated DRIVE_STATUS structure
*
*/
DRIVE_STATUS mtrd24MotorDrive::decodeMtrd24StatusMsg(QString statusMsg)
{
    DRIVE_STATUS status;
    int found;

    memset(&status,0,sizeof(DRIVE_STATUS));

    status.alarm_code_present               = (bool)statusMsg.count('A');
    status.drive_disabled                   = (bool)statusMsg.count('D');
    status.drive_faulted                    = (bool)statusMsg.count('E');
    status.homing                           = (bool)statusMsg.count('H');
    status.jogging                          = (bool)statusMsg.count('J');
    status.in_motion_from_feed_jog_command  = (bool)statusMsg.count('F');
    status.in_motion_from_any_command       = (bool)statusMsg.count('M');
    status.wait_time                        = (bool)statusMsg.count('T');
    status.wait_input                       = (bool)statusMsg.count('W');
    
    
    found = statusMsg.count('R');
    if(found == 2)
    {
        status.enabled_and_ready = true;
    }

    found = statusMsg.count('S');
    if(found == 2)
    {
        status.stopping_a_motion = true;
    }

    return status;
}
/*
*   ERROR_CODE Mtrd24FeedToSensor(int sensor_num, SENSE_CONDITION condition)
*
*   Brief:  Executes FeedToSensor move command via RS485. See motor manual pg.58. 
*           Motor will use current Accl, Decel, Vel and distance parameters.
*           Passed in sensor data will be used as trigger input to stop the motor.
*   Return: Response code indicating request result. If no error,
*           Motor will move and stop according to passed in parameters.
*
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24FeedToSensor(IO_INPUT_NUMBER sensor_num, SENSE_CONDITION condition)
{
    ERROR_CODE response_code;
    ERROR_CODE send_code;
    TRAJECTORY parameters;
    QString    receive_data;

    //execute Feed to sensor
    parameters.input_no         = sensor_num;//1-4
    parameters.input_condition  = condition;

    //Build command 
    buildCommand(EXECUTE_FEED_TO_SENSOR_COMMAND, parameters);
    send_code = sendCommand();

    if(send_code != NO_ERROR)
    {
        return send_code;
    }

    /*
    *   Get Motor's response
    */
    response_code = receiveResponse(&receive_data);

    std::cerr << "buildCommand(EXECUTE_FEED_TO_SENSOR_COMMAND, parameters); = " << receive_data.toStdString() << std::endl;

       return response_code;

}

/*
*   ERROR_CODE Mtrd24SeekHome(int sensor_num, SENSE_CONDITION condition)
*
*   Brief:  Executes SeekHome move command via RS485. See motor manual pg.94. 
*           Motor will use current Accl, Decel, Vel and distance parameters.
*           Passed in sensor data will be used as trigger input to home the motor.
*   Return: Response code indicating request result. If no error,
*           Motor will move and stop according to passed in parameters.
*
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24SeekHome(IO_INPUT_NUMBER sensor_num, SENSE_CONDITION condition)
{
    ERROR_CODE response_code;
    ERROR_CODE send_code;
    TRAJECTORY parameters;
    QString    receive_data;

    //execute Feed to sensor
    parameters.input_no         = sensor_num;//1-4
    parameters.input_condition  = condition;

    //Build command 
    buildCommand(EXECUTE_SEEK_HOME_COMMAND, parameters);
    send_code = sendCommand();

    if(send_code != NO_ERROR)
    {
        return send_code;
    }

    /*
    *   Get Motor's response
    */
    response_code = receiveResponse(&receive_data);

    std::cerr << "buildCommand(EXECUTE_SEEK_HOME_COMMAND, parameters); = " << receive_data.toStdString() << std::endl;

    return response_code;

}

/*
*   ERROR_CODE Mtrd24FeedToDistance(long distance_in_steps);
*
*   Brief:  Executes FeedTolength move command via RS485. See motor manual pg.54. 
*           Motor will use current Accl, Decel, Vel parameters.
*           Passed in distance_in_steps will be used as distance. If distance_in_steps is 0,
*           then motor will use the last value used for distance. 
*   Return: Response code indicating request result. If no error,
*           Motor will move and stop according to passed in parameters.
*
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24FeedToDistance(long distance_in_steps)
{
    ERROR_CODE response_code;
    ERROR_CODE send_code;
    TRAJECTORY parameters;
    QString    receive_data;

    //execute Feed to distance
    parameters.distance         = distance_in_steps;
    
    //Build command 
    buildCommand(EXECUTE_FEED_TO_DISTANCE_COMMAND, parameters);
    send_code = sendCommand();

    if(send_code != NO_ERROR)
    {
        return send_code;
    }

    /*
    *   Get Motor's response
    */
    response_code = receiveResponse(&receive_data);

    std::cerr << "buildCommand(EXECUTE_FEED_TO_SENSOR_COMMAND, parameters); = " << receive_data.toStdString() << std::endl;

       return response_code;
}

/*
*   ERROR_CODE Mtrd24FeedToPosition(long position_in_steps);
*
*   Brief:  Executes FeedToPosition (absolute)move command via RS485. See motor manual pg.57. 
*           Motor will use current Accl, Decel, Vel parameters.
*           Passed in position_in_steps will be used as position. If position_in_steps is 0,
*           then motor will use the last value used for distance. 
*   Return: Response code indicating request result. If no error,
*           Motor will move and stop according to passed in parameters.
*
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24FeedToPosition(long position_in_steps)
{
    ERROR_CODE result;
    TRAJECTORY parameters;
    QString    receive_data;

    //load position
    parameters.distance = position_in_steps;
    
    //Build command 
    buildCommand(EXECUTE_FEED_TO_POSITION_COMMAND, parameters);
    result = sendCommand();
    if(result != NO_ERROR)
    {
        return result;
    }

    /*
    *   Get Motor's response
    */
    result = receiveResponse(&receive_data);

    std::cerr << "buildCommand(EXECUTE_FEED_TO_POSITION_COMMAND, parameters); = " << receive_data.toStdString() << std::endl;

    return result;
}


/*
*   ERROR_CODE setHomingTrajectoryParameters(void)
*
*   Brief:  Sends Homing trajectory parameters to motor drive via RS485. 
*   Return: Response code indicating request result. 
*
*/
ERROR_CODE mtrd24MotorDrive::setHomingTrajectoryParameters(void)
{
    ERROR_CODE response_code;
    ERROR_CODE send_code;
    ERROR_CODE result;
    TRAJECTORY parameters;
    QString    receive_data;

    //set accl rate rev/sec
    result = setMtrd24AcclRate( 2 );
    if(result != NO_ERROR)
    {
        return result;
    }

    //set decel rate rev/sec
    result = setMtrd24DecelRate( 2 );
    if(result != NO_ERROR)
    {
        return result;
    }

    //set move dist in steps
    result = setMtrd24MoveDistance( 1 );
    if(result != NO_ERROR)
    {
        return result;
    }

    //set velocity in revs/sec
    result = setMtrd24Velocity( 2 );

    return result;
}

/*
*   ERROR_CODE Mtrd24FindHome(void)
*
*   Brief:  Executes Weight Stack homing routine. It stablishes home position. 
*           If motor is already at home, it will first move away from home, and then find home. 
*   Return: Response code indicating request result. If no error,
*           Motor will move and stop at home position.
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24FindHome(void)
{
    QString         response_data;
    DRIVE_STATUS    motor_status;
    ERROR_CODE      result;
    bool            home;
    long            steps;
    double             d;

    //clear home variable
    home_position_found = false;

    //Set trajectory parmeters
    result = setHomingTrajectoryParameters();
    if(result != NO_ERROR)
    {
        return result;
    }

    //get home sensor state
    result = isMtrd24AtHome(&home);
    if(result != NO_ERROR)
    {
        return result;
    }

    if(home == false)
    {
        std::cerr << "Motor is Not Home " << std::endl;
        //result = Mtrd24FeedToSensor(HOME_INPUT, LOW);
        result = Mtrd24SeekHome(HOME_INPUT, LOW);
        if(result != NO_ERROR)
        {
            return result;
        }   

        //request status until move is complete
        result = Mtrd24WaitUntilMotionComplete();        
        if(result != NO_ERROR)
        {
            return result;
        }

    }
    else
    {
        std::cerr << "Motor is Home, move away 1st than rehome " << std::endl;       
        result = Mtrd24FeedToDistance(25000);
        if(result != NO_ERROR)
        {
            return result;
        }

        //request status until move is complete
        result = Mtrd24WaitUntilMotionComplete();
        if(result != NO_ERROR)
        {
            return result;
        }

        std::cerr << "Motor is done moving away! Rehoming NOW!! " << std::endl;

        result = result = Mtrd24SeekHome(HOME_INPUT, LOW);
        if(result != NO_ERROR)
        {
            return result;
        }

        //request status until move is complete
        result = Mtrd24WaitUntilMotionComplete();        
        if(result != NO_ERROR)
        {
            return result;
        }
        
    }

    result = setMtrd24ZeroPosition();
    std::cerr << "Motor is Homing is Complete!! " << std::endl;

    home_position_found = true;

    return result;
    
}

/*
*   bool isMtrd24HomePositionDetermined(void)
*
*   Brief:  Function returns whether Motor home position has been determined. 
*           Home position is stablished by calling Mtrd24FindHome()
*/
bool mtrd24MotorDrive::isMtrd24HomePositionDetermined(void)
{
    return home_position_found;
}

/*
*   ERROR_CODE Mtrd24ReturnHome(void)
*
*   Brief:  Function sends drive commnand to move to home position . Assumes that Mtrd24FindHome()
*           has been previously called since that function determines the home position.
*   Return: Response code indicating request result. If no error,
*           Motor will move and stop at home position.
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24ReturnHome(void)
{
    QString         response_data;
    long            encoder_position;
    ERROR_CODE      result;
    

    std::cerr << "Mtrd24ReturnHome " << std::endl;

    //back home
    result = Mtrd24FeedToPosition(0);
    if(result != NO_ERROR)
    {
        return result;
    }

    //request status until move is complete
    result = Mtrd24WaitUntilMotionComplete();        
    if(result != NO_ERROR)
    {
        return result;
    }

    result = requestMtrd24EncoderPosition(&response_data);
    if(result != NO_ERROR)
    {
        return result;
    }
    
    //validate position
    response_data.remove(0,4);//remove header data
    encoder_position = response_data.toLong();
    std::cerr << "Encoder Pos = " << encoder_position << std::endl;
    std::cerr << "Encoder  - Limit = " << (long)(-MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    std::cerr << "Encoder  + Limit = " << (long)( MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    if(encoder_position < (long)(-MTRD24_ENCODER_PULSE_TOLERANCE) ||
       encoder_position > (long)( MTRD24_ENCODER_PULSE_TOLERANCE))
    {
        return MOTOR_STALL;     
    }

    return result;

}


/*
*   ERROR_CODE setMtrd24AcclRate(SENSE_CONDITION condition)
*
*   Brief:  Function sends Travel limints sense inputs to motor drive via RS485. 
*   Return: Response code indicating send result. 
*
*/
ERROR_CODE mtrd24MotorDrive::setMtrd24TravelLimits(SENSE_CONDITION condition)
{
    TRAJECTORY parameters;
    QString    receive_data;
    ERROR_CODE response_code;
    ERROR_CODE send_code;      
    
    parameters.travel_limits_input_condition = condition;

    //Build command to send accl
    buildCommand(SET_TRAVEL_LIMITS_COMMAND, parameters);
    send_code = sendCommand();

    if(send_code != NO_ERROR)
    {
        return send_code;
    }

    /*
    *   Get Motor's response
    */
    response_code = receiveResponse(&receive_data);

    std::cerr << "buildCommand(SET_TRAVEL_LIMITS_COMMAND, parameters); = " << receive_data.toStdString() << std::endl;

    return response_code;
}

/*
*   ERROR_CODE setMtrd24AcclRate(unsigned int revs_per_sec_rate)
*
*   Brief:  Function sends Accl rate to motor drive via RS485. 
*   Return: Response code indicating send result. 
*
*/
ERROR_CODE mtrd24MotorDrive::setMtrd24AcclRate(unsigned int revs_per_sec_rate)
{
    TRAJECTORY parameters;
    QString    receive_data;
    ERROR_CODE response_code;
    ERROR_CODE send_code;      
    
    parameters.accl_rate = revs_per_sec_rate;

    //Build command to send accl
    buildCommand(SET_ACCL_COMMAND, parameters);
    send_code = sendCommand();

    if(send_code != NO_ERROR)
    {
        return send_code;
    }

    /*
    *   Get Motor's response
    */
    response_code = receiveResponse(&receive_data);

    std::cerr << "buildCommand(SET_ACCL_COMMAND, parameters); = " << receive_data.toStdString() << std::endl;

    return response_code;
}

/*
*   ERROR_CODE setMtrd24DecelRate(unsigned int revs_per_sec_rate)
*
*   Brief:  Function sends Decel rate to motor drive via RS485. 
*   Return: Response code indicating send result. 
*
*/
ERROR_CODE mtrd24MotorDrive::setMtrd24DecelRate(unsigned int revs_per_sec_rate)
{
    TRAJECTORY parameters;
    QString    receive_data;
    ERROR_CODE response_code;
    ERROR_CODE send_code;
       
    
    parameters.decel_rate = revs_per_sec_rate;

    //Build command to Get Motor Status
    buildCommand(SET_DECEL_COMMAND, parameters);
    send_code = sendCommand();

    if(send_code != NO_ERROR)
    {
        return send_code;
    }

    /*
    *   Get Motor's response
    */
    response_code = receiveResponse(&receive_data);

    std::cerr << "buildCommand(SET_DECEL_COMMAND, parameters); = " << receive_data.toStdString() << std::endl;

    return response_code;

}

/*
*   ERROR_CODE setMtrd24MoveDistance(long distance_in_steps);
*
*   Brief:  Function sends Move Distance in motor steps to motor drive via RS485. 
*   Return: Response code indicating send result. 
*
*/
ERROR_CODE mtrd24MotorDrive::setMtrd24MoveDistance(long distance_in_steps)
{
    TRAJECTORY parameters;
    QString    receive_data;
    ERROR_CODE response_code;
    ERROR_CODE send_code;
       
    
    parameters.distance = distance_in_steps;

    //Build command to Get Motor Status
    buildCommand(SET_DISTANCE_COMMAND, parameters);
    send_code = sendCommand();

    if(send_code != NO_ERROR)
    {
        return send_code;
    }

    /*
    *   Get Motor's response
    */
    response_code = receiveResponse(&receive_data);

    std::cerr << "buildCommand(SET_DISTANCE_COMMAND, parameters); = " << receive_data.toStdString() << std::endl;

    return response_code;

}

/*
*   ERROR_CODE setMtrd24Velocity(unsigned int revs_per_sec);
*
*   Brief:  Function sends Motor Speed in revs per sec to motor drive via RS485. 
*   Return: Response code indicating send result. 
*
*/
ERROR_CODE mtrd24MotorDrive::setMtrd24Velocity(unsigned int revs_per_sec)
{
    TRAJECTORY parameters;
    QString    receive_data;
    ERROR_CODE response_code;
    ERROR_CODE send_code;
       
    
    //set velocity in rev/sec
    parameters.velocity = revs_per_sec;

    //Build command to Get Motor Status
    buildCommand(SET_VELOCITY_COMMAND, parameters);
    send_code = sendCommand();

    if(send_code != NO_ERROR)
    {
        return send_code;
    }

    /*
    *   Get Motor's response
    */
    response_code = receiveResponse(&receive_data);

    std::cerr << "buildCommand(SET_VELOCITY_COMMAND, parameters); = " << receive_data.toStdString() << std::endl;

    return response_code;

}

/*
*   ERROR_CODE Mtrd24WaitUntilMotionComplete(void);
*
*   Brief:  Function will continously request Motor Status
*           until "in motion" flags are cleared.
*   Return: Response code indicating result. 
*
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24WaitUntilMotionComplete(void)
{
    QString         response_data;
    ERROR_CODE      result;
    DRIVE_STATUS    motor_status;

    //request status until move is complete
    do
    {
        result = requestMtrd24Status(&response_data);
        if(result != NO_ERROR)
        {
            return result;
        }

        motor_status = decodeMtrd24StatusMsg(response_data);
        
    } while (motor_status.in_motion_from_any_command == true);

    /*
    *   check for errors
    */
    if(motor_status.drive_faulted == true || motor_status.alarm_code_present == true || motor_status.drive_disabled == true)
    {
        result = MOTOR_DRIVE_ERROR_PRESENT;
    }

    return result;

}

/*
*   ERROR_CODE Mtrd24TestDrive(void)
*
*   Brief:  Executes Weight Stack Test routine. First it homes the motor then,
*           it drops the three 5kg weights and then returns home.
*   Return: Response code indicating result. 
*
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24TestDrive(void)
{
    QString         response_data;
    DRIVE_STATUS    motor_status;
    ERROR_CODE      result;
    bool            home;

    //home the rig
    result = Mtrd24FindHome();
    if(result != NO_ERROR)
    {
        return result;
    }

    result = Mtrd24DropFirst5KgAtLevel();
    if(result != NO_ERROR)
    {
        return result;
    }

    result = Mtrd24DropSecond5Kg();
    if(result != NO_ERROR)
    {
        return result;
    }

    result = Mtrd24DropThird5Kg();
    if(result != NO_ERROR)
    {
        return result;
    }

    result = Mtrd24ReturnHome();

    return result;
}

/*
*   ERROR_CODE setMtrd24ZeroPosition(void)
*
*   Brief:  Function sends command to set current encoder position to 0.
*   Return: Response code indicating request result. If no error,
*
*/
ERROR_CODE mtrd24MotorDrive::setMtrd24ZeroPosition(void)
{
    QString         receive_data;
    TRAJECTORY      parameters;
    ERROR_CODE      result;
    

    //use distance parameter to set position
    parameters.distance = 0;

    //Build command to set Encoder position
    buildCommand(SET_ENCODER_POSITION, parameters);
    result = sendCommand();
    if(result != NO_ERROR)
    {
        return result;
    }

    /*
    *   Get Motor's response
    */
    result = receiveResponse(&receive_data);
    std::cerr << "buildCommand(SET_ENCODER_POSITION, parameters); = " << receive_data.toStdString() << std::endl;
    if(result != NO_ERROR)
    {
        return result;
    }
    

    //Build command to set Motor position
    buildCommand(SET_MOTOR_POSITION, parameters);
    result = sendCommand();
    if(result != NO_ERROR)
    {
        return result;
    }

    /*
    *   Get Motor's response
    */
    result = receiveResponse(&receive_data);
    std::cerr << "buildCommand(SET_MOTOR_POSITION, parameters); = " << receive_data.toStdString() << std::endl;

    return result;
     
}

/*
*   ERROR_CODE requestMtrd24EncoderPosition(QString position)
*
*   Brief:  Requests Motor's encoder position via RS485. 
*   Return: Response code indicating request result. If no error,
*           string will contain encoder value.
*
*/
ERROR_CODE mtrd24MotorDrive::requestMtrd24EncoderPosition(QString *position)
{
    ERROR_CODE result;
    TRAJECTORY parameters;

    //Build command to Get Encoder value
    buildCommand(GET_ENCODER_POSITION, parameters);
    result = sendCommand();

    if(result != NO_ERROR)
    {
        return result;
    }


    /*
    *   Get Motor's response
    */
    result = receiveResponse(position);

    return result;
}

/*
*   ERROR_CODE requestMtrd24MotorPosition(QString position)
*
*   Brief:  Requests Motor's absolute position via RS485. 
*   Return: Response code indicating request result. If no error,
*           string will contain absolute position value.
*
*/
ERROR_CODE mtrd24MotorDrive::requestMtrd24MotorPosition(QString *position)
{
    ERROR_CODE result;
    TRAJECTORY parameters;

    //Build command to Get Encoder value
    buildCommand(GET_MOTOR_POSITION, parameters);
    result = sendCommand();

    if(result != NO_ERROR)
    {
        return result;
    }


    /*
    *   Get Motor's response
    */
    result = receiveResponse(position);

    return result;
}

/*
*   ERROR_CODE Mtrd24DropFirst5KgAtTilt(void)
*
*   Brief:  Requests Weight Stack Motor to drop the 1st 5Kg via RS485. 
*           Assumes Load Cell is at a tilt position.
*   Return: Response code indicating request result. Assumes Motor current
*           position is at home.
*
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24DropFirst5KgAtTilt(void)
{
    ERROR_CODE  result;
    TRAJECTORY  parameters;
    QString     response_data;
    long        encoder_position;

    std::cerr << "Mtrd24DropFirst5Kg(void) " << encoder_position << std::endl;

    //initial 5kg
    result = Mtrd24FeedToPosition(DISTANCE_STEPS_TO_DROP_FIRST_5KG_AT_TILT);
    if(result != NO_ERROR)
    {
        return result;
    }

    //request status until move is complete
    result = Mtrd24WaitUntilMotionComplete();        
    if(result != NO_ERROR)
    {
        return result;
    }

    //Ok, move is done. Get encoder position
    result = requestMtrd24EncoderPosition(&response_data);    
    if(result != NO_ERROR)
    {
        return result;
    }
    
    //validate position
    response_data.remove(0,4);
    encoder_position = response_data.toLong();
    std::cerr << "Encoder Pos = " << encoder_position << std::endl;
    std::cerr << "Encoder  - Limit = " << (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT - MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    std::cerr << "Encoder  + Limit = " << (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    if(encoder_position < (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT - MTRD24_ENCODER_PULSE_TOLERANCE) ||
       encoder_position > (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + MTRD24_ENCODER_PULSE_TOLERANCE))
    {
        return MOTOR_STALL;     
    }
    
    return result;

}

/*
*   ERROR_CODE Mtrd24DropFirst5KgAtLevel(void)
*
*   Brief:  Requests Weight Stack Motor to drop the 1st 5Kg via RS485. 
*           Assumes Load Cell is at a level position.
*   Return: Response code indicating request result. Assumes Motor current
*           position is at home.
*
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24DropFirst5KgAtLevel(void)
{
    ERROR_CODE  result;
    TRAJECTORY  parameters;
    QString     response_data;
    long        encoder_position;

    std::cerr << "Mtrd24DropFirst5Kg(void) " << encoder_position << std::endl;

    //initial 5kg
    result = Mtrd24FeedToPosition(DISTANCE_STEPS_TO_DROP_FIRST_5KG_AT_LEVEL);
    if(result != NO_ERROR)
    {
        return result;
    }

    //request status until move is complete
    result = Mtrd24WaitUntilMotionComplete();        
    if(result != NO_ERROR)
    {
        return result;
    }

    //Ok, move is done. Get encoder position
    result = requestMtrd24EncoderPosition(&response_data);    
    if(result != NO_ERROR)
    {
        return result;
    }
    
    //validate position
    response_data.remove(0,4);
    encoder_position = response_data.toLong();
    std::cerr << "Encoder Pos = " << encoder_position << std::endl;
    std::cerr << "Encoder  - Limit = " << (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_LEVEL - MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    std::cerr << "Encoder  + Limit = " << (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_LEVEL + MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    if(encoder_position < (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_LEVEL - MTRD24_ENCODER_PULSE_TOLERANCE) ||
       encoder_position > (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_LEVEL + MTRD24_ENCODER_PULSE_TOLERANCE))
    {
        return MOTOR_STALL;     
    }
    
    return result;

}

/*
*   ERROR_CODE Mtrd24DropSecond5Kg(void)
*
*   Brief:  Requests Weight Stack Motor to drop the 2nd 5Kg via RS485. 
*   Return: Response code indicating request result. Assumes Motor current
*           position has already dropped the first 5kg.
*
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24DropSecond5Kg(void)
{
    ERROR_CODE  result;
    TRAJECTORY  parameters;
    QString     response_data;
    long        encoder_position;

    std::cerr << "Mtrd24DropSecond5Kg(void) " << encoder_position << std::endl;
    
    //initial 5kg has already been dropped
    result = Mtrd24FeedToDistance(DISTANCE_STEPS_TO_DROP_NEXT_5KG);
    if(result != NO_ERROR)
    {
        return result;
    }

    //request status until move is complete
    result = Mtrd24WaitUntilMotionComplete();        
    if(result != NO_ERROR)
    {
        return result;
    }

    //Ok, move is done. Get encoder position
    result = requestMtrd24EncoderPosition(&response_data);
    if(result != NO_ERROR)
    {
        return result;
    }
    
    //validate position
    response_data.remove(0,4);
    encoder_position = response_data.toLong();
    std::cerr << "Encoder Pos = " << encoder_position << std::endl;
    std::cerr << "Encoder  - Limit = " << (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG - MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    std::cerr << "Encoder  + Limit = " << (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG + MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    if(encoder_position < (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG - MTRD24_ENCODER_PULSE_TOLERANCE) ||
       encoder_position > (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG + MTRD24_ENCODER_PULSE_TOLERANCE))
    {
        return MOTOR_STALL;     
    }
    
    return result;

}

/*
*   ERROR_CODE Mtrd24DropThird5Kg(void)
*
*   Brief:  Requests Weight Stack Motor to drop the 3rd 5Kg via RS485. 
*   Return: Response code indicating request result. Assumes Motor current
*           position has already dropped the first and second 5kg weights.
*
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24DropThird5Kg(void)
{
    ERROR_CODE  result;
    TRAJECTORY  parameters;
    QString     response_data;
    long        encoder_position;

    std::cerr << "Mtrd24DropThird5Kg(void) " << encoder_position << std::endl;
    
    //1st and second 5kg has already been dropped
    result = Mtrd24FeedToDistance(DISTANCE_STEPS_TO_DROP_NEXT_5KG);
    if(result != NO_ERROR)
    {
        return result;
    }

    //request status until move is complete
    result = Mtrd24WaitUntilMotionComplete();        
    if(result != NO_ERROR)
    {
        return result;
    }

    //Ok, move is done. Get encoder position
    result = requestMtrd24EncoderPosition(&response_data);
    if(result != NO_ERROR)
    {
        return result;
    }
    
    //validate position
    response_data.remove(0,4);
    encoder_position = response_data.toLong();
    std::cerr << "Encoder Pos = " << encoder_position << std::endl;
    std::cerr << "Encoder  - Limit = " << (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG - MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    std::cerr << "Encoder  + Limit = " << (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG + MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    if(encoder_position < (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG - MTRD24_ENCODER_PULSE_TOLERANCE) ||
       encoder_position > (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG + MTRD24_ENCODER_PULSE_TOLERANCE))
    {
        return MOTOR_STALL;     
    }
    
    return result;

}

/*
*   ERROR_CODE Mtrd24DropSecondAndThird5Kg(void)
*
*   Brief:  Requests Weight Stack Motor to drop the 2nd and 3rd 5Kg weights via RS485. 
*   Return: Response code indicating request result. Assumes Motor current
*           position has already dropped the first 5kg weight.
*
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24DropSecondAndThird5Kg(void)
{
    ERROR_CODE  result;
    TRAJECTORY  parameters;
    QString     response_data;
    long        encoder_position;

    std::cerr << "Mtrd24DropSecondAndThird5Kg(void) " << encoder_position << std::endl;
    
    //initial 5kg has already been dropped
    result = Mtrd24FeedToDistance(DISTANCE_STEPS_TO_DROP_NEXT_5KG + DISTANCE_STEPS_TO_DROP_NEXT_5KG);
    if(result != NO_ERROR)
    {
        return result;
    }

    //request status until move is complete
    result = Mtrd24WaitUntilMotionComplete();        
    if(result != NO_ERROR)
    {
        return result;
    }

    //Ok, move is done. Get encoder position
    result = requestMtrd24EncoderPosition(&response_data);
    if(result != NO_ERROR)
    {
        return result;
    }
    
    //validate position
    response_data.remove(0,4);
    encoder_position = response_data.toLong();
    std::cerr << "Encoder Pos = " << encoder_position << std::endl;
    std::cerr << "Encoder  - Limit = " << (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG - MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    std::cerr << "Encoder  + Limit = " << (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG + MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    if(encoder_position < (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG - MTRD24_ENCODER_PULSE_TOLERANCE) ||
       encoder_position > (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG + MTRD24_ENCODER_PULSE_TOLERANCE))
    {
        return MOTOR_STALL;     
    }
    
    return result;

}

/*
*   ERROR_CODE Mtrd24Drop15KgAtLevel(void)
*
*   Brief:  Requests Weight Stack Motor to drop all three 5Kg weights via RS485. 
*   Return: Response code indicating request result. Assumes Motor current
*           position is at home.
*
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24Drop15KgAtLevel(void)
{
    ERROR_CODE  result;
    TRAJECTORY  parameters;
    QString     response_data;
    long        encoder_position;

    std::cerr << "Mtrd24Drop15Kg(void) " << encoder_position << std::endl;
    
    //assumes initial positon is HOME 
    result = Mtrd24FeedToPosition(DISTANCE_STEPS_TO_DROP_FIRST_5KG_AT_LEVEL + DISTANCE_STEPS_TO_DROP_NEXT_5KG + DISTANCE_STEPS_TO_DROP_NEXT_5KG);
    if(result != NO_ERROR)
    {
        return result;
    }

    //request status until move is complete
    result = Mtrd24WaitUntilMotionComplete();        
    if(result != NO_ERROR)
    {
        return result;
    }

    //Ok, move is done. Get encoder position
    result = requestMtrd24EncoderPosition(&response_data);
    if(result != NO_ERROR)
    {
        return result;
    }
    
    //validate position
    response_data.remove(0,4);
    encoder_position = response_data.toLong();
    std::cerr << "Encoder Pos = " << encoder_position << std::endl;
    std::cerr << "Encoder  - Limit = " << (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_LEVEL + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG - MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    std::cerr << "Encoder  + Limit = " << (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_LEVEL + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG + MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    if(encoder_position < (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_LEVEL + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG - MTRD24_ENCODER_PULSE_TOLERANCE) ||
       encoder_position > (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_LEVEL + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG + MTRD24_ENCODER_PULSE_TOLERANCE))
    {
        return MOTOR_STALL;     
    }
    
    return result;

}

/*
*   ERROR_CODE Mtrd24Drop15KgAtTilt(void)
*
*   Brief:  Requests Weight Stack Motor to drop all three 5Kg weights via RS485. 
*   Return: Response code indicating request result. Assumes Motor current
*           position is at home.
*
*/
ERROR_CODE mtrd24MotorDrive::Mtrd24Drop15KgAtTilt(void)
{
    ERROR_CODE  result;
    TRAJECTORY  parameters;
    QString     response_data;
    long        encoder_position;

    std::cerr << "Mtrd24Drop15Kg(void) " << encoder_position << std::endl;
    
    //assumes initial positon is HOME 
    result = Mtrd24FeedToPosition(DISTANCE_STEPS_TO_DROP_FIRST_5KG_AT_TILT + DISTANCE_STEPS_TO_DROP_NEXT_5KG + DISTANCE_STEPS_TO_DROP_NEXT_5KG);
    if(result != NO_ERROR)
    {
        return result;
    }

    //request status until move is complete
    result = Mtrd24WaitUntilMotionComplete();        
    if(result != NO_ERROR)
    {
        return result;
    }

    //Ok, move is done. Get encoder position
    result = requestMtrd24EncoderPosition(&response_data);
    if(result != NO_ERROR)
    {
        return result;
    }
    
    //validate position
    response_data.remove(0,4);
    encoder_position = response_data.toLong();
    std::cerr << "Encoder Pos = " << encoder_position << std::endl;
    std::cerr << "Encoder  - Limit = " << (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG - MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    std::cerr << "Encoder  + Limit = " << (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG + MTRD24_ENCODER_PULSE_TOLERANCE) << std::endl;
    if(encoder_position < (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG - MTRD24_ENCODER_PULSE_TOLERANCE) ||
       encoder_position > (long)(ENCODER_PULSES_TO_DROP_FIRST_5KG_AT_TILT + ENCODER_PULSES_TO_DROP_NEXT_5KG + ENCODER_PULSES_TO_DROP_NEXT_5KG + MTRD24_ENCODER_PULSE_TOLERANCE))
    {
        return MOTOR_STALL;     
    }
    
    return result;

}