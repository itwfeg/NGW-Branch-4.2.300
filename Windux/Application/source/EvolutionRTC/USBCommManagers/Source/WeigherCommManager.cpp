#include <PAL_platformheader.h>
//#include <winsock2.h>      /* for htons, ntohs, etc. */ // replaced by PAL header


#include "RealTimeController.h"
#include "UsbManager.h"
#include "WeigherManager.h"
#include "WeigherCommManager.h"
#include "USBEvolHeader.h"

#include "Weigher.h"
#include "WeigherWakeup.h"
#include "WeigherStatus.h"
#include "WeigherInquire.h"
#include "WeigherConfiguration.h"
#include "WeigherMode.h"
#include "WrapperOpen.h"
#include "WeigherControl.h"
#include "WeigherRequestWakeup.h"
#include "WeigherCat3DateTime.h"
#include "WeigherCat3PageRequest.h"
#include "WeigherCat3Statistics.h"
#include "WeigherCat3RecordStatus.h"
#include "WeigherCat3AuditRecords.h"
#include "WeigherManager.h"
#include "WeigherSystemWakeup.h"
#include "UsbWeigherSystemStatus.h"
#include "WeigherSystemConfiguration.h"
#include "WeigherSystemInquire.h"
#include "SystemReset.h"
#include "UsbRemoveComponent.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "UInteger.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherCommManager::dataType = 0;
std::string WeigherCommManager::dataTypeName("WeigherCommManager");
#endif

util_thread::Mutex<> WeigherCommManager::instanceMutex_;
// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
WeigherCommManager::WeigherCommManager(USBD_NODE_ID node_id, SUsbDescriptors * descs, WeigherKind wgKind) : Hobart::CEvolMsgReceiver("WeigherCommManager"), CUsbCommBase( descs,node_id ), nodeId( node_id )
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
    instanceMutex_.acquire();
	HobString thisAddr = Hex::toString(PtrToUlong(this));
	Debug::trace("WeigherCommManager", "WeigherCommManager(" + thisAddr + "):  Hello");	
    nodeId = node_id;
	this->wgKind = wgKind;	// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)

    pMyWeigher = NULL;
    registerEvent(System, SysReset);

    Debug::trace("WeigherCommManager", "Create Pipes:");
    createPipes();
    startPipes();       
    
    pMyWeigher = RealTimeController::getInstance()->NewUSBWeigher(nodeId, this, 0x5555, wgKind); // AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)

    usbEvolHdr.setDestinationPhysicalAddress(0x5555);


#if defined ( __linux__ )
    Debug::trace("WeigherCommManager", HobString("PipeHandle =") + Hex::toString(getMyPipeHandle()));
#endif
	Debug::trace("WeigherCommManager", "WeigherCommManager(" + thisAddr + "):  Done");	

    instanceMutex_.release();
}

WeigherCommManager::~WeigherCommManager()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif

	HobString thisAddr = Hex::toString(PtrToUlong(this));

	Debug::trace("WeigherCommManager", "~WeigherCommManager(" + thisAddr + "):  Entry");	

    UsbRemoveComponent removeMe(nodeId);
    dispatchEvent(&removeMe);

    deregisterAllEvents();
	Debug::trace("WeigherCommManager", "~WeigherCommManager():  deregisterAllEvents()");  

    //shutdown the msg thread
    ShutdownNicely();
	Debug::trace("WeigherCommManager", "~WeigherCommManager():  ShutdownNicely()");  

    if ( pMyWeigher )
    {
    	delete pMyWeigher;
    	pMyWeigher = NULL;
    }
	Debug::trace("WeigherCommManager", "~WeigherCommManager():  pMyWeigher = NULL");  

	Debug::trace("WeigherCommManager", "~WeigherCommManager(" + thisAddr + "):  Done.");
}

void WeigherCommManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
    usbEvolHdr.setSourcePhysicalAddress(0x00);
    usbEvolHdr.setSourceLogicalAddress(USBEvolHeader::USER_INTERFACE);

    bool peripheralReply = false;

    switch ( msg->getDeviceType() )
    {
        case Weigher:
            peripheralReply = handleWeigherEvent( (WgEvent) msg->getEventType(), msg->Data() );
            break;
        case WeigherSystem:
            peripheralReply = handleWeigherSysEvent( (SysEvent) msg->getEventType(), msg->Data() );
            break;
        case System:
            peripheralReply = handleSysEvent( (SysEvent) msg->getEventType(), msg->Data() );
            break;
        default:
            break;
    }

}

void WeigherCommManager::sendBulkMessageCallBack( CUsbPipe::EvolPipeType UNUSED(pipe_type), DWORD UNUSED(result), bool UNUSED(user_defined_error)  )
{
    UNREFERENCED_PARAMETER(user_defined_error);
    UNREFERENCED_PARAMETER(result);
    UNREFERENCED_PARAMETER(pipe_type);
}

void WeigherCommManager::receiveErrorCallBack( CUsbPipe::EvolPipeType UNUSED(pipe_type), DWORD error, bool user_defined_error )
{
    UNREFERENCED_PARAMETER(pipe_type);
    printUSBError( error, user_defined_error );
}


