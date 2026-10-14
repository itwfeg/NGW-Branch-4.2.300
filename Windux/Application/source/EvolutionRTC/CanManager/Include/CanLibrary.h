/**
 * @file CanLibrary.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Responsible for retrieving information from the CAN messages
 * Created on: Jun 29, 2015, Updated, Refactored, and Formatted 10/29/15 by Alex Zavala
 * @addtogroup CAN
 * @{
 */
#ifndef CANLIBRARY_H_
#define CANLIBRARY_H_
#include <vector>
#include <map>
#include <list>
#if defined ( __linux__ )
#include <sys/socket.h>
#include <linux/can.h>
#else
// These headers do not exist for Windows. There are two options
// 1. Cygwin
// 2. Winsock
// See http://stackoverflow.com/questions/2952733/using-sys-socket-h-functions-on-windows to see talk on this discussion
#endif // __linux__
#include "WeigherEvent.h"
#include "Printer.h"
#include "PrinterEvent.h"
#include "WrapperEvent.h"
#include "LabelApplierEvent.h"
#include "KpcSystemEvents.h"
#include "SystemEvent.h"
#include "BoardSiteEvent.h"
#if defined ( __linux__ )
#include "KpcBootEvent.h"
#include "KpcBootUpgraderEvent.h"
#endif // __linux__


typedef enum
{
	REQ_NONE,
	REQ_WAKEUP,
	REQ_STATUS,
	REQ_CONFIG,
	REQ_SENSORS,
	REQ_CYCLE_COUNT,
	REQ_SYSTEM_RESET,
	REQ_SYSTEM_INFO,
	REQ_CAMERA_THRESHOLD,
	REQ_MFG_RUNIN,
	REQ_VERSION,
	REQ_WB_CURRENT_TIMINGS,
	REQ_WB_DEFAULT_TIMINGS,
	REQ_WB_TEMPORARY_TIMINGS,
	REQ_WB_WRITE_TIMINGS,
	REQ_WB_WRITE_DEFAULT_TIMINGS
}WrapperRequest;

/**
 * @class CanLibrary
 * @brief Class that retrieves information from the CAN peripheral messages
 * 
 */
class CanLibrary {
    public:

        /**
         * @brief Constructor
         * 
         */
        CanLibrary();
        
        /**
         * @brief Destructor
         * 
         */
        ~CanLibrary();
        
#if defined ( __linux__ )			// added #defines to indiv sections in case we need to bring something back later
        /**
         * @brief Gets the peripheral event for a specific cob id
         * 
         * @param cobID the CAN message cob id
         * @return the peripheral event
         */
        unsigned short getEventForCobId(canid_t cobID);
        
        /**
         * @brief Gets the CAN message size for a specific cob id
         * 
         * @param cobID the CAN message cob id
         * @return the message size
         */
        int getMsgSizeForCobId(canid_t cobID);

        /**
         * @brief Sets the CAN message size for a specific cob id
         *
         * @param cobID the CAN message cob id
         * @param size  size to be assigned to specific id
         */
        void setMsgSizeForCobId(canid_t cobID, int size);
        
        /**
         * @brief Gets the cob id for a weigher event
         * 
         * @param event weigher event whose cob id we are searching for
         * @return the cob id for the event requested
         */
        canid_t getWeigherCobID(WgEvent event);
        
        /**
         * @brief Gets the cob id for a printer event
         * 
         * @param event a printer event
         * @param prType the printer type (FirstPrinter=0,SecondPrinter=1,ThirdPrinter=2)
         * @return the cob id for the printer event requested
         */
        canid_t getPrinterCobID(PrEvent event, PrinterType prType);
        
        /**
         * @brief Gets the printer position based on the cob id
         * 
         * @param cobID the CAN cob id
         * @return the position of the printer that sent the cob id
         */
        PrinterType getPrinterPosition(canid_t cobID);
        
        /**
         * @brief Gets the cob id for a wrapper event
         * 
         * @param event a wrapper event
         * @return the cob id for the wrapper event requested
         */
        canid_t getWrapperCobID(WrEvent event);
        
        /**
           * @brief Gets the cob id for a kpc bootloader event
           *
           * @param event a wrapper event
           * @return the cob id for the wrapper event requested
           */
        canid_t getKPCBootCobID(KPCBootEvent event);

        /**
           * @brief Gets the cob id for a kpc boot upgrader event
           *
           * @param event a wrapper event
           * @return the cob id for the wrapper event requested
           */
        canid_t getKPCBootUpgraderCobID(KPCBootUpgraderEvent event);

