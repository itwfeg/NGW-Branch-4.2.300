/**
 * @file CanManager.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Header file for managing messages to and from the CAN bus
 * Created on: Jun 24, 2015
 * @addtogroup CAN
 * @{
 */
#ifndef CANMANAGER_H_
#define CANMANAGER_H_
#include <string>
#include <list>
#include <vector>
#include <fcntl.h>
#if defined( __linux__ )
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#endif // __linux__
#include "CEvolMsgReceiver.h"
#include "CTLSThread.h"
#include "GpioDeviceReset.h"
#include "CanTranslator.h"

#include <util/thread/mutex.h>

/**
 * @class CanManager
 * @brief Singleton class that manages messages to and from the CAN bus
 * 
 */
class CanManager : public Util::CRunnableClass {
	public:

		typedef enum {
			_PrimaryBus,
			_Boot
		}CANBUS;

		/**
		* @brief Gets an instance of the CAN manager
		*
		*/
		static CanManager *getInstance( CANBUS bus = _Boot );

#if defined ( __linux__ )           // added #defines to indiv sections in case we need to bring something back later

		/**
         * @brief opens the CAN interface
         *
         */
		void open();

		/**
         * @brief Initializes the CAN translator
         * 
         */
		void initializeTranslator();
        
		/**
         * @brief Checks whether the CAN interface is open
         * 
         * @return true if the CAN interface successfully opened, false otherwise
         */
		bool isOpen(){return interfaceOpen;}
        
        /**
         * @brief Sends a CAN frame
         * 
         * @param pCanFrame pointer to the CAN frame we are sending
         * @return the number of bytes sent over the CAN bus
         */
		int send(can_frame *pCanFrame);
        
        /**
         * @brief Probes the CAN bus for connected peripherals
         * 
         */
		void probeBus();

		void setTranslatorUpdaterManager(Hobart::CEvolMsgReceiver *pManager) {
			if ( pTranslator )
				pTranslator->assignUpdateManager(pManager);
		}

		void setTranslatorWrapperManager(Hobart::CEvolMsgReceiver *pManager) {
			if ( pTranslator )
				pTranslator->assignWrapperManager(pManager);
		}

		void clrTranslatorUpdateManager() {
			if ( pTranslator )
				pTranslator->clrUpdateManager();
		}

		void clrBulkFrameStatus()  {
			if ( pTranslator )
				pTranslator->clearBulkFrameFlag();
		}
		bool getBulkFrameStatus()  {
			if ( pTranslator )
				return pTranslator->hasBulkFrameSent();
			return false;
		}

        /**
         * @brief shuts down the can interface can0
         */
		void shutdown();
#endif // __linux__         

	private:
        /**
         * @brief Constructor
         * 
         */
		CanManager( CANBUS bus );
        
        /**
         * @brief Destructor
         * 
         */
		~CanManager();

		static util_thread::Mutex<> instanceMutex_;
		static CanManager *pInstancePrimary;
		static CanManager *pInstanceBoot;

        virtual DWORD Run();

#if defined ( __linux__ )

        /**
         * @brief Reads the CAN bus for incoming/outgoing CAN frames
         * 
         */
		Util::CTLSThread *myRunThread;
		
		bool interfaceOpen;

		struct sockaddr_can socAddr;
		int sockFd;

		static const int Ngw1FilterSize =  5;
		static const int Hti3FilterSize = 34;
		static const int NGW3FilterSize = 25;
		static const can_filter aws3Filter[Ngw1FilterSize];
		static const can_filter htiFilter[Hti3FilterSize];
		static const can_filter ngw3Filter[NGW3FilterSize];

		static const unsigned long Mask16Range = 0x7f0;
		static const unsigned long Mask8Range = 0x7f8;
		static const unsigned long Mask4Range = 0x7fc;
	    static const unsigned long MaskMatch = 0x7ff;


		std::string ifname;

		CanTranslator *pTranslator;

		static const unsigned int ErrorBitMask	= 0x20000000;
		static const canid_t CobIDMask			= 0x0FFFFFFF;

		Util::CSmartCriticalSection  sendLock;

		//device control lines
		GpioDeviceReset weigherCntl;
		GpioDeviceReset printer1Cntl;
		GpioDeviceReset printer2Cntl;
		GpioDeviceReset printer3Cntl;
		GpioDeviceReset wrapperCntl;
		GpioDeviceReset aws3WrapperCntl;

#endif // __linux__

};
/** @} */
#endif /* CANMANAGER_H_ */