bool WeigherCommManager::handleWeigherEvent(WgEvent wgEventType, const char *buffer)
{
	char str[256];
	memset(str, 0, 256);

    DWORD result = OK;
    bool is_custom_code = false;

    switch (wgEventType)
    {
        case WgConfig:
            {
				Debug::trace("WeigherCommManager.Message", "WgConfig");	

				WeigherConfiguration wgConfig(buffer);

				Debug::trace("WeigherCommManager.Message", "WgConfig:  wgKind = " + Integer::toString(wgKind));	

	            if (wgKind == WgUsbAWS)
				{
				// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
				Usb2WgConfig usbWgConfig;
					memset((char *) &usbWgConfig, 0, sizeof(usbWgConfig));

					if (Debug::isTracing("WeigherManager.WeigherConfig") == true)							
					{
						wgConfig.show();
					}

					usbWgConfig.function_code = WgConfig;
					usbWgConfig.configType = (UINT16) ((WgConfigType) htons((u_short) wgConfig.getConfigType()));
					wgConfig.getDeviceSignature((UINT8 *) &usbWgConfig.config.ultimaWgCfg.device_signature);
					usbWgConfig.config.ultimaWgCfg.center_of_maintenance_zone = htonl(wgConfig.getCenterOfMaintenanceZone());
					usbWgConfig.config.ultimaWgCfg.scale_factor = htonl(wgConfig.getScaleFactor());
					usbWgConfig.config.ultimaWgCfg.max_weight = htonl(wgConfig.getMaxWeight());
					usbWgConfig.config.ultimaWgCfg.gain_factor = htonl(wgConfig.getGainFactor());
					usbWgConfig.config.ultimaWgCfg.zero_reference = htonl(wgConfig.getZeroReference());
					usbWgConfig.config.ultimaWgCfg.calibration_reference = htonl(wgConfig.getCalibrationReference());
					usbWgConfig.config.ultimaWgCfg.rate_option_time = htons(wgConfig.getRateOptionTime());
					usbWgConfig.config.ultimaWgCfg.auto_zero_maintenance_limit = htons(wgConfig.getAutoZeroMaintenanceLimit());
					usbWgConfig.config.ultimaWgCfg.prepack_motion_count = htons(wgConfig.getPrepackMotionCount());
					usbWgConfig.config.ultimaWgCfg.initialize_zero_time = htons(wgConfig.getInitializeZeroTime());
					usbWgConfig.config.ultimaWgCfg.auto_zero_time = htons(3);
					usbWgConfig.config.ultimaWgCfg.rate_option_limit = htons(wgConfig.getRateOptionLimit());
					usbWgConfig.config.ultimaWgCfg.zero_reference_limit = htons(wgConfig.getZeroReferenceLimit());
					usbWgConfig.config.ultimaWgCfg.calibration_reference_limit = htons(wgConfig.getCalibrationReferenceLimit());
					usbWgConfig.config.ultimaWgCfg.small_motion_limit = htons(wgConfig.getSmallMotionLimit());
					usbWgConfig.config.ultimaWgCfg.large_motion_limit = htons(wgConfig.getLargeMotionLimit());
					usbWgConfig.config.ultimaWgCfg.large_motion_count = htons(wgConfig.getLargeMotionCount());
					usbWgConfig.config.ultimaWgCfg.small_motion_count = htons(wgConfig.getSmallMotionCount());
					usbWgConfig.config.ultimaWgCfg.no_motion_count = htons(wgConfig.getNoMotionCount());
					usbWgConfig.config.ultimaWgCfg.zero_tracking_option = htons(wgConfig.getZeroTrackingOption());
					usbWgConfig.config.ultimaWgCfg.negative_weight_option = htons(wgConfig.getNegativeWeightOption());
					usbWgConfig.config.ultimaWgCfg.powerup_maintenance_zone_limit = htons(wgConfig.getPowerupMaintenanceZoneLimit());
					usbWgConfig.config.ultimaWgCfg.operate_maintenance_zone_limit = htons(wgConfig.getOperateMaintenanceZoneLimit());
					usbWgConfig.config.ultimaWgCfg.filter_speed = (UINT8) wgConfig.getFilterSpeed();
					usbWgConfig.config.ultimaWgCfg.reference_voltage = wgConfig.getReferenceVoltage();
					usbWgConfig.config.ultimaWgCfg.last_calibration_date = htonl(wgConfig.getLastCalibrationDate());
					usbWgConfig.config.ultimaWgCfg.number_of_calibrations = htons(wgConfig.getNumberOfCalibrations());
					usbWgConfig.config.ultimaWgCfg.last_configuration_date = htonl(wgConfig.getLastConfigurationDate());
					usbWgConfig.config.ultimaWgCfg.number_of_configurations = htons(wgConfig.getNumberOfConfigurations());
					usbWgConfig.config.weigher_type = (UINT8) wgConfig.getWeigherType();

					// Dual-Range Support (haberrc)
#ifndef CHECK_FT
					// For Fastrack, per FT-708, this should always be set to true (dual range limit based on net weight, not gross weight), and never be set to false.
					wgConfig.setNetWeightDualRangeLimitCheck(true);
#endif
					usbWgConfig.config.flags = (UINT8) wgConfig.getFlags();

					usbWgConfig.config.min_weight_to_print = htonl(wgConfig.getMinWeightToPrint());
					usbWgConfig.config.weigher_model = (UINT8) wgConfig.getWeigherModel();

					usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
					usbEvolHdr.setMessageLength(sizeof(usbWgConfig));
					usbEvolHdr.Serialize( str );

					memcpy( &str[10], (char *) &usbWgConfig, sizeof(usbWgConfig));
					sendUsbInterruptMessage( ( const char * ) str,  sizeof(usbWgConfig) + 10, result, is_custom_code );
				}
				else
				{
					// EZ-USB Weighing Device
					UsbWgConfig usbWgConfig;
					memset((char *) &usbWgConfig, 0, sizeof(usbWgConfig));

					if (Debug::isTracing("WeigherManager.WeigherConfig") == true)							
					{
						wgConfig.show();
					}

					usbWgConfig.function_code = WgConfig;
					usbWgConfig.configType = (UINT16) ((WgConfigType) htons((u_short) wgConfig.getConfigType()));
					wgConfig.getDeviceSignature((UINT8 *) &usbWgConfig.config.ultimaWgCfg.device_signature);
					usbWgConfig.config.ultimaWgCfg.center_of_maintenance_zone = htonl(wgConfig.getCenterOfMaintenanceZone());
					usbWgConfig.config.ultimaWgCfg.scale_factor = htonl(wgConfig.getScaleFactor());
					usbWgConfig.config.ultimaWgCfg.max_weight = htonl(wgConfig.getMaxWeight());
					usbWgConfig.config.ultimaWgCfg.gain_factor = htonl(wgConfig.getGainFactor());
					usbWgConfig.config.ultimaWgCfg.zero_reference = htonl(wgConfig.getZeroReference());
					usbWgConfig.config.ultimaWgCfg.calibration_reference = htonl(wgConfig.getCalibrationReference());
					usbWgConfig.config.ultimaWgCfg.rate_option_time = htons(wgConfig.getRateOptionTime());
					usbWgConfig.config.ultimaWgCfg.auto_zero_maintenance_limit = htons(wgConfig.getAutoZeroMaintenanceLimit());
					usbWgConfig.config.ultimaWgCfg.prepack_motion_count = htons(wgConfig.getPrepackMotionCount());
					usbWgConfig.config.ultimaWgCfg.initialize_zero_time = htons(wgConfig.getInitializeZeroTime());
					usbWgConfig.config.ultimaWgCfg.auto_zero_time = htons(3);
					usbWgConfig.config.ultimaWgCfg.rate_option_limit = htons(wgConfig.getRateOptionLimit());
					usbWgConfig.config.ultimaWgCfg.zero_reference_limit = htons(wgConfig.getZeroReferenceLimit());
					usbWgConfig.config.ultimaWgCfg.calibration_reference_limit = htons(wgConfig.getCalibrationReferenceLimit());
					usbWgConfig.config.ultimaWgCfg.small_motion_limit = htons(wgConfig.getSmallMotionLimit());
					usbWgConfig.config.ultimaWgCfg.large_motion_limit = htons(wgConfig.getLargeMotionLimit());
					usbWgConfig.config.ultimaWgCfg.large_motion_count = htons(wgConfig.getLargeMotionCount());
					usbWgConfig.config.ultimaWgCfg.small_motion_count = htons(wgConfig.getSmallMotionCount());
					usbWgConfig.config.ultimaWgCfg.no_motion_count = htons(wgConfig.getNoMotionCount());
					usbWgConfig.config.ultimaWgCfg.zero_tracking_option = htons(wgConfig.getZeroTrackingOption());
					usbWgConfig.config.ultimaWgCfg.negative_weight_option = htons(wgConfig.getNegativeWeightOption());
					usbWgConfig.config.ultimaWgCfg.powerup_maintenance_zone_limit = htons(wgConfig.getPowerupMaintenanceZoneLimit());
					usbWgConfig.config.ultimaWgCfg.operate_maintenance_zone_limit = htons(wgConfig.getOperateMaintenanceZoneLimit());
					usbWgConfig.config.ultimaWgCfg.filter_speed = (UINT8) wgConfig.getFilterSpeed();
					usbWgConfig.config.ultimaWgCfg.reference_voltage = wgConfig.getReferenceVoltage();
					usbWgConfig.config.ultimaWgCfg.last_calibration_date = htonl(wgConfig.getLastCalibrationDate());
					usbWgConfig.config.ultimaWgCfg.number_of_calibrations = htons(wgConfig.getNumberOfCalibrations());
					usbWgConfig.config.ultimaWgCfg.last_configuration_date = htonl(wgConfig.getLastConfigurationDate());
					usbWgConfig.config.ultimaWgCfg.number_of_configurations = htons(wgConfig.getNumberOfConfigurations());
					usbWgConfig.config.weigher_type = (UINT8) wgConfig.getWeigherType();

					usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
					usbEvolHdr.setMessageLength(sizeof(usbWgConfig));
					usbEvolHdr.Serialize( str );

					memcpy( &str[10], (char *) &usbWgConfig, sizeof(usbWgConfig));
					sendUsbInterruptMessage( ( const char * ) str,  sizeof(usbWgConfig) + 10, result, is_custom_code );
				}
		    }
            break;
        case WgMode:
            {
                Debug::trace("WeigherCommManager.Message", "WgMode");

                // Constuct the Evol Message Buffer!!
                WeigherMode wgMode(buffer);
                UltimaWgMode ultimaWgMode;

                ultimaWgMode.function_code = WgMode;
                ultimaWgMode.mode = (UINT8)wgMode.getMode();

                usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
                usbEvolHdr.setMessageLength(sizeof(ultimaWgMode));
                usbEvolHdr.Serialize( str );

                memcpy( &str[10], (char *) &ultimaWgMode, sizeof(ultimaWgMode));
                sendUsbInterruptMessage( ( const char * ) str,  sizeof(ultimaWgMode) + 10, result, is_custom_code );

            }
            break;
        case WgControl:
            {
                Debug::trace("WeigherCommManager.Message", "WgControl");

                // Constuct the Evol Message Buffer!!
                WeigherControl wgControl(buffer);
                UltimaWgControl ultimaWgControl;

                ultimaWgControl.function_code = WgControl;
                ultimaWgControl.control = wgControl.getControl();

                usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
                usbEvolHdr.setMessageLength(sizeof(ultimaWgControl));
                usbEvolHdr.Serialize( str );

                memcpy( &str[10], (char *) &ultimaWgControl, sizeof(ultimaWgControl));
                sendUsbInterruptMessage( ( const char * ) str,  sizeof(ultimaWgControl) + 10, result, is_custom_code );

            }
            break;
        case WgReqStatus:
            {
                Debug::trace("WeigherCommManager.Message", "WgReqStatus");

                UltimaWgGeneric genMsg;
                memset((char *) &genMsg, 0, sizeof(genMsg));

                genMsg.function_code = WgStatus;

                usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
                usbEvolHdr.setMessageLength(sizeof(genMsg));
                usbEvolHdr.Serialize( str );

                memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
                sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
            }
            break;
        case WgReqInquire:
            {
                Debug::trace("WeigherCommManager.Message", "WgReqInquire");

                UltimaWgGeneric genMsg;
                memset((char *) &genMsg, 0, sizeof(genMsg));

                genMsg.function_code = WgInquire;

                usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
                usbEvolHdr.setMessageLength(sizeof(genMsg));
                usbEvolHdr.Serialize( str );

                memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
                sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

            }
            break;
        case WgReqWakeup:
            {
                Debug::trace("WeigherCommManager.Message", "WgReqWakeup");

                WeigherRequestWakeup wgReqWakeup(buffer);

                UltimaWgGeneric genMsg;
                memset((char *) &genMsg, 0, sizeof(genMsg));

                genMsg.function_code = WgWakeup;

                UINT16 physicalAddress = wgReqWakeup.getPhysicalAddress();

                usbEvolHdr.setDestinationPhysicalAddress(physicalAddress);
                usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
                usbEvolHdr.setMessageLength(sizeof(genMsg));
                usbEvolHdr.Serialize( str );

                memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
                sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
            }
            break;
        case WgEnable:
            {
                Debug::trace("WeigherCommManager.Message", "WgEnable");

                UltimaWgGeneric genMsg;
                memset((char *) &genMsg, 0, sizeof(genMsg));

                genMsg.function_code = WgEnable;

                usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
                usbEvolHdr.setMessageLength(sizeof(genMsg));
                usbEvolHdr.Serialize( str );

                memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
                sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
            }
            break;
        case WgDisable:
            {
                Debug::trace("WeigherCommManager.Message", "WgDisable");

                UltimaWgGeneric genMsg;
                memset((char *) &genMsg, 0, sizeof(genMsg));

                genMsg.function_code = WgDisable;

                usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
                usbEvolHdr.setMessageLength(sizeof(genMsg));
                usbEvolHdr.Serialize( str );

                memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
                sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

            }
            break;
        case WgRezero:
            {
                Debug::trace("WeigherCommManager.Message", "WgRezero");

                UltimaWgGeneric genMsg;
                memset((char *) &genMsg, 0, sizeof(genMsg));

                genMsg.function_code = WgRezero;

                usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
                usbEvolHdr.setMessageLength(sizeof(genMsg));
                usbEvolHdr.Serialize( str );

                memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
                sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

            }
            break;
        case WgReset:
            {
                Debug::trace("WeigherCommManager.Message", "WgReset");

                UltimaWgGeneric genMsg;
                memset((char *) &genMsg, 0, sizeof(genMsg));

                genMsg.function_code = WgReset;

                usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
                usbEvolHdr.setMessageLength(sizeof(genMsg));
                usbEvolHdr.Serialize( str );

                memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
                sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

            }
            break;
        case WgColdReset:			//	PTR 387 & 388
        {
            Debug::trace("WeigherCommManager.Message", "WgColdReset");

            UltimaWgGeneric genMsg;
            memset((char *) &genMsg, 0, sizeof(genMsg));

            genMsg.function_code = WgColdReset;

            usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
            usbEvolHdr.setMessageLength(sizeof(genMsg));
            usbEvolHdr.Serialize( str );

            memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
            sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

        }
        break;
        case WgCat3ReqStatistics:
        {
        	Debug::trace("WeigherCommManager.Message", "WgCat3ReqStatistics");

            UltimaWgGeneric genMsg;
            memset((char *) &genMsg, 0, sizeof(genMsg));

            genMsg.function_code = WgCat3ReqStatistics;

            usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
            usbEvolHdr.setMessageLength(sizeof(genMsg));
            usbEvolHdr.Serialize( str );

            memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
            sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

        	break;
        }
        case WgCat3DateTime:
        {
        	Debug::trace("WeigherCommManager.Message", "WgCat3DateTime");
        	break;
        }
        case WgCat3ReqPage:
        {
        	Debug::trace("WeigherCommManager.Message", "WgCat3ReqPage");
        	/*
        	UltimaWgGeneric genMsg;
            memset((char *) &genMsg, 0, sizeof(genMsg));

            genMsg.function_code = WgCat3ReqPage;

            usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
            usbEvolHdr.setMessageLength(sizeof(genMsg));
            usbEvolHdr.Serialize( str );

            memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
            sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
			*/
        	break;
        }
        case WgCat3ReqNextRecords:
        {
        	Debug::trace("WeigherCommManager.Message", "WgCat3ReqNextRecords");
        	break;
        }
        case WgCat3WriteRecord:
        {
        	Debug::trace("WeigherCommManager.Message", "WgCat3WriteRecord");
        	break;
        }
        default             :
            Debug::trace("WeigherCommManager.Error", HobString("Unknown weigher message type: 0x") +
                    HobString(Hex::toString(wgEventType)));
            break;
    }

    if ( result != OK ) //error
    {
        printUSBError( result, is_custom_code );
    }

    return true;
}

