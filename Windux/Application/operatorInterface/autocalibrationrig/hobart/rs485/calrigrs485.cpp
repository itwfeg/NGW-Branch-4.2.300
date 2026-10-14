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
#include <QMutex>

#if defined ( __linux__ )		// Windows 
#include <unistd.h>
#include <sys/ioctl.h>
#endif

#include "common.h"
#include <string.h>

#include <math.h>
#include <stdint.h>

#include "hobart/rs485/calrigrs485.h"



// BCM board
//const std::string RS485Manager::UART = "/dev/ttymxc3";
// ARA / ARK board
const std::string RS485Manager::UART = "/dev/ttymxc4";

extern QMutex g_serial_mutex;


RS485Manager::RS485Manager()
{
    myInt = 0;

#if defined ( __linux__ )

    //open port
    tty_fd = open( UART.data(), O_RDWR );

    if( tty_fd < 0 ) {
        std::cerr << "RS485 Failed to open( UART.data(), O_RDWR )" << tty_fd << std::endl;
        return;
    }

    std::cerr << "RS485 Opened" << tty_fd << std::endl;

    //get the UART configuration
    if ( tcgetattr( tty_fd, &uart_config ) < 0 ) {
        std::cerr << "RS485 failed to get UART configuration, tcgetattr( tty_fd, &uart_config )" << std::endl;
        return;
    }

    //set up UART for _115200_BAUD 8n1 transmit rate
    uart_config.c_oflag = 0;
    uart_config.c_iflag = 0;
    uart_config.c_cflag |= CS8|CLOCAL|CREAD;
    uart_config.c_lflag &= ~(ICANON);
    uart_config.c_lflag &= ~(ECHO);
    uart_config.c_cc[VTIME] = 200;
    uart_config.c_cc[VMIN] = 0;

    if ( cfsetspeed( &uart_config, _115200_BAUD ) < 0 ) {
        std::cerr << "RS485 failed to Set UART configuration, cfsetspeed( &uart_config, _115200_BAUD )" << std::endl;
        return;
    }

    //remove the current configuration
    if ( tcflush(tty_fd, TCIOFLUSH) < 0) {
        std::cerr << "RS485 failed to Flush UART, tcflush(tty_fd, TCIOFLUSH)" << std::endl;
        return;
    }

    //set the new configuration
    if ( tcsetattr( tty_fd, TCSANOW,  &uart_config ) < 0 ) {
        std::cerr << "RS485 failed to Set UART Attr, tcsetattr( tty_fd, TCSANOW,  &uart_config )" << std::endl;
        return;
    }

    struct serial_rs485 rs485;
    int _cl_rs485_delay = 0;

    // enable/disable rs485 direction control
    if ( ioctl(tty_fd, TIOCGRS485, &rs485) < 0) {
        std::cerr << "RS485 failed to Get RS485 MODE, ioctl(tty_fd, TIOCGRS485, &rs485)  )" << std::endl;
        return;
    }

    // Enable RS485 mode:
    rs485.flags |= SER_RS485_ENABLED;

    // Set logical level for RTS pin equal to 1 when sending:
    rs485.flags |= SER_RS485_RTS_ON_SEND;
    // or, set logical level for RTS pin equal to 0 when sending:
    //rs485.flags &= ~(SER_RS485_RTS_ON_SEND);

    // Set logical level for RTS pin equal to 1 after sending:
    //rs485.flags |= SER_RS485_RTS_AFTER_SEND;
    // or, set logical level for RTS pin equal to 0 after sending:
    rs485.flags &= ~(SER_RS485_RTS_AFTER_SEND);

    // Set rts delay before send, if needed:
    rs485.delay_rts_before_send = _cl_rs485_delay;

    // Set rts delay after send, if needed:
    rs485.delay_rts_after_send = _cl_rs485_delay;

    // Set this flag if you want to receive data even while sending data
    rs485.flags |= SER_RS485_RX_DURING_TX;

    if ( ioctl(tty_fd, TIOCSRS485, &rs485) < 0) {
        std::cerr << "RS485 failed to Set RS485 MODE, ioctl(tty_fd, TIOCSRS485, &rs485)  )" << std::endl;
        return;
    }

    std::cerr << "RS485 Config Init Done, tty_fd = )" << tty_fd << std::endl << std::endl;

#endif
}

