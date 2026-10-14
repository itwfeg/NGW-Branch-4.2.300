/**
 * @file RS485Manager.h
 * @copyright Copyright (c) 2018 Hobart Corportation
 * @author Aaron Swift
 * @brief Responsible for sending label data over RS485
 * @addtogroup CommManagers
 * @{
 */
#ifndef rs485manager_h
#define rs485manager_h

#include "CEvolMsgReceiver.h"
#include "CTLSThread.h"
#include "Printer.h"
#include "PrinterEvent.h"
#include "PrinterPrinterType.h"
#include "PrinterLabelTransferStatus.h"

#if defined ( __linux__ )		// Windows 
#include <linux/serial.h>
#include <termios.h>
#endif // __linux__

#include <atomic>
#include <queue>
#include <utility>

#define TIOCGRS485              0x542E
#define TIOCSRS485              0x542F
#define _MAX_ASYNC_BAUD         3000000
#define _2000000_BAUD			2000000
#define _500000_BAUD			500000
#define _115200_BAUD		 	115200

struct LabelTransfer
{
    unsigned int id;
    unsigned char * pImage;
    unsigned long size;
    unsigned long xferSize;
    bool complete;
};

/**
 * @class RS485Manager
 * @brief Manager for the RS485 Serial Bus
 * Singleton class responsible for pacing data chunks across RS485 to the CAN printer with a 1ms delay between each chunk
 * 
 */
class RS485Manager: public Hobart::CEvolMsgReceiver, public Util::CRunnableClass {
    public:

        typedef enum RS485_States
        {
        	_Init,
        	_Transfer,
        	_WaitForReady,
        	_CheckTransferStatus,
        } RS485States;

        /**
         * @brief Get instance function for our RS485 singleton
         * 
         */
        static RS485Manager* getInstance();
        
        /**
         * @brief Destructor for the RS485 manager
         * 
         */
        ~RS485Manager();
        
        /**
         * @brief Checks whether we successfully initialized the RS485 interface
         * 
         * @return true if we successfully initialized the RS485 interface, false otherwise
         */
        bool isinitialized_();
        
        void setTargetSize( unsigned short size ) { targetSize_ = size; }

        bool getSendCmd() { return sendCmd_; }
        void clearSendCmd() { sendCmd_ = false; }

        /**
         * @param msg the message received
         */
        virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

        /**
         * @brief Assigns label image for the RS485 interface to process
         * @param id: the station id of the target printer (FirstPrinter)
         * @param pImage: pointer to the label image
         * @param size: size of the overall transfer
         * @param xferSize: how much we can transfer at a time
         * @param complete: has the complete image been assigned
         * @return
         */
        bool assignLabelImage( unsigned int id, unsigned char *pImage, unsigned long size, unsigned long xferSize, bool complete );

        bool isBusy() { return busy_; }

        RS485Manager::RS485States getState();


        /**
         * @brief copy test pattern into the transfer buffer and send.
         *        this method is to be used for board site testing only!
         *
         */
        void transferTestPattern();

        /**
         * @brief Sends a number of bytes over the RS485 line
         * 
         * @param pData the pointer to the label data
         * @param numBytes the number of bytes to send
         * @return the error code sent back from sending the data over rs485
         */


    protected:
        virtual DWORD Run();


    private:


        static RS485Manager* pInstance;
        static const std::string UART;

        std::atomic<RS485States> state_;
        
        RS485States prevState_;
        
        //Concurrency variables
		Util::CSmartCriticalSection initLock;
        Util::CSmartCriticalSection csLock;

        Util::CTLSThread *pThread_;
        
        //RS485 configuration variables
#if defined ( __linux__ )
        struct serial_rs485 configuration;
		struct termios uart_config;
#endif // __linux__

        //The file descriptor for the RS485 channel
		int tty_fd;
        
        //Initialization variables
        bool initialized_;
        
        //Check if the interface is being used
        bool busy_;
        

        bool sendCmd_;

        /* 
            Flow control can come in before we return from the transfer() function
            this bool lets us know we got the flow control message
        */
        bool gotFlowControl_;

        //size to the image to transfer
        long imageSize_;
        
        //size of the target printer image buffer
        unsigned short targetSize_;

        //image buffer
        unsigned char *pBfrBase_;
        unsigned char *pBfr_;

        std::queue < LabelTransfer > labelQueue;

		#define CHECK_SUM_SIZE 	4

        #define LINE_SIZE_SERVICE	56

        //save extra bytes for next series of transfers
        unsigned char rBfr[LINE_SIZE_SERVICE];

        //max payload transfer at a time plus a 4 byte checksum at the end
        unsigned long transferSize_;

        //status of the payload just sent
        bool reSend_;

        //counter for number of retransmit of payload
        unsigned long reSendCntr_;

        // flag for last payload to transfer
        bool lastTransfer_;

        //printer type we are processing ( FirstPrinter / SecondPrinter )
        unsigned int stationId_;

        //count of bytes transfered
        unsigned int bytesXfrd_;

        // has the complete image been assigned
        bool complete_;
        /**
         * @brief the constructor for the RS485Manager
         * 
         */
        RS485Manager();


        /**
         * @brief copy the image chunk into the transfer buffer, calculate checksum and send payload to printer
         *
         */
        void transfer();

        #if 1	/* added for testing remove when finished */
    	unsigned char 	pattern1_[4090];
    	unsigned char 	pattern2_[4090];

    	bool sPattern_;
		#endif
};
/** @} */
#endif //rs485manager_h