        /**
         * @brief Gets the minimum cob id bounds for the printer position
         * 
         * @param prType the printer type (FirstPrinter,SecondPrinter,ThirdPrinter)
         * @return the minimum cob id for the printer position
         */
        canid_t getMinPrinterCobID(PrinterType prType);
        
        /**
         * @brief Gets the maximum cob id bound for the printer position
         * 
         * @param prType the printer type (FirstPrinter,SecondPrinter,ThirdPrinter)
         * @return the maximum cob id for the printer position
         */
        canid_t getMaxPrinterCobID(PrinterType prType);
        
        /**
         * @brief Gets the minimum cob id bound for the weigher
         * 
         * @return the minimum cob id for the weigher
         */
        canid_t getMinWeigherCobID(){return minWeigherCobID;}
        
        /**
         * @brief Gets the maximum cob id bound for the weigher
         * 
         * @return the maximum cob id for the weigher
         */
        canid_t getMaxWeigherCobID(){return maxWeigherCobID;}
        
        /**
         * @brief Gets the minimum cob id bound for the wrapper
         * 
         * @return the minimum cob id for the wrapper
         */
        canid_t getMinWrapperCobID(){return minWrapperCobID;}
        
        /**
         * @brief Gets the maximum cob id bound for the wrapper
         * 
         * @return the maximum cob id for the wrapper
         */
        canid_t getMaxWrapperCobID(){return maxWrapperCobID;}
        
        canid_t getWrapperGenericReqCobID() { return 0x566; }

        /**
         * @brief Gets the  cob id bound for the KPC system error message
         *
         * @return the cob id for the KPC system error message
         */
        canid_t getKpcSystemErrorCobID() { return kpcSystemErrCobID; }

        /**
         * @brief Gets the minimum cob id bound for the kpc bootloader
         *
         * @return the minimum cob id for the kpc bootloader
         */
        canid_t getMinKPCBootCobID(){return minKPCBootCobID;}

        /**
         * @brief Gets the maximum cob id bound for the kpc bootloader
         *
         * @return the maximum cob id for the kpc bootloader
         */
        canid_t getMaxKPCBootCobID(){return maxKPCBootCobID;}

        /**
         * @brief Gets the minimum cob id bound for the kpc bootloader
         *
         * @return the minimum cob id for the kpc bootloader
         */
        canid_t getMinKPCBootUpgraderCobID(){return minKPCBootUpgraderCobID;}

        /**
         * @brief Gets the maximum cob id bound for the kpc bootloader
         *
         * @return the maximum cob id for the kpc bootloader
         */
        canid_t getMaxKPCBootUpgraderCobID(){return maxKPCBootUpgraderCobID;}

        canid_t getBoardSiteCobID(BoardSiteEvent event);

        /**
         * @brief Gets the minimum cob id for board site testing.
         *
         * @return the minimum cob id for board site testing 
         */
        canid_t getMinARKBoardSiteCobID(){return minARKBoardSiteCobID;}

        /**
         * @brief Gets the maximum cob id for board site testing.
         *
         * @return the maximum cob id for board site testing 
         */
        canid_t getMaxARKBoardSiteCobID(){return maxARKBoardSiteCobID;}
        

        WrapperRequest getWrapperRequest( WrEvent wrEvent );
        WrapperRequest getTimingsCurrentRequest() 	{ return REQ_WB_CURRENT_TIMINGS; }
        WrapperRequest getTimingsDefaultRequest() 	{ return REQ_WB_DEFAULT_TIMINGS; }
        WrapperRequest getTimingsTemporaryRequest()	{ return REQ_WB_TEMPORARY_TIMINGS; }
        /**
         * @brief Checks whether we support this cob id
         * Takes the cob id and checks if it falls under a peripheral's accepted message range
         * @param cob the cob id being checked
         * @return true if we support the cob, false otherwise
         */
        bool acceptedCobId(unsigned int cob);
        
        /**
         * @brief Checks whether we support this cob id based on filter settings.
         * @param cob the cob id being checked
         * @return true if we support the cob, false otherwise
         */
        bool frameFilter(canid_t cobID);
        
#endif // __linux__

    private:

#if defined ( __linux__ )           // added #defines to indiv sections in case we need to bring something back later

        /**
         * @brief Builds the weigher cob id library
         * 
         */
        void buildWeigherLibrary();
        
        /**
         * @brief Adds a weigher CAN message to be tracked
         * 
         * @param cobId the cob id for the weigher event
         * @param wgEvent the weigher event we are tying the cob id to
         * @param outgoing true if this message is outgoing, false if incoming
         * @param canMsgSize size of the CAN message
         */
        void addWeigherMessage(unsigned int cobId, WgEvent wgEvent, bool outgoing, unsigned int canMsgSize);
        
