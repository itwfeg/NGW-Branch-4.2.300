/*
 * KpcBootEvent.h
 *
 *  Created on: Mar 11, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTEVENT_H_
#define KPCBOOTEVENT_H_

typedef enum KPCBootEvent{
	KpcBootGeneric,			//0
	KpcBootStatus,
	KpcBootVersion,
	KpcBootAppVersion,
	KpcBootAck,
	KpcBootReady,			//5
	KpcBootReqStatus,		
	KpcBootCmd,
	KpcBootFrame,
	KpcBootFrame1,
	KpcBootFrame2,			//10
	KpcBootFrame3,			
	KpcBootFrame4,
	KpcBootFrame5,
	KpcBootFrame6,
	KpcBootFrame7,			//15
	KpcBootFrame8,			
	KpcBootFlush,
	KpcBootReqVersion,
	KpcBootReqAppVersion,
	KpcBootDone,			//20
	KpcBootRestart,			
	KpcNoEvent,
	KpcBootBulkMsg,

	KpcBootStartUpgrade,		//    number of nodes for UI to present
	KpcBootUpgradeStatus,		// 25 tell UI the status (nodeId,state,version,error) dont know what triggers it
	KpcBootUpgradeProgress,     //    tell the UI the current progress. Prolly response to KpcStartUpgrade
	KpcBootUpgradeFinished,		//    close UI upgrade page
	KpcBootUpgradeRequired,		//    ask UI to open upgrade page
	KpcBootUpgradeUIReady,		//    UI has signaled the upgrade page is ready
	KpcBootReqUpgradeInfo,		// 30 UI requests upgrade information
	KpcBootUpgradeInfo,			//    upgrade information

	// SOF-3478
	KpcBootUpgradeCancel,		//    cancel upgrade 
	KpcBootUpgradeRequested,	//	  UI made decision to go ahead with upgrade
	KpcBootUpgradeUpdateUI,		//    Update UI upgrade is ready after decision has been made to move forward
	// SOF-3478 END

	KPCBootEventEnd				// 35
}KPCBootEvent_t;

typedef enum block_transfer_types
{
    //block transfer function codes
	 __BlockNoTransfer,
	 __BlockReadConfigOffsets,
	 __BlockWriteConfigOffsets,
	 __BlockReadConfigSysCtrl,
	 __BlockWriteConfigSysCtrl,
	 __BlockReadConfigNodeCtrl,
	 __BlockWriteConfigNodeCtrl,
	 __BlockReadConfigWrapperCtrl,
	 __BlockWriteConfigWrapperCtrl,	
	 __BlockReadConfigComm,
	 __BlockWriteConfigComm,			//10
	
	 __BlockReadConfigMotors,
	 __BlockWriteConfigMotors,       
	 __BlockReadConfigVbs,
	 __BlockWriteConfigVbs,       
	 __BlockReadConfigPwm,
	 __BlockWriteConfigPwm,       
	 __BlockReadConfigTemp,
	 __BlockWriteConfigTemp,       
	 __BlockReadConfigHeater,
	 __BlockWriteConfigHeater,
	       
	 __BlockReadConfigPosEncoder,
	 __BlockWriteConfigPosEncoder,       
	 __BlockReadConfigPkgSensor,
	 __BlockWriteConfigPkgSensor,       
	 __BlockReadConfigFlash,
	 __BlockWriteConfigFlash,       
	 __BlockReadConfigWrapperGen,
	 __BlockWriteConfigWrapperGen,       
	 __BlockReadConfigLabelApplier,
	 __BlockWriteConfigLabelApplier,       
	
	 __BlockWriteProgramFlash, //31
	 __BlockWriteDataFalsh,
	 __BlockWriteBootFlash,
	 __BlockReadSystemInfo,
	 __BlockWriteSystemInfo,
	 __BlockWriteEvolConfig,
	 __BlockReadEvolConfig,
	 __BlockWriteCameraRScanDigital,	//written back to master (response to a read)	
	 __BlockReadCameraRScanDigital,		//received 
	 __BlockWriteCameraRScanAnalog,
	
	 __BlockReadCameraRScanAnalog,
	 __BlockWriteCameraLScanDigital,
	 __BlockReadCameraLScanDigital,	
	 __BlockWriteCameraLScanAnalog,
	 __BlockReadCameraLScanAnalog,
	 __BlockWriteCameraHeightScan,
	 __BlockReadCameraHeightScan,
	 __BlockWriteCameraReset,
	 __BlockReadCalCameraRight,	
	 __BlockWriteCalCameraRight,	

	 __BlockReadCalCameraLeft,	
	 __BlockWriteCalCameraLeft,
	 __BlockWriteWrapBoxFlash,
	 __BlockWriteWrapBoxRam,
	 __BlockReadVarifyWrapBoxes,
	 __BlockWriteVarifyWrapBoxes,
	 __BlockReadCameraPixelThreshold,
	 __BlockWriteCameraPixelThreshold,
	 __BlockReadCameraRevision,
	 __BlockWriteCameraRevision,
	
	 __BlockReadMotor1Profile,
	 __BlockWriteMotor1Profile,
	 __BlockReadMotor2Profile,
	 __BlockWriteMotor2Profile
	
}__BlockTransferType_t;


#endif /* KPCBOOTEVENT_H_ */