RS485Manager::~RS485Manager()
{
#if defined ( __linux__ )

    //close my interface
    if ( 0 <= tty_fd ) {
        if ( close( tty_fd ) < 0 ) {
            std::cerr << "RS485Manager.Error ~RS485Manager(): failed to close RS485 handle." << std::endl;
        }
    }

#endif
}


#if 0
RS485Manager::RS485Manager()
{
    myInt = 0;

#if defined ( __linux__ )		// Windows 
	//open port
    tty_fd = open( UART.data(), O_RDWR | O_NONBLOCK  );
	//tty_fd = open( UART.data(), O_RDWR );
    if( tty_fd < 0 ) {
		std::cerr << "RS485 Failed to open( UART.data(), O_RDWR | O_NONBLOCK  );" <<tty_fd << std::endl;
    } else {
		std::cerr << "RS485 Opened( UART.data(), O_RDWR | O_NONBLOCK  );" <<tty_fd << std::endl;
        
		//get the UART configuration
        if( tcgetattr( tty_fd, &uart_config ) < 0 ) {
            //Debug::trace( "RS485Manager.init", "failed to get UART configuration." );
			std::cerr << "RS485 failed to get UART configuration, tcgetattr( tty_fd, &uart_config )" << std::endl;
        } else {
            
            //set up UART for _115200_BAUD 8n1 transmit rate
            uart_config.c_oflag = 0;
            uart_config.c_iflag = 0;
            uart_config.c_cflag |= CS8|CLOCAL|CREAD;
			uart_config.c_lflag &= ~(ICANON);
			uart_config.c_lflag &= ~(ECHO);
			uart_config.c_cc[VTIME] = 200;
			uart_config.c_cc[VMIN] = 0;


			if( cfsetspeed( &uart_config, _115200_BAUD ) < 0 ) {
                //Debug::trace( "RS485Manager.init", "failed to set the new UART speed." );
				std::cerr << "RS485 failed to Set UART configuration, cfsetspeed( &uart_config, _115200_BAUD )" << std::endl;
            } else {
                //remove the current configuration
                if( tcflush(tty_fd, TCIOFLUSH) < 0) {
					std::cerr << "RS485 failed to Flush UART, tcflush(tty_fd, TCIOFLUSH)" << std::endl;
                } else {
                    //set the new configuration
                    if( tcsetattr( tty_fd, TCSANOW,  &uart_config ) < 0 ) {
						std::cerr << "RS485 failed to Set UART Attr, tcsetattr( tty_fd, TCSANOW,  &uart_config )" << std::endl;
                    } else {
						struct serial_rs485 rs485;
						int _cl_rs485_delay = 0;

						/* enable/disable rs485 direction control */
						if(ioctl(tty_fd, TIOCGRS485, &rs485) < 0) {
							if (_cl_rs485_delay >= 0) {
								/* error could be because hardware is missing rs485 support so only print when actually trying to activate it */
								//Debug::trace("RS485Manager.init", "Error 0, setting RS-485 mode " );
								std::cerr << "RS485 failed to Get RS485 MODE, ioctl(tty_fd, TIOCGRS485, &rs485)  )" << std::endl;
							}
						} else if (_cl_rs485_delay >= 0) {
#if 1
							rs485.flags |= SER_RS485_ENABLED | SER_RS485_RTS_ON_SEND;
							rs485.flags &= ~(SER_RS485_RTS_AFTER_SEND);
							rs485.flags &= ~(SER_RS485_RX_DURING_TX);
							rs485.delay_rts_after_send = _cl_rs485_delay;
							rs485.delay_rts_before_send = _cl_rs485_delay;//0
#else
                            //rs485.flags |= SER_RS485_ENABLED;
							//rs485.flags &= ~(SER_RS485_RTS_ON_SEND);
							//rs485.flags |= SER_RS485_RTS_AFTER_SEND;
							//rs485.flags |= SER_RS485_RX_DURING_TX;
							//rs485.delay_rts_after_send = _cl_rs485_delay;
							//rs485.delay_rts_before_send = 0;
							std::cerr << "RS485 Set Flags Unchanged" << std::endl;

#endif
							if(ioctl(tty_fd, TIOCSRS485, &rs485) < 0) {
								//perror("Error setting RS-485 mode");
								//Debug::trace("RS485Manager.init", "Error 1, setting RS-485 mode " );
								std::cerr << "RS485 failed to Set RS485 MODE, ioctl(tty_fd, TIOCSRS485, &rs485)  )" << std::endl;
							}
						} else {
							rs485.flags &= ~(SER_RS485_ENABLED | SER_RS485_RTS_ON_SEND | SER_RS485_RTS_AFTER_SEND);
							rs485.delay_rts_after_send = 0;
							rs485.delay_rts_before_send = 0;
							if(ioctl(tty_fd, TIOCSRS485, &rs485) < 0) {
								//Debug::trace("RS485Manager.init", "Error 2 setting RS-485 mode " );
								std::cerr << "RS485 failed to Set RS485 MODE, ioctl(tty_fd, TIOCSRS485, &rs485) )" << std::endl;
							}
							std::cerr << "RS485 Set Flags 2" << std::endl;
						}

                        std::cerr << "RS485 Config Init Done, tty_fd = )" << tty_fd << std::endl << std::endl;

                    }
                }
            }
        }
    }
#endif

}

