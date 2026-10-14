/**
 * @file CANWrapperManager.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Alex Zavala
 * @brief Responsible for taking the appropriate actions from sending/receiving messages from the Wrapper and UI
 * 
 */
#include "WrapperEvent.h"
#include "Wrapper.h"
#include "CANWrapperManager.h"
#include "Configuration.h"
#include "EncoderPositionRange.h"

#include "WrapperInquire.h"
#include "WrapperWakeup.h"
#include "WrapperConfiguration.h"
#include "WrapperGeneric.h"
#include "WrapperMode.h"
#include "WrapperOpen.h"
#include "WrapperSensors.h"
#include "WrapperRequestInquire.h"
#include "WrapperRequestSensors.h"
#include "WrapperRequestStatus.h"
#include "WrapperRequestWakeup.h"
#include "WrapperSystemStatus.h"
#include "WrapperSystemConfiguration.h"
#include "WrapperSystemInquire.h"
#include "WrapperSystemRequestStatus.h"
#include "WrapperSystemRequestInquire.h"
#include "WrapperSystemRequestWakeup.h"
#include "WrapperRequestErrorHistory.h"
#include "WrapperStatusTracking.h"
#include "WrapperSensorsTracking.h"
#include "WrapperErrorHistoryTracking.h"
#include "WrapperTest.h"
#include "WrapperLoad.h"
#include "WrapperWrapperKind.h"

#include "WrapperJog.h"
#include "WrapperCycle.h"
#include "WrapperWrap.h"
#include "WrapperControl.h"

#include "EvolWrapperWakeup.h"
#include "EvolWrapperStatus.h"
#include "EvolWrapperControl.h"
#include "EvolWrapperLoad.h"
#include "EvolWrapperWrap.h"
#include "EvolWrapperInquire.h"
#include "EvolWrapperConfiguration.h"
#include "EvolWrapperPackageTimings.h"
#include "EvolWrapperRequestPackageTimings.h"
#include "EvolWrapperTest.h"
#include "EvolWrapperTestComplete.h"
#include "EvolWrapperCycle.h"
#include "EvolWrapperPackageTimingsPointer.h"
#include "EvolWrapperResetDefaultPackageTimings.h"
#include "EvolWrapperUnlockPackageTimings.h"
#include "EvolWrapperTrayNames.h"
#include "EvolWrapperCameraScan.h"
#include "EvolWrapperSystemStatus.h"
#include "EvolWrapperErrorHistory.h"

#include "EvolWrapperCameraCalibration.h"
#include "EvolWrapperCameraThreshold.h"
#include "EvolWrapperWriteCameraThreshold.h"
#include "EvolWrapperSystemInfo.h"
#include "EvolWrapperSystemInfoWrite.h"
#include "EvolWrapperReqSystemReset.h"
#include "EvolWrapperMotorProfile.h"

#if 0
#include "EvolWrapperPackageHistory.h"
#endif

#include "EvolWrapperCameraHeightScan.h"
#include "EvolWrapperCamerasTracking.h"
#include "WeigherStatus.h"
#include "UpgradePeripheral.h"
#include "EvolWrapperKnifeTime.h"
#include "EvolWrapperPackageDischarge.h"
#include "EvolWrapperReqUIModel.h"
#include "EvolWrapperUIModel.h"
#include "HobDateTime.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "Long.h"
#include "Bool.h"
#include "HobString.h"

#include "PAL_platformheader.h"

#ifdef USE_CYCLE_COUNTER
    #include "EvolWrapperCycleCount.h"
    #include "EvolWrapperSaveCycleCount.h"
    #include "EvolWrapperReqCycleCount.h"
#endif

