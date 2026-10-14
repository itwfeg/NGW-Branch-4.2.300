/**
 * @file devicemsgconfiguration.h
 * Configuration Device events and message classes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef DEVICEMSGCONFIGURATION_H
#define DEVICEMSGCONFIGURATION_H

#include "devicemsg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Configuration Device Event Types.
/**
 * Enumeration of events for the @c deviceMsgType::Configuration device.
 */
class configurationDeviceMsgEventType
{
public:

    /// Event Types.
    enum type
    {
        CfgScaleRead                        = 20,
        CfgScaleWrite,
        CfgScaleSave,

        CfgCountrySettingsRead              = 30,
        CfgCountrySettingsWrite,
        CfgCountrySettingsSave,

        CfgFrequentShopperRead              = 40,
        CfgFrequentShopperWrite,
        CfgFrequentShopperSave,

        CfgPrintedFieldsRead                = 50,
        CfgPrintedFieldsWrite,
        CfgPrintedFieldsSave,

        CfgOperatorAccessRead               = 60,
        CfgOperatorAccessWrite,
        CfgOperatorAccessSave,

        CfgScaleOperationRead               = 70,
        CfgScaleOperationWrite,
        CfgScaleOperationSave,

        CfgGenericTextReadAll               = 80,
        CfgGenericTextWriteAll,
        CfgGenericTextUpdated,

        CfgStoreInfoRead                    = 90,
        CfgStoreInfoWrite,
        CfgStoreInfoSave,
        CfgStoreInfoRestore,

        CfgCurrencyRead                     = 100,
        CfgCurrencyWrite,
        CfgCurrencySave,

        CfgSelfServiceRead                  = 110,
        CfgSelfServiceWrite,
        CfgSelfServiceSave,

        CfgScaleUpgradeRead                 = 120,
        CfgScaleUpgradeWrite,
        CfgScaleUpgradeSave,

        CfgEmailRead                        = 130,
        CfgEmailWrite,
        CfgEmailSave,

        CfgPrepackRead                      = 140,
        CfgPrepackWrite,
        CfgPrepackSave,

        CfgExpirationLabelNoteRead          = 150,
        CfgExpirationLabelNoteWrite,
        CfgExpirationLabelNoteSave,

        CfgMarqueeRead                      = 160,
        CfgMarqueeWrite,
        CfgMarqueeSave,

        CfgFTPRead                          = 170,
        CfgFTPWrite,
        CfgFTPSave,

        CfgSynchronizationRead              = 180,
        CfgSynchronizationWrite,
        CfgSynchronizationSave,
        CfgSynchronizationRestore,
        CfgSyncListRead,
        CfgSyncListWrite,
        CfgSyncListSave,
        CfgSyncListRestore,

        CfgCommInterfaceRead                = 190,
        CfgCommInterfaceWrite,
        CfgCommInterfaceSave,

        CfgScaleManagementRead              = 200,
        CfgScaleManagementWrite,
        CfgScaleManagementSave,

        CfgRotationOverrideRead             = 210,
        CfgRotationOverrideWrite,
        CfgRotationOverrideSave,

        CfgReportsSetupRead                 = 220,
        CfgReportsSetupWrite,
        CfgReportsSetupSave,

        CfgGCDRead                          = 230,
        CfgGCDWrite,
        CfgGCDSave,
        CfgGCDServiceConfigChanged,                 // Message received from the UI when Service tech. turns on/off the GCD.

        CfgAccumulateItemsRead              = 240,
        CfgAccumulateItemsWrite,
        CfgAccumulateItemsSave,

        CfgDeleteAllFiles                   = 245,
        CfgBatchApplyDeleteAllFiles,

        CfgSaved                            = 253,
        CfgNetworkSaved                     = 254
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef deviceMsgBiDirectional<deviceMsgType::Configuration, configurationDeviceMsgEventType::CfgScaleRead> configurationScaleReadDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Configuration, configurationDeviceMsgEventType::CfgScaleWrite> configurationScaleWriteDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Configuration, configurationDeviceMsgEventType::CfgScaleSave> configurationScaleSaveDeviceMsg;

typedef deviceMsgBiDirectional<deviceMsgType::Configuration, configurationDeviceMsgEventType::CfgScaleOperationRead> configurationScaleOperationReadDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Configuration, configurationDeviceMsgEventType::CfgScaleOperationWrite> configurationScaleOperationWriteDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Configuration, configurationDeviceMsgEventType::CfgScaleOperationSave> configurationScaleOperationSaveDeviceMsg;

typedef deviceMsgBiDirectional<deviceMsgType::Configuration, configurationDeviceMsgEventType::CfgCurrencyRead> configurationCurrencyReadDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Configuration, configurationDeviceMsgEventType::CfgCurrencyWrite> configurationCurrencyWriteDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Configuration, configurationDeviceMsgEventType::CfgCurrencySave> configurationCurrencySaveDeviceMsg;

/** @} */

#endif // DEVICEMSGCONFIGURATION_H
