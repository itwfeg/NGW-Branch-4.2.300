/**
 * @file CANWrapperManager.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Alex Zavala
 * @brief The header file for the CAN Wrapper Manager
 * 
 * @addtogroup CAN
 * @{
 */
#ifndef CANWrapperManager_H
#define CANWrapperManager_H

#include "CEvolMsgReceiver.h"
#include "CHobartTimer.h"

#include "OpModeEvent.h"
#include "WeigherEvent.h"
#include "WrapperEvent.h"
#include "SystemEvent.h"
#include "WrapperSensors.h"
#include "WrapperErrorHistory.h"
#include "WrapperStatus.h"
#include "EvolWrapperStatus.h"
#include "WrapperPackageHistory.h"
#include "EvolWrapperPackageHistory.h"
#include "EvolWrapperCameraScan.h"
#include "EvolWrapperSensors.h"
#include "EvolWrapperPackageDesc.h"
#include "VersionManagerEvent.h"
#include "EvolWrapperPkgDetectorStatus.h"
#include "EvolWrapperTest.h"

class Configuration;
class EvolWrapperTrayNames;
class EvolWrapperPackageTimings;

/**
 * @class CANWrapperManager
 * @brief Manager for the CAN Wrapper
 * Manages these specific actions: 
 * -sending/requesting messages to/from the wrapper
 * -tracking sensor/status information
 * -checking wrapper peripheral connectivity
 * -keeping track of package and package error history
 */
class CANWrapperManager : public Hobart::CEvolMsgReceiver, public Hobart::CHobartTimer {
    public:
        static int ignoreWakeups;
        
        /**
         * @brief the CANWrapperManager constructor
         * 
         * @param pMyWrapper pointer to the wrapper message handler
         * @param currentOpMode the operating mode passed in
         * @param wrKind the kind of wrapper we are creating this manager for
         */
        CANWrapperManager(Hobart::CEvolMsgReceiver *pMyWrapper = NULL,
                       OpModeEvent currentOpMode = OpModeOperate,
                       WrapperKind wrKind = WrNGWWrapper);
        
        /**
         * @brief the CAN Wrapper Manager destructor
         * 
         */
        ~CANWrapperManager();
        
        /**
         * @brief The timeout handler for the timers created in the Wrapper manager
         * 
         * @param timer_id the id of the timer that timed out
         */
        virtual void timeoutProcess( __int32 timer_id );
        
        /**
         * @brief Event handler for incoming messages
         * 
         * @param msg the message received
         */
        virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );
        
        /**
         * @brief Checks if the wrapper kind is of type Evol Wrapper
         * 
         * @return true if the wrapper kind is EvolutionWrapper, false otherwise
         */
        bool isWrapperEvolWrapper();
        
    protected:
        ULONG wrapperProbeTime_;
        ULONG requestSensorTime;
        ULONG wrapperTestTime_;
        bool probeWrapper;
        bool trackCameras;
        bool upgradeInProgress;
        bool requestSensorReading;
        bool trackSensors;
        
    private:
        /**
         * @enum CANWrapperManagerTimer
         * @brief Enumeration of the type of timers
         * Contains the collection of Wrapper Manager Timers
         */
        enum CANWrapperManagerTimer {
            wrapperProbe = 1,       /**< wrapperProbe. Wrapper peripheral connectivity timer*/
            wrapperNoProbeResponse, /**< wrapperNoProbeResponse. Currently unused*/
            sensorTimerExpired,     /**< sensorTimerExpired.  For tracking wrapper sensor changes.*/
            statusTimerExpired,     /**< statusTimerExpired.  For tracking wrapper status changes.*/
            wrapperTestTimerExpired /**< wrapperTestTimerExpired. For checking whether a test timed out.*/
        };
        int probeTimeoutCntr_;
        
        Hobart::CEvolMsgReceiver *pMyWrapper;
        Configuration *pConfig;

        INT32 ticksPerSecond;

        WrapperSensors lastSenseMsg;
        EvolWrapperSensors lastEvolWrapperSensors;

        WrapperErrorHistory lastErrorHistoryMsg;                //UWS error history
        WrapperPackageHistory wrapperPackageHistory_;

        EvolWrapperPackageHistory evolWrapperPackageHistory_;

        WrapperStatus lastWrapperStatus;
        EvolWrapperStatus lastEvolWrapperStatus;

        OpModeEvent currentOpMode;
        UINT16 physicalAddress;

        bool probeTimerOn;
        bool savingWrapperTimings_;
        bool waitingToCycleWrapper_;

        WrapperKind wrKind;

        bool sendGripperPWMTimings_;
        bool requestedErrorHistory_;
        bool requestedPkgTimings_;
        bool requestWrapperModelUI_;

        EvolWrapperTrayNames *wrTrayNames_;
        EvolWrapperTest *holdTestMsg_;

        EvolWrapperCameraScan lastLeftScan;
        EvolWrapperCameraScan lastRightScan;

        EvolWrapperPkgDetectorStatus elevatorDetectorStatus;
        EvolWrapperPkgDetectorStatus sealerDetectorStatus;

        ULONG evolWrapperCycleCount_;