int CANWrapperManager::ignoreWakeups = 0;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CANWrapperManager::dataType = 0;
std::string CANWrapperManager::dataTypeName("CANWrapperManager"); 
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
CANWrapperManager::CANWrapperManager(Hobart::CEvolMsgReceiver *pMyWrapper,
        OpModeEvent currentOpMode,
        WrapperKind wrKind) {
    HobString debugHeader = HobString("CANWrapperManager(): ");
    Debug::trace("CANWrapperManager", debugHeader+"-- Entry");

#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
    //Parameter variables
    this->pMyWrapper                = pMyWrapper;
    this->physicalAddress           = physicalAddress;
    this->currentOpMode             = currentOpMode;
    this->wrKind                    = wrKind;
    
    //Configuration variables
    pConfig                         = Configuration::getInstance();
    ticksPerSecond                  = 10000;
    probeTimeoutCntr_               = 0;
    wrapperProbeTime_               = pConfig->getWrapperProbeTime() * ticksPerSecond;
    requestSensorTime               = ticksPerSecond / 8;
    
    //Boolean flags
    probeWrapper                    = true;
    trackSensors                    = false;
    trackCameras                    = false;
    sendGripperPWMTimings_          = false;
    requestedErrorHistory_          = false;
    requestWrapperModelUI_          = false;
    savingWrapperTimings_           = false;
    waitingToCycleWrapper_          = false;
    requestedPkgTimings_            = false;
    upgradeInProgress               = false;
#ifdef USE_CYCLE_COUNTER
    evolWrapperCycleCount_          = 0;
#endif

    //Wrapper manager pointers
    wrTrayNames_                    = NULL;
    holdTestMsg_                    = NULL;
    (currentOpMode == OpModeService) ? probeTimerOn = false : probeTimerOn = true;
    
    //Start Wrapper Timers
    createTimer(wrapperProbeTime_, wrapperProbe, CHobartTimer::PERIODIC_TIMER);

    // Register for Wrapper messages
    registerWrapperEvents();
    registerWrapperSystemEvents();

    //Dispatch the wrapper kind
    sendWrapperKind();  
    
    //Request a configuration from the wrapper
    requestInquireMsg();

    //get the wrap box timings
    if(pMyWrapper) {
        EvolWrapperRequestPackageTimings rtm(EvolWrapperRequestPackageTimings::CurrentTimings);
        const char *pCC = rtm.serialize();
        pMyWrapper->send(pCC, rtm.getSerializedLength());

        requestedPkgTimings_ = true;
    } else {
        Debug::trace("CANWrapperManager", debugHeader+"pMyWrapper is NULL! Unable to send EvolWrapperRequestPackageTimings");
    }
    Debug::trace("CANWrapperManager", debugHeader+"-- Exit");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CANWrapperManager::~CANWrapperManager() {
    HobString debugHeader = HobString("~CANWrapperManager(): ");
    Debug::trace("CANWrapperManager", debugHeader+"Entry");

#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
    cancelTimer(wrapperProbe);
    cancelTimer(sensorTimerExpired);

    deregisterAllEvents();
    if (wrKind == WrUltimaWrapper) {
        WrapperStatus lastWrapperStatus;
        lastWrapperStatus.setManagerDeleted();
        dispatchEvent(&lastWrapperStatus);
    } else {
        lastEvolWrapperStatus = EvolWrapperStatus();

        lastEvolWrapperStatus.setManagerDeleted();
        dispatchEvent(&lastEvolWrapperStatus);
    }

    if(wrTrayNames_) {
        delete wrTrayNames_;
    }
    //shutdown the msg thread
    ShutdownNicely();
    Debug::trace("CANWrapperManager", debugHeader+"Exit");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg ) {
    HobString debugHeader = HobString("processEvent(): ");
    if( msg.get() != NULL ) {
        switch (msg->getDeviceType()) {
            case Wrapper:
                handleWrapperEvent((WrEvent) msg->getEventType(), msg->Data());
                break;
            case WrapperSystem:
                handleWrapperSysEvent((SysEvent) msg->getEventType(), msg->Data());
                break;
            default:
                Debug::trace("CANWrapperManager.Error", debugHeader+"***** Unknown message, Device Type = " +
                         inttostring(msg->getDeviceType()) + ", EventType = " + inttostring(msg->getEventType()));
                break;
        }
    } else {
        Debug::trace("CANWrapperManager.Error", debugHeader+"Received NULL message");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::handleWrapperEvent(WrEvent wrEvent, const char *recvBuffer) {
    HobString debugHeader = HobString("handleWrapperEvent(): ");
    switch (wrEvent) {
        case WrWakeup: {
            Debug::trace("CANWrapperManager.ProbeWrapper", debugHeader+"WrWakeup");
            WrapperWakeup wrWakeup(recvBuffer);
            wrWakeup.setSourceObject(this);

            dispatchEvent(&wrWakeup);
            receivedWrapperFeedback();
            break;
        }
        case WrEvolReqSystemReset: {//Added 4/24/14 Alex Zavala
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolReqSystemReset");
            requestEvolWrapperSystemReset();
            receivedWrapperFeedback();
            break;
        }
        case WrEvolWakeup: {
            if (CANWrapperManager::ignoreWakeups == 0) {
                Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolWakeup");
                EvolWrapperWakeup wrWakeup(recvBuffer);
                wrWakeup.setSourceObject(this);
                
                dispatchEvent(&wrWakeup);
                receivedWrapperFeedback();
            } else {
                Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolWakeup: ignoring WrEvolWakeup received!");
            }
            break;
        }
        case WrInquire: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrInquire");
            WrapperInquire wi(recvBuffer);
            wi.setSourceObject(this);

            if( sendGripperPWMTimings_ ) {
                Debug::trace("CANWrapperManager.PWMTimings", debugHeader+"WrInquire: sendGripperPWMTimings_ is true");

                // Change the gripper PWM timings
                WrapperTime time(0, 60, 255);
                WrapperGripperTiming& timing0 = wi.getWrapperGripperTiming0();
                timing0.setSetback(time);
                timing0.setPwm074();

                WrapperGripperTiming& timing1 = wi.getWrapperGripperTiming1();
                time.setValue(100);
                timing1.setSetback(time);
                timing1.setPwm074();

                WrapperGripperTiming& timing2 = wi.getWrapperGripperTiming2();
                time.setValue(140);
                timing2.setSetback(time);
                timing2.setPwm074();

                WrapperGripperTiming& timing3 = wi.getWrapperGripperTiming3();
                time.setValue(225);
                timing3.setSetback(time);
                timing3.setPwm038();

                // Now make it a config message and send it down to the wrapper
                wi.show();
                wi.setEventType(WrConfig);
                const char *pCC = wi.serialize();
                if(pMyWrapper) {
                    pMyWrapper->send(pCC, wi.getSerializedLength());
                } else {
                    Debug::trace("CANWrapperManager.Error", debugHeader+"WrInquire: pMyWrapper is NULL! Unable to send WrConfig");
                }
                
                // Send a ReqInquire so that with the reply we report the new settings
                WrapperRequestInquire wri(this);
                pCC = wri.serialize();
                
                if(pMyWrapper) {
                    pMyWrapper->send(pCC, wri.getSerializedLength());
                } else {
                    Debug::trace("CANWrapperManager.Error", debugHeader+"WrInquire: pMyWrapper is NULL! Unable to send WrEvolRequestInquire");
                }
                Debug::trace("CANWrapperManager.PWMTimings", debugHeader+"WrInquire: setting sendGripperPWMTimings_ to false");
                sendGripperPWMTimings_ = false;
            } else {
                dispatchEvent(&wi);
                if(Debug::isTracing("CANWrapperManager.WrapperInquire") == true) {
                    wi.show();
                }
            }
            receivedWrapperFeedback();
            break;
        }
        case WrEvolInquire: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolInquire");
            EvolWrapperInquire wi(recvBuffer);
            wi.setSourceObject(this);

            if (Debug::isTracing("CANWrapperManager.Message") == true) {
                HobString m(recvBuffer);
                HobString h = HobString("EvolWrapperInquire message received: ") + HobString(Hex::toString(m.data(), sizeof(EvolWrapperInquire)));
                Debug::trace("CANWrapperManager.Message", debugHeader+h);

                Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolInquire: wi.show()  ------------------------------------------- START");
                wi.show();
                Debug::trace("CANWrapperManager.Message", "WrEvolInquire: wi.show()  ------------------------------------------- END");
            }
            
            dispatchEvent(&wi);
            receivedWrapperFeedback();
            break;
        }
        case WrOpen: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrOpen");
            WrapperOpen wrOpen(recvBuffer);
            wrOpen.setSourceObject(this);
            dispatchEvent(&wrOpen);
            // test code for new evolution wrapper
            if(wrKind == WrEvolWrapper) {
                wrOpen.show();
            }
            receivedWrapperFeedback();
            break;
        }
        case WrReqStatus: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrReqStatus");
            WrapperRequestStatus wrs(recvBuffer);
            wrs.setSourceObject(this);
            const char *pCC = wrs.serialize();
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wrs.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrReqStatus");
            }
            
            if (wrKind == WrUltimaWrapper) {
                lastWrapperStatus = WrapperStatus();
            } else {
                EvolWrapperStatus tmp;
                lastEvolWrapperStatus = tmp;
            }
            break;
        } 
        case WrStatusTracking: {
            Debug::trace("CANWrapperManager.Error", debugHeader+"WrStatusTracking -- warning this code has been removed");
            break;
        }
        case WrStatus: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrStatus");
            WrapperStatus statusMsg(recvBuffer);

            if(!statusMsg.equals(lastWrapperStatus)) {
                //  Retain the current state of lastWrapperStatus.isPeripheralComm()
                //  for when we call receivedWrapperFeedback() at the end.
                bool isComm = lastWrapperStatus.isPeripheralComm();
                Debug::trace("CANWrapperManager.Message", debugHeader+"WrStatus: Status is different.");
                
                if(Debug::isTracing("CANWrapperManager.WrapperStatus") == true) {
                    statusMsg.show();
                }

                WrapperStateInfo thisStateInfo(statusMsg.getStatusInfo().getStateInfo());
                WrapperStateInfo lastStateInfo(lastWrapperStatus.getStatusInfo().getStateInfo());

                if((thisStateInfo.isIdle() == true) && (lastStateInfo.isIdle() == false)) {
                    Debug::trace("CANWrapperManager.Error", debugHeader+"WrStatus: Display Idle Wrapper Error");
                }

                // Maintain package history out of packages that go through the
                // elevator (which appears to be the strategy used in Ultima)
                WrapperPackageInfo& packageInfo = statusMsg.getPackageInfo(ElevatorPosition);
                if( packageInfo.isPresent() ) {
                    addPackageToHistory(packageInfo.getPackCode(), packageInfo.getWidth(), packageInfo.getLength());
                }
                statusMsg.setSourceObject(this);

                if(isComm) {
                    dispatchEvent(&statusMsg);
                }
                lastWrapperStatus = statusMsg;
                lastWrapperStatus.setPeripheralComm(isComm);
            }
            receivedWrapperFeedback();
            break;
        }
        case WrEvolStatus: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolStatus");
            EvolWrapperStatus statusMsg(recvBuffer);

            if(!statusMsg.equals(lastEvolWrapperStatus)) {
                //  Retain the current state of lastEvolWrapperStatus.isPeripheralComm()
                //  for when we call receivedWrapperFeedback() at the end.
                bool isComm = lastEvolWrapperStatus.isPeripheralComm();
                if(Debug::isTracing("CANWrapperManager.WrapperStatus") == true) {
                    statusMsg.show();
                }

                statusMsg.setSourceObject(this);
                if(isComm) {
                    dispatchEvent(&statusMsg);
                }
                lastEvolWrapperStatus = statusMsg;
                lastEvolWrapperStatus.setPeripheralComm(isComm);
            }
            receivedWrapperFeedback();
            break;
        }
        case WrConfig: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrConfig");
            WrapperConfiguration wc(recvBuffer);
            wc.setSourceObject(this);
            const char *pCC = wc.serialize();
            
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wc.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrConfig");
            }
            if(Debug::isTracing("CANWrapperManager.WrapperConfig") == true) {
                wc.show();
            }
            break;
        }
        case WrEvolConfig: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolConfig *******************  TESTING for Config!!!");
            EvolWrapperConfiguration wc(recvBuffer);
            wc.setSourceObject(this);

            if (Debug::isTracing("EvolWrapperConfiguration") == true) {
                wc.show();
            }
            const char *pCC = wc.serialize();
            
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wc.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrEvolConfig");
            }
            break;
        }
        case WrAbort: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrAbort");
            sendMessageToWrapper(recvBuffer);
            break;
        }
        case WrEnable: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEnable");
            sendMessageToWrapper(recvBuffer);
            break;
        }
        case WrDisable: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrDisable");
            sendMessageToWrapper(recvBuffer);
            break;
        }
        case WrReset: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrReset");
            sendMessageToWrapper(recvBuffer);
            break;
        }
        case WrResume: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrResume");
            sendMessageToWrapper(recvBuffer);
            break;
        }
        case WrPause: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrPause");
            sendMessageToWrapper(recvBuffer);
            break;
        }
        case WrReqErrorHistory: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrReqErrorHistory");
            sendMessageToWrapper(recvBuffer);

            lastErrorHistoryMsg = WrapperErrorHistory();
            requestedErrorHistory_ = true;
            break;
        }
        case WrMode: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrMode");
            WrapperMode wm(recvBuffer);
            wm.setSourceObject(this);
            const char *pCC = wm.serialize();
            
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wm.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrMode");
            }
            break;
        }
        case WrTest: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrTest");
            WrapperTest wt(recvBuffer);
            wt.setSourceObject(this);
            const char *pCC = wt.serialize();
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wt.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrTest");
            }
            if(Debug::isTracing("CANWrapperManager.WrapperTest") == true) {
                wt.show();
            }
            break;
        }
        case WrReqSensors: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrReqSensors");
            sendMessageToWrapper(recvBuffer);
            lastSenseMsg = WrapperSensors();
            break;
        }
        case WrSensorsTracking: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrSensorsTracking");
            WrapperSensorsTracking wrTracking(recvBuffer);
            trackSensors = wrTracking.getSensorsTracking();

            if(trackSensors) {
                lastSenseMsg = WrapperSensors();
                createTimer(requestSensorTime*10, sensorTimerExpired, CHobartTimer::PERIODIC_TIMER);
                requestSensorMsg();
            } else {
                cancelTimer( sensorTimerExpired );
            }
            break;
        }
        case WrSensors: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrSensors");
            WrapperSensors wrapperSense(recvBuffer);
            wrapperSense.setSourceObject(this);

            if(!wrapperSense.equals(lastSenseMsg)) {
                if ((currentOpMode == OpModeService) &&
                        (wrapperSense.getEncoderCount() == MarkPosition)) {
                    Debug::trace("CANWrapperManager.Error", debugHeader+"WrSensors: Send Beep Message");
                }
                lastSenseMsg = wrapperSense;
                dispatchEvent(&wrapperSense);
            }
            receivedWrapperFeedback();
            break;
        }
        case WrErrorHistoryTracking: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrErrorHistoryTracking -- warning this code has been removed");
            WrapperErrorHistoryTracking wrErrorHistoryTracking(recvBuffer);
            break;
        }
        case WrErrorHistory: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrErrorHistory");

            WrapperErrorHistory wrapperErrorHistory(recvBuffer);
            wrapperErrorHistory.setSourceObject(this);

            if (wrapperErrorHistory.equals(lastErrorHistoryMsg) == false || requestedErrorHistory_ ) {
                if (Debug::isTracing("CANWrapperManager.ErrorHistory") == true) {
                    wrapperErrorHistory.show();
                }

                lastErrorHistoryMsg = wrapperErrorHistory;
                dispatchEvent(&wrapperErrorHistory);
                requestedErrorHistory_ = false;
            }
            receivedWrapperFeedback();
            break;
        }   
        case WrReqInquire: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrReqInquire");
            requestInquireMsg();
            break;
        }
        case WrReqWakeup: {
            if (CANWrapperManager::ignoreWakeups == 0) {
                Debug::trace("CANWrapperManager.Message", debugHeader+"WrReqWakeup");

                WrapperRequestWakeup wrw(recvBuffer);
                wrw.setSourceObject(this);
                wrw.setPhysicalAddress(physicalAddress);
                const char *pCC = wrw.serialize();
                if(pMyWrapper) {
                    pMyWrapper->send(pCC, wrw.getSerializedLength());
                } else {
                    Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrReqWakeup");
                }
            } else {
                //lostContact = true;
                Debug::trace("CANWrapperManager.ProbeWrapper", debugHeader+"WrReqWakeup: Ignoring WrReqWakeup");
            }
            break;
        }
        case WrReqKind: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrReqKind");
            sendWrapperKind();
            break;
        }
        case WrWrap: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrWrap");
            WrapperWrap ww(recvBuffer);
            ww.setSourceObject(this);
            const char *pCC = ww.serialize();
            if(pMyWrapper) {
                pMyWrapper->send(pCC, ww.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrWrap");
            }
            if (Debug::isTracing("CANWrapperManager.WrapperWrap") == true) {
                ww.show();
            }
            break;
        }
        case WrCycle: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrCycle");
            WrapperCycle wc(recvBuffer);
            wc.setSourceObject(this);
            const char *pCC = wc.serialize();
            
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wc.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrCycle");
            }
            if (Debug::isTracing("CANWrapperManager.WrapperCycle") == true) {
                wc.show();
            }
            break;
        }
        case WrJog: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrJog");
            WrapperJog wj(recvBuffer);
            wj.setSourceObject(this);
            const char *pCC = wj.serialize();
            
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wj.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrJog");
            }
            break;
        }
        case WrControl: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrControl");
            WrapperControl wrControl(recvBuffer);
            wrControl.setSourceObject(this);
            const char *pCC = wrControl.serialize();
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wrControl.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrControl");
            }
            break;
        }
        case WrLoad: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrLoad");
            WrapperLoad wrLoad(recvBuffer);
            wrLoad.setSourceObject(this);
            const char *pCC = wrLoad.serialize();
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wrLoad.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrLoad");
            }
            break;
        }
        case WrEvolControl: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolControl *******************  TESTING for Control!!!");
            EvolWrapperControl wc(recvBuffer);
            wc.setSourceObject(this);

            if (Debug::isTracing("EvolWrapperControl") == true) {
                wc.show();
            }

            const char *pCC = wc.serialize();
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wc.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrEvolControl");
            }
            break;
        }
        case WrEvolLoad: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolLoad");
            EvolWrapperLoad wrLoad(recvBuffer);
            wrLoad.setSourceObject(this);
            const char *pCC = wrLoad.serialize();
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wrLoad.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrEvolLoad");
            }
            break;
        }
        case WrEvolWrap: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolWrap");
