#ifndef CONFIGURATIONEVENT_H
#define CONFIGURATIONEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	// Configuration Generated Events - ConfigurationDev
	typedef enum ConfigurationEvent
	{
		// This data type uses the common data type events defined in DtEvent above.

		CfgScaleRead = 20,
		CfgScaleWrite,
		CfgScaleSave,
		// Deprecated  CfgScaleWriteSingleField,
		// Deprecated  CfgScaleSaveSingleField,

		CfgCountrySettingsRead = 30,
		CfgCountrySettingsWrite,
		CfgCountrySettingsSave,

		CfgFrequentShopperRead = 40,
		CfgFrequentShopperWrite,
		CfgFrequentShopperSave,

		CfgPrintedFieldsRead = 50,
		CfgPrintedFieldsWrite,
		CfgPrintedFieldsSave,

		CfgSupportedLabelLengthsRead  = 55,
		CfgSupportedLabelLengthsWrite = 56,
		CfgSupportedLabelLengthsSave  = 57,

		CfgOperatorAccessRead = 60,
		CfgOperatorAccessWrite,
		CfgOperatorAccessSave,

		CfgScaleOperationRead = 70,
		CfgScaleOperationWrite,
		CfgScaleOperationSave,

		CfgGenericTextReadAll = 80,
		CfgGenericTextWriteAll,
		CfgGenericTextUpdated,

		CfgStoreInfoRead = 90,
		CfgStoreInfoWrite,
		CfgStoreInfoSave,
		CfgStoreInfoRestore,

		CfgCurrencyRead = 100,
		CfgCurrencyWrite,
		CfgCurrencySave,

		CfgSelfServiceRead = 110,
		CfgSelfServiceWrite,
		CfgSelfServiceSave,

		CfgScaleUpgradeRead = 120,
		CfgScaleUpgradeWrite,
		CfgScaleUpgradeSave,

		CfgEmailRead = 130,
		CfgEmailWrite,
		CfgEmailSave,

		CfgPrepackRead = 140,
		CfgPrepackWrite,
		CfgPrepackSave,

		CfgExpirationLabelNoteRead = 150,
		CfgExpirationLabelNoteWrite,
		CfgExpirationLabelNoteSave,

		CfgMarqueeRead = 160,
		CfgMarqueeWrite,
		CfgMarqueeSave,

		CfgFTPRead = 170,
		CfgFTPWrite,
		CfgFTPSave,
		CfgFTPResponseWrite = 177,
    

		CfgSynchronizationRead = 180,
		CfgSynchronizationWrite,
		CfgSynchronizationSave,
		CfgSynchronizationRestore,
		CfgSyncListRead,
		CfgSyncListWrite,
		CfgSyncListSave,
		CfgSyncListRestore,

		CfgCommInterfaceRead = 190,
		CfgCommInterfaceWrite,
		CfgCommInterfaceSave,

		CfgScaleManagementRead = 200,
		CfgScaleManagementWrite,
		CfgScaleManagementSave,

		// SOF-4769: Scheduled Merge
		CfgCommIntfaceScheduledRead = 205,
		CfgCommIntfaceScheduledWrite,
		CfgCommIntfaceScheduledSave,

		CfgRotationOverrideRead = 210,
		CfgRotationOverrideWrite,
		CfgRotationOverrideSave,

		CfgReportsSetupRead = 220,
		CfgReportsSetupWrite,
		CfgReportsSetupSave,

		CfgGCDRead = 230,
		CfgGCDWrite,
		CfgGCDSave,
		CfgGCDServiceConfigChanged,  // Message received from the UI when Service tech. turns on/off the GCD.

		CfgAccumulateItemsRead = 240,
		CfgAccumulateItemsWrite,
		CfgAccumulateItemsSave,

		CfgDeleteAllFiles = 245,
		CfgBatchApplyDeleteAllFiles,
		CfgDeleteAllFilesInProgress,
		CfgDeleteAllFilesFinished,

		CfgSaved = 253,
		CfgNetworkSaved = 254
	}   ConfigurationEvent_t;

#endif // CONFIGURATIONEVENT_H