//======================================================================== EVENT HANDLERS ===========================================================
        /**
         * @brief The handler for Wrapper messages
         * 
         * @param wrEvent the type of wrapper event
         * @param recvBuffer the char array containing the wrapper message
         */
        void handleWrapperEvent(WrEvent wrEvent, const char *recvBuffer);
        
        /**
         * @brief Event handler for Wrapper system events
         * 
         * @param sysEvent the system event type
         * @param recvBuffer the buffer containing the wrapper system message
         */
        void handleWrapperSysEvent(SysEvent sysEvent, const char *recvBuffer);
//======================================================================== MISC. FUNCTIONS ==========================================================
        /**
         * @brief Registers the list of Wrapper Messages
         * 
         */
        void registerWrapperEvents();
        
        /**
         * @brief Registers the list of Wrapper System messages
         * 
         */
        void registerWrapperSystemEvents();
        
        /**
         * @brief Restarts the timer that checks whether we still have communication with the Wrapper
         * 
         */
        void receivedWrapperFeedback();
//======================================================================== PACKAGE FUNCTIONS ========================================================
        /**
         * @brief Helper function for determining whether we need to add this package to the wrapper package history
         *
         * @param packCode the package code
         * @param width the width of the package
         * @param length the length of the package
         */
        void addPackageToHistory(WrPackCode packCode, UINT16 width, UINT16 length);
        
        /**
         * @brief Adds a package's details to the package history
         * 
         * @param wrPackageDesc the package description used for details
         */
        void addPackageToEvolHistory(EvolWrapperPackageDesc &wrPackageDesc);
//======================================================================== SEND MESSAGES ============================================================
        /**
         * @brief Does necessary checks before sending a message to the Wrapper
         * 
         * @param buffer the message to be sent to the Wrapper
         */
        void sendWrapperMessage(const char* buffer);
        
        /**
         * @brief Dispatches the kind of Wrapper this manager is using
         * 
         */
        void sendWrapperKind();
        
        /**
         * @brief Send a wrapper test to the Wrapper
         * 
         * @param testMsg the message specifying which test to run
         */
        void sendEvolWrapperTest(EvolWrapperTest *testMsg = 0);     //  HA-81
//======================================================================== REQUEST MESSAGES =========================================================
        /**
         * @brief Requests a wakeup from the Wrapper
         * 
         */
        void requestWakeupMsg();
        
        /**
         * @brief Requests status information from the Wrapper
         * 
         */
        void requestStatusMsg();
        
        /**
         * @brief Requests sensor information from the Wrapper
         * 
         */
        void requestSensorMsg();
        
        /**
         * @brief Sends a request for a configuration from the Wrapper
         * 
         */
        void requestInquireMsg();
        
        /**
         * @brief Requests an error history of the Wrapper
         * 
         */
        void requestErrorHistoryMsg();
        
        /**
         * @brief Requests a cycle count from the Wrapper
         * 
         */
        void requestEvolWrapperCycleCount();
        
        /**
         * @brief Requests a system information message from the Wrapper
         * 
         */
        void requestEvolWrapperSystemInfo();
        
        /**
         * @brief Requests data points from a specific wrapper camera
         * 
         * @param whichCamera the camera from which we are requesting data
         */
        void requestCamerasScanMsg(EvolWrapperTest::WrEvolCameraTestParam whichCamera);
        
        /**
         * @brief Requests a system wakeup from the Wrapper
         * 
         */
        void requestSystemWakeupMsg();
        
        /**
         * @brief Requests a system reset of the Wrapper
         * 
         */
        void requestEvolWrapperSystemReset();//Added 4/24/14 Alex Zavala

//======================================================================== GETTERS/SETTERS ==========================================================
        /**
         * @brief Getter for the wrapper's tray names
         * 
         * @return a pointer to the list of tray names
         */
        EvolWrapperTrayNames* getEvolWrapperTrayNames() const;
        
        /**
         * @brief Setter for the wrapper's tray names
         * 
         * @param trayNames the list of new tray names
         */
        void setEvolWrapperTrayNames(EvolWrapperTrayNames *trayNames);
        
        /**
         * @brief Getter for the wrapper's cycle count
         * 
         * @return the wrapper's cycle count
         */
        ULONG getWrapperCycleCount() const;
        
        /**
         * @brief Setter for the wrapper's cycle count
         * 
         * @param count the wrapper's cycle count
         */
        void setWrapperCycleCount(ULONG count);
        
#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};
/** @} */
#endif //CANWrapperManager_H