#ifdef USE_CYCLE_COUNTER
            evolWrapperCycleCount_++;
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolWrap:  evolWrapperCycleCount_ = " + inttostring(evolWrapperCycleCount_));
#endif
            EvolWrapperWrap ww(recvBuffer);
            ww.setSourceObject(this);
            const char *pCC = ww.serialize();
            
            if(pMyWrapper) {
                pMyWrapper->send(pCC, ww.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrEvolWrap");
            }
            if (Debug::isTracing("CANWrapperManager.WrapperWrap") == true) {
                ww.show();
            }
            break;
        }
        case WrSetGripperPWMTimings: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrSetGripperPWMTimings");
            // Send a ReqInquire so that with the reply we send the Gripper PWM Timings
            Debug::trace("CANWrapperManager.PWMTimings", "Sending a request for inquire");
            WrapperRequestInquire wri(this);
            const char *pCC = wri.serialize();
            
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wri.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to WrSetGripperPWMTimings");
            }
            Debug::trace("CANWrapperManager.PWMTimings", debugHeader+"WrSetGripperPWMTiminings: Setting sendGripperPWMTimings_ to true");
            sendGripperPWMTimings_ = true;
            break;
        }
        case WrReqPackageHistory: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrReqPackageHistory");
            if(wrKind == WrUltimaWrapper) {
                dispatchEvent(&wrapperPackageHistory_);
            } else if(wrKind == WrEvolWrapper) {
                evolWrapperPackageHistory_.setSourceObject(this);
                dispatchEvent(&evolWrapperPackageHistory_);
            }   
            break;
        }
        case WrEvolRequestPackageTimings: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolRequestPackageTimings");
            EvolWrapperRequestPackageTimings wrTimings(recvBuffer);
            wrTimings.setSourceObject(this);
            const char *pCC = wrTimings.serialize();
            
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wrTimings.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrEvolRequestPackageTimings");
            }
            break;
        }
        case WrEvolCamerasTracking: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolCamerasTracking -- NOT PORTED!!");
            EvolWrapperCamerasTracking wrTracking(recvBuffer);

            trackCameras = wrTracking.getCamerasTracking();
            if(trackCameras)
                requestCamerasScanMsg(EvolWrapperTest::LeftCameraScanAnalog);
            break;
        }
        case WrCameraScan: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrCameraScan");
            EvolWrapperCameraScan cameraScan(recvBuffer);
            cameraScan.setSourceObject(this);

            if((cameraScan.getCamera() == LeftCamera && !cameraScan.equals(lastLeftScan)) ||
                 (cameraScan.getCamera() == RightCamera && !cameraScan.equals(lastRightScan))) {
                if(cameraScan.getCamera() == LeftCamera) {
                    lastLeftScan = cameraScan;
                } else {
                    lastRightScan = cameraScan;
                }
                dispatchEvent(&cameraScan);
            }
            receivedWrapperFeedback();
            break;
        }
        case WrCameraCalibration: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrCameraCalibration");
            EvolWrapperCameraCalibration cameraCal(recvBuffer);
            cameraCal.setSourceObject(this);
            
            dispatchEvent(&cameraCal);
            receivedWrapperFeedback();
            break;
        }
        case WrEvolTest: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolTest");
            EvolWrapperTest *wt = new EvolWrapperTest(recvBuffer);
            if (wt) {
                wt->setSourceObject(this);
                sendEvolWrapperTest(wt);
            }
            break;
        }
        case WrEvolTestComplete: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolTestComplete");
            EvolWrapperTestComplete tcMsg(recvBuffer);
            tcMsg.setSourceObject(this);
            tcMsg.show();

            dispatchEvent(&tcMsg);

            if (( trackCameras ) && ( tcMsg.getTestCode() == EvolWrapperTest::CameraTest ))
            {
                if ( tcMsg.getData1() == EvolWrapperTest::LeftCameraScanAnalog )
                    requestCamerasScanMsg( EvolWrapperTest::RightCameraScanAnalog );
                else if ( tcMsg.getData1() == EvolWrapperTest::RightCameraScanAnalog )
                    requestCamerasScanMsg( EvolWrapperTest::LeftCameraScanAnalog );
                else if ( tcMsg.getData1() == EvolWrapperTest::CameraHeight )
                    requestCamerasScanMsg( EvolWrapperTest::CameraHeight );
            }

            receivedWrapperFeedback();
            break;
        }
        case WrEvolSensors: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolSensors");
            EvolWrapperSensors wrapperSensors(recvBuffer);
            wrapperSensors.setSourceObject(this);
            dispatchEvent(&wrapperSensors);
            receivedWrapperFeedback();
            break;
        }
        case WrEvolCycle: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolCycle");
            EvolWrapperCycle wc(recvBuffer);
            wc.setSourceObject(this);
            const char *pCC = wc.serialize();
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wc.getSerializedLength());
            } else {
                Debug::trace("CANPrinterManager.error", debugHeader+"pMyWrapper is NULL! Unable to send WrEvolCycle");
            }
            if(Debug::isTracing("CANWrapperManager.WrapperCycle") == true) {
                wc.show();
            }
            break;
        }
        case WrEvolPackageDesc: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolPackageDesc");
            EvolWrapperPackageDesc wrPackageDesc(recvBuffer);
            wrPackageDesc.setSourceObject(this);
            string trayName;
            
            if(wrTrayNames_) {
                wrTrayNames_->getTrayName(wrPackageDesc.getWrapBoxIndex(), trayName);
            } else {
                Debug::trace("CANWrapperManager", debugHeader+"WrEvolPackageDesc:  wrTrayNames_ is 0!");
            }
            Debug::trace("CANWrapperManager", debugHeader+"WrEvolPackageDesc:  trayName is " + trayName);
            
            //  Add the tray name to the wrPackageDesc msg also.
            wrPackageDesc.setTrayName(trayName);

            //  Add this wrPackageDesc to the history before we send it to the browser.
            addPackageToEvolHistory(wrPackageDesc);

            dispatchEvent(&wrPackageDesc);
            receivedWrapperFeedback();
            break;
        }
        case WrEvolPackageTimings: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolPackageTimings");

            EvolWrapperPackageTimingsPointer wrTimingsPtr(recvBuffer);
            wrTimingsPtr.setSourceObject(this);

            //  Set up the tray names every time we receive this message, provided it is not the Factory defaults.
            EvolWrapperPackageTimings *wrapperTimings = wrTimingsPtr.getTimingsPointer();

            if(wrapperTimings) {
                EvolWrapperTrayNames *trayNames = new EvolWrapperTrayNames(wrapperTimings);
                if(trayNames) {
                    Debug::trace("CANWrapperManager", debugHeader+"WrEvolPackageTimings:  Setting the new tray names.");
                    setEvolWrapperTrayNames(trayNames);
                }
            }
            if(requestedPkgTimings_) {
				// this creates a small memory leak but allows more than one manager to
				// process this message (and prevents a crash)
                //if(wrapperTimings) {
                //    delete wrapperTimings;
                //}
                requestedPkgTimings_ = false;
            } else {
                dispatchEvent(&wrTimingsPtr);
            }
            receivedWrapperFeedback();
            break;
        }
        case WrEvolSavePackageTimings: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolSavePackageTimings");
            EvolWrapperPackageTimingsPointer wrTimingsPtr(recvBuffer);
            wrTimingsPtr.setSourceObject(this);

            EvolWrapperPackageTimings *wrapperTimings = wrTimingsPtr.getTimingsPointer();
            if(wrapperTimings && (wrapperTimings->getDisposition() == WRITE_PERMANENT || wrapperTimings->getDisposition() == WRITE_TEMPORARY)) {
                Debug::trace("CANWrapperManager", debugHeader+"WrEvolSavePackageTimings:  Getting the tray names from the message.  Disposition = " + HobString(Integer::toString(wrapperTimings->getDisposition())));
                EvolWrapperTrayNames *trayNames = new EvolWrapperTrayNames(wrapperTimings);
                if(trayNames) {
                    Debug::trace("CANWrapperManager", debugHeader+"WrEvolSavePackageTimings:  Setting the new tray names.");
                    setEvolWrapperTrayNames(trayNames);
                }
                const char *pCC = wrTimingsPtr.serialize();
                if(pCC) {
                    if(pMyWrapper) {
                    pMyWrapper->send(pCC, wrTimingsPtr.getSerializedLength());
                    savingWrapperTimings_ = true;
                    } else {
                        Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrEvolSavePackageTimings");
                    }
                } else {
                    Debug::trace("CANWrapperManager.Error", debugHeader+"pCC is NULL! Unable to send WrEvolSavePackageTimings");
                }
            }
            break;
        }
        case WrEvolResetDefaultPackageTimings: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolResetDefaultPackageTimings");
            EvolWrapperResetDefaultPackageTimings resetTimings(recvBuffer);
            resetTimings.setSourceObject(this);
            const char *pCC = resetTimings.serialize();
            if(pMyWrapper) {
                pMyWrapper->send(pCC, resetTimings.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrEvolResetDefaultPackageTimings");
            }
            break;
        }
        case WrEvolSavePackageTimingsComplete: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolSavePackageTimingsComplete");
            WrapperGeneric wsptc(WrEvolSavePackageTimingsComplete, this);

            dispatchEvent(&wsptc);
            receivedWrapperFeedback();

            savingWrapperTimings_ = false;
            if(waitingToCycleWrapper_) {
                sendEvolWrapperTest();
                waitingToCycleWrapper_ = false;
            } else {
                Debug::trace("CANWrapperManager.Message", debugHeader+"waitingToCycleWrapper_ is false, ignoring WrEvolSavePackageTimingsComplete event");
            }
            break;
        }
        case WrEvolCameraHeightScanTracking: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolCameraHeightScanTracking");
            EvolWrapperCamerasTracking wrTracking(recvBuffer);
            trackCameras = wrTracking.getCamerasTracking();
            
            if(trackCameras) {
                requestCamerasScanMsg(EvolWrapperTest::CameraHeight);
            } else {
                Debug::trace("CANWrapperManager.Message", debugHeader+"trackCameras is FALSE, ignoring WrEvolCameraHeightScanTracking event");
            }
            break;
        }
        case WrCameraHeightScan: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrCameraHeightScan");
            EvolWrapperCameraHeightScan heightScan(recvBuffer);
            heightScan.setSourceObject(this);

            dispatchEvent(&heightScan);
            receivedWrapperFeedback();
            break;
        }
        case WrEvolUpgradeInProgress: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolUpgradeInProgress");
            //bool might not be needed since we can cancel the probe timer here! -- ats
            upgradeInProgress = true;
            cancelTimer(wrapperProbe);
            break;
        }
        case WrEvolEventCapture: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolEventCapture");
            EvolWrapperEventCapture msg(recvBuffer);
            
            Debug::trace("CANWrapperManager.Message", debugHeader + "\n" +
				  HobString("event capture type = ") + Integer::toString(msg.getEventCaptureType()) + "\n" +
				  HobString("event pkg id = ") + Integer::toString(msg.getPackageId()) + "\n" +
				  HobString("event value = ") + Long::toString(msg.getValue()) + "\n" +
				  HobString("event error num = ") + Bool::toString(msg.getErrorNumber()));
            
            //get the current date/time stamp of this knifeTime
            HobDateTime dt;
            msg.setEventMonth(dt.getMonth());
            msg.setEventDay(dt.getDay());
            msg.setEventYear(dt.getYear());
            msg.setEventHour(dt.getHour());
            msg.setEventMinutes(dt.getMinute());
            msg.setEventSeconds(dt.getSecond());
            msg.setSourceObject(this);
            
            dispatchEvent(&msg);
            receivedWrapperFeedback();
            break;
        }
        case WrEvolErrorHistory: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolErrorHistory");
            EvolWrapperErrorHistory evolWrapperErrorHistory(recvBuffer);
            evolWrapperErrorHistory.setSourceObject(this);
            
            //do not log any error message from the wrapper with an error code of NoWrapperError (0).
            if(evolWrapperErrorHistory.getWrapperError() != (UINT16)EvolWrapperErrorInfo::NoWrapperError) {
                dispatchEvent(&evolWrapperErrorHistory);
            } else {
                Debug::trace("CANWrapperManager.Message", debugHeader+"received error message with a code of zero from Wrapper");
            }
            receivedWrapperFeedback();
            break;
        }
