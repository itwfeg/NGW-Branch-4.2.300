/**
 * @file CanTranslator.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Header file for translating messages to and from CAN
 * Created on: Jun 24, 2015
 * @addtogroup CAN
 * @{
 */

#ifndef CANTRANSLATOR_H_
#define CANTRANSLATOR_H_
#include <deque>
#include "CEvolMsgReceiver.h"
#include "WeigherEvent.h"
#include "PrinterEvent.h"
#include "WrapperEvent.h"
#include "SystemEvent.h"
#include "LabelApplierEvent.h"
#include "CanLibrary.h"
#if defined ( __linux__ )
#include "KpcBootEvent.h"
#include "KPCBootReady.h"
#include "KPCBootCmd.h"
#endif // __linux__

class CanManager;
class PrinterManager;
class WeigherManager;
class WrapperManager;
class LabelApplierManager;
class BoardSiteTestManager;
class FpmManager;

/**
 * @class CanTranslator
 * @brief Class for translating messages to and from CAN
 * 
 */
class CanTranslator :  public Hobart::CEvolMsgReceiver {
    public:
        /**
         * @brief Constructor
         * 
         */
        CanTranslator();
        
        /**
         * @brief Destructor
         * 
         */
        ~CanTranslator();
        
#if defined ( __linux__ )		// added #defines to indiv sections in case we need to bring something back later
        /**
         * @brief The event handler for processing peripheral messages
         * 
         * @param msg the message being processed
         */
        virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );
        
        /**
         * @brief Translates a CAN frame to its corresponding peripheral message
         * 
         * @param pCanFrame the CAN frame to be translated
         */
        void translateMessage(can_frame *pCanFrame);
        
        /**
         * @brief Sends a request for wakeup to the weighing peripheral
         * 
         */
        void sendRequestWeigherWakeup();
        
        /**
         * @brief Sends a request for wakeup to the specified printing peripheral
         * 
         * @param prType the printer number
         */
        void sendRequestPrinterWakeup(PrinterType prType);
        
        /**
         * @brief Sends a request for wakeup to all of the printing peripherals
         * 
         */
        void sendRequestAllPrinterWakeup();
        
        /**
         * @brief Sends a request for wakeup to the wrapping peripheral
         * 
         */
        void sendRequestWrapperWakeup();

        /**
        * @brief set the pointer to the update manager
        * @param *pManager pointer to the update manager
		*/
        void assignUpdateManager(Hobart::CEvolMsgReceiver *pManager) { pUpdateManager = pManager; }

        /**
        * @brief set the pointer to the wrapper manager
        * @param *pManager pointer to the wrapper manager
		*/
        void assignWrapperManager(Hobart::CEvolMsgReceiver *pManager) { pWrapperManager = pManager; }

        /**
        * @brief set the pointer to the can manager
        * @param *pManager pointer to the can manager
		*/
        void assignCanManager(CanManager *pManager);

        /**
        * @brief clears the pointer to the wrapper update manager
		*/
        void clrUpdateManager() { pUpdateManager = 0; }

        /**
        * @brief determines if CAN id can be translated
        * @param id is the message cob id.
		*/
        bool isTranslatable(canid_t id);
        
        bool hasBulkFrameSent()	{ return bulkFrameStatus; }
        void clearBulkFrameFlag() 	{ bulkFrameStatus = false; }
#endif // __linux__

    private:
