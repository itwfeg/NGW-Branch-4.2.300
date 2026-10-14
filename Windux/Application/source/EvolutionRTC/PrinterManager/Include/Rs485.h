/*
 * Rs485.h
 *
 *  Created on: May 20, 2015
 *      Author: aaron
 */

#ifndef RS485_H_
#define RS485_H_

#if defined ( __linux__ )
#include <linux/serial.h>
#include <termios.h>
#endif // __linux__

#include <string>
#include "CSmartPtr.h"
#include "CTLSThread.h"

class RS485
{

	public:
		static RS485 *getInstance();
		~RS485();

		int sendLabelImage(unsigned char *pData, int size);
		bool isInterfaceInitialized();

	private:
		static RS485 *pInstance;
		RS485();

		Util::CSmartCriticalSection  sendLock;
		Util::CSmartCriticalSection  initLock;

#if defined ( __linux__ )
		struct serial_rs485 configuration;
		struct termios uart_config;
#endif // __linux__

		int tty_fd;
		int bufferSize;
		int nBytes;
		unsigned char *pBuffer;

		bool interfaceInitialized;

		static const std::string UART;

		#define TIOCGRS485      0x542E
		#define TIOCSRS485      0x542F

		void send();
};

#endif /* RS485_H_ */