#ifdef USE_CYCLE_COUNTER
        case WrEvolReqCycleCount: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolReqCycleCount:  evolWrapperCycleCount_ = " + inttostring(evolWrapperCycleCount_));
            EvolWrapperCycleCount evolWrapperCycleCount(this, evolWrapperCycleCount_);
            
            dispatchEvent(&evolWrapperCycleCount);
            break;
        }
        case WrEvolCycleCount: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolCycleCount");
            EvolWrapperCycleCount evolWrapperCycleCount(recvBuffer);
            evolWrapperCycleCount.setSourceObject(this);
            
            evolWrapperCycleCount_ = evolWrapperCycleCount.getWrapperCycleCount();
            
            dispatchEvent(&evolWrapperCycleCount);
            receivedWrapperFeedback();
            break;
        }
        case WrEvolSaveCycleCount: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolSaveCycleCount");
            EvolWrapperSaveCycleCount evolWrapperSaveCycleCount(recvBuffer);
            evolWrapperSaveCycleCount.setWrapperCycleCount(evolWrapperCycleCount_);
            
            evolWrapperSaveCycleCount.setSourceObject(this);
            const char *pCC = evolWrapperSaveCycleCount.serialize();
            if(pMyWrapper) {
                pMyWrapper->send(pCC, evolWrapperSaveCycleCount.getSerializedLength());
            } else {
                Debug::trace("CANPrinterManager.error", debugHeader+"pMyWrapper is NULL! Unable to send WrEvolSaveCycleCount");
            }
            break;
        }