        /**
		 * @brief Adds a kpc bootloader CAN message to be tracked
		 *
		 * @param cobId the cob id for the weigher event
		 * @param KPCBootEvent the weigher event we are tying the cob id to
		 * @param outgoing true if this message is outgoing, false if incoming
		 * @param canMsgSize size of the CAN message
		 */
        void addKPCBootMessage(unsigned int cobId, KPCBootEvent kpcEvent, bool outgoing, unsigned int canMsgSize);

        /**
		 * @brief Adds a kpc boot upgrader CAN message to be tracked
		 *
		 * @param cobId the cob id for the weigher event
		 * @param KPCBootUpgraderEvent the weigher event we are tying the cob id to
		 * @param outgoing true if this message is outgoing, false if incoming
		 * @param canMsgSize size of the CAN message
		 */
        void addKPCBootUpgraderMessage(unsigned int cobId, KPCBootUpgraderEvent kpcEvent, bool outgoing, unsigned int canMsgSize);

        /**
         * @brief Adds a board site CAN message to be tracked
         * 
         * @param cobId the cob id for the board site event
         * @param BoardSiteEvent the baord site event we are tying the cob id to
         * @param outgoing true if this message is outgoing, false if incoming
         * @param canMsgSize size of the CAN message
         */
        void addBoardSiteMessage(unsigned int cobId, BoardSiteEvent brdEvent, bool outgoing, unsigned int canMsgSize);

        /**
         * @brief Builds the printer cob id library
         * 
         */
        void buildPrinterLibrary();
        
        /**
         * @brief Adds a printer CAN message to be tracked
         * 
         * @param cobId the cob id for the printer event
         * @param prEvent the printer event we are tying the cob id to
         * @param outgoing true if this message is outgoing, false if incoming
         * @param prNum the printer type (FirstPrinter=0,SecondPrinter=1,ThirdPrinter=2)
         * @param canMsgSize size of the CAN message
         */
        void addPrinterMessage(unsigned int cobId, PrEvent prEvent, bool outgoing, PrinterType prNum, unsigned int canMsgSize);
        
        /**
         * @brief Adds an outgoing printer CAN message to be tracked
         * Helper function for the addPrinterMessage function
         * @param cobId the cob id for the printer event
         * @param prEvent the printer event we are tying the cob id to
         * @param prNum the printer type (FirstPrinter=0,SecondPrinter=1,ThirdPrinter=2)
         */
        void addOutgoingPrinterMessage(unsigned int& cobId, PrEvent& prEvent, PrinterType& prNum);
        
        /**
         * @brief Adds an incoming printer CAN message to be tracked
         * Helper function for the addPrinterMessage function
         * @param cobId the printer event we are tying the cob id to
         * @param prEvent the printer event we are tying the cob id to
         * @param prNum the printer type (FirstPrinter=0,SecondPrinter=1,ThirdPrinter=2)
         * @param canMsgSize size of the CAN message
         */
        void addIncomingPrinterMessage(unsigned int& cobId, PrEvent& prEvent, PrinterType& prNum, unsigned int& canMsgSize);
        
        /**
        * @brief Builds the wrapper cob id library
        * 
        */
        void buildWrapperLibrary();
        
        /**
		* @brief Builds the kpc bootloader cob id library
		*
		*/
        void buildKPCBootLibrary();


        /**
		* @brief Builds the kpc boot upgrader cob id library
		*
		*/
        void buildKPCBootUpgraderLibrary();

        /**
         * @brief Adds a wrapper CAN message to be tracked
         * 
         * @param cobId the cob id for the wrapper event
         * @param wrEvent the wrapper event we are tying the cob id to
         * @param outgoing true if this message is outgoing, false if incoming
         * @param canMsgSize size of the CAN message
         */
        void addWrapperMessage(unsigned int cobId, WrEvent wrEvent, bool outgoing, unsigned int canMsgSize);
        
        /**
         * @brief Helper function that checks whether we support the printer cob id
         * Checks whether the cob id falls within the printer's accepted message range
         * @param cob the cob id being checked
         * @return true if we support the cob, false otherwise
         */
        bool acceptedPrinterCobId(unsigned int cob);
        
        /**
         * @brief Helper function that checks whether we support the weigher cob id
         * Checks whether the cob id falls within the weigher's accepted message range
         * @param cob the cob id being checked
         * @return true if we support the cob, false otherwise
         */
        bool acceptedWeigherCobId(unsigned int cob);
        
