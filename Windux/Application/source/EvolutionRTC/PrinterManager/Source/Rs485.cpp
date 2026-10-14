/*
 * RS485.cpp
 *
 *  Created on: May 20, 2015
 *      Author: aaron
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "Rs485.h"
#include "Debug.h"


RS485 *RS485::pInstance = 0;
const std::string RS485::UART = "/dev/ttymxc3";

RS485::RS485() {
	Util::CLockCriticalSection lock( initLock );
	interfaceInitialized = false;

	/* open port for writes only */
	int tty_fd = open( UART.data(), O_WRONLY | O_NONBLOCK  );
	if(tty_fd < 0) {
		Debug::trace( "RS485.init", "failed to open Uart: " + UART );
	} else {
		/* get the uart configuration */
		if(tcgetattr( tty_fd, &uart_config ) < 0) {
			Debug::trace( "RS485.init", "failed to get Uart configuration." );
		} else {
			/* setup UART for 3Mbps 8n1 transmit rate */
			uart_config.c_oflag = 0;
			uart_config.c_iflag = 0;
			uart_config.c_cflag = CS8|CLOCAL;

			/* set the baud rate to 3Mbps synchronous */
			if( cfsetspeed( &uart_config, __MAX_BAUD ) < 0 ) {
				Debug::trace( "RS485.init", "failed to set the new Uart speed." );
			} else {
				/* remove the current configuration */
				if( tcflush(tty_fd, TCIOFLUSH) < 0) {
					Debug::trace( "RS485.init", "failed to flush Uart configuration." );
				} else {
					/* set the new configuration */
					if(tcsetattr( tty_fd, TCSANOW,  &uart_config ) < 0) {
						Debug::trace("RS485.init", "failed to set the new uart configuration." );
					} else {
						Debug::trace("RS485.init", "Uart configuration set." );
						interfaceInitialized = true;
					}
				}
			}
		}
	}

}

RS485 *RS485::getInstance() {
    if(pInstance == 0) {
    	pInstance = new RS485();
    	if(pInstance != NULL) {
    		/* if the interface does not initialize
    		 * then the instance was not created */
    		if(!pInstance->isInterfaceInitialized()) {
    			delete pInstance;
    			pInstance = 0;
    		}
    	}
    }
    return pInstance;
}


RS485::~RS485() {
	if(tty_fd < 0) {
		if(close( tty_fd ) < 0) {
			Debug::trace( "RS485", "failed to close RS485 handle." );
		}
	}
}

int RS485::sendLabelImage(unsigned char *pData, int size) {
	Util::CLockCriticalSection lock( sendLock );

	bufferSize = size;
	pBuffer = pData;

	send();

	return nBytes;
}

bool RS485::isInterfaceInitialized() {
	return interfaceInitialized;
}

void RS485::send() {
	nBytes = write( tty_fd, pBuffer, bufferSize  );
}