RS485Manager::~RS485Manager()
{
#if defined ( __linux__ )
	//close my interface
	if( tty_fd > 0 ) {
		if(close( tty_fd ) < 0) {
			std::cerr << "RS485Manager.Error ~RS485Manager(): failed to close RS485 handle." << std::endl;
		}
	}
#endif 
}
#endif

int RS485Manager::RS485Send(unsigned char *buffer, int length)
{
	int nBytes;
    
    {
        QMutexLocker guard(&g_serial_mutex);
        
    
	    nBytes = write( tty_fd, buffer, length );
    }
	//tcdrain( tty_fd );
	
    //std::cerr << "nBytes = write( rs485.get_tty_fd(), &payload[0], size ) = ;" << nBytes << std::endl;

	return nBytes;
}

int RS485Manager::RS485ReadBytes(unsigned char *buffer, int length)
{
	int nBytes;

	nBytes = read( tty_fd, buffer, length );
	
    std::cerr << "nBytes = read( tty_fd, buffer, length ) = ;" << nBytes << std::endl;

	if(nBytes > 0)
    {
        unsigned char myChar;

        for(int i=0; i<nBytes; i++)
        {
            myChar = buffer[i];
            std::cerr << '[' << myChar << ']' << ',' << std::endl;
        }
    }

	return nBytes;
}

int RS485Manager::RS485ReadLine(unsigned char *buffer)
{
	int nBytes, totalBytes = 0;
	unsigned char myChar;

    //Clear passed in buffer
    memset(buffer, 0, RS485_MAX_LINE_LENGTH);

	nBytes = read( tty_fd, &myChar, 1 );

	while(nBytes > 0)
	{
		std::cerr << myChar << std::endl;
		
		totalBytes ++;

		*buffer = myChar;
		buffer ++;

		if(myChar == CR_CHARACTER || totalBytes == RS485_MAX_LINE_LENGTH)
		{
			//end of line reached
			break;
		}
		else
		{
			nBytes = read( tty_fd, &myChar, 1 );
		}
	}
	
    std::cerr << "int RS485Manager::RS485ReadLine(unsigned char *buffer) = " << totalBytes << std::endl;

	return totalBytes;
}