        /**
         * @brief Helper function that checks whether we support the wrapper cob id
         * Checks whether the cob id falls within the wrapper's accepted message range
         * @param cob the cob id being checked
         * @return true if we support the cob, false otherwise
         */
        bool acceptedWrapperCobId(unsigned int cob);

        /**
         * @brief Helper function that checks whether we support the kpc boot cob id
         * Checks whether the cob id falls within the kpc's accepted message range
         * @param cob the cob id being checked
         * @return true if we support the cob, false otherwise
         */
        bool acceptedKPCBootCobId(unsigned int cob);

         /**
         * @brief Helper function that checks whether we support the kpc boot cob id
         * Checks whether the cob id falls within the kpc's accepted message range
         * @param cob the cob id being checked
         * @return true if we support the cob, false otherwise
         */
        bool acceptedKPCBootUpgraderCobId(unsigned int cob);
         

        /**
		* @brief Builds the board site cob id library
		*
		*/
        void buildBoardSiteLibrary();

         /**
         * @brief Helper function that checks whether we support the board site cob id
         * Checks whether the cob id falls within the board site accepted message range
         * @param cob the cob id being checked
         * @return true if we support the cob, false otherwise
         */
        bool acceptedBoardSiteCobId(unsigned int cob);
        
        /**
         * @brief function that intializes the frame filter based on what type of system we are.
         *		  system types "AWS3 and NGW1 only support CAN upgrade messages
         *		  				NGW3 supports all CAN messages
         */
        void setupFrameFilter();

        //Cob Id Ranges for the Peripherals
        static const canid_t kpcSystemErrCobID = 0x099;
        static const canid_t minPrinter1CobID  = 0x100;
        static const canid_t maxPrinter1CobID  = 0x1FF;
        
        static const canid_t minPrinter2CobID  = 0x200;
        static const canid_t maxPrinter2CobID  = 0x2FF;
        
        static const canid_t minPrinter3CobID  = 0x300;
        static const canid_t maxPrinter3CobID  = 0x3FF;
        
        static const canid_t minWeigherCobID   = 0x400;
        static const canid_t maxWeigherCobID   = 0x4FF;
		
		static const canid_t minWrapperCobID   = 0x500;
        static const canid_t maxWrapperCobID   = 0x720;
        
        static const canid_t minKPCBootCobID   = 0x040;
        static const canid_t maxKPCBootCobID   = 0x07F;

        static const canid_t minKPCBootUpgraderCobID   = 0x0B0;
        static const canid_t maxKPCBootUpgraderCobID   = 0x0F0;

        static const canid_t minARKBoardSiteCobID      = 0x0F1;
        static const canid_t maxARKBoardSiteCobID      = 0x0FF;

        //map of outgoing messages
        typedef std::map< WgEvent, std::vector<canid_t> > WgCanMap;
        WgCanMap wgCanMap;
        typedef std::map< PrEvent, std::vector<canid_t> > PrCanMap;
        PrCanMap prCanMap;
        PrCanMap pr2CanMap;
        PrCanMap pr3CanMap;
        typedef std::map< WrEvent, std::vector<canid_t> > WrCanMap;
        WrCanMap wrCanMap;
        typedef std::map< KPCBootEvent, std::vector<canid_t> > KpcBootCanMap;
        KpcBootCanMap kpcBootCanMap;

        typedef std::map< KPCBootUpgraderEvent, std::vector<canid_t> > KpcBootUpgraderCanMap;
        KpcBootUpgraderCanMap kpcBootUpgraderCanMap;

        typedef std::map< BoardSiteEvent, std::vector<canid_t> > BrdSiteCanMap;
        BrdSiteCanMap brdSiteCanMap;

        /**
         * @struct CanLibMessage
         * @brief Stores information about each CAN peripheral message
         * 
         */
        typedef struct {
            canid_t cobId;              /**<cobId. The CAN cob id*/
            unsigned short eventType;   /**<eventType. The peripheral event*/
            int msgSize;                /**<msgSize. Size of the CAN message in bytes*/
        } CanLibMessage;

        //list of in comming messages
        typedef std::list<CanLibMessage> MsgList;
        MsgList prList;
        MsgList pr2List;
        MsgList pr3List;
        MsgList wrList;
        MsgList wgList;
        MsgList kpcBootList;
        MsgList kpcBootUpgraderList;
        MsgList brdSiteList;

        typedef std::vector<canid_t> FrameFilter;
        FrameFilter filter;

        std::string mySystemType;
#endif // __linux__

};
/** @} */
#endif
