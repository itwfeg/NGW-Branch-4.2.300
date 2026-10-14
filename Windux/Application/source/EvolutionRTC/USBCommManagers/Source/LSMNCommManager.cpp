//#include <netinet/in.h>/* for htonl, ntohl etc */
//#include "vxWorks.h"
#include <PAL_platformheader.h>
//#include <winsock2.h>      /* for htons, ntohs, etc. */ // replaced by PAL header


#include "Configuration.h"
#include "RealTimeController.h"
#include "UsbManager.h"
#include "LSMNCommManager.h"
#include "CommHeader.h"
#include "Weigher.h"
#include "WeigherWakeup.h"
#include "WeigherStatus.h"
#include "WeigherInquire.h"
#include "WeigherConfiguration.h"
#include "WeigherMode.h"
#include "WrapperOpen.h"
#include "WeigherControl.h"
#include "WeigherRequestWakeup.h"

#include "LabelApplier.h"
#include "LabelApplierWakeup.h"
#include "LabelApplierStatus.h"
#include "LabelApplierInquire.h"
#include "LabelApplierConfiguration.h"
#include "LabelApplierMode.h"
#include "LabelApplierTest.h"
#include "LabelApplierLabelReady.h"
#include "LabelApplierLabelTaken.h"
#include "LabelApplierSensors.h"
#include "LabelApplierSetup.h"
#include "LabelApplierOff.h"
#include "LabelApplierPrint.h"
#include "LabelApplierRequestCloseWindow.h"
#include "LabelApplierRequestWakeup.h"
#include "LabelApplierCloseWindow.h"
#include "LabelApplierOpenWindow.h"
#include "LabelApplierPackageReady.h"
#include "LabelApplierUltimaAutoMode.h"

#include "ILACycle.h"

#include "WrapperWakeup.h"
#include "WrapperSensors.h"
#include "WrapperStatus.h"
#include "WrapperInquire.h"
#include "WrapperMode.h"
#include "WrapperTest.h"
#include "WrapperJog.h"
#include "WrapperCycle.h"
#include "WrapperWrap.h"
#include "WrapperControl.h"
#include "WrapperLoad.h"
#include "WrapperErrorHistory.h"
#include "WrapperRequestWakeup.h"

#include "SystemStatusInfo.h"
#include "WeigherSystemWakeup.h"
#include "WeigherSystemStatus.h"
#include "WeigherSystemConfiguration.h"
#include "WeigherSystemInquire.h"

#include "LabelApplierSystemWakeup.h"
#include "LabelApplierSystemStatus.h"
#include "LabelApplierSystemConfiguration.h"
#include "LabelApplierSystemInquire.h"

#include "WrapperSystemWakeup.h"
#include "WrapperSystemStatus.h"
#include "WrapperSystemInquire.h"
#include "WrapperSystemConfiguration.h"

#include "UsbRemoveComponent.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"

#define EP2_MAX_PACKET_SIZE 64

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LSMNCommManager::dataType = 0;
std::string LSMNCommManager::dataTypeName("LSMNCommManager"); 
#endif


LSMNCommManager::LSMNCommManager(USBD_NODE_ID nodeId, SUsbDescriptors * descs ): CUsbCommBase( descs, nodeId ), nodeId( nodeId )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	messageNum = 0;
	xmitting = false;
	recvdUltimaMessage = false;

	this->nodeId = nodeId;


	createPipes();
	startPipes();
}

LSMNCommManager::~LSMNCommManager()
{	
	UsbRemoveComponent removeMe(nodeId, this);
	dispatchEvent(&removeMe);

	//shutdown the msg thread
	ShutdownNicely();

	deregisterAllEvents();

	map<CommHeader::DeviceAddress, UltimaDeviceType *>::iterator pos = evolMgrMap.begin();

	while (pos != evolMgrMap.end())
	{
		if (pos->second != NULL)
		{
			// delete a manager
			delete pos->second->pDeviceMgr;
			delete pos->second;
		}
		pos++;
	}

	evolMgrMap.clear();

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	}

void LSMNCommManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
	{
	try
		{
			UINT16 physicalAddress = 0;
			if (ultimaAddrMap.find(msg->getSourceObject()) != ultimaAddrMap.end())
			{
				physicalAddress = ultimaAddrMap[msg->getSourceObject()];
			}

			commHdr.setFunctionCode(CommHeader::LOW_SPEED_TRANSMIT);
			commHdr.setSourcePhysicalAddress(0x00);
			commHdr.setSourceLogicalAddress(CommHeader::USER_INTERFACE);
			commHdr.setDestinationPhysicalAddress(physicalAddress);
			commHdr.setMessagePriority(0x09);
			commHdr.setDestinationPriority(0x07);
			commHdr.setAllocation(0x01);
			commHdr.setResponse(0x01);
			commHdr.setStatus(0x00);
			commHdr.setBufferLength(sizeof(bfrDataOUT));

			bool peripheralReply = false;

			switch (msg->getDeviceType())
			{
				case Weigher			:	peripheralReply = handleWeigherEvent((WgEvent) msg->getEventType(),  msg->Data());
											break;
				case WeigherSystem		:	peripheralReply = handleWeigherSysEvent((SysEvent) msg->getEventType(), msg->Data());
											break;
				case LabelApplier		:	peripheralReply = handleLabelApplierEvent((LaEvent) msg->getEventType(), msg->Data());
											break;
				case LabelApplierSystem	:	peripheralReply = handleLabelApplierSysEvent((SysEvent) msg->getEventType(), msg->Data());
											break;
				case Wrapper			:	//peripheralReply = handleWrapperEvent((WrEvent) msg->getEventType(), msg->Data());
											break;
				case WrapperSystem		:	peripheralReply = handleWrapperSysEvent((SysEvent) msg->getEventType(), msg->Data());
											break;
				default					:
											break;
			}
			}
	catch (...)
		{
		Debug::trace("LSMNCommManager.Message.Out", "LSMNCommManager::processEvent. caught exception");
		}

	}
	