#endif
        case WrEvolStartMfgRunin: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolStartMfgRunin");
            WrapperGeneric msg(WrEvolStartMfgRunin, this);

            dispatchEvent(&msg);
            break;
        }
        case WrEvolPkgDetectorStatus: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolPkgDetectorStatus");
            EvolWrapperPkgDetectorStatus evolWrapperPkgDetectorStatus(recvBuffer);
            
            if(evolWrapperPkgDetectorStatus.isSensorLocationElevator()) {
                elevatorDetectorStatus = evolWrapperPkgDetectorStatus;
            } else if(evolWrapperPkgDetectorStatus.isSensorLocationSealer()) {
                sealerDetectorStatus = evolWrapperPkgDetectorStatus;
            }
            receivedWrapperFeedback();
            break;
        }
        case WrEvolReqElevatorPkgDetectorStatus: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolReqElevatorPkgDetectorStatus");
            elevatorDetectorStatus.setSourceObject(this);
            dispatchEvent(&elevatorDetectorStatus);
            break;
        }
        case WrEvolReqSealerPkgDetectorStatus: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolReqSealerPkgDetectorStatus");
            sealerDetectorStatus.setSourceObject(this);
            dispatchEvent(&sealerDetectorStatus);
            break;
        }
        case WrEvolRequestCameraThreshold: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolRequestCameraThreshold");
            sendMessageToWrapper(recvBuffer);
            break;
        }
        case WrEvolCameraThreshold: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolCameraThreshold");
            EvolWrapperCameraThreshold evolWrapperCameraThreshold(recvBuffer);
            evolWrapperCameraThreshold.setSourceObject(this);
            
            dispatchEvent(&evolWrapperCameraThreshold);
            receivedWrapperFeedback();
            break;
        }
        case WrEvolWriteCameraThreshold: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolWriteCameraThreshold");
            EvolWrapperWriteCameraThreshold evolWrapperWriteCameraThreshold(recvBuffer);
            evolWrapperWriteCameraThreshold.setSourceObject(this);
            const char *pCC = evolWrapperWriteCameraThreshold.serialize();
            
            if(pMyWrapper) {
                pMyWrapper->send(pCC, evolWrapperWriteCameraThreshold.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrEvolWriteCameraThreshold");
            }
            break;
        }
        case WrEvolSystemInfo: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolSystemInfo");
            EvolWrapperSystemInfo ewsi(recvBuffer);         
            ewsi.setSourceObject(this);
            
            dispatchEvent(&ewsi);
            receivedWrapperFeedback();
            break;
        }
        case WrReqEvolSystemInfo: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrReqEvolSystemInfo");
            sendMessageToWrapper(recvBuffer);
            break;
        }
        case WrEvolSystemInfoErase: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolSystemInfoErase");
            sendMessageToWrapper(recvBuffer);
            break;      
        }
        case WrEvolSystemInfoWrite: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolSystemInfoWrite");
            if(pMyWrapper) {
                EvolWrapperSystemInfoWrite ewsiw(recvBuffer);
                ewsiw.setSourceObject(this);
                const char *pCC = ewsiw.serialize();
                if(pCC) {
                    pMyWrapper->send(pCC, ewsiw.getSerializedLength());
                } else {
                    Debug::trace("CANWrapperManager", debugHeader+"pCC is NULL! Unable to send WrEvolSystemInfoWrite");
                }
            } else {
                Debug::trace("CANPrinterManager.error", debugHeader+"pMyWrapper is NULL! Unable to send WrEvolSystemInfoWrite");
            }
            break;  
        }
        case WrEvolManufacturingRunInNotice: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolManufacturingRunInNotice");
            WrapperGeneric  evolManufacturingRunInNotice(recvBuffer);           
            evolManufacturingRunInNotice.setSourceObject(this);
            
            dispatchEvent(&evolManufacturingRunInNotice);
            receivedWrapperFeedback();
            break;
        }
        case WrEvolMotorProfile: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolMotorProfile");
            EvolWrapperMotorProfile evolWrapperMotorProfile(recvBuffer);
            evolWrapperMotorProfile.setSourceObject(this);
            
            dispatchEvent(&evolWrapperMotorProfile);
            receivedWrapperFeedback();
            break;
        }
        case WrEvolPackageDischarge: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrEvolPackageDischarge");
            EvolWrapperPackageDischarge ewpd(recvBuffer);
            ewpd.setSourceObject(this);
            ewpd.show();
            
            dispatchEvent(&ewpd);
            receivedWrapperFeedback();
            break;
        }
        case WrReqUIWrapperModel: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrReqUIWrapperModel:  requestWrapperModelUI_ = " + booltostring(requestWrapperModelUI_));

            EvolWrapperReqUIModel evolWrapperReqUIModel(recvBuffer);
            evolWrapperReqUIModel.setSourceObject(this);
            
            //send this request only once per powerup.
            if(!requestWrapperModelUI_) {
                requestWrapperModelUI_ = true;          
                dispatchEvent(&evolWrapperReqUIModel);
            }
            break;
        }
        case WrUIWrapperModel: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"WrUIWrapperModel");

            EvolWrapperUIModel evolWrapperUIModel(recvBuffer);
            evolWrapperUIModel.setSourceObject(this);
            const char *pCC = evolWrapperUIModel.serialize();
            
            if(pMyWrapper) {
                pMyWrapper->send(pCC, evolWrapperUIModel.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send WrUIWrapperModel");
            }
            break;  
        }
        default: {
            Debug::trace("CANWrapperManager.Error", debugHeader+HobString("unsupported event: 0x") +
                        HobString(Hex::toString((UINT32) wrEvent)));                                                    
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::handleWrapperSysEvent(SysEvent sysEvent, const char *recvBuffer) {
    HobString debugHeader = HobString("handleWrapperSysEvent(): ");
    switch (sysEvent) {
        case SysWakeup: {
            Debug::trace("CANWrapperManager.SystemMessage", debugHeader+"SysWakeup");
            receivedWrapperFeedback();
            break;
        }
        case SysConfig: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"SysConfig");
            
            WrapperSystemConfiguration wsc(recvBuffer);
            wsc.setSourceObject(this);
            const char *pCC = wsc.serialize();
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wsc.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send SysConfig");
            }
            break;
        }
        case SysInquire: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"SysInquire");
            WrapperSystemInquire wsi(recvBuffer);
            wsi.setSourceObject(this);
            
            dispatchEvent(&wsi);
            receivedWrapperFeedback();
            break;
        }
        case SysStatus: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"SysStatus");
            WrapperSystemStatus wss(recvBuffer);
            wss.setSourceObject(this);
            
            dispatchEvent(&wss);
            receivedWrapperFeedback();
            break;
        }
        case SysReqStatus: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"SysReqStatus");
            WrapperSystemRequestStatus wsrs(recvBuffer);
            wsrs.setSourceObject(this);
            const char *pCC = wsrs.serialize();
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wsrs.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send SysReqStatus");
            }
            break;
        }
        case SysReqInquire: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"SysReqInquire");
            WrapperSystemRequestInquire wsri(recvBuffer);
            wsri.setSourceObject(this);
            const char *pCC = wsri.serialize();
            if(pMyWrapper) {
                pMyWrapper->send(pCC, wsri.getSerializedLength());
            } else {
                Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Unable to send SysReqInquire");
            }
            break;
        }
        case SysUsbWrStatus: {
            Debug::trace("CANWrapperManager.Message", debugHeader+"SysUsbWrStatus");

            EvolWrapperSystemStatus wss(recvBuffer);
            wss.setSourceObject(this);
            dispatchEvent(&wss);

            //  PTR 1565:   Each time we receive a WR_SYS_STATUS message, request the WR_SYSTEM_INFO message.
            //              This should ensure a check for upgrade for older Mfg. images.
            requestEvolWrapperSystemInfo();
            receivedWrapperFeedback();
            break;
        }
        default: {
            Debug::trace("CANWrapperManager.Error", debugHeader+HobString("Received unsupported SysEvent: 0x") +
                    HobString(Hex::toString((UINT32) sysEvent)));
            break;
        }        
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::addPackageToHistory(WrPackCode packCode, UINT16 width, UINT16 length) {
    HobString debugHeader = HobString("addPackageToHistory(): ");
    Debug::trace("CANWrapperManager.PackageHistory", debugHeader+"New package wrapped: packcode " +
            inttostring(packCode) + ", width = " + uinttostring(width) + ", length = " + uinttostring(length));

    if( PackCode0 <= packCode && packCode <= PackCode11 ) {
        wrapperPackageHistory_.addPackage(packCode, width, length);
    } else {
        Debug::trace("CANWrapperManager.PackageHistory", debugHeader+"Discarded package information: packcode out of range");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::addPackageToEvolHistory(EvolWrapperPackageDesc &wrPackageDesc) {
    evolWrapperPackageHistory_.addPackage(wrPackageDesc.getId(), wrPackageDesc.getWidth(), wrPackageDesc.getLength(), wrPackageDesc.getHeight(),
                                            wrPackageDesc.getLip(), wrPackageDesc.getFilmWidthSelection(), wrPackageDesc.getTrayName(),
                                            wrPackageDesc.hasUsedWrapBoxBestFit(), wrPackageDesc.hasModifiedNetStretch());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::timeoutProcess( __int32 timer_id ) {
    if( timer_id == wrapperProbe ) {
        probeTimeoutCntr_++;
        switch (probeTimeoutCntr_) {
            case 0:
            case 1://have received some messages from the wrapper since last timeout.
                break;
            case 2: {
                Debug::trace("CANWrapperManager.WrapperProbe", HobString("Probe Timeout requesting wakeup"));
                requestWakeupMsg();

                break;
            }
            case 3: {
                Debug::trace("CANWrapperManager.ProbeWrapper", " CANWrapperManager has lost contact");
                if((wrKind == WrUltimaWrapper) && (lastWrapperStatus.isPeripheralComm() == true)) {
                    lastWrapperStatus = WrapperStatus();

                    lastWrapperStatus.clearPeripheralComm();
                    dispatchEvent(&lastWrapperStatus);
                } else if((wrKind == WrEvolWrapper) && (lastEvolWrapperStatus.isPeripheralComm() == true)) {
                    if (lastEvolWrapperStatus.getCurrErrorInfo().getGeneralWrapper()-501 == 201) {
                        Debug::trace("CANWrapperManager.ProbeWrapper", "lostContact == true - gen error was already 201 so setting comm to false");
                        lastEvolWrapperStatus = EvolWrapperStatus();
                        lastEvolWrapperStatus.clearPeripheralComm();
                    } else {
                        lastEvolWrapperStatus = EvolWrapperStatus();

                        EvolWrapperErrorInfo ewei = lastEvolWrapperStatus.getCurrErrorInfo();
                        //ewei.setGeneralWrapper(WrapperProbeNoResponse);
                        ewei.setGeneralWrapper(201);
                        lastEvolWrapperStatus.setCurrErrorInfo(ewei);
                        Debug::trace("CANWrapperManager.ProbeWrapper", "lostContact == true - setting gen error to 201");
                    }
                    dispatchEvent(&lastEvolWrapperStatus);
                }
                probeTimeoutCntr_ = 0;
                break;
            }
        }
    } else if( timer_id == sensorTimerExpired ) {
        if (trackSensors == true) {
            requestSensorMsg(); 
        }
    } else if(timer_id == wrapperTestTimerExpired) {
        EvolWrapperReqSystemReset evolWrapperReqSystemReset(this);
        const char *pCC = evolWrapperReqSystemReset.serialize();
        pMyWrapper->send(pCC, evolWrapperReqSystemReset.getSerializedLength()); 
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::registerWrapperEvents() {
    //Wrapper Request Messages
    registerEvent(Wrapper, WrReqWakeup);
    registerEvent(Wrapper, WrReqInquire);
    registerEvent(Wrapper, WrReqStatus);
    registerEvent(Wrapper, WrReqSensors);
    registerEvent(Wrapper, WrReqErrorHistory);
    registerEvent(Wrapper, WrEvolReqSystemReset);//Added 4/24/14 Alex Zavala
    
    //Wrapper Tracking Messages
    registerEvent(Wrapper, WrSensorsTracking);
    registerEvent(Wrapper, WrStatusTracking);
    registerEvent(Wrapper, WrErrorHistoryTracking);
    registerEvent(Wrapper, WrEvolCamerasTracking);
    registerEvent(Wrapper, WrEvolCameraHeightScanTracking);
    
    //Wrapper Messages
    registerEvent(Wrapper, WrAbort);
    registerEvent(Wrapper, WrEnable);
    registerEvent(Wrapper, WrDisable);
    registerEvent(Wrapper, WrReset);
    registerEvent(Wrapper, WrConfig);
    registerEvent(Wrapper, WrMode);
    registerEvent(Wrapper, WrWrap);
    registerEvent(Wrapper, WrPause);
    registerEvent(Wrapper, WrResume);
    registerEvent(Wrapper, WrLoad);
    registerEvent(Wrapper, WrKind);
    
    //Wrapper Package Messages
    registerEvent(Wrapper, WrSetGripperPWMTimings);
    registerEvent(Wrapper, WrReqPackageHistory);
    registerEvent(Wrapper, WrEvolRequestPackageTimings);
    registerEvent(Wrapper, WrEvolSavePackageTimings);
    registerEvent(Wrapper, WrEvolResetDefaultPackageTimings);
    
    registerEvent(Wrapper, WrEvolLoad);
    registerEvent(Wrapper, WrEvolWrap);
    registerEvent(Wrapper, WrEvolConfig);
    registerEvent(Wrapper, WrEvolTest);
    
    //Wrapper Cycle Messages
    registerEvent(Wrapper, WrEvolCycleCount);
    registerEvent(Wrapper, WrEvolReqCycleCount);
    registerEvent(Wrapper, WrEvolSaveCycleCount);
    
    //Wrapper Camera Messages
    registerEvent(Wrapper, WrCameraCalibration);
    registerEvent(Wrapper, WrEvolRequestCameraThreshold);
    registerEvent(Wrapper, WrEvolCameraThreshold);
    registerEvent(Wrapper, WrEvolWriteCameraThreshold);
    
    //Wrapper System Information Messages
    registerEvent(Wrapper, WrReqEvolSystemInfo);
    registerEvent(Wrapper, WrEvolSystemInfoErase);
    registerEvent(Wrapper, WrEvolSystemInfoWrite);
    registerEvent(Weigher, WgStatus);
    
    //Wrapper Upgrade Messages
    registerEvent(Wrapper, WrEvolUpgradeInProgress);
    registerEvent(Wrapper, WrUIWrapperModel);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::registerWrapperSystemEvents() {
    registerEvent(WrapperSystem, SysConfig);
    registerEvent(WrapperSystem, SysReqStatus);
    registerEvent(WrapperSystem, SysReqInquire);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::receivedWrapperFeedback() {
    if ((wrKind == WrUltimaWrapper) && (lastWrapperStatus.isPeripheralComm() == false)) {
        lastWrapperStatus.setPeripheralComm();
        dispatchEvent(&lastWrapperStatus);
        sendWrapperKind();
    } else if((wrKind == WrEvolWrapper) && (lastEvolWrapperStatus.isPeripheralComm() == false || lastEvolWrapperStatus.getCurrErrorInfo().getGeneralWrapper()-501 == 201)) {
        Debug::trace("CANWrapperManager.ProbeWrapper", "receivedWrapperFeedback - lastEvolStatus comm was false or was 201");
        lastEvolWrapperStatus.setPeripheralComm();

        EvolWrapperErrorInfo ewei = lastEvolWrapperStatus.getCurrErrorInfo();
        if (ewei.getGeneralWrapper()-501 == 201) {
            Debug::trace("CANWrapperManager.ProbeWrapper", "receivedWrapperFeedback - lastEvolStatus resetting gen error from 201 to 0");
            ewei.setGeneralWrapper(0);
            lastEvolWrapperStatus.setCurrErrorInfo(ewei);
        }
        dispatchEvent(&lastEvolWrapperStatus);
        sendWrapperKind();
    }
    probeTimeoutCntr_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::requestWakeupMsg() {
    if(pMyWrapper) {
        WrapperRequestWakeup wrw(this, physicalAddress);
        const char *pCC = wrw.serialize();
        pMyWrapper->send(pCC, wrw.getSerializedLength());
    } else {
        Debug::trace("CANWrapperManager.Error", "requestWakeupMsg(): pMyWrapper is NULL!");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::requestStatusMsg() {
    if(pMyWrapper) {
        WrapperRequestStatus reqStatus(this);
        const char *pCC = reqStatus.serialize();
        pMyWrapper->send(pCC, reqStatus.getSerializedLength());
    } else {
        Debug::trace("CANWrapperManager.Error", "requestStatusMsg(): pMyWrapper is NULL!");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::requestSensorMsg() {
    if(pMyWrapper) {
        WrapperRequestSensors reqSense(this);
        const char *pCC = reqSense.serialize();
        pMyWrapper->send(pCC, reqSense.getSerializedLength());
    } else {
        Debug::trace("CANWrapperManager.Error", "requestSensorMsg(): pMyWrapper is NULL!");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::requestInquireMsg() {
    if(pMyWrapper) {
        WrapperRequestInquire wsri(this);
        const char *pCC = wsri.serialize();
        pMyWrapper->send(pCC, wsri.getSerializedLength());
    } else {
        Debug::trace("CANWrapperManager.Error", "requestInquireMsg(): pMyWrapper is NULL!");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::requestErrorHistoryMsg() {
    if(pMyWrapper) {
        WrapperRequestErrorHistory reqErrorHistory(this);
        const char *pCC = reqErrorHistory.serialize();
        pMyWrapper->send(pCC, reqErrorHistory.getSerializedLength());
    } else {
        Debug::trace("CANWrapperManager.Error", "requestErrorHistoryMsg(): pMyWrapper is NULL!");
    }
}

#ifdef USE_CYCLE_COUNTER
///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::requestEvolWrapperCycleCount() {
    if(pMyWrapper) {
        EvolWrapperReqCycleCount evolWrapperReqCycleCount(this);
        const char *pCC = evolWrapperReqCycleCount.serialize();
        pMyWrapper->send(pCC, evolWrapperReqCycleCount.getSerializedLength());
    } else {
        Debug::trace("CANWrapperManager.Error", "requestEvolWrapperCycleCount(): pMyWrapper is NULL!");
    }
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::requestEvolWrapperSystemInfo() {
    if(pMyWrapper) {
        WrapperGeneric wg(WrReqEvolSystemInfo, this);
        const char *pCC = wg.serialize();
        pMyWrapper->send(pCC, wg.getSerializedLength());
    } else {
        Debug::trace("CANWrapperManager.Error", "requestEvolWrapperSystemInfo(): pMyWrapper is NULL!");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::requestCamerasScanMsg(EvolWrapperTest::WrEvolCameraTestParam whichCamera) {
    if(pMyWrapper) {
        // We can only request one camera's data points at a time. Pass which one you want into this method.
        // The choices are listed in EvolWrapperTest.h
        // When we get the WrEvolTestComplete message for it, we will request the other side.
        EvolWrapperTest ewt(this, EvolWrapperTest::CameraTest);
        ewt.setDataInt1((INT16)whichCamera);
        const char *pCC = ewt.serialize();
        pMyWrapper->send(pCC, ewt.getSerializedLength());
    } else {
        Debug::trace("CANWrapperManager.Error", "requestCamerasScanMsg(): pMyWrapper is NULL!");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::requestSystemWakeupMsg() {
    if(pMyWrapper) {
        WrapperSystemRequestWakeup wsrw(this);
        const char *pCC = wsrw.serialize();
        pMyWrapper->send(pCC, wsrw.getSerializedLength());
    } else {
        Debug::trace("CANWrapperManager.Error", "requestSystemWakeupMsg(): pMyWrapper is NULL!");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::requestEvolWrapperSystemReset() {
    if(pMyWrapper) {
        WrapperGeneric wg(WrEvolReqSystemReset, this);
        const char *pCC = wg.serialize();
        pMyWrapper->send(pCC, wg.getSerializedLength());
    } else {
        Debug::trace("CANWrapperManager.Error", "requestEvolWrapperSystemReset(): pMyWrapper is NULL!");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::sendMessageToWrapper(const char* buffer) {
    HobString debugHeader = HobString("sendMessageToWrapper(): ");
    
    WrapperGeneric wg(buffer);
    if(pMyWrapper) {
        wg.setSourceObject(this);
        const char *pCC = wg.serialize();
        if(pCC) {
        } else {
            Debug::trace("CANWrapperManager.Error", debugHeader+"pCC is NULL! Failed to send Wrapper event:"+HobString(Integer::toString(wg.getEventType())));
        }
    } else {
        Debug::trace("CANWrapperManager.Error", debugHeader+"pMyWrapper is NULL! Failed to send Wrapper event:"+HobString(Integer::toString(wg.getEventType())));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::sendWrapperKind() {
    WrapperWrapperKind wrapperKind(this, wrKind);
    dispatchEvent(&wrapperKind);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::sendEvolWrapperTest(EvolWrapperTest *testMsg) {
    HobString debugHeader = HobString("sendEvolWrapperTest(): ");
    Debug::trace("CANWrapperManager.Message", debugHeader + "Begin");
    if(savingWrapperTimings_) {
        //  HA-81: Wait until we get the WrEvolSavePackageTimingsComplete back
        //  before sending any test messages - especially CycleWrapper.
        //  Just save the message for now.
        if(testMsg) {
            holdTestMsg_ = testMsg;
            waitingToCycleWrapper_ = true;
            Debug::trace("CANWrapperManager.Message", debugHeader+"Hold the test message. waitingToCycleWrapper_ = true.");
        }
    } else {
        EvolWrapperTest *tempTestMsg = NULL;

        if(!testMsg && holdTestMsg_) {
            tempTestMsg = holdTestMsg_;
            holdTestMsg_ = NULL;
            Debug::trace("CANWrapperManager.Message", debugHeader+"Using holdTestMsg_");
        } else if(testMsg && !holdTestMsg_) {
            tempTestMsg = testMsg;
            Debug::trace("CANWrapperManager.Message", debugHeader+"Using msg passed in.");
        } else {
            Debug::trace("CANWrapperManager.Message", debugHeader+"Problem! We have no test message!!!");
        }
        if(tempTestMsg) {
            const char *pCC = tempTestMsg->serialize();
            pMyWrapper->send(pCC, tempTestMsg->getSerializedLength());
            Debug::trace("CANWrapperManager.Message", debugHeader+"Test msg sent to wrapper.");

            if (Debug::isTracing("CANWrapperManager.WrapperTest") == true) {
                tempTestMsg->show();
            }
            delete tempTestMsg;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
EvolWrapperTrayNames CANWrapperManager::getEvolWrapperTrayNames() const {
    return wrTrayNames_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANWrapperManager::setEvolWrapperTrayNames(EvolWrapperTrayNames *trayNames) {
    //  Save the current values, but release any current data first.
    if(wrTrayNames_) {
        delete wrTrayNames_;
    }
    wrTrayNames_ = trayNames;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ULONG CANWrapperManager::getWrapperCycleCount() const {
    return evolWrapperCycleCount;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ULONG CANWrapperManager::setWrapperCycleCount(ULONG count) {
    evolWrapperCycleCount_ = count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CANWrapperManager::isWrapperEvolWrapper() {
    return ( wrKind == WrEvolWrapper );
}