bool WeigherCommManager::handleWeigherSysEvent(SysEvent sysEventType, const char *buffer)
{
    char str[255];
    DWORD result = OK;
    bool is_custom_code = false;

    switch (sysEventType)
    {
        case SysReqWakeup:
            {
                Debug::trace("WeigherCommManager.Message","SysReqWakeup");

                SysGenericMsg genMsg;
                memset((char *) &genMsg, 0, sizeof(genMsg));

                genMsg.function_code = SysWakeup;

                usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
                usbEvolHdr.setMessageLength(sizeof(genMsg));
                usbEvolHdr.Serialize( str );

                memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
                sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
            }
            break;
        case SysConfig:
            {
                Debug::trace("WeigherCommManager.Message","SysConfig");

                WeigherSystemConfiguration wgSysConfig(buffer);

                SysConfigMsg sysConfig;

                sysConfig.function_code = SysConfig;
                sysConfig.configType = (UINT16)((SysConfigType) htons((u_short)wgSysConfig.getSysConfigType()));
                wgSysConfig.getDeviceSignature((UINT8 *) &sysConfig.system_config.device_signature);
                sysConfig.system_config.station_address = (DeviceAddress) htons(wgSysConfig.getStationAddress());
                sysConfig.system_config.controller_address = (DeviceAddress) htons(wgSysConfig.getControllerAddress());

                usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
                usbEvolHdr.setMessageLength(sizeof(sysConfig));
                usbEvolHdr.Serialize( str );

                memcpy( &str[10], (char *) &sysConfig, sizeof(sysConfig));
                sendUsbInterruptMessage( ( const char * ) str,  sizeof(sysConfig) + 10, result, is_custom_code );
            }
            break;
        case SysReqStatus:
            {
                Debug::trace("WeigherCommManager.Message", "SysReqStatus");

                SysGenericMsg genMsg;
                memset((char *) &genMsg, 0, sizeof(genMsg));

                genMsg.function_code = SysStatus;

                usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
                usbEvolHdr.setMessageLength(sizeof(genMsg));
                usbEvolHdr.Serialize( str );

                memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
                sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
            }
            break;
        case SysReqInquire:
            {
                Debug::trace("WeigherCommManager.Message", "SysReqInquire");

                SysGenericMsg genMsg;
                memset((char *) &genMsg, 0, sizeof(genMsg));

                genMsg.function_code = SysInquire;

                usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
                usbEvolHdr.setMessageLength(sizeof(genMsg));
                usbEvolHdr.Serialize( str );

                memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
                sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
            }
            break;
		case SysReset:
		{
                          Debug::trace("WeigherCommManager.Message", "SysReset");

                          SysGenericMsg genMsg;
                          memset((char *) &genMsg, 0, sizeof(genMsg));

                          genMsg.function_code = SysReset;

                          usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
                          usbEvolHdr.setMessageLength(sizeof(genMsg));
                          usbEvolHdr.Serialize( str );

                          memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
                          sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
                      }
                      break;
        default:
                      Debug::trace("WeigherCommManager.Error", HobString("Unknown weigher system message type: 0x") +
                              HobString(Hex::toString(sysEventType)));
                      break;
    }

    if ( result != OK ) //error
    {
        printUSBError( result, is_custom_code );
    }

    return true;
}