#if defined ( __linux__ )

        std::mutex mutex_;

        CanManager *pCanManager;
        CanLibrary *pCanLibrary;
        FpmManager* pSerialFlash;
        BoardSiteTestManager *pSiteManager;
        
        Hobart::CEvolMsgReceiver *pPrinter1Manager;
        Hobart::CEvolMsgReceiver *pPrinter2Manager;
        Hobart::CEvolMsgReceiver *pPrinter3Manager;
        Hobart::CEvolMsgReceiver *pWeigherManager;
        Hobart::CEvolMsgReceiver *pWrapperManager;
        Hobart::CEvolMsgReceiver *pUpdateManager;
        Hobart::CEvolMsgReceiver *pLabelApplier;
        Hobart::CEvolMsgReceiver *pKPCBootManager;
        Hobart::CEvolMsgReceiver *pKPCBootUpgrader;
        
        bool bulkFrameStatus;
        bool NGWScanFinished_;

        std::vector<unsigned char> statusBffr;

        /**
         * @struct CanMsg
         * @brief Contains the necessary information for transferring CAN messages
         * 
         */
        class CanMsg {
        public:
            canid_t id;                         /**<id. CAN cob id*/
            bool    msgComplete;                /**<msgComplete. true if the bytes received match the size of the message, false otherwise*/
            int     msgSize;                    /**<msgSize. The size of the message*/
            int     numBytesRcvd;               /**<numBytesRcvd. The number of bytes received of this message*/
            std::vector<unsigned char> msgData; /**<msgData. Pointer to the CAN data*/

            CanMsg() : id( 0 ), msgComplete( false ), msgSize( 0 ), numBytesRcvd( 0 ) {msgData.reserve(1024);}
            CanMsg( const CanMsg& other ) { copy( other ); }
            virtual ~CanMsg() {}

            CanMsg& operator = ( const CanMsg& rhs ) {
            	copy( rhs );
            	return *this;
            }

        protected:
            virtual void copy( const CanMsg& other ) {
            	id = other.id;
            	msgComplete = other.msgComplete;
            	msgSize = other.msgSize;
            	numBytesRcvd = other.numBytesRcvd;
            	msgData = other.msgData;
            }
        };
        
        //Queue of partial messages
        typedef std::map<canid_t, CanMsg> CanMsgQueue;
        CanMsgQueue rxMsgQueue;
        
        typedef std::pair<int, int> CanMsgRange;
        typedef std::list<CanMsgRange> CanMsgRangeList;
        CanMsgRangeList contiguousMessages;

        std::string mySystemType;

        static const unsigned short phTempsC[81];

        // Product id's of different CAN based peripherals
        const int CAN_KPC_PRODUCT_ID			= 0x220;	//ARK board peripheral controller
        const int CAN_ARK_PRINTER_PRODUCT_ID	= 0x230;	//ARK board printer only
        const int CAN_WEIGHER_PRODUCT_ID		= 0x240;	//Prepack CAN weigher
        const int CAN_SS_KPC_PRODUCT_ID         = 0x250;    //ARK board self service peripheral controller        
        const int CAN_PRINTER_PRODUCT_ID		= 0x260;	//Prepack CAN printer

        /**
         * @brief Adds a CAN frame to the given CAN stored message
         * Adds the data from the CAN frame to the given CAN message's msgData member and returns if the message is complete and ready to send
         * @param pCanFrame the CAN frame of who's data we will be using
         * @param pCanMsg the CAN message that is being modified
         * @param size the CAN message size
         * @return true if the message has been full received, false otherwise
         */
        bool convertFrameToCanMsg(can_frame *pCanFrame, CanMsg *pCanMsg, int size);
        
        //Methods to translate downstream messages
        /**
         * @brief Event handler for weigher events sent to the CAN translator
         * 
         * @param wgEvent the weigher event
         * @param recvBuffer buffer containing the message received
         */
        void translateWeigherEvent(WgEvent wgEvent, const char *recvBuffer);
        
        /**
         * @brief Event handler for printer events sent to the CAN translator
         * 
         * @param prEvent the printer event
         * @param recvBuffer buffer containing the message received
         */
        void translatePrinterEvent(PrEvent prEvent, const char *recvBuffer, int bufLen);
        
        /**
         * @brief Event handler for wrapper events sent to the CAN translator
         * 
         * @param wrEvent the wrapper event
         * @param recvBuffer buffer containing the message received
         */
        void translateWrapperEvent(WrEvent wrEvent, const char *recvBuffer);
        
        /**
         * @brief Event handler for Label Applier events sent to the CAN translator
         *
         * @param laEvent the applier event
         * @param recvBuffer buffer containing the message received
         */
        void translateLabelApplierEvent(LaEvent laEvent, const char *recvBuffer);


        //Methods to translate CAN Messages to Hobart Messages
        /**
         * @brief Translates the given CAN message to a weigher message
         * 
         * @param pCanMsg the CAN message to be translated to weigher messages
         */
        void translateWeigherCanMsg(const CanMsg *pCanMsg);
        
        /**
         * @brief Translates the given CAN message to a printer message
         * 
         * @param pCanMsg the CAN message to be translated to printer messages
         */
        void translatePrinterCanMsg(const CanMsg *pCanMsg);
        
        /**
         * @brief Translates the given CAN message to a wrapper message
         * 
         * @param pCanMsg the CAN message to be translated to wrapper messages
         */
        void translateWrapperCanMsg(const CanMsg *pCanMsg);
        
        /**
         * @brief Translates the given CAN message to a KPC system message
         *
         * @param pCanMsg the CAN message to be translated to KPC system messages
         */
        void translateKpcSystemCanMsg(const CanMsg *pCanMsg);

        /**
         * @brief Translates the given CAN Status message to a wrapper message
         *
         * @param pCanMsg the CAN message to be translated to wrapper messages
         */
        void handleWrStatusCanMsg(can_frame *pCanFrame);

        /**
         * @brief Sends a request for the weigher event to the CAN manager
         * 
         * @param wgEvent the weigher event requested
         */
        void sendWeigherRequest(WgEvent wgEvent);
        
        /**
         * @brief Sends a request for the printer event to the CAN manager
         * 
         * @param prEvent the printer event requested
         * @param prType the printer number (first, second, third)
         */
        void sendPrinterRequest(PrEvent prEvent, PrinterType prType);
        
        /**
         * @brief Sends a request for the wrapper event to the CAN manager
         * 
         * @param wrEvent the wrapper event requested
         */
        void sendWrapperRequest(WrEvent wrEvent);
        
        /**
        * @brief Checks if the cob id is within the range of our peripherals
        * 
        * @param cob the CAN cob id
        * @return true if the cob id is a supported cob id by one of our peripherals(printer, weigher, wrapper)
        */
        bool acceptedCobId(unsigned int cob);
        
        /**
        * @brief Sends a single CAN frame to the CAN manager
        * 
        * @param canMsg the can frame to be transmitted
        */
        void sendCanFrame(can_frame& canMsg);
        
        /**
         * @brief Sends a CAN message that is greater than 8 bytes
         * Example usage:
         *  WeigherConfiguration wc(recvBuffer);
         *  sendLargeCANMessage((const char*) wc, sizeof(wc), pCanLibrary->getWeigherCobID(WgConfig), true);
         * @note If your buffer is smaller than "size", this function will have undefined behavior when dereferencing out of the bounds of recvBuffer!!!
         * @note This is used for sending a CAN message from a Big Endian system!
         * @param recvBuffer the buffer containing the message to be sent
         * @param size the size of the buffer containing the message to be sent
         * @param cobId the cob id of the message being sent
         * @param littleEndian true if you want the data to be sent in Little Endian format, Big Endian if false
         */
        void sendLargeCANMessage(const char* recvBuffer, unsigned int size, unsigned int cobId, bool littleEndian);
        
        /**
         * @brief Registers for the weigher events
         * 
         * @note Here is the location you should add any weigher events that you want the translator to listen for
         */
        void registerWeigherEvents();
        
        /**
         * @brief Registers for the printer events
         * 
         * @note Here is the location you should add any printer events that you want the translator to listen for
         */
        void registerPrinterEvents();
        
        /**
         * @brief Registers for the kpc bootloader events
         *
         * @note Here is the location you should add any kpc boot events that you want the translator to listen for
         */
        void registerKPCBootEvents();

        /**
         * @brief Registers for the kpc boot upgrader events
         *
         * @note Here is the location you should add any kpc boot events that you want the translator to listen for
         */
        void registerKPCBootUpgraderEvents();

        /**
         * @brief Deegisters for the kpc boot upgrader events
         *
         * @note Here is the location you should add any kpc boot events that you want the translator to listen for
         */
        void deregisterKPCBootUpgraderEvents();

        /**
         * @brief Registers for the wrapper events
         * NOTE: Here is the location you should add any wrapper events that you want the translator to listen for
         */
        void registerWrapperEvents();

        /**
         * @brief Sends a printer message
         * Does all the necessary error checking for sending a printer message to the printerPosition
         * @param msg message buffer to send to the printer
         * @param msgLength the length of the message buffer
         * @param prType the printer we want to send the message to
         */
        void sendPrinterMessage(const char* msg, int msgLength, PrinterType prType);
        void sendPrinterMessage(const EvolMessage *msg);
        
        /**
         * @brief Sends a wrapper message
         * 
         * @param msg message buffer to send to the weigher
         * @param msgLength the length of the message buffer
         */
        void sendWeigherMessage(const char* msg, int msgLength);
        void sendWeigherMessage(const EvolMessage *msg);
        /**
         * @brief Sends a wrapper message
         * 
         * @param msg message buffer to send to the wrapper
         * @param msgLength the length of the message buffer
         */
        void sendWrapperMessage(const char* msg, int msgLength);
        void sendWrapperMessage(const EvolMessage *msg);
        
        /**
         * @brief Gets the most significant byte from a short
         * 
         * @param value the short we are getting the most significant byte from
         */
        unsigned char getMsbFromShort(unsigned short value);
        
        /**
         * @brief Gets the least significant byte from a short
         * 
         * @param value the short we are getting the least significant byte from
         */
        unsigned char getLsbFromShort(unsigned short value);
        
        /**
         * @brief Gets the most significant byte from a long
         * 
         * @param value the long we are getting the most significant byte from
         */
        unsigned char getMsbFromLong(unsigned long value);
        
        /**
         * @brief Gets a byte from a long
         * 
         * @param value the long we are getting the byte from
         * @param index the index of the byte we want
         */
        unsigned char getCharFromLong(unsigned long value, int index);
        
        /**
         * @brief Gets the least significant byte from a long
         * 
         * @param value the long we are getting the least significant byte from
         */
        unsigned char getLsbFromLong(unsigned long value);
        
        /**
         * @brief Converts to characters into a short in Big Endian
         * 
         * @param msbByte the most significant byte
         * @param lsbByte the least significant byte
         */
        unsigned short charToShort(unsigned char msbByte, unsigned char lsbByte);
        
        /**
         * @brief Converts a character buffer to an unsigned integer
         * NOTE: pMsbByte must be contain at least 4 characters otherwise this function will crash!!
         * @param pMsbByte pointer to the beginning of the character buffer
         */
        static unsigned int charToInt(const unsigned char *pMsbByte);

        static char makeValidChar( char c );

        /**
		 * @brief Event handler for Kpc Bootloader events sent to the CAN translator
		 *
		 * @param event the kpc bootloader event
		 * @param recvBuffer buffer containing the message received
		 */
        void translateKPCBootEvent(KPCBootEvent event, const char *recvBuffer);

        /**
		 * @brief Translates the given CAN message to a KPC bootloader message
		 *
		 * @param pCanMsg the CAN message to be translated to KPC bootloader messages
		 */
		void translateKPCBootCanMsg(const CanMsg *pCanMsg);

		void sendKPCBootMessage(const EvolMessage *msg);
		void sendKPCBootMessage(const char* msg, int msgLength);

        /**
		 * @brief Event handler for Kpc Boot upgraderevents sent to the CAN translator
		 *
		 * @param event the kpc bootloader event
		 * @param recvBuffer buffer containing the message received
		 */
        void translateKPCBootUpgraderEvent(KPCBootUpgraderEvent event, const char *recvBuffer);

        /**
		 * @brief Translates the given CAN message to a KPC boot upgrader message
		 *
		 * @param pCanMsg the CAN message to be translated to KPC bootloader messages
		 */
		void translateKPCBootUpgraderCanMsg(const CanMsg *pCanMsg);

        void sendKPCBootUpgraderMessage(const EvolMessage *msg);
		void sendKPCBootUpgraderMessage(const char* msg, int msgLength);

        /**
		 * @brief Event handler for board site events sent to the CAN translator
		 *
		 * @param event the board site event
		 * @param recvBuffer buffer containing the message received
		 */
        void translateBoardSiteEvent(BoardSiteEvent event, const char *recvBuffer);

        /**
		 * @brief Translates the given CAN message to a board site message
		 *
		 * @param pCanMsg the CAN message to be translated to board site messages
		 */
		void translateBoardSiteCanMsg(const CanMsg *pCanMsg);


#endif // __linux__
};
/** @} */
#endif /* CANTRANSLATOR_H_ */