bool LSMNCommManager::handleWeigherEvent(WgEvent wgEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;
	bool sendPeripheralMessage = false;

	switch (wgEventType)
	{
		case WgConfig:
		{
			Debug::trace("LSMNCommManager.Weigher.Message.Out", "WgConfig");	
			WeigherConfiguration wgConfig(buffer);
			UltimaWgConfig ultimaWgConfig;

			if (Debug::isTracing("WeigherManager.WeigherConfig") == true)							
			{
				wgConfig.show();
			}

			ultimaWgConfig.function_code = WgConfig;
			ultimaWgConfig.configType = htons((UINT16)wgConfig.getConfigType());

			wgConfig.getDeviceSignature((UINT8 *) &ultimaWgConfig.config.device_signature);
			ultimaWgConfig.config.center_of_maintenance_zone = htonl(wgConfig.getCenterOfMaintenanceZone());
			ultimaWgConfig.config.scale_factor = htonl(wgConfig.getScaleFactor());
			ultimaWgConfig.config.max_weight = htonl(wgConfig.getMaxWeight());
			ultimaWgConfig.config.gain_factor = htonl(wgConfig.getGainFactor());
			ultimaWgConfig.config.zero_reference = htonl(wgConfig.getZeroReference());
			ultimaWgConfig.config.calibration_reference = htonl(wgConfig.getCalibrationReference());
			ultimaWgConfig.config.rate_option_time = htons(wgConfig.getRateOptionTime());
			ultimaWgConfig.config.auto_zero_maintenance_limit = htons(wgConfig.getAutoZeroMaintenanceLimit());
			ultimaWgConfig.config.prepack_motion_count = htons(wgConfig.getPrepackMotionCount());
			ultimaWgConfig.config.initialize_zero_time = htons(wgConfig.getInitializeZeroTime());
			ultimaWgConfig.config.auto_zero_time = htons(wgConfig.getAutoZeroTime());
			ultimaWgConfig.config.rate_option_limit = htons(wgConfig.getRateOptionLimit());
			ultimaWgConfig.config.zero_reference_limit = htons(wgConfig.getZeroReferenceLimit());
			ultimaWgConfig.config.calibration_reference_limit = htons(wgConfig.getCalibrationReferenceLimit());
			ultimaWgConfig.config.small_motion_limit = htons(wgConfig.getSmallMotionLimit());
			ultimaWgConfig.config.large_motion_limit = htons(wgConfig.getLargeMotionLimit());
			ultimaWgConfig.config.large_motion_count = htons(wgConfig.getLargeMotionCount());
			ultimaWgConfig.config.small_motion_count = htons(wgConfig.getSmallMotionCount());
			ultimaWgConfig.config.no_motion_count = htons(wgConfig.getNoMotionCount());
			ultimaWgConfig.config.zero_tracking_option = htons(wgConfig.getZeroTrackingOption());
			ultimaWgConfig.config.negative_weight_option = htons(wgConfig.getNegativeWeightOption());
			ultimaWgConfig.config.powerup_maintenance_zone_limit = htons(wgConfig.getPowerupMaintenanceZoneLimit());
			ultimaWgConfig.config.operate_maintenance_zone_limit = htons(wgConfig.getOperateMaintenanceZoneLimit());
			ultimaWgConfig.config.filter_speed = (UINT8)wgConfig.getFilterSpeed();
			ultimaWgConfig.config.reference_voltage = wgConfig.getReferenceVoltage();
			ultimaWgConfig.config.last_calibration_date = htonl(wgConfig.getLastCalibrationDate());
			ultimaWgConfig.config.number_of_calibrations = htons(wgConfig.getNumberOfCalibrations());
			ultimaWgConfig.config.last_configuration_date = htonl(wgConfig.getLastConfigurationDate());
			ultimaWgConfig.config.number_of_configurations = htons(wgConfig.getNumberOfConfigurations());

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
			commHdr.setMessageLength(sizeof(ultimaWgConfig));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &ultimaWgConfig, sizeof(ultimaWgConfig));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(ultimaWgConfig) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WgMode:
		{
			Debug::trace("LSMNCommManager.Weigher.Message.Out", "WgMode");	

			// Constuct the Evol Message Buffer!!
			WeigherMode wgMode(buffer);
			UltimaWgMode ultimaWgMode;

			ultimaWgMode.function_code = WgMode;
			ultimaWgMode.mode = (UINT8)wgMode.getMode();

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
			commHdr.setMessageLength(sizeof(ultimaWgMode));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &ultimaWgMode, sizeof(ultimaWgMode));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(ultimaWgMode) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WgControl:
		{
			Debug::trace("LSMNCommManager.Weigher.Message.Out", "WgControl");	

			// Constuct the Evol Message Buffer!!
			WeigherControl wgControl(buffer);
			UltimaWgControl ultimaWgControl;

			ultimaWgControl.function_code = WgControl;
			ultimaWgControl.control = wgControl.getControl();

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
			commHdr.setMessageLength(sizeof(ultimaWgControl));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &ultimaWgControl, sizeof(ultimaWgControl));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(ultimaWgControl) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WgReqStatus:
		{
			Debug::trace("LSMNCommManager.Weigher.Message.Out", "WgReqStatus");	

			UltimaWgGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = WgStatus;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WgReqInquire:
		{
			Debug::trace("LSMNCommManager.Weigher.Message.Out", "WgReqInquire");	
			UltimaWgGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = WgInquire;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WgReqWakeup:
		{
			Debug::trace("LSMNCommManager.Weigher.Message.Probe", "WgReqWakeup");	
			WeigherRequestWakeup wgReqWakeup(buffer);

			UltimaWgGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = WgWakeup;

			UINT16 physicalAddress = wgReqWakeup.getPhysicalAddress();

			if (evolMgrMap.find(physicalAddress) == evolMgrMap.end())
			{
				usbEvolHdr.setDestinationPhysicalAddress(physicalAddress);
			}

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WgEnable:
		{
			Debug::trace("LSMNCommManager.Weigher.Message.Out", "WgEnable");	

			UltimaWgGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = WgEnable;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WgDisable:
		{
			Debug::trace("LSMNCommManager.Weigher.Message.Out", "WgDisable");	

			UltimaWgGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = WgDisable;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WgRezero:
		{
			Debug::trace("LSMNCommManager.Weigher.Message.Out", "WgRezero");	

			UltimaWgGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = WgRezero;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WgReset:
		{
			Debug::trace("LSMNCommManager.Weigher.Message.Out", "WgReset");	

			UltimaWgGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = WgReset;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WEIGHER);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		default:
			Debug::trace("LSMNCommManager.Weigher.Error", HobString("handleWeigherEvent -- unknown event: 0x") +
															HobString(Hex::toString((UINT32) wgEventType)));							
		break;
	}

	return sendPeripheralMessage;
}

bool LSMNCommManager::handleWeigherSysEvent(SysEvent sysEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;
	bool sendPeripheralMessage = false;

	Debug::trace("LSMNCommManager.Weigher.Message.Out", "handleWeigherSysEvent");

	switch (sysEventType)
	{
		case SysReqWakeup:
		{
			Debug::trace("LSMNCommManager.Weigher.SystemMessage.Out", HobString("LSMN -- SysReqWakeup"));
			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = SysWakeup;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case SysConfig:
		{
			Debug::trace("LSMNCommManager.Weigher.SystemMessage.Out", HobString("LSMN -- SysConfig"));
			WeigherSystemConfiguration wgSysConfig(buffer);

			SysConfigMsg sysConfig;

			sysConfig.function_code = SysConfig;
			sysConfig.configType = htons((UINT16)wgSysConfig.getSysConfigType());
			wgSysConfig.getDeviceSignature((UINT8 *) &sysConfig.system_config.device_signature);
			sysConfig.system_config.station_address = (DeviceAddress) htons(wgSysConfig.getStationAddress());
			sysConfig.system_config.controller_address = (DeviceAddress) htons(wgSysConfig.getControllerAddress());

			commHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setMessageLength(sizeof(sysConfig));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &sysConfig, sizeof(sysConfig));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(sysConfig) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case SysReqStatus:
		{
			Debug::trace("LSMNCommManager.Weigher.SystemMessage.Out", HobString("LSMN -- SysReqStatus"));
			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = SysStatus;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case SysReqInquire:
		{
			Debug::trace("LSMNCommManager.Weigher.SystemMessage.Out", HobString("LSMN -- SysReqInquire"));

			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = SysInquire;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case SysReset:
		{	
			Debug::trace("LSMNCommManager.Weigher.SystemMessage.Out", HobString("Ultima Weigher -- SysReset"));

			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = SysReset;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		default:
		{
			Debug::trace("LSMNCommManager.Weigher.Error", HobString("LSMN -- Some Other System Event: ") +
												 HobString(Integer::toString((UINT16) sysEventType)));
		}
		break;
	}
	return sendPeripheralMessage;
}

bool LSMNCommManager::handleLabelApplierEvent(LaEvent laEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;
	bool sendPeripheralMessage = false;

	Debug::trace("LSMNCommManager.LabelApplier.Message", "handleLabelApplierEvent");

	switch (laEventType)
	{
		case LaConfig:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaConfig");	
			LabelApplierConfiguration laConfig(buffer);

			UltimaLaConfig ultimaLaConfig;

			ultimaLaConfig.function_code = LaConfig;
			ultimaLaConfig.configType = (UINT8) laConfig.getConfigType();

			ultimaLaConfig.config.has_weigher = laConfig.isWeigherStationAttached();
			ultimaLaConfig.config.has_primary_printer = laConfig.isPrimaryStationAttached();
			ultimaLaConfig.config.has_secondary_printer = laConfig.isSecondaryStationAttached();
			ultimaLaConfig.config.left_to_right = laConfig.isLeftToRight();
			ultimaLaConfig.config.front_discharge = laConfig.isFrontDischarge();
			ultimaLaConfig.config.primary_justify_font = laConfig.isPrimaryJustifyFont();
			ultimaLaConfig.config.secondary_justify_font = laConfig.isSecondaryJustifyFont();
			ultimaLaConfig.config.transfer_station_type = (UINT8)laConfig.getTransferStationType();
			ultimaLaConfig.config.primary_printer_label_offset = laConfig.getPrimaryPrinterLabelOffset();
			ultimaLaConfig.config.secondary_printer_label_offset = laConfig.getSecondaryPrinterLabelOffset();
			ultimaLaConfig.config.weigher_conveyor_sensitivity = (UINT8) laConfig.getWeigherConveyorSensitivity();
			ultimaLaConfig.config.primary_printer_conveyor_sensitivity = (UINT8) laConfig.getPrimaryPrinterConveyorSensitivity();
			ultimaLaConfig.config.secondary_printer_conveyor_sensitivity = (UINT8) laConfig.getSecondaryPrinterConveyorSensitivity();
			ultimaLaConfig.config.primary_printer_max_justification_distance = laConfig.getPrimaryPrinterMaxJustificationDistance();
			ultimaLaConfig.config.secondary_printer_max_justification_distance = laConfig.getSecondaryPrinterMaxJustificationDistance();
			ultimaLaConfig.config.diagonal_label_placement = laConfig.isDiagonalLabelPlacement();
			ultimaLaConfig.config.diagonal_offset = laConfig.getDiagonalOffset();
			ultimaLaConfig.config.ministation_label_placement = (UINT8) laConfig.getMiniStationLabelPlacement();
			

			laConfig.getDeviceSignature((UINT8 *) &ultimaLaConfig.config.device_signature);

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(ultimaLaConfig));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &ultimaLaConfig, sizeof(ultimaLaConfig));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(ultimaLaConfig) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaMode:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaMode");	

			// Constuct the Evol Message Buffer!!
			LabelApplierMode laMode(buffer);
			UltimaLaMode ultimaLaMode;

			ultimaLaMode.function_code = LaMode;
			ultimaLaMode.mode = (UINT8)laMode.getMode();

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(ultimaLaMode));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &ultimaLaMode, sizeof(ultimaLaMode));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(ultimaLaMode) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaPackageReady:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaPackageReady");	

			// Constuct the Evol Message Buffer!!
			LabelApplierPackageReady laPackageReady(buffer);
			UltimaLaPackageReady ultimaLaPackageReady;

			ultimaLaPackageReady.function_code = LaPackageReady;
			ultimaLaPackageReady.package_id = laPackageReady.getPackageId();

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(ultimaLaPackageReady));
			commHdr.Serialize( str );
			
			memcpy( &str[19], (char *) &ultimaLaPackageReady, sizeof(ultimaLaPackageReady));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(ultimaLaPackageReady) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaLabelReady:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaLabelReady");	

			// Constuct the Evol Message Buffer!!
			LabelApplierLabelReady laLabelReady(buffer);
			UltimaLaLabelReady ultimaLaLabelReady;

			ultimaLaLabelReady.function_code = LaLabelReady;
			if (laLabelReady.isCommSwitchToPrimary())
				ultimaLaLabelReady.station = PrimaryPrinterStation;			//	Done for the Bakery CLA
			else
				ultimaLaLabelReady.station = (UINT8)laLabelReady.getStationId();

			ultimaLaLabelReady.package_id = laLabelReady.getPackageId();
			ultimaLaLabelReady.pad0 = '\0';

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(ultimaLaLabelReady));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &ultimaLaLabelReady, sizeof(ultimaLaLabelReady));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(ultimaLaLabelReady) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaLabelTaken:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaLabelTaken");	

			// Constuct the Evol Message Buffer!!
			LabelApplierLabelTaken laLabelTaken(buffer);
			UltimaLaLabelTaken ultimaLaLabelTaken;

			ultimaLaLabelTaken.function_code = LaLabelTaken;
			if (laLabelTaken.isCommSwitchToPrimary())
				ultimaLaLabelTaken.station = PrimaryPrinterStation;			//	Done for the Bakery CLA
			else
				ultimaLaLabelTaken.station = (UINT8)laLabelTaken.getStationId();

			ultimaLaLabelTaken.package_id = laLabelTaken.getPackageId();
			ultimaLaLabelTaken.pad0 = '\0';

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(ultimaLaLabelTaken));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &ultimaLaLabelTaken, sizeof(ultimaLaLabelTaken));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(ultimaLaLabelTaken) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaSetup:
		{
			LabelApplierSetup laSetup(buffer);
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", HobString("LaSetup: Rot1 ") +
					HobString(Integer::toString(laSetup.getPrimaryLabelRotationDegreesUltima())));

			UltimaLaSetup setupMsg;
			setupMsg.function_code = LaSetup;

			setupMsg.pri_label_size = htons(laSetup.getPrimaryLabelSize());
			setupMsg.pri_label_rotation = htons(laSetup.getPrimaryLabelRotationDegreesUltima());
			setupMsg.sec_label_size = htons(laSetup.getSecondaryLabelSize());
			setupMsg.sec_label_rotation = htons(laSetup.getSecondaryLabelRotationDegreesUltima());

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(setupMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &setupMsg, sizeof(setupMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(setupMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaReqInquire:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaReqInquire");	

			UltimaLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaInquire;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaReqWakeup:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Probe", "LaReqWakeup");	

			LabelApplierRequestWakeup laRequestWakeup(buffer);

			UltimaLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaWakeup;

			UINT16 physicalAddress = laRequestWakeup.getPhysicalAddress();

			if (evolMgrMap.find(physicalAddress) == evolMgrMap.end())
			{
				usbEvolHdr.setDestinationPhysicalAddress(physicalAddress);
			}

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaReqOff:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaReqOff");	

			UltimaLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaOff;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaOn:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaOn");	

			UltimaLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaOn;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaReqStatus:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaReqStatus");	

			UltimaLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaStatus;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaTest:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaTest");	
			LabelApplierTest laTest(buffer); // use default constructor

			UltimaLaTest testMsg;
			memset((char *) &testMsg, 0, sizeof(testMsg));

			testMsg.function_code = LaTest;

			testMsg.station = (UINT8)laTest.getStationId();
			testMsg.test = (UINT8)laTest.getTestId();
			testMsg.result = htons(laTest.getResult());
			testMsg.parm1 = htons(laTest.getParm1());
			testMsg.parm2 = htons(laTest.getParm2());

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(testMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &testMsg, sizeof(testMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(testMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaReqCloseWindow:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaReqCloseWindow");	

			// Constuct the Evol Message Buffer!!
			LabelApplierRequestCloseWindow requestCloseWindow(buffer);

			UltimaLaCloseWindow closeWindowMsg;
			memset((char *) &closeWindowMsg, 0, sizeof(closeWindowMsg));

			closeWindowMsg.function_code = LaCloseWindow;
			closeWindowMsg.package_id = requestCloseWindow.getPackageId();

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(closeWindowMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &closeWindowMsg, sizeof(closeWindowMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(closeWindowMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaReqSensors:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaReqSensors");	

			UltimaLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaSensors;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaEnable:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaEnable");	

			UltimaLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaEnable;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaDisable:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaDisable");	

			UltimaLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaDisable;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case LaReset:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "LaReset");	

			UltimaLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaReset;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		default:
			Debug::trace("LSMNCommManager.LabelApplier.Error", HobString("handleLabelApplierEvent -- unknown event: 0x") +
															HobString(Hex::toString((UINT32) laEventType)));							
		break;
	}
	return sendPeripheralMessage;
}

bool LSMNCommManager::handleLabelApplierSysEvent(SysEvent sysEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;
	bool sendPeripheralMessage = false;

	Debug::trace("LSMNCommManager.LabelApplier.Message.Out", "handleLabelApplierSysEvent");

	switch (sysEventType)
	{
		case SysReqWakeup:
		{
			Debug::trace("LSMNCommManager.LabelApplier.SystemMessage.Out", HobString("SysReqWakeup"));
			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = SysWakeup;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );
		}
		break;

		case SysConfig:
		{
			Debug::trace("LSMNCommManager.LabelApplier.SystemMessage.Out", HobString("SysConfig"));
			LabelApplierSystemConfiguration laSysConfig(buffer);

			SysConfigMsg sysConfig;

			sysConfig.function_code = SysConfig;
			sysConfig.configType = htons((UINT16)laSysConfig.getSysConfigType());
			laSysConfig.getDeviceSignature((UINT8 *) &sysConfig.system_config.device_signature);
			sysConfig.system_config.station_address =  (DeviceAddress) htons(laSysConfig.getStationAddress());
			sysConfig.system_config.controller_address = (DeviceAddress) htons(laSysConfig.getControllerAddress());

			commHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setMessageLength(sizeof(sysConfig));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &sysConfig, sizeof(sysConfig));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(sysConfig) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case SysReqStatus:
		{
			Debug::trace("LSMNCommManager.LabelApplier.SystemMessage.Out", HobString("SysReqStatus"));
			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaSysStatus;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			commHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case SysReqInquire:
		{
			Debug::trace("LSMNCommManager.LabelApplier.SystemMessage.Out", HobString("SysReqInquire"));
			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = SysInquire;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case SysReset:
		{	
			Debug::trace("LSMNCommManager.LabelApplier.SystemMessage.Out", HobString("SysReset"));

			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = SysReset;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		default:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Error", HobString("Some Other System Event: ") +
													  HobString(Integer::toString((UINT16) sysEventType)));
		}
		break;
	}
	return sendPeripheralMessage;
}
#if 0	//add back if lsmn support is added back to the project
bool LSMNCommManager::handleWrapperEvent(WrEvent wrEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;
	bool sendPeripheralMessage = false;

	Debug::trace("LSMNCommManager.Wrapper.Message", "handleWrapperEvent");

	switch (wrEventType)
	{
		case WrConfig:
		{
			Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrConfig");	

			// Constuct the Evol Message Buffer!!
			WrapperConfiguration wrConfig(buffer);

			UltimaWrConfig configMsg;
			memset((char *) &configMsg, 0, sizeof(configMsg));

			configMsg.wrEvent = WrConfig;
			configMsg.configType = htons((UINT16) wrConfig.getConfigType());
			wrConfig.getDeviceSignature((UINT8 *) &(configMsg.config.deviceSignature));
			configMsg.config.pauseTimeout = wrConfig.getPauseTimeout();
			configMsg.config.idleTimeout = wrConfig.getIdleTimeout();
			configMsg.config.filmTension = (UINT8)wrConfig.getFilmTension();
			configMsg.config.packWidth = (UINT8)wrConfig.getPackWidth();
			configMsg.config.packageGrandTotal = htonl(wrConfig.getPackageGrandTotal());
			configMsg.config.wrapperCycleTotal = htonl(wrConfig.getWrapperCycleTotal());
			configMsg.config.elevatorCollapseTotal = htons(wrConfig.getElevatorCollapseTotal());
			configMsg.config.intakeJamTotal = htons(wrConfig.getIntakeJamTotal());
			configMsg.config.lengthJamTotal = htons(wrConfig.getLengthJamTotal());
			configMsg.config.widthJamTotal = htons(wrConfig.getWidthJamTotal());
			configMsg.config.coverOpenTotal = htons(wrConfig.getCoverOpenTotal());
			configMsg.config.sealerBeltOpenTotal = htons(wrConfig.getSealerBeltOpenTotal());
			configMsg.config.packageTooTallTotal = htons(wrConfig.getPackageTooTallTotal());
			configMsg.config.pusherHomeErrorTotal = htons(wrConfig.getPusherHomeErrorTotal());
			configMsg.config.filmPullErrorTotal = htons(wrConfig.getFilmPullErrorTotal());

			if (wrConfig.isCelsiusEnable() == true)
			{
				configMsg.config.celsius_enable = 1;
			}

			if (wrConfig.isWideFilmTestEnable() == true)
			{
				configMsg.config.wide_film_test_enable = 1;
			}

			if (wrConfig.isNarrowFilmTestEnable() == true)
			{
				configMsg.config.narrow_film_test_enable = 1;
			}

			if (wrConfig.isHeaterEnable() == true)
			{
				configMsg.config.heater_enable = 1;
			}

			configMsg.config.heaterSetting = htons(wrConfig.getHeaterSetting());
			configMsg.config.packageLengthAdjustment = htons(wrConfig.getPackageLengthAdjustment());
			configMsg.config.packageWidthAdjustment = htons(wrConfig.getPackageWidthAdjustment());
			configMsg.config.errorTotalsDate = htonl(wrConfig.getErrorTotalsDate());
			configMsg.config.packCodeTotalsDate = htonl(wrConfig.getPackCodeTotalsDate());

			WrapperTiming heartbeat(wrConfig.getHeartbeat());
			configMsg.config.heartbeat.on.min = heartbeat.getOnMin();
			configMsg.config.heartbeat.on.value = heartbeat.getOnValue();
			configMsg.config.heartbeat.on.max = heartbeat.getOnMax();
			configMsg.config.heartbeat.off.min = heartbeat.getOffMin();
			configMsg.config.heartbeat.off.value = heartbeat.getOffValue();
			configMsg.config.heartbeat.off.max = heartbeat.getOffMax();

			WrapperGripperTiming wrapGripTime0(wrConfig.getWrapperGripperTiming0());
			WrapperTime	wrapSetback0(wrapGripTime0.getSetback());
			configMsg.config.gripper.setback0.min = wrapSetback0.getMin();
			configMsg.config.gripper.setback0.value = wrapSetback0.getValue();
			configMsg.config.gripper.setback0.max = wrapSetback0.getMax();
			configMsg.config.gripper.pwm0 = (UINT8)wrapGripTime0.getPwm();
				
			WrapperGripperTiming wrapGripTime1(wrConfig.getWrapperGripperTiming1());
			WrapperTime	wrapSetback1(wrapGripTime1.getSetback());
			configMsg.config.gripper.setback1.min = wrapSetback1.getMin();
			configMsg.config.gripper.setback1.value = wrapSetback1.getValue();
			configMsg.config.gripper.setback1.max = wrapSetback1.getMax();
			configMsg.config.gripper.pwm1 = (UINT8)wrapGripTime1.getPwm();
				
			WrapperGripperTiming wrapGripTime2(wrConfig.getWrapperGripperTiming2());
			WrapperTime	wrapSetback2(wrapGripTime2.getSetback());
			configMsg.config.gripper.setback2.min = wrapSetback2.getMin();
			configMsg.config.gripper.setback2.value = wrapSetback2.getValue();
			configMsg.config.gripper.setback2.max = wrapSetback2.getMax();
			configMsg.config.gripper.pwm2 = (UINT8)wrapGripTime2.getPwm();
				
			WrapperGripperTiming wrapGripTime3(wrConfig.getWrapperGripperTiming3());
			WrapperTime	wrapSetback3(wrapGripTime3.getSetback());
			configMsg.config.gripper.setback3.min = wrapSetback3.getMin();
			configMsg.config.gripper.setback3.value = wrapSetback3.getValue();
			configMsg.config.gripper.setback3.max = wrapSetback3.getMax();
			configMsg.config.gripper.pwm3 = (UINT8)wrapGripTime3.getPwm();
				
			for (int i = 0; i < NumberPackCodes; i++)
			{
				WrapperPackCodeInfo packCodeInfo(wrConfig.getWrapperPackCodeInfo((WrPackCode) i));

				packCodeInfo.getDescription((UINT8 *) &(configMsg.config.packCode[i].description));
				configMsg.config.packCode[i].packCode =		(UINT8)packCodeInfo.getPackCode();
				configMsg.config.packCode[i].packWidth =	(UINT8)packCodeInfo.getPackWidth();
				configMsg.config.packCode[i].packLength =	(UINT8)packCodeInfo.getPackLength();

				WrapperPackSize packSize(packCodeInfo.getPackSize());
				configMsg.config.packCode[i].packSize.minLength = htons(packSize.getMinLength());
				configMsg.config.packCode[i].packSize.maxLength = htons(packSize.getMaxLength());
				configMsg.config.packCode[i].packSize.minWidth = htons(packSize.getMinWidth());
				configMsg.config.packCode[i].packSize.maxWidth = htons(packSize.getMaxWidth());

				WrapperTiming sideClamp(packCodeInfo.getSideClamp());
				configMsg.config.packCode[i].sideClamp.on.min = sideClamp.getOnMin();
				configMsg.config.packCode[i].sideClamp.on.value = sideClamp.getOnValue();
				configMsg.config.packCode[i].sideClamp.on.max = sideClamp.getOnMax();
				configMsg.config.packCode[i].sideClamp.off.min = sideClamp.getOffMin();
				configMsg.config.packCode[i].sideClamp.off.value = sideClamp.getOffValue();
				configMsg.config.packCode[i].sideClamp.off.max = sideClamp.getOffMax();

				WrapperTiming gripper(packCodeInfo.getGripper());
				configMsg.config.packCode[i].gripper.on.min = gripper.getOnMin();
				configMsg.config.packCode[i].gripper.on.value = gripper.getOnValue();
				configMsg.config.packCode[i].gripper.on.max = gripper.getOnMax();
				configMsg.config.packCode[i].gripper.off.min = gripper.getOffMin();
				configMsg.config.packCode[i].gripper.off.value = gripper.getOffValue();
				configMsg.config.packCode[i].gripper.off.max = gripper.getOffMax();

				WrapperTiming knife(packCodeInfo.getKnife());
				configMsg.config.packCode[i].knife.on.min = knife.getOnMin();
				configMsg.config.packCode[i].knife.on.value = knife.getOnValue();
				configMsg.config.packCode[i].knife.on.max = knife.getOnMax();
				configMsg.config.packCode[i].knife.off.min = knife.getOffMin();
				configMsg.config.packCode[i].knife.off.value = knife.getOffValue();
				configMsg.config.packCode[i].knife.off.max = knife.getOffMax();

				if (packCodeInfo.isSlideBlock() == true)
				{
					configMsg.config.packCode[i].options.slideBlock = 1;
				}

				if (packCodeInfo.isBooster() == true)
				{
					configMsg.config.packCode[i].options.booster = 1;
				}

				configMsg.config.packCode[i].packageTotal = htonl(packCodeInfo.getPackageTotal());
				configMsg.config.packCode[i].jamTotal = htonl(packCodeInfo.getJamTotal());
			}


			commHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setMessageLength(sizeof(configMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &configMsg, sizeof(configMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(configMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WrMode:
		{
			Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrMode");

			// Constuct the Evol Message Buffer!!
			WrapperMode wrMode(buffer);

			UltimaWrMode ultimaWrMode;

			ultimaWrMode.wrEvent = WrMode;
			ultimaWrMode.mode = (UINT8)wrMode.getWrOpMode();

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setMessageLength(sizeof(ultimaWrMode));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &ultimaWrMode, sizeof(ultimaWrMode));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(ultimaWrMode) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WrAbort:
		case WrDisable:
		case WrEnable:
		case WrPause:
		case WrReset:
		case WrResume:
		{
			switch ( wrEventType )
			{
				case WrAbort:
					Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrAbort");
					break;
				case WrDisable:
					Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrDisable");
					break;
				case WrEnable:
					Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrEnable");
					break;
				case WrPause:
					Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrPause");
					break;
				case WrReset:
					Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrReset");
					break;
				case WrResume:
					Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrResume");
					break;
			}

			UltimaWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = (UINT8)wrEventType;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WrReqStatus: 
		{
			Debug::trace("LSMNCommManager.Wrapper.Message.Probe", "WrReqStatus");

			UltimaWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = WrStatus;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WrReqInquire:
		{
			Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrReqInquire");

			UltimaWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = WrInquire;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WrReqWakeup:
		{
			Debug::trace("LSMNCommManager.Wrapper.Message.Probe", "WrReqWakeup");

			WrapperRequestWakeup wrRequestWakeup(buffer);

			UltimaWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = WrWakeup;

			UINT16 physicalAddress = wrRequestWakeup.getPhysicalAddress();

			if (evolMgrMap.find(physicalAddress) == evolMgrMap.end())
			{
				usbEvolHdr.setDestinationPhysicalAddress(physicalAddress);
			}


			commHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WrTest:
		{
			Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrTest");

			WrapperTest wrTest(buffer);

			UltimaWrTest testMsg;
			memset((char *) &testMsg, 0, sizeof(testMsg));

			testMsg.wrEvent = WrTest;
			testMsg.test = (UINT8)wrTest.getWrTestCode();

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setSourceLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setMessageLength(sizeof(testMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &testMsg, sizeof(testMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(testMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WrReqSensors:
		{
			Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrReqSensors");

			UltimaWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = WrSensors;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setSourceLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WrReqErrorHistory:
		{
			Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrReqErrorHistory");

			UltimaWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = WrErrorHistory;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setSourceLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WrWrap:
		{
			Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrWrap");

			WrapperWrap ww(buffer);

			UltimaWrWrap wrapMsg;
			memset((char *) &wrapMsg, 0, sizeof(wrapMsg));

			wrapMsg.wrEvent = WrWrap;
			wrapMsg.intakeMode = (UINT8)ww.getUltimaIntakeMode();
			wrapMsg.packageId = ww.getPackageId();
			wrapMsg.pullFilm = ww.isPullFilm();
			wrapMsg.packCode = (UINT8)ww.getWrPackCode();

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setSourceLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setMessageLength(sizeof(wrapMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &wrapMsg, sizeof(wrapMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(wrapMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WrCycle:
		{
			Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrCycle");

			WrapperCycle wc(buffer);

			UltimaWrCycle cycleMsg;
			memset((char *) &cycleMsg, 0, sizeof(cycleMsg));

			cycleMsg.wrEvent = WrCycle;
			cycleMsg.packageId = wc.getPackageId();

			if (wc.isPullFilm() == true)
			{
				cycleMsg.pullFilm = 1;
			}

			cycleMsg.packCode = (UINT8)wc.getWrPackCode();

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setSourceLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setMessageLength(sizeof(cycleMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &cycleMsg, sizeof(cycleMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(cycleMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WrJog:
		{
			Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrJog");

			WrapperJog wj(buffer);

			UltimaWrJog jogMsg;
			memset((char *) &jogMsg, 0, sizeof(jogMsg));

			jogMsg.wrEvent = WrJog;
			jogMsg.packageId = wj.getPackageId();

			if (wj.isPullFilm() == true)
			{
				jogMsg.pullFilm = 1;
			}

			jogMsg.packCode = (UINT8)wj.getWrPackCode();

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setSourceLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setMessageLength(sizeof(jogMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &jogMsg, sizeof(jogMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(jogMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WrControl:
		{
			Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrControl");

			WrapperControl wrControl(buffer);

			UltimaWrControl controlMsg;
			memset((char *) &controlMsg, 0, sizeof(controlMsg));

			controlMsg.wrEvent = WrControl;
			controlMsg.heaterSetting = htons(wrControl.getHeaterSetting());

			if (wrControl.isHeaterEnable() == true)
			{
				controlMsg.heaterEnable = 1;
			}

			controlMsg.filmTension = (UINT8)wrControl.getFilmTension();
			controlMsg.packWidth = (UINT8)wrControl.getFilmWidth();

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setSourceLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setMessageLength(sizeof(controlMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &controlMsg, sizeof(controlMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(controlMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case WrLoad:
		{
			Debug::trace("LSMNCommManager.Wrapper.Message.Out", "WrLoad");

			WrapperLoad wrLoad(buffer);

			UltimaWrLoad loadMsg;
			memset((char *) &loadMsg, 0, sizeof(loadMsg));

			loadMsg.wrEvent = WrLoad;
			loadMsg.filmWidth = (UINT8)wrLoad.getWrPackWidth();

			commHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setSourceLogicalAddress(USBEvolHeader::WRAPPER);
			commHdr.setMessageLength(sizeof(loadMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &loadMsg, sizeof(loadMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(loadMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;
		default:
		Debug::trace("LSMNCommManager.Wrapper.Error", HobString("handleWrapperEvent -- unknown event: 0x") +
														HobString(Hex::toString((UINT32) wrEventType)));							
		break;
	}

	return sendPeripheralMessage;
}
#endif

bool LSMNCommManager::handleWrapperSysEvent(SysEvent sysEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;
	bool sendPeripheralMessage = false;
	Debug::trace("LSMNCommManager.Wrapper.Message.Out", "handleWrapperSysEvent");

	switch (sysEventType)
	{
		case SysReqWakeup:
		{
			Debug::trace("LSMNCommManager.Wrapper.SystemMessage.Out", HobString("LSMN -- SysReqWakeup"));
			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = SysWakeup;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setSourceLogicalAddress(CommHeader::SYSTEM);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case SysConfig:
		{
			Debug::trace("LSMNCommManager.Wrapper.SystemMessage.Out", HobString("LSMN -- SysConfig"));
			WrapperSystemConfiguration wrSysConfig(buffer);

			SysConfigMsg sysConfig;

			sysConfig.function_code = SysConfig;
			sysConfig.configType = htons((UINT16)wrSysConfig.getSysConfigType());
			wrSysConfig.getDeviceSignature((UINT8 *) &sysConfig.system_config.device_signature);
			sysConfig.system_config.station_address = (DeviceAddress) htons(wrSysConfig.getStationAddress());
			sysConfig.system_config.controller_address = (DeviceAddress) htons(wrSysConfig.getControllerAddress());

			commHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setMessageLength(sizeof(sysConfig));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &sysConfig, sizeof(sysConfig));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(sysConfig) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case SysReqStatus:
		{
			Debug::trace("LSMNCommManager.Wrapper.SystemMessage.Out", HobString("LSMN -- SysReqStatus"));
			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = SysStatus;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case SysReqInquire:
		{
			Debug::trace("LSMNCommManager.Wrapper.SystemMessage.Out", HobString("LSMN -- SysReqInquire"));
			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = SysInquire;

			commHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			commHdr.setMessageLength(sizeof(genMsg));
			commHdr.Serialize( str );

			memcpy( &str[19], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 19, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		default:
		{
			Debug::trace("WrapperManager.Error", HobString("LSMN -- Some Other System Event: ") +
												 HobString(Integer::toString((UINT16) sysEventType)));
			Debug::trace("LSMNCommManager.Wrapper.Error", HobString("LSMN -- Some Other System Event: ") +
												 HobString(Integer::toString((UINT16) sysEventType)));
		}
		break;
	}
	return sendPeripheralMessage;
}

void LSMNCommManager::processUsbMessage( std::string & msg, CUsbPipe::EvolPipeType UNUSED(pipe_type) )
{
	UNREFERENCED_PARAMETER(pipe_type);

	const char *pMsg = msg.c_str();

	CommHeader usbCommHdr((char *) pMsg, true);

	UINT16 physicalAddress = usbCommHdr.getSourcePhysicalAddress();
	UINT16 logicalAddress = usbCommHdr.getSourceLogicalAddress();

//	pipe_type;
	
	pMsg+= sizeof(usbCommHdr);

	if (evolMgrMap.find(physicalAddress) == evolMgrMap.end())
	{
		// Check for new device WakeUp message!
		if (logicalAddress == USBEvolHeader::WEIGHER)
		{
			Debug::trace("LSMNCommManager.Weigher.Message.In", "Ultima Weigher -- WgWakeup: ************************************************");

	   		// Create a new thread
			Hobart::CEvolMsgReceiver *pMgr = RealTimeController::getInstance()->NewUltimaWeigher(nodeId, this, physicalAddress, WgUltima);
			
			// Add to the routing table
			ultimaAddrMap.insert(make_pair(pMgr, physicalAddress));

			UltimaDeviceType *pUDT = new UltimaDeviceType;

			pUDT->pDeviceMgr = pMgr;
			pUDT->deviceType = CommHeader::WEIGHER;

			evolMgrMap.insert(make_pair(physicalAddress, pUDT));
		}
		else if (logicalAddress == USBEvolHeader::CLA)
		{
			UltimaLaGeneric *genMsg = (UltimaLaGeneric *) pMsg;
			Hobart::CEvolMsgReceiver *pMgr = NULL;

			if (genMsg->function_code == LaILACycle)
			{
				Debug::trace("LSMNCommManager.LabelApplier.Message.In", "Ultima ILA -- First LaILACycle: ************************************************");
				// Create a new thread
				pMgr = RealTimeController::getInstance()->NewILA(nodeId, this);
			}
			else
			{
				Debug::trace("LSMNCommManager.LabelApplier.Message.In", "Ultima CLA -- First ClaWakeup: ************************************************");

				bool readyToRun = false;
 				if (genMsg->function_code == LaWakeup)
				{
	 				UltimaLaWakeup *wakeupMsg = (UltimaLaWakeup *) pMsg;
					readyToRun =  ((wakeupMsg->ready_to_run != 0) ? true : false);
				}

				// Create a new thread
				pMgr = RealTimeController::getInstance()->NewUltimaLabelApplier(nodeId, this, readyToRun, physicalAddress);
			}

			// Add to the routing table
			ultimaAddrMap.insert(make_pair(pMgr, physicalAddress));

			UltimaDeviceType *pUDT = new UltimaDeviceType;

			pUDT->pDeviceMgr = pMgr;
			pUDT->deviceType = CommHeader::CLA;

			evolMgrMap.insert(make_pair(physicalAddress, pUDT));
		}
		else if (logicalAddress == CommHeader::WRAPPER)
		{
			Debug::trace("LSMNCommManager.Wrapper.Message.In", "Ultima Wrapper -- First WrWakeup: ************************************************");

//			UINT8 byteStream[] = {	0x04, 0x00, 0x01, 0x17, 0xCD, 0xA3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//									0x01, 0x54, 0x00, 0x00, 0x02, 0x00, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC2, 0x00, 0x21, 0x0F, 0x00, 0x55,
//									0x00, 0x9C, 0xC2, 0x00, 0x21, 0x0E, 0x00, 0x5B, 0x00, 0xA7, 0x02, 0x00, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 };

	   		// Create a new thread
			Hobart::CEvolMsgReceiver *pMgr = RealTimeController::getInstance()->NewUltimaWrapper(nodeId, this, physicalAddress);
			
			// Add to the routing table
			ultimaAddrMap.insert(make_pair(pMgr, physicalAddress));

			UltimaDeviceType *pUDT = new UltimaDeviceType;

			pUDT->pDeviceMgr = pMgr;
			pUDT->deviceType = CommHeader::WRAPPER;

			evolMgrMap.insert(make_pair(physicalAddress, pUDT));
		}
		else if (logicalAddress == CommHeader::SYSTEM)
		{
//			cout << "LSMN -- ReceivedSystem Wakeup Message: " << cHdr.getSourceUniqueAddress() << endl;
//			Debug::trace("LSMNCommManager.Message.In", HobString("LSMN -- ReceivedSystem Wakeup Message") +
//					HobString(Hex::toString(cHdr.getSourceUniqueAddress())));
		}
		else
		{
//			cout << "LSMN -- Unknown Message logicalAddress:" << cHdr.getSourceUniqueAddress() << endl;
//			Debug::trace("LSMNCommManager.Message.In", HobString("LSMN -- Unknown Message logicalAddress") +
//					HobString(Hex::toString(cHdr.getSourceUniqueAddress())));
//			cHdr.show();
		}
	}
	else
	{
		Hobart::CEvolMsgReceiver *pHobThread = evolMgrMap[physicalAddress]->pDeviceMgr;
		CommHeader::DeviceType deviceType = evolMgrMap[physicalAddress]->deviceType;

		if ((logicalAddress == CommHeader::SYSTEM) && (deviceType == CommHeader::WEIGHER))
		{
			receivedUltimaWeigherSysMessage((SysGenericMsg *) pMsg, pHobThread);
		}
		else if ((logicalAddress == CommHeader::SYSTEM) && (deviceType == CommHeader::CLA))
		{
			receivedUltimaClaSysMessage((SysGenericMsg *) pMsg, pHobThread);
		}
		else if ((logicalAddress == CommHeader::SYSTEM) && (deviceType == CommHeader::WRAPPER))
		{
			receivedUltimaWrapperSysMessage((SysGenericMsg *) pMsg, pHobThread);
		}
		else if (logicalAddress == CommHeader::WEIGHER)
		{
			receivedUltimaWeigherMessage((UltimaWgGeneric *) pMsg, pHobThread);
		}
		else if (logicalAddress == CommHeader::CLA)
		{
			receivedUltimaClaMessage((UltimaLaGeneric *) pMsg, pHobThread);
		}
		else if (logicalAddress == CommHeader::WRAPPER)
		{
			receivedUltimaWrapperMessage((UltimaWrGeneric *) pMsg, pHobThread);
		}
		else
		{
//			cout << "LSMN -- Unknown Message logicalAddress:" << cHdr.getSourceUniqueAddress() << endl;
//			Debug::trace("LSMNCommManager.Error", HobString("LSMN -- Unknown Message logicalAddress: ") +
//					HobString(Hex::toString(cHdr.getSourceUniqueAddress())));
//			cHdr.show();
		}
	}
}

void LSMNCommManager::receivedUltimaWeigherSysMessage(SysGenericMsg *genMsg, Hobart::CEvolMsgReceiver *pHobThread)
{

	switch (genMsg->function_code)
	{
		case SysWakeup:
		{	
			Debug::trace("LSMNCommManager.Weigher.SystemMessage.In", HobString("Ultima Weigher -- SysWakeup"));
			SysWakeupMsg *sysWakeupMsg = (SysWakeupMsg *) genMsg;

		 	WeigherSystemWakeup wswu(this, (SysCfgStatus) sysWakeupMsg->sysCfgStatus);

			const char *pWswuMsg = wswu.serialize();
			int mLength = wswu.getSerializedLength();

			pHobThread->send(pWswuMsg, mLength);
		}
		break;

		case SysStatus:
		{
			Debug::trace("LSMNCommManager.Weigher.SystemMessage.In", HobString("Ultima Weigher -- SysStatus"));
			SysStatusMsg *systemStatusMsg = (SysStatusMsg *) genMsg;

			SystemStatusInfo *pSystemStatus = new SystemStatusInfo((ntohs(systemStatusMsg->system_status.bit_error)),
																   (ntohs(systemStatusMsg->system_status.address_error)),
																   (ntohs(systemStatusMsg->system_status.rom_checksum)),
																   (ntohl(systemStatusMsg->system_status.flags)),
																    systemStatusMsg->system_status.partno,
																   (ntohs(systemStatusMsg->system_status.system_checksum)),
																   (ntohl(systemStatusMsg->system_status.init_flags)));

			WeigherSystemStatus wss(this, pSystemStatus);

			const char *pWssMsg = wss.serialize();
			int mLength = wss.getSerializedLength();

			pHobThread->send(pWssMsg, mLength);

			delete pSystemStatus;
		}
		break;

		case SysInquire:
		{	
			Debug::trace("LSMNCommManager.Weigher.SystemMessage.In", HobString("Ultima Weigher -- SysInquire"));

			SysInquireMsg *systemInquireMsg = (SysInquireMsg *) genMsg;

		 	WeigherSystemInquire wsi(this,
		 							 (SysConfigType) ntohs(systemInquireMsg->configType),
		 							 (UINT8 *) &systemInquireMsg->system_config.device_signature,
		 							 (DeviceAddress) ntohs(systemInquireMsg->system_config.station_address),
		 							 (DeviceAddress) ntohs(systemInquireMsg->system_config.controller_address));

			const char *pWsiMsg = wsi.serialize();
			int mLength = wsi.getSerializedLength();

			pHobThread->send(pWsiMsg, mLength);
		}
		break;

		default:
		Debug::trace("LSMNCommManager.Weigher.Error", HobString("Ultima Weigher -- Received Some Other System Event: ") +
											 HobString(Integer::toString((UINT16) (genMsg->function_code))));
		break;
	}

}

void LSMNCommManager::receivedUltimaWeigherMessage(UltimaWgGeneric *genMsg, Hobart::CEvolMsgReceiver *pHobThread)
{
	switch (genMsg->function_code)
	{
		case WgWakeup:
		{	
			Debug::trace("LSMNCommManager.Weigher.Message.WgWakeup", "WgWakeup");	

			UltimaWgWakeup *wakeupMsg = (UltimaWgWakeup *) genMsg;

			WeigherWakeup ww(this, (WgCfgStatus) wakeupMsg->wgCfgStatus);

			const char *pWwMsg = ww.serialize();
			int mLength = ww.getSerializedLength();

			pHobThread->send(pWwMsg, mLength);							
		} 
		break;

		case WgStatus:
		{
			UltimaWgStatus *statusMsg = (UltimaWgStatus *) genMsg;

			WeigherStatus ws(this); // use default constructor
			ws.setStatusBits(ntohs(statusMsg->status));
			ws.setCounts(ntohl(statusMsg->counts));
			ws.setRawCounts(ntohl(statusMsg->raw_counts));

			string message("Incoming WgStatus: ");
			if ( ws.isLargeMotion() ) message.append("large ");
			if ( ws.isSmallMotion() ) message.append("small ");
			if ( ws.isPositiveNonZeroStableWeight() ) message.append("positive ");
			if ( ws.isWithinZeroQuarterGrad() ) message.append("zero ");

			Debug::trace("LSMNCommManager.Weigher.Message.In", message);	

			if ((Debug::isTracing("LSMNCommManager.Weigher.WeigherStatus") == true) && (ws.isPositiveNonZeroStableWeight() == true))
			{
				ws.show();
			}

			const char *pWsMsg = ws.serialize();
			int mLength = ws.getSerializedLength();

			pHobThread->send(pWsMsg, mLength);
		}
		break;

		case WgInquire:
		{	
			Debug::trace("LSMNCommManager.Weigher.Message.In", "WgInquire");	

			UltimaWgInquire *inquireMsg = (UltimaWgInquire *) genMsg;

			WeigherInquire wi(this); // use default constructor

			wi.setConfigType((WgConfigType) ntohs(inquireMsg->configType));
			wi.setDeviceSignature((UINT8 *) &(inquireMsg->config.device_signature));
			wi.setCenterOfMaintenanceZone(ntohl(inquireMsg->config.center_of_maintenance_zone));
			wi.setScaleFactor(ntohl(inquireMsg->config.scale_factor));
			wi.setMaxWeight(ntohl(inquireMsg->config.max_weight));
			wi.setGainFactor(ntohl(inquireMsg->config.gain_factor));
			wi.setZeroReference(ntohl(inquireMsg->config.zero_reference));
			wi.setCalibrationReference(ntohl(inquireMsg->config.calibration_reference));
			wi.setRateOptionTime(ntohs(inquireMsg->config.rate_option_time));
			wi.setAutoZeroMaintenanceLimit(ntohs(inquireMsg->config.auto_zero_maintenance_limit));
			wi.setPrepackMotionCount(ntohs(inquireMsg->config.prepack_motion_count));
			wi.setInitializeZeroTime(ntohs(inquireMsg->config.initialize_zero_time));
			wi.setAutoZeroTime(ntohs(inquireMsg->config.auto_zero_time));
			wi.setRateOptionLimit(ntohs(inquireMsg->config.rate_option_limit));
			wi.setZeroReferenceLimit(ntohs(inquireMsg->config.zero_reference_limit));
			wi.setCalibrationReferenceLimit(ntohs(inquireMsg->config.calibration_reference_limit));
			wi.setSmallMotionLimit(ntohs(inquireMsg->config.small_motion_limit));
			wi.setLargeMotionLimit(ntohs(inquireMsg->config.large_motion_limit));
			wi.setLargeMotionCount(ntohs(inquireMsg->config.large_motion_count));
			wi.setSmallMotionCount(ntohs(inquireMsg->config.small_motion_count));
			wi.setNoMotionCount(ntohs(inquireMsg->config.no_motion_count));
			wi.setZeroTrackingOption(ntohs(inquireMsg->config.zero_tracking_option));
			wi.setNegativeWeightOption(ntohs(inquireMsg->config.negative_weight_option));
			wi.setPowerupMaintenanceZoneLimit(ntohs(inquireMsg->config.powerup_maintenance_zone_limit));
			wi.setOperateMaintenanceZoneLimit(ntohs(inquireMsg->config.operate_maintenance_zone_limit));
			wi.setFilterSpeed((WgFilterSpeed) inquireMsg->config.filter_speed);
			wi.setReferenceVoltage(inquireMsg->config.reference_voltage);
			wi.setLastCalibrationDate(ntohl(inquireMsg->config.last_calibration_date));
			wi.setNumberOfCalibrations(ntohs(inquireMsg->config.number_of_calibrations));
			wi.setLastConfigurationDate(ntohl(inquireMsg->config.last_configuration_date));
			wi.setNumberOfConfigurations(ntohs(inquireMsg->config.number_of_configurations));

			if (Debug::isTracing("WeigherManager.WeigherInquire") == true)
			{
				wi.show();
			}

			const char *pWiMsg = wi.serialize();
			int mLength = wi.getSerializedLength();

			pHobThread->send(pWiMsg, mLength);
		}
		break;

		default:
		Debug::trace("LSMNCommManager.Weigher.Error", HobString("Ultima Weigher -- Received Some Other Event: ") +
											 HobString(Integer::toString((UINT16) (genMsg->function_code))));
		break;
	}
}

void LSMNCommManager::receivedUltimaClaSysMessage(SysGenericMsg *genMsg, Hobart::CEvolMsgReceiver *pHobThread)
{
	switch (genMsg->function_code)
	{
		case SysWakeup:
		{	
			//cout << "Ultima CLA -- SysWakeup: " << endl;
			Debug::trace("LSMNCommManager.CLA.SystemMessage.In", HobString("Ultima CLA -- SysWakeup"));
			SysWakeupMsg *sysWakeupMsg = (SysWakeupMsg *) genMsg;

		 	LabelApplierSystemWakeup laswu(this, (SysCfgStatus) sysWakeupMsg->sysCfgStatus);

			const char *pLaswuMsg = laswu.serialize();
			int mLength = laswu.getSerializedLength();

			pHobThread->send(pLaswuMsg, mLength);
		}
		break;

		case SysInquire:
		{	
			//cout << "Ultima CLA -- SysInquire: " << endl;
			Debug::trace("LSMNCommManager.CLA.SystemMessage.In", HobString("Ultima CLA -- SysInquire"));
			SysInquireMsg *systemInquireMsg = (SysInquireMsg *) genMsg;

		 	WeigherSystemInquire wsi(this,
		 							 (SysConfigType) ntohs(systemInquireMsg->configType),
		 							 (UINT8 *) &systemInquireMsg->system_config.device_signature,
		 							 (DeviceAddress) ntohs(systemInquireMsg->system_config.station_address),
		 							 (DeviceAddress) ntohs(systemInquireMsg->system_config.controller_address));

			const char *pWsiMsg = wsi.serialize();
			int mLength = wsi.getSerializedLength();

			pHobThread->send(pWsiMsg, mLength);
		}
		break;

		default:
		Debug::trace("LSMNCommManager.CLA.Error", HobString("Ultima CLA -- Received Some Other System Event: ") +
											 HobString(Integer::toString((UINT16) (genMsg->function_code))));
		break;
	}
}

void LSMNCommManager::receivedUltimaClaMessage(UltimaLaGeneric *genMsg, Hobart::CEvolMsgReceiver *pHobThread)
{
	switch (genMsg->function_code)
	{
		case LaWakeup:
		{	
			Debug::trace("LSMNCommManager.LabelApplier.Message.LaWakeup", "LaWakeup");	

			UltimaLaWakeup *wakeupMsg = (UltimaLaWakeup *) genMsg;

			LabelApplierWakeup laWakeup(this, ((wakeupMsg->ready_to_run != 0) ? true : false));

			const char *pLawuMsg = laWakeup.serialize();
			int mLength = laWakeup.getSerializedLength();

			pHobThread->send(pLawuMsg, mLength);							
		}
		break;

		case LaStatus:
		{	
			Debug::trace("LSMNCommManager.LabelApplier.Message.In", "LaStatus");	
			UltimaLaStatus *statusMsg = (UltimaLaStatus *) genMsg;

			LabelApplierStatus las(this); // use default constructor
			las.setStatusBits(ntohs(statusMsg->status));

			const char *pLasMsg = las.serialize();
			int mLength = las.getSerializedLength();

			pHobThread->send(pLasMsg, mLength);
		}
		break;

		case LaInquire:
		{	
			Debug::trace("LSMNCommManager.LabelApplier.Message.In", "LaInquire");	
			UltimaLaInquire *inquireMsg = (UltimaLaInquire *) genMsg;

			LabelApplierInquire lai(this); // use default constructor

			lai.setConfigType((LaConfigType) LaPermanent);

			lai.setWeigherStationAttached( ((inquireMsg->config.has_weigher != 0) ? true : false) );
			lai.setPrimaryStationAttached( ((inquireMsg->config.has_primary_printer != 0) ? true : false) );
			lai.setSecondaryStationAttached( ((inquireMsg->config.has_secondary_printer != 0) ? true : false) );
			lai.setLeftToRight( ((inquireMsg->config.left_to_right != 0) ? true : false) );
			lai.setFrontDischarge( ((inquireMsg->config.front_discharge != 0) ? true : false) );
			lai.setPrimaryJustifyFont( ((inquireMsg->config.primary_justify_font != 0) ? true : false) );
			lai.setSecondaryJustifyFont( ((inquireMsg->config.secondary_justify_font != 0) ? true : false) );
			lai.setTransferStationType((TransferStationType) inquireMsg->config.transfer_station_type);
			lai.setPrimaryPrinterLabelOffset(inquireMsg->config.primary_printer_label_offset);
			lai.setSecondaryPrinterLabelOffset(inquireMsg->config.secondary_printer_label_offset);
			lai.setWeigherConveyorSensitivity((ConveyorSensitivity) inquireMsg->config.weigher_conveyor_sensitivity);
			lai.setPrimaryPrinterConveyorSensitivity((ConveyorSensitivity) inquireMsg->config.primary_printer_conveyor_sensitivity);
			lai.setSecondaryPrinterConveyorSensitivity((ConveyorSensitivity) inquireMsg->config.secondary_printer_conveyor_sensitivity);
			lai.setPrimaryPrinterMaxJustificationDistance(inquireMsg->config.primary_printer_max_justification_distance);
			lai.setSecondaryPrinterMaxJustificationDistance(inquireMsg->config.secondary_printer_max_justification_distance);
			lai.setDiagonalLabelPlacement( ((inquireMsg->config.diagonal_label_placement != 0) ? true : false) );
			lai.setDiagonalOffset(inquireMsg->config.diagonal_offset);
			lai.setMiniStationLabelPlacement((LaPlacement) inquireMsg->config.ministation_label_placement);

			const char *pWiMsg = lai.serialize();
			int mLength = lai.getSerializedLength();

			pHobThread->send(pWiMsg, mLength);
		}
		break;

		case LaSensors:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.In", "LaSensors");	
			UltimaLaSensors *getSensorMsg = (UltimaLaSensors *) genMsg;

			LabelApplierSensors lags(	this,
										getSensorMsg->sensor_state[ExpectPackage],
										getSensorMsg->sensor_state[WeigherEntry],
										getSensorMsg->sensor_state[Pr1Entry],
										getSensorMsg->sensor_state[Pr1Gate],
										getSensorMsg->sensor_state[Pr1LabelPosition],
										getSensorMsg->sensor_state[Pr1Valid],
										getSensorMsg->sensor_state[Pr2Entry],
										getSensorMsg->sensor_state[Pr2Gate],
										getSensorMsg->sensor_state[Pr2LabelPosition],
										getSensorMsg->sensor_state[Pr2Valid] );

			const char *pLagsMsg = lags.serialize();
			int mLength = lags.getSerializedLength();

			pHobThread->send(pLagsMsg, mLength);
		}
		break;

		case LaSysStatus:
		{
			Debug::trace("LSMNCommManager.LabelApplier.SystemMessage.In", "LaSysStatus");	
			UltimaLaSystemStatus *systemStatusMsg = (UltimaLaSystemStatus *) genMsg;

			SystemStatusInfo *pMasterSystemStatus = new SystemStatusInfo((ntohs(systemStatusMsg->master_status.bit_error)),
																		 (ntohs(systemStatusMsg->master_status.address_error)),
																		 (ntohs(systemStatusMsg->master_status.rom_checksum)),
																		 (ntohl(systemStatusMsg->master_status.flags)),
																		  systemStatusMsg->master_status.partno,
																		 (ntohs(systemStatusMsg->master_status.system_checksum)),
																		 (ntohl(systemStatusMsg->master_status.init_flags)));

			SystemStatusInfo *pSlave1SystemStatus = new SystemStatusInfo((ntohs(systemStatusMsg->slave_1_status.bit_error)),
																		 (ntohs(systemStatusMsg->slave_1_status.address_error)),
																		 (ntohs(systemStatusMsg->slave_1_status.rom_checksum)),
																		 (ntohl(systemStatusMsg->slave_1_status.flags)),
																		  systemStatusMsg->slave_1_status.partno,
																		 (ntohs(systemStatusMsg->slave_1_status.system_checksum)),
																		 (ntohl(systemStatusMsg->slave_1_status.init_flags)));

			SystemStatusInfo *pSlave2SystemStatus = new SystemStatusInfo((ntohs(systemStatusMsg->slave_2_status.bit_error)),
																		 (ntohs(systemStatusMsg->slave_2_status.address_error)),
																		 (ntohs(systemStatusMsg->slave_2_status.rom_checksum)),
																		 (ntohl(systemStatusMsg->slave_2_status.flags)),
																		  systemStatusMsg->slave_2_status.partno,
																		 (ntohs(systemStatusMsg->slave_2_status.system_checksum)),
																		 (ntohl(systemStatusMsg->slave_2_status.init_flags)));

			LabelApplierSystemStatus lass(this, pMasterSystemStatus, pSlave1SystemStatus, pSlave2SystemStatus);

			const char *pLassMsg = lass.serialize();
			int mLength = lass.getSerializedLength();

			pHobThread->send(pLassMsg, mLength);

			delete pSlave2SystemStatus;
			delete pSlave1SystemStatus;
			delete pMasterSystemStatus;
		}
		break;

		case LaOff:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.In", "LaOff");	

			LabelApplierOff lao(this); // use default constructor

			const char *pLaoMsg = lao.serialize();
			int mLength = lao.getSerializedLength();

			pHobThread->send(pLaoMsg, mLength);
		}
		break;

		case LaPrintPackage:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.In", "LaPrintPackage");	

			UltimaLaPrint *printMsg = (UltimaLaPrint *) genMsg;

			LabelApplierPrint lap(this); // use default constructor

			lap.setPackageId(printMsg->package_id);

			const char *pLapMsg = lap.serialize();
			int mLength = lap.getSerializedLength();

			pHobThread->send(pLapMsg, mLength);
		}
		break;

		case LaUltimaAutoMode:
		{
			//Debug::trace("LSMNCommManager.LabelApplier.Message.In", lsmnTraceString + "LaUltimaAutoMode");	

			LabelApplierUltimaAutoMode lauam(this); // use default constructor

			const char *pLauamMsg = lauam.serialize();
			int mLength = lauam.getSerializedLength();

			pHobThread->send(pLauamMsg, mLength);
		}
		break;

		case LaOpenWindow:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.In", "LaOpenWindow");	

			// Constuct the Evol Message Buffer!!
			UltimaLaOpenWindow *openWindowMsg = (UltimaLaOpenWindow *) genMsg;

			LabelApplierOpenWindow laOpenWindow(this); // use default constructor

			laOpenWindow.setPackageId(openWindowMsg->package_id);

			const char *pOpenWindowMsg = laOpenWindow.serialize();
			int mLength = laOpenWindow.getSerializedLength();

			pHobThread->send(pOpenWindowMsg, mLength);
		}
		break;

		case LaCloseWindow:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.In", "LaCloseWindow");	

			// Constuct the Evol Message Buffer!!
			UltimaLaCloseWindow *closeWindowMsg = (UltimaLaCloseWindow *) genMsg;

			LabelApplierCloseWindow laCloseWindow(this); // use default constructor

			laCloseWindow.setPackageId(closeWindowMsg->package_id);

			const char *pCloseWindowMsg = laCloseWindow.serialize();
			int mLength = laCloseWindow.getSerializedLength();

			pHobThread->send(pCloseWindowMsg, mLength);
		}
		break;

		case LaILACycle:
		{
			Debug::trace("LSMNCommManager.LabelApplier.Message.In", "LaILACycle");	

			ILACycle ilaCycle;

			const char *pILACycle = ilaCycle.serialize();
			int mLength = ilaCycle.getSerializedLength();

			pHobThread->send(pILACycle, mLength);
		}
		break;

		default:
		Debug::trace("LSMNCommManager.LabelApplier.Error", HobString("Ultima LabelApplier -- Received Some Other Event: ") +
										 HobString(Integer::toString((UINT16) (genMsg->function_code))));
		break;
	}
}

void LSMNCommManager::receivedUltimaWrapperSysMessage(SysGenericMsg *genMsg, Hobart::CEvolMsgReceiver *pHobThread)
{
	switch (genMsg->function_code)
	{
		case SysWakeup:{	
							Debug::trace("LSMNCommManager.Wrapper.SystemMessage.In", "Ultima Wrapper -- SysWakeup");	
							SysWakeupMsg *sysWakeupMsg = (SysWakeupMsg *) genMsg;

						 	WrapperSystemWakeup wswu(this, (SysCfgStatus) sysWakeupMsg->sysCfgStatus);

							const char *pWswuMsg = wswu.serialize();
							int mLength = wswu.getSerializedLength();

							pHobThread->send(pWswuMsg, mLength);
						}
						break;
		case SysStatus:	{
							Debug::trace("LSMNCommManager.Wrapper.SystemMessage.In", "Ultima Wrapper -- SysStatus");	
							SysStatusMsg *systemStatusMsg = (SysStatusMsg *) genMsg;

							SystemStatusInfo *pSystemStatus = new SystemStatusInfo((ntohs(systemStatusMsg->system_status.bit_error)),
																				   (ntohs(systemStatusMsg->system_status.address_error)),
																				   (ntohs(systemStatusMsg->system_status.rom_checksum)),
																				   (ntohl(systemStatusMsg->system_status.flags)),
																				    systemStatusMsg->system_status.partno,
																				   (ntohs(systemStatusMsg->system_status.system_checksum)),
																				   (ntohl(systemStatusMsg->system_status.init_flags)));

							WrapperSystemStatus wss(this, pSystemStatus);

							const char *pWssMsg = wss.serialize();
							int mLength = wss.getSerializedLength();

							pHobThread->send(pWssMsg, mLength);

							delete pSystemStatus;
						}
						break;
		case SysInquire:{	
							Debug::trace("LSMNCommManager.Wrapper.SystemMessage.In", "Ultima Wrapper -- SysInquire");	

							SysInquireMsg *systemInquireMsg = (SysInquireMsg *) genMsg;

						 	WrapperSystemInquire wsi(this,
						 							 (SysConfigType) ntohs(systemInquireMsg->configType),
						 							 (UINT8 *) &systemInquireMsg->system_config.device_signature,
						 							 (DeviceAddress) ntohs(systemInquireMsg->system_config.station_address),
						 							 (DeviceAddress) ntohs(systemInquireMsg->system_config.controller_address));

							const char *pWsiMsg = wsi.serialize();
							int mLength = wsi.getSerializedLength();

							pHobThread->send(pWsiMsg, mLength);
						}
						break;
		default:
						Debug::trace("LSMNCommManager.Wrapper.Error", HobString("Ultima Wrapper -- Received Some Other System Event: ") +
															 HobString(Integer::toString((UINT16) (genMsg->function_code))));
						break;
	}
}

void LSMNCommManager::receivedUltimaWrapperMessage(UltimaWrGeneric *genMsg, Hobart::CEvolMsgReceiver *pHobThread)
{
	switch (genMsg->wrEvent)
	{
		case WrWakeup:	{	
							Debug::trace("LSMNCommManager.Wrapper.Message.WrWakeup", "WrWakeup");	

							UltimaWrWakeup *ultimaWakeupMsg = (UltimaWrWakeup *) genMsg;

							WrapperWakeup wakeupMsg(this); // use default constructor
							wakeupMsg.setWrCfgStatus((WrCfgStatus) ultimaWakeupMsg->wrCfgStatus);

							const char *pWakeupMsg = wakeupMsg.serialize();
							int mLength = wakeupMsg.getSerializedLength();

							pHobThread->send(pWakeupMsg, mLength);
						} 
						break;

		case WrStatus:	{
							Debug::trace("LSMNCommManager.Wrapper.Message.WrStatus", "WrStatus");	
							UltimaWrStatus *ultimaStatusMsg = (UltimaWrStatus *) genMsg;

							WrapperStatus statusMsg(this); // use default constructor

							WrapperStatusInfo statusField;

							ultimaStatusMsg->status.field = ntohl(ultimaStatusMsg->status.field);
							statusField.setStatusBits(ultimaStatusMsg->status.bits);
							statusMsg.setStatusInfo(statusField);

							WrapperErrorInfo errorField;

							errorField.setErrorBits(ultimaStatusMsg->error.bits);
							statusMsg.setErrorInfo(errorField);

						    WrapperErrorInfo historyField;

							historyField.setErrorBits(ultimaStatusMsg->history.bits);
							statusMsg.setHistoryInfo(historyField);

							statusMsg.setOperatingMode((WrOpMode) ultimaStatusMsg->mode);
							statusMsg.setWrapperEvent((WrapperEvent) ultimaStatusMsg->event);

						    WrapperHeaterInfo heaterField;

							ultimaStatusMsg->heater.field = ntohl(ultimaStatusMsg->heater.field);
							heaterField.setHeaterBits(ultimaStatusMsg->heater.bits);
							statusMsg.setHeaterInfo(heaterField);

							WrapperPackageInfo wrPackageInfo;

							for (UINT16 i = 0; i < NumWrapperPositions; i++)
							{
								ultimaStatusMsg->package[i].bits.length = ntohs(ultimaStatusMsg->package[i].bits.length);
								ultimaStatusMsg->package[i].bits.width = ntohs(ultimaStatusMsg->package[i].bits.width);

								wrPackageInfo.setPackageBits(ultimaStatusMsg->package[i].bits);
								statusMsg.setPackageInfo(wrPackageInfo, i);
							}

							const char *pUltimaStatusMsg = statusMsg.serialize();
							int mLength = statusMsg.getSerializedLength();

							pHobThread->send(pUltimaStatusMsg, mLength);
						}
						break;

		case WrInquire:	{	
							Debug::trace("LSMNCommManager.Wrapper.Message.In", "WrInquire");	
							UltimaWrInquire *inquireMsg = (UltimaWrInquire *) genMsg;

							WrapperInquire wi(this); // use default constructor

							wi.setConfigType((WrConfigType) ntohs(inquireMsg->configType));
							wi.setDeviceSignature((UINT8 *) &(inquireMsg->config.deviceSignature));
							wi.setPauseTimeout(inquireMsg->config.pauseTimeout);
							wi.setIdleTimeout(inquireMsg->config.idleTimeout);
							wi.setFilmTension((WrFilmTension) inquireMsg->config.filmTension);
							wi.setPackWidth((WrPackWidth) inquireMsg->config.packWidth);
							wi.setPackageGrandTotal((UINT32) ntohl(inquireMsg->config.packageGrandTotal));
							wi.setWrapperCycleTotal((UINT32) ntohl(inquireMsg->config.wrapperCycleTotal));
							wi.setElevatorCollapseTotal((UINT16) ntohs(inquireMsg->config.elevatorCollapseTotal));
							wi.setIntakeJamTotal((UINT16) ntohs(inquireMsg->config.intakeJamTotal));
							wi.setLengthJamTotal((UINT16) ntohs(inquireMsg->config.lengthJamTotal));
							wi.setWidthJamTotal((UINT16) ntohs(inquireMsg->config.widthJamTotal));
							wi.setCoverOpenTotal((UINT16) ntohs(inquireMsg->config.coverOpenTotal));
							wi.setSealerBeltOpenTotal((UINT16) ntohs(inquireMsg->config.sealerBeltOpenTotal));
							wi.setPackageTooTallTotal((UINT16) ntohs(inquireMsg->config.packageTooTallTotal));
							wi.setPusherHomeErrorTotal((UINT16) ntohs(inquireMsg->config.pusherHomeErrorTotal));
							wi.setFilmPullErrorTotal((UINT16) ntohs(inquireMsg->config.filmPullErrorTotal));

							if (inquireMsg->config.celsius_enable == 1)
							{
								wi.setCelsiusEnable();
							}
							else
							{
								wi.clearCelsiusEnable();
							}

							if (inquireMsg->config.wide_film_test_enable == 1)
							{
								wi.setWideFilmTestEnable();
							}
							else
							{
								wi.clearWideFilmTestEnable();
							}

							if (inquireMsg->config.narrow_film_test_enable == 1)
							{
								wi.setNarrowFilmTestEnable();
							}
							else
							{
								wi.clearNarrowFilmTestEnable();
							}

							if (inquireMsg->config.heater_enable == 1)
							{
								wi.setHeaterEnable();
							}
							else
							{
								wi.clearHeaterEnable();
							}

							wi.setHeaterSetting((UINT16) ntohs(inquireMsg->config.heaterSetting));
							wi.setPackageLengthAdjustment((UINT16) ntohs(inquireMsg->config.packageLengthAdjustment));
							wi.setPackageWidthAdjustment((UINT16) ntohs(inquireMsg->config.packageWidthAdjustment));
							wi.setErrorTotalsDate((UINT32) ntohl(inquireMsg->config.errorTotalsDate));
							wi.setPackCodeTotalsDate((UINT32) ntohl(inquireMsg->config.packCodeTotalsDate));

							WrapperTiming heartbeat(inquireMsg->config.heartbeat.on.min,
													inquireMsg->config.heartbeat.on.value,
													inquireMsg->config.heartbeat.on.max,
													inquireMsg->config.heartbeat.off.min,
													inquireMsg->config.heartbeat.off.value,
													inquireMsg->config.heartbeat.off.max);
							wi.setHeartbeat(heartbeat);

							WrapperTime setback(inquireMsg->config.gripper.setback0.min,
												inquireMsg->config.gripper.setback0.value,
												inquireMsg->config.gripper.setback0.max);
							WrapperGripperTiming wrapGripTime(setback, (WrGripperPwm) inquireMsg->config.gripper.pwm0);
							wi.setWrapperGripperTiming0(wrapGripTime);

							setback = WrapperTime(inquireMsg->config.gripper.setback1.min,
												  inquireMsg->config.gripper.setback1.value,
												  inquireMsg->config.gripper.setback1.max);
							wrapGripTime = WrapperGripperTiming(setback, (WrGripperPwm) inquireMsg->config.gripper.pwm1);
							wi.setWrapperGripperTiming1(wrapGripTime);

							setback = WrapperTime(inquireMsg->config.gripper.setback2.min,
												  inquireMsg->config.gripper.setback2.value,
												  inquireMsg->config.gripper.setback2.max);
							wrapGripTime = WrapperGripperTiming(setback, (WrGripperPwm) inquireMsg->config.gripper.pwm2);
							wi.setWrapperGripperTiming2(wrapGripTime);

							setback = WrapperTime(inquireMsg->config.gripper.setback3.min,
												  inquireMsg->config.gripper.setback3.value,
												  inquireMsg->config.gripper.setback3.max);
							wrapGripTime = WrapperGripperTiming(setback, (WrGripperPwm) inquireMsg->config.gripper.pwm3);
							wi.setWrapperGripperTiming3(wrapGripTime);

							for (int i = 0; i < NumberPackCodes; i++)
							{
								WrapperPackCodeInfo packCodeInfo;

								packCodeInfo.setDescription((UINT8 *) &inquireMsg->config.packCode[i].description);
								packCodeInfo.setPackCode((WrPackCode) inquireMsg->config.packCode[i].packCode);
								packCodeInfo.setPackWidth((WrPackWidth) inquireMsg->config.packCode[i].packWidth);
								packCodeInfo.setPackLength((WrPackLength) inquireMsg->config.packCode[i].packLength);

								WrapperPackSize packSize((UINT16) ntohs(inquireMsg->config.packCode[i].packSize.minLength),
														 (UINT16) ntohs(inquireMsg->config.packCode[i].packSize.maxLength),
														 (UINT16) ntohs(inquireMsg->config.packCode[i].packSize.minWidth),
														 (UINT16) ntohs(inquireMsg->config.packCode[i].packSize.maxWidth));
								packCodeInfo.setPackSize(packSize);

								WrapperTiming sideClamp(inquireMsg->config.packCode[i].sideClamp.on.min,
														inquireMsg->config.packCode[i].sideClamp.on.value,
														inquireMsg->config.packCode[i].sideClamp.on.max,
														inquireMsg->config.packCode[i].sideClamp.off.min,
														inquireMsg->config.packCode[i].sideClamp.off.value,
														inquireMsg->config.packCode[i].sideClamp.off.max);
								packCodeInfo.setSideClamp(sideClamp);

								WrapperTiming gripper(inquireMsg->config.packCode[i].gripper.on.min,
													  inquireMsg->config.packCode[i].gripper.on.value,
													  inquireMsg->config.packCode[i].gripper.on.max,
													  inquireMsg->config.packCode[i].gripper.off.min,
													  inquireMsg->config.packCode[i].gripper.off.value,
													  inquireMsg->config.packCode[i].gripper.off.max);
								packCodeInfo.setGripper(gripper);

								WrapperTiming knife(inquireMsg->config.packCode[i].knife.on.min,
													inquireMsg->config.packCode[i].knife.on.value,
													inquireMsg->config.packCode[i].knife.on.max,
													inquireMsg->config.packCode[i].knife.off.min,
													inquireMsg->config.packCode[i].knife.off.value,
													inquireMsg->config.packCode[i].knife.off.max);
								packCodeInfo.setKnife(knife);

								if (inquireMsg->config.packCode[i].options.slideBlock == 1)
								{
									packCodeInfo.setSlideBlock();
								}
								else
								{
									packCodeInfo.clearSlideBlock();
								}

								if (inquireMsg->config.packCode[i].options.booster == 1)
								{
									packCodeInfo.setBooster();
								}
								else
								{
									packCodeInfo.clearBooster();
								}

								packCodeInfo.setPackageTotal((UINT32) ntohl(inquireMsg->config.packCode[i].packageTotal));
								packCodeInfo.setJamTotal((UINT32) ntohl(inquireMsg->config.packCode[i].jamTotal));

								wi.setWrapperPackCodeInfo(packCodeInfo);
							}

							const char *pWiMsg = wi.serialize();
							int mLength = wi.getSerializedLength();

							pHobThread->send(pWiMsg, mLength);
						}
						break;
		case WrSensors:	{	
							Debug::trace("LSMNCommManager.Wrapper.Message.In", "WrSensors");	
							UltimaWrSensors *senseMsg = (UltimaWrSensors *) genMsg;

							WrapperSensors wrSense(this); // use default constructor

							wrSense.setHeaterReading(ntohs(senseMsg->heaterReading));
							wrSense.setEncoderCount(ntohs(senseMsg->encoderCount));
							wrSense.setIntakeCount(ntohs(senseMsg->intakeCount));

							if (senseMsg->stopSwitch == 1)
							{
								wrSense.setStopSwitch();
							}
							else
							{
								wrSense.clearStopSwitch();
							}

							if (senseMsg->startSwitch == 1)
							{
								wrSense.setStartSwitch();
							}
							else
							{
								wrSense.clearStartSwitch();
							}

							if (senseMsg->packageLengthSwitch == 1)
							{
								wrSense.setPackageLengthSwitch();
							}
							else
							{
								wrSense.clearPackageLengthSwitch();
							}

							if (senseMsg->packageSenseSwitch == 1)
							{
								wrSense.setPackageSenseSwitch();
							}
							else
							{
								wrSense.clearPackageSenseSwitch();
							}

							if (senseMsg->packageWidthSwitch == 1)
							{
								wrSense.setPackageWidthSwitch();
							}
							else
							{
								wrSense.clearPackageWidthSwitch();
							}

							if (senseMsg->pusherHomeSwitch == 1)
							{
								wrSense.setPusherHomeSwitch();
							}
							else
							{
								wrSense.clearPusherHomeSwitch();
							}

							if (senseMsg->packageJamSwitch == 1)
							{
								wrSense.setPackageJamSwitch();
							}
							else
							{
								wrSense.clearPackageJamSwitch();
							}

							if (senseMsg->wideFilmSwitch == 1)
							{
								wrSense.setWideFilmSwitch();
							}
							else
							{
								wrSense.clearWideFilmSwitch();
							}

							if (senseMsg->narrowFilmSwitch == 1)
							{
								wrSense.setNarrowFilmSwitch();
							}
							else
							{
								wrSense.clearNarrowFilmSwitch();
							}

							if (senseMsg->slideBlockState == 1)
							{
								wrSense.setSlideBlockState();
							}
							else
							{
								wrSense.clearSlideBlockState();
							}

							if (senseMsg->heaterState == 1)
							{
								wrSense.setHeaterState();
							}
							else
							{
								wrSense.clearHeaterState();
							}

							if (senseMsg->compressorState == 1)
							{
								wrSense.setCompressorState();
							}
							else
							{
								wrSense.clearCompressorState();
							}

							if (senseMsg->mainMotorState == 1)
							{
								wrSense.setMainMotorState();
							}
							else
							{
								wrSense.clearMainMotorState();
							}

							if (senseMsg->dischargeMotorState == 1)
							{
								wrSense.setDischargeMotorState();
							}
							else
							{
								wrSense.clearDischargeMotorState();
							}

							wrSense.setFilmLengthState((WrPackLength) senseMsg->filmLengthState);
							wrSense.setFilmWidthState((WrPackWidth) senseMsg->filmWidthState);

							if (senseMsg->knifeState == 1)
							{
								wrSense.setKnifeState();
							}
							else
							{
								wrSense.clearKnifeState();
							}

							if (senseMsg->leftSideClampState == 1)
							{
								wrSense.setLeftSideClampState();
							}
							else
							{
								wrSense.clearLeftSideClampState();
							}

							if (senseMsg->rightSideClampstate == 1)
							{
								wrSense.setRightSideClampState();
							}
							else
							{
								wrSense.clearRightSideClampState();
							}

							if (senseMsg->gripperState == 1)
							{
								wrSense.setGripperState();
							}
							else
							{
								wrSense.clearGripperState();
							}

							const char *pWsMsg = wrSense.serialize();
							int mLength = wrSense.getSerializedLength();

							pHobThread->send(pWsMsg, mLength);
						}
						break;
		case WrOpen:
						{
							Debug::trace("LSMNCommManager.Wrapper.Message.In", "WrOpen");	
							WrapperOpen openMsg(this); // use default constructor

							const char *pOpenMsg = openMsg.serialize();
							int mLength = openMsg.getSerializedLength();

							pHobThread->send(pOpenMsg, mLength);
						}
						break;
		case WrErrorHistory:
						{
							Debug::trace("LSMNCommManager.Wrapper.Message.In", "WrErrorHistory");	
							WrapperErrorHistory errorHistoryMsg(this); // use default constructor

							UltimaWrErrorHistory *errHistoryMsg = (UltimaWrErrorHistory *) genMsg;

							WrapperErrorInfo wrErrorInfo;
							WrapperPackageInfo wrPackageInfo;

							UINT16 insertPosition = 0;

							for (UINT16 i = 0; i < MaxErrorHistoryBuffers; i++)
							{
								wrErrorInfo.setErrorBits(errHistoryMsg->data[i].error.bits);
								wrPackageInfo.setPackageBits(errHistoryMsg->data[i].package.bits);

								if (errorHistoryMsg.isReportableError(wrErrorInfo.getErrorSource()) == true)
								{
									errorHistoryMsg.setErrorInfo(wrErrorInfo, insertPosition);
									errorHistoryMsg.setPackageInfo(wrPackageInfo, insertPosition++);
								}
							}

							const char *pErrMsg = errorHistoryMsg.serialize();
							int mLength = errorHistoryMsg.getSerializedLength();

							pHobThread->send(pErrMsg, mLength);
						}
						break;
		default:		
						Debug::trace("LSMNCommManager.Wrapper.Error", HobString("LSMN Error: Unknown  message type: 0x") +
															  HobString(Hex::toString(genMsg->wrEvent)));
						break;
	}
	}