bool WeigherCommManager::handleSysEvent(SysEvent sysEventType, const char *buffer)
{
    char str[255];
    DWORD result = OK;
    bool is_custom_code = false;

    switch (sysEventType)
    {
        case SysReset:
            {
                Debug::trace("WeigherCommManager.Message", "SysReset");

                SystemReset sysReset(buffer);

                if (sysReset.getUsbNodeId() == nodeId)
                {
                    SysGenericMsg genMsg;
                    memset((char *) &genMsg, 0, sizeof(genMsg));

                    genMsg.function_code = SysReset;

                    usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
                    usbEvolHdr.setMessageLength(sizeof(genMsg));
                    usbEvolHdr.Serialize( str );

                    memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
                    sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
                }
            }
            break;
        default:
            Debug::trace("WeigherCommManager.Error", HobString("Unknown system message type: 0x") +
                    HobString(Hex::toString(sysEventType)));
            break;
    }

    if ( result != OK ) //error
    {
        printUSBError( result, is_custom_code );
    }

    return true;
}

void WeigherCommManager::processUsbMessage( std::string & msg, CUsbPipe::EvolPipeType UNUSED(pipe_type) )
{
    UNREFERENCED_PARAMETER(pipe_type);

    const char * ptr = msg.c_str();

    USBEvolHeader usbEvolHeader((char *) ptr, false);

    UINT16 physicalAddress = usbEvolHeader.getSourcePhysicalAddress();
    UINT16 logicalAddress = usbEvolHeader.getSourceLogicalAddress();

	ptr += 10;
    if (logicalAddress == USBEvolHeader::SYSTEM)
    {    	
        receivedWeigherSysMessage((SysGenericMsg *) ptr); //, physicalAddress);
    }
    else if (logicalAddress == USBEvolHeader::WEIGHER)
    {
        receivedWeigherDevMessage((UltimaWgGeneric *) ptr, physicalAddress); //, logicalAddress);
    }
    else
    {
		ptr -= 10;
    	Debug::trace("WeigherCommManager.Error", HobString("Bad Header!!!:") + Hex::toString( ptr, 10, true ));
    	ptr += 10;
    	Debug::trace("WeigherCommManager.Error", HobString("Body:") + Hex::toString( ptr, 2, true ));
    }
 }

