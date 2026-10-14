#ifndef VERSIONMANAGEREVENT_H
#define VERSIONMANAGEREVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"

	// VersionManager Handled Events - VersionManagerDev
	typedef enum VersionManagerEvent
	{
		UpgradeAutomatic,
		UpgradeManual,
		WriteFileToSCEEP,
		PeripheralReqUpdateCode,
		PeripheralUpdateCode,
		RemoveUsbComponent,
		SystemShutDownRequest,
		SystemShutDownResponse,
		ProcessDBFilesReq,
		FirmwareUpdateStatus,
		FirmwareUpdateError,
		AddUsbComponent,
		PeripheralUpdateComplete,
		InitiateUpgradeProcess,
		UpgradeComplete,
		UsbWrapperUpgradeStatus,
		UpgradeXferStatus,
		PeripheralUpgradeList,
		CancelUpgrade,
		BeginUpgrade,
		RemoveWrapperSWComponents,
		ComponentCheckComplete,
		UpgradeAutomaticViaServer,
		ResetFPCComponent,
		RemoveCLASWComponents, //Added 9/24 Alex Zavala
		UpdateNow2016,
		SendToDept2016

	}   VersionManagerEvent_t;
	// for ErrVersionManagerDev
	typedef enum VersionManagerDevError
	{
		VersionManagerErrUnknownError = VersionManagerDev * DEVICE_ERROR_OFFSET,
		VersionManagerBackupIndependentFormat,
		VersionManagerRestoreIndependentFormat,
		VersionManagerLoadingNewFirmware,
		VersionManagerErrorLoadingNewFirmware,
		VersionManagerCurSysInvalid,
		VersionManagerResSysInvalid,
		VersionManagerNothingNewToInstall,
		VersionManagerUsbUpgradeComplete,
		VersionManager1STEvolPathInvalid,
		VersionManager2NDEvolPathInvalid,
		VersionManagerSCEEPWriteStarted,
		VersionManagerSCEEPWriteComplete,
		VersionManagerSCEEPWriteFailed,
		VersionManagerUsbWeigherDownloading,
		VersionManagerUsbWeigherDownloadSucceeded,
		VersionManagerUsbWeigherDownloadFailed,
		VersionManagerCantFindEvolutionINI,
		VersionManagerFlashFileStart,
		VersionManagerFlashFileComplete,
		VersionManagerUsbPrinterDownloading,
		VersionManagerUsbPrinterDownloadSucceeded,
		VersionManagerUsbPrinterDownloadFailed,
		VersionManagerUsbPrinterRetryOverflow,
		VMUSBWrapperRequestRemoveWrapperTarFiles,

		VersionManagerUsbDeviceDownloading,
		VersionManagerUsbDeviceDownloadSucceeded,
		VersionManagerUsbDeviceDownloadFailed,
		VersionManagerUsbDeviceResetting,
        VersionManagerAtmelDeviceDownloading,
        VersionManagerAtmelDeviceDownloadSucceeded,
        VersionManagerAtmelDeviceDownloadFailed,

		VersionManagerUsbDriveStillPresent = 120080,
		VMUSBCLARequestRemoveCLATarFiles	//Added 9/23 Alex Zavala
	}   VersionManagerDevError_t;

#endif // VERSIONMANAGEREVENT_H
