/*
 * Sam7xCANUpdater.h
 *
 *  Created on: May 13, 2015
 *      Author: aaron
 */
/**
 * @file CanTranslator.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Header file for supporting of firmware updates to all SAM7X CAN based peripherals
 * Created on: May 13, 2015 Modified and Formatted 10/28/15 by Alex Zavala
 * @addtogroup CAN
 * @{
 */

#ifndef SAM7XCANUPDATER_H_
#define SAM7XCANUPDATER_H_
#include <string>
#include <list>
#include <fcntl.h>
#if defined ( __linux__ )
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#else
// These headers do not exist for Windows. There are two options
// 1. Cygwin
// 2. Winsock
// See http://stackoverflow.com/questions/2952733/using-sys-socket-h-functions-on-windows to see talk on this discussion
#endif // __linux__
#include "CTLSThread.h"

/**
* @class Sam7xCANUpdater
* @brief Singleton class that manages firmware updates to all SAM7X CAN based peripherals
*
*/

class Sam7xCANUpdater
{

	public:
		/**
		* @brief Gets an instance of the updater
		*
		*/
		static Sam7xCANUpdater *getInstance();

        /**
        * @brief Destructor
        *
        */
		~Sam7xCANUpdater();

		/**
        * @brief method for processing the list of know CAN devices for firmware updates
        *
        */
		void processDevices();




	private:

#if defined ( __linux__ )
		static Sam7xCANUpdater *pInstance;

		Util::CSmartCriticalSection  processLock;
		Util::CSmartCriticalSection  initLock;

		typedef enum
		{
			_SAM7XWrapper,
			_SAM7XWeigher,
			_SAM7XPrinter1,
			_SAM7XPrinter2,
			_SAM7XPrinter3,
			_SAM7XNone

		}Sam7xDevices;

		typedef std::list< Sam7xDevices > Sam7xDevicesList;
		Sam7xDevicesList scanList;

        /**
        * @brief Constructor
        *
        */
		Sam7xCANUpdater();

        /**
        * @brief initializes the updater
        *
        */
		void init();

        /**
        * @brief initializes the GPIO forcing all peripherals to reset
        *
        */
		void initGpio();

        /**
        * @brief opens the CANBus
        *
        */
		int openCANBus();

		struct sockaddr_can socAddr;
		int sockFd;


		typedef enum _GpioControl
		{
			_OFF,
			_ON

		}GpioControl;

        /**
        * @brief controls wrapper reset line
        * @param GpioControl on or off
        */
		void wrapperResetControl(GpioControl control);

        /**
        * @brief controls weigher reset line
        * @param GpioControl on or off
        */
		void weigherResetControl(GpioControl control);

        /**
        * @brief controls pritner1 reset line
        * @param GpioControl on or off
        */
		void printer1ResetControl(GpioControl control);

		/**
        * @brief controls pritner2 reset line
        * @param GpioControl on or off
        */
		void printer2ResetControl(GpioControl control);

        /**
        * @brief controls pritner3 reset line
        * @param GpioControl on or off
        */
		void printer3ResetControl(GpioControl control);


		static const std::string gpioPath;
		static const std::string upgradePath;
		static const std::string wrapperDirectory;
		static const std::string weigherDirectory;
		static const std::string printerDirectory;

		static const std::string wrapperResetGpio;
		static const std::string weigherResetGpio;
		static const std::string printer1ResetGpio;
		static const std::string printer2ResetGpio;
		static const std::string printer3ResetGpio;

		static const std::string ifname;

		static const unsigned long imageStartAddr = 0x00108200;
		static const int frameDataSize	= 8;

		/* device responses to commands */
		static const int COMMAND_OK 	= 0;
		static const int COMMAND_NOK 	= 1;
		static const int COMMAND_WAIT 	= 2;

		static const unsigned char WeigherNodeID 	= 1;
		static const unsigned char Printer1NodeID 	= 2;
		static const unsigned char Printer2NodeID 	= 3;
		static const unsigned char Printer3NodeID 	= 4;
		static const unsigned char WrapperNodeID 	= 5;


        /**
        * @brief open communication interface of device
        *
        */
		bool openCommInterface();

		/**
        * @brief transmit a CAN frame
        *
        */
		int sendCanMsg( can_frame *pFrame );

		/**
        * @brief
        *
        */
		Sam7xDevices getDeviceFromId( int id );

        /**
        * @brief
        *
        */
		int readProductId();

		/**
        * @brief
        *
        */
		bool isHobartDevice();

        /**
        * @brief
        *
        */
		bool isDeviceInterfaceOpen( can_frame *pFrame );

        /**
        * @brief
        *
        */
		bool isUpgradeAvailable( Sam7xDevices device );

        /**
        * @brief
        *
        */
		bool doesIDMatchDevice( int id, Sam7xDevices device );

        /**
        * @brief
        *
        */
		int receiveCanMsg( can_frame *pFrame );

		/**
        * @brief
        *
        */
		void packDataLong(unsigned char *pBuffer, unsigned long data1, unsigned long data2);

		/**
        * @brief
        *
        */
		int handleDeviceResponse(int response);

        /**
        * @brief
        *
        */
		int calNumTrasfers(int size, int remainder);

		/**
        * @brief
        *
        */
		int updateDevice(Sam7xDevices device);

        /**
        * @brief
        *
        */
		int send(unsigned char *pImage, int imageSize);

		/**
        * @brief
        *
        */
		int updateDeviceNodeID(Sam7xDevices device);

		/**
        * @brief
        *
        */
		int launchApplication(Sam7xDevices device);
#endif // __linux__

};



#endif