void WeigherCommManager::receivedWeigherSysMessage(SysGenericMsg *genMsg) // , HobThread *pHobThread)
{

    switch (genMsg->function_code)
    {
        case SysWakeup:
            {
                Debug::trace("WeigherCommManager.Message", "SysWakeup");

                SysWakeupMsg *sysWakeupMsg = (SysWakeupMsg *) genMsg;

                WeigherSystemWakeup wswu(this, (SysCfgStatus) sysWakeupMsg->sysCfgStatus);

                const char *pWswuMsg = wswu.serialize();
                int mLength = wswu.getSerializedLength();

                if (pMyWeigher != NULL)
                    pMyWeigher->send(pWswuMsg, mLength);
            }
            break;
        case SysStatus:
            {
                Debug::trace("WeigherCommManager.Message", "SysStatus");
                // Fix PT #3576, AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
                if (wgKind == WgUsbAWS)
                {
					// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
					Usb2WgSysStatus *sysStatusMsg = (Usb2WgSysStatus *) genMsg;

                    UsbWeigherSystemStatus sysStatus(this,
                            myWeigherType,
                            wgKind);
                    sysStatus.setFlags(ntohl(sysStatusMsg->flags));
                    sysStatus.setInitFlags(ntohl(sysStatusMsg->init_flags));

                    sysStatus.setSerialNumber(ntohl(sysStatusMsg->weigher_serial_num));

                    sysStatus.setSoftwareWeigherVersion(ntohs(sysStatusMsg->software_major_revision), ntohs(sysStatusMsg->software_minor_revision), (UINT16)0);
                    sysStatus.setHardwareVersion(sysStatusMsg->hardware_major_revision, sysStatusMsg->hardware_minor_revision);

                    if ( Debug::isTracing("WeigherCommManager.WeigherSystemStatus") )
                        sysStatus.show();

                    const char *pSysStatus = sysStatus.serialize();
                    int mLength = sysStatus.getSerializedLength();

                    if (pMyWeigher != NULL)
						pMyWeigher->send(pSysStatus, mLength);
                }
                else
                {
				    // EZ-USB Weighing Device
                    UsbWgSysStatus *sysStatusMsg = (UsbWgSysStatus *) genMsg;

                    UsbWeigherSystemStatus sysStatus(this,
                            myWeigherType,
                            wgKind);
                    sysStatus.setFlags(ntohl(sysStatusMsg->flags));
                    sysStatus.setInitFlags(ntohl(sysStatusMsg->init_flags));

                    sysStatus.setSerialNumber(ntohl(sysStatusMsg->serial_number));

                    sysStatus.setSoftwareVersion(ntohs(sysStatusMsg->software_major_revision), ntohs(sysStatusMsg->software_minor_revision));
                    sysStatus.setHardwareVersion(sysStatusMsg->hardware_major_revision, sysStatusMsg->hardware_minor_revision);

                    if ( Debug::isTracing("WeigherCommManager.WeigherSystemStatus") )
                        sysStatus.show();

                    const char *pSysStatus = sysStatus.serialize();
                    int mLength = sysStatus.getSerializedLength();

                    if (pMyWeigher != NULL)
                        pMyWeigher->send(pSysStatus, mLength);
                }
            }
            break;
        case SysInquire:
            {
                Debug::trace("WeigherCommManager.Message", "SysInquire");

                SysInquireMsg *systemInquireMsg = (SysInquireMsg *) genMsg;

                WeigherSystemInquire wsi(this,
                        (SysConfigType) ntohs(systemInquireMsg->configType),
                        (UINT8 *) &systemInquireMsg->system_config.device_signature,
                        (DeviceAddress) ntohs(systemInquireMsg->system_config.station_address),
                        (DeviceAddress) ntohs(systemInquireMsg->system_config.controller_address));

                const char *pWsiMsg = wsi.serialize();
                int mLength = wsi.getSerializedLength();

                if (pMyWeigher != NULL)
                    pMyWeigher->send(pWsiMsg, mLength);
            }
            break;
        default:
            Debug::trace("WeigherCommManager.Error", HobString("Received Unknown weigher system message type: 0x") +
                    HobString(Hex::toString(genMsg->function_code)));
            break;
    }
}

void WeigherCommManager::receivedWeigherDevMessage(UltimaWgGeneric *genMsg, UINT16 physicalAddress) // , HobThread *pHobThread)
{
    switch (genMsg->function_code)
    {
        case WgWakeup:
            {
                Debug::trace("WeigherCommManager.Message", "WgWakeup:  physicalAddress = " + HobString(Hex::toString(physicalAddress)));

                UltimaWgWakeup *wakeupMsg = (UltimaWgWakeup *) genMsg;

                if (pMyWeigher == NULL)
                {
                    //Debug::trace("WeigherManager.WeigherWakeup", "************************************************");

                    // Create a new thread
                    // AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
                    //pMyWeigher = RealTimeController::getInstance()->NewUSBWeigher(nodeId, this, physicalAddress, wgKind); // AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)

                    //usbEvolHdr.setDestinationPhysicalAddress(physicalAddress);
                }
                else
                {
                    WeigherWakeup ww(this, (WgCfgStatus) wakeupMsg->wgCfgStatus);

                    const char *pWwMsg = ww.serialize();
                    int mLength = ww.getSerializedLength();

                    pMyWeigher->send(pWwMsg, mLength);
                }
            }
            break;
        case WgStatus:
		{
			Debug::trace("WeigherCommManager.Message", "WgStatus");
            if (pMyWeigher == NULL) {
                instanceMutex_.acquire();
			if (pMyWeigher == NULL)
			{
				if (( wgKind == WgUsbAWS ) || ( wgKind == WgUsbApc ))
				{
			            Debug::trace("WeigherManager.WeigherWakeup", "************************************************");

			            // Create a new thread
			            // AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
			            pMyWeigher = RealTimeController::getInstance()->NewUSBWeigher(nodeId, this, physicalAddress, wgKind); // AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)

			            usbEvolHdr.setDestinationPhysicalAddress(physicalAddress);
			    }
			}
                instanceMutex_.release();
            }
                // AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
			if (wgKind == WgUsbAWS)
			{
				// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
				Usb2WgStatus *statusMsg = (Usb2WgStatus *) genMsg;

				WeigherStatus ws(this); // use default constructor
				ws.setStatusBits(ntohs(statusMsg->status));
				ws.setCounts(ntohl(statusMsg->counts));
				ws.setRawCounts(ntohl(statusMsg->raw_counts));
				ws.setFilteredUncalibratedCounts(ntohl(statusMsg->filtered_uncalibrated_counts));
				ws.setADCounts(ntohl(statusMsg->AD_counts));

				if ((Debug::isTracing("WeigherCommManager.WeigherStatus") == true) && (ws.isPositiveNonZeroStableWeight() == true))
				{
					ws.show();
				}

				const char *pWsMsg = ws.serialize();
				int mLength = ws.getSerializedLength();

				// pMyWeigher can be NULL, this should be handled
				// TODO: Log this error TRS
                if (pMyWeigher != NULL)
	            	pMyWeigher->send(pWsMsg, mLength);
			}
			else
			{
				// EZ-USB Weighing Device
				UltimaWgStatus *statusMsg = (UltimaWgStatus *) genMsg;

				WeigherStatus ws(this); // use default constructor
				ws.setStatusBits(ntohs(statusMsg->status));
				ws.setCounts(ntohl(statusMsg->counts));
				ws.setRawCounts(ntohl(statusMsg->raw_counts));
				Debug::trace("WeigherCommManager.WeigherStatus", "counts = " + HobString(Integer::toString(statusMsg->counts)) + ",  raw_counts = " + HobString(Integer::toString(statusMsg->raw_counts)));
				ws.setFilteredUncalibratedCounts(ntohl(0));
				ws.setADCounts(ntohl(0));

				if ((Debug::isTracing("WeigherCommManager.WeigherStatus") == true) && (ws.isPositiveNonZeroStableWeight() == true))
				{
					ws.show();
				}

				const char *pWsMsg = ws.serialize();
				int mLength = ws.getSerializedLength();

				if (pMyWeigher != NULL)
					pMyWeigher->send(pWsMsg, mLength);
			}
		}
		break;

		case WgInquire:
		{	
			Debug::trace("WeigherCommManager.Message", "WgInquire");	

			if (wgKind == WgUsbAWS)
			{
				// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
				Debug::trace("WeigherCommManager.Message", "WgInquire - WgUsbAWS");	
				Usb2WgInquire *inquireMsg = (Usb2WgInquire *) genMsg;

				WeigherInquire wi(this); // use default constructor

				wi.setConfigType((WgConfigType) ntohs(inquireMsg->configType));
				wi.setDeviceSignature((UINT8 *) &(inquireMsg->config.ultimaWgCfg.device_signature));
				wi.setCenterOfMaintenanceZone(ntohl(inquireMsg->config.ultimaWgCfg.center_of_maintenance_zone));
				wi.setScaleFactor(ntohl(inquireMsg->config.ultimaWgCfg.scale_factor));
				wi.setMaxWeight(ntohl(inquireMsg->config.ultimaWgCfg.max_weight));
				wi.setGainFactor(ntohl(inquireMsg->config.ultimaWgCfg.gain_factor));
				wi.setZeroReference(ntohl(inquireMsg->config.ultimaWgCfg.zero_reference));
				wi.setCalibrationReference(ntohl(inquireMsg->config.ultimaWgCfg.calibration_reference));
				wi.setRateOptionTime(ntohs(inquireMsg->config.ultimaWgCfg.rate_option_time));
				wi.setAutoZeroMaintenanceLimit(ntohs(inquireMsg->config.ultimaWgCfg.auto_zero_maintenance_limit));
				wi.setPrepackMotionCount(ntohs(inquireMsg->config.ultimaWgCfg.prepack_motion_count));
				wi.setInitializeZeroTime(ntohs(inquireMsg->config.ultimaWgCfg.initialize_zero_time));
				wi.setAutoZeroTime(ntohs(inquireMsg->config.ultimaWgCfg.auto_zero_time));
				wi.setRateOptionLimit(ntohs(inquireMsg->config.ultimaWgCfg.rate_option_limit));
				wi.setZeroReferenceLimit(ntohs(inquireMsg->config.ultimaWgCfg.zero_reference_limit));
				wi.setCalibrationReferenceLimit(ntohs(inquireMsg->config.ultimaWgCfg.calibration_reference_limit));
				wi.setSmallMotionLimit(ntohs(inquireMsg->config.ultimaWgCfg.small_motion_limit));
				wi.setLargeMotionLimit(ntohs(inquireMsg->config.ultimaWgCfg.large_motion_limit));
				wi.setLargeMotionCount(ntohs(inquireMsg->config.ultimaWgCfg.large_motion_count));
				wi.setSmallMotionCount(ntohs(inquireMsg->config.ultimaWgCfg.small_motion_count));
				wi.setNoMotionCount(ntohs(inquireMsg->config.ultimaWgCfg.no_motion_count));
				wi.setZeroTrackingOption(ntohs(inquireMsg->config.ultimaWgCfg.zero_tracking_option));
				wi.setNegativeWeightOption(ntohs(inquireMsg->config.ultimaWgCfg.negative_weight_option));
				wi.setPowerupMaintenanceZoneLimit(ntohs(inquireMsg->config.ultimaWgCfg.powerup_maintenance_zone_limit));
				wi.setOperateMaintenanceZoneLimit(ntohs(inquireMsg->config.ultimaWgCfg.operate_maintenance_zone_limit));
				wi.setFilterSpeed((WgFilterSpeed) inquireMsg->config.ultimaWgCfg.filter_speed);
				wi.setLastCalibrationDate(ntohl(inquireMsg->config.ultimaWgCfg.last_calibration_date));
				wi.setNumberOfCalibrations(ntohs(inquireMsg->config.ultimaWgCfg.number_of_calibrations));
				wi.setLastConfigurationDate(ntohl(inquireMsg->config.ultimaWgCfg.last_configuration_date));
				wi.setNumberOfConfigurations(ntohs(inquireMsg->config.ultimaWgCfg.number_of_configurations));
				wi.setWeigherType((WeigherType) (inquireMsg->config.weigher_type));

				// Dual-Range Support (haberrc)
				wi.setFlags((UINT8)(inquireMsg->config.flags));
				wi.setMinWeightToPrint(ntohl(inquireMsg->config.min_weight_to_print));
				wi.setWeigherModel((WeigherModel) (inquireMsg->config.weigher_model));

				if (Debug::isTracing("WeigherManager.WeigherInquire") == true)
				{
					wi.show();
				}

				if (wi.getAutoZeroTime() != 3)
				{
					wi.setAutoZeroTime(3);								
				}
				
				const char *pWiMsg = wi.serialize();
				int mLength = wi.getSerializedLength();

				if(pMyWeigher)
				{
					pMyWeigher->send(pWiMsg, mLength);
					Debug::trace("WeigherCommManager.Message", "WgInquire - WgUsbAWS - Inquire msg sent to pMyWeigher");	
				}
			}
			else
			{
				// EZ-USB Weighing Device
				Debug::trace("WeigherCommManager.Message", "WgInquire - EZUsb");	
				UsbWgInquire *inquireMsg = (UsbWgInquire *) genMsg;

				WeigherInquire wi(this); // use default constructor

				wi.setConfigType((WgConfigType) ntohs(inquireMsg->configType));
				wi.setDeviceSignature((UINT8 *) &(inquireMsg->config.ultimaWgCfg.device_signature));
				wi.setCenterOfMaintenanceZone(ntohl(inquireMsg->config.ultimaWgCfg.center_of_maintenance_zone));
				wi.setScaleFactor(ntohl(inquireMsg->config.ultimaWgCfg.scale_factor));
				wi.setMaxWeight(ntohl(inquireMsg->config.ultimaWgCfg.max_weight));
				wi.setGainFactor(ntohl(inquireMsg->config.ultimaWgCfg.gain_factor));
				wi.setZeroReference(ntohl(inquireMsg->config.ultimaWgCfg.zero_reference));
				wi.setCalibrationReference(ntohl(inquireMsg->config.ultimaWgCfg.calibration_reference));
				wi.setRateOptionTime(ntohs(inquireMsg->config.ultimaWgCfg.rate_option_time));
				wi.setAutoZeroMaintenanceLimit(ntohs(inquireMsg->config.ultimaWgCfg.auto_zero_maintenance_limit));
				wi.setPrepackMotionCount(ntohs(inquireMsg->config.ultimaWgCfg.prepack_motion_count));
				wi.setInitializeZeroTime(ntohs(inquireMsg->config.ultimaWgCfg.initialize_zero_time));
				wi.setAutoZeroTime(ntohs(inquireMsg->config.ultimaWgCfg.auto_zero_time));
				wi.setRateOptionLimit(ntohs(inquireMsg->config.ultimaWgCfg.rate_option_limit));
				wi.setZeroReferenceLimit(ntohs(inquireMsg->config.ultimaWgCfg.zero_reference_limit));
				wi.setCalibrationReferenceLimit(ntohs(inquireMsg->config.ultimaWgCfg.calibration_reference_limit));
				wi.setSmallMotionLimit(ntohs(inquireMsg->config.ultimaWgCfg.small_motion_limit));
				wi.setLargeMotionLimit(ntohs(inquireMsg->config.ultimaWgCfg.large_motion_limit));
				wi.setLargeMotionCount(ntohs(inquireMsg->config.ultimaWgCfg.large_motion_count));
				wi.setSmallMotionCount(ntohs(inquireMsg->config.ultimaWgCfg.small_motion_count));
				wi.setNoMotionCount(ntohs(inquireMsg->config.ultimaWgCfg.no_motion_count));
				wi.setZeroTrackingOption(ntohs(inquireMsg->config.ultimaWgCfg.zero_tracking_option));
				wi.setNegativeWeightOption(ntohs(inquireMsg->config.ultimaWgCfg.negative_weight_option));
				wi.setPowerupMaintenanceZoneLimit(ntohs(inquireMsg->config.ultimaWgCfg.powerup_maintenance_zone_limit));
				wi.setOperateMaintenanceZoneLimit(ntohs(inquireMsg->config.ultimaWgCfg.operate_maintenance_zone_limit));
				wi.setFilterSpeed((WgFilterSpeed) inquireMsg->config.ultimaWgCfg.filter_speed);
				wi.setLastCalibrationDate(ntohl(inquireMsg->config.ultimaWgCfg.last_calibration_date));
				wi.setNumberOfCalibrations(ntohs(inquireMsg->config.ultimaWgCfg.number_of_calibrations));
				wi.setLastConfigurationDate(ntohl(inquireMsg->config.ultimaWgCfg.last_configuration_date));
				wi.setNumberOfConfigurations(ntohs(inquireMsg->config.ultimaWgCfg.number_of_configurations));
				wi.setWeigherType((WeigherType) (inquireMsg->config.weigher_type));

				if (Debug::isTracing("WeigherManager.WeigherInquire") == true)
				{
					wi.show();
				}

				if (wi.getAutoZeroTime() != 3)
				{
					wi.setAutoZeroTime(3);								
				}
				
				const char *pWiMsg = wi.serialize();
				int mLength = wi.getSerializedLength();

				if(pMyWeigher)
				{
					pMyWeigher->send(pWiMsg, mLength);
					Debug::trace("WeigherCommManager.Message", "WgInquire - EZUsb - Inquire msg sent to pMyWeigher");
				}
			}
		}
		break;
		case WgCat3AuditStatistics:
		{
			/* See APCCommManager.cpp */
			break;
		}
		case WgCat3Records:
		{
			/* See APCCommManager.cpp */
			break;
		}
		case WgCat3RecordStatus:
		{
			/* See APCCommManager.cpp */
			break;
		}
		case WgCat3Error:
		{
			/* See APCCommManager.cpp */
			break;
		}
        default:
            Debug::trace("WeigherCommManager.Error", HobString("Received Unknown weigher message type: 0x") +
                    HobString(Hex::toString(genMsg->function_code)));
            break;
    }
}
