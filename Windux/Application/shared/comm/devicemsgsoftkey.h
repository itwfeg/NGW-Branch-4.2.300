/**
 * @file devicemsgsoftkey.h
 * Softkey Device events and message classes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef DEVICEMSGSOFTKEY_H
#define DEVICEMSGSOFTKEY_H

#include "devicemsg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Softkey Device Event Types.
/**
 * Enumeration of events for the @c deviceMsgType::Softkey device.
 */
class softkeyDeviceMsgEventType
{
public:

    /// Event Types.
    enum type
    {
        // Softkey
        ReadSoftkey                         = 20,
        ReadAllSoftkeys,
        WriteSoftkey,
        WriteAllSoftkeys,
        DeleteSoftkey,
        DeleteAllSoftkeys,
        SaveSoftkey,                                // 26
        RestoreSoftkey,
        FirstSoftkey,                               // 28
        LastSoftkey,
        NextSoftkey,                                // 30
        PreviousSoftkey,
        CreateNewSoftkey,                           // 32
        RestoreFlashkeyAsSoftkey,
        ExecuteSoftkey,                             // 34
        BatchApplyDeleteSoftkey,

        // SoftkeyFeature
        ReadSoftkeyFeature                  = 40,
        ReadAllSoftkeyFeatures,
        WriteSoftkeyFeature,
        WriteAllSoftkeyFeatures,
        DeleteSoftkeyFeature,
        DeleteAllSoftkeyFeatures,
        SaveSoftkeyFeature,                         // 46
        RestoreSoftkeyFeature,
        FirstSoftkeyFeature,                        // 48
        LastSoftkeyFeature,
        NextSoftkeyFeature,                         // 50
        PreviousSoftkeyFeature,
        CreateNewSoftkeyFeature,                    // 52
        BatchApplyDeleteSoftkeyFeature,

        // SoftkeyStyle
        ReadSoftkeyStyle                    = 60,
        ReadAllSoftkeyStyles,
        WriteSoftkeyStyle,
        WriteAllSoftkeyStyles,
        DeleteSoftkeyStyle,
        DeleteAllSoftkeyStyles,
        SaveSoftkeyStyle,                           // 66
        RestoreSoftkeyStyle,
        FirstSoftkeyStyle,                          // 68
        LastSoftkeyStyle,
        NextSoftkeyStyle,                           // 70
        PreviousSoftkeyStyle,
        CreateNewSoftkeyStyle,                      // 72
        ReadAllSoftkeyStyleNumbers,
        WriteAllSoftkeyStyleNumbers,                // 74
        BatchApplyDeleteSoftkeyStyle,

        // SoftkeyGroup
        ReadSoftkeyGroup                    = 80,
        ReadAllSoftkeyGroups,
        WriteSoftkeyGroup,
        WriteAllSoftkeyGroups,
        DeleteSoftkeyGroup,
        DeleteAllSoftkeyGroups,
        SaveSoftkeyGroup,                           // 86
        RestoreSoftkeyGroup,
        FirstSoftkeyGroup,                          // 88
        LastSoftkeyGroup,
        NextSoftkeyGroup,                           // 90
        PreviousSoftkeyGroup,
        CreateNewSoftkeyGroup,                      // 92
        LoadSoftkeyGroup,                                   //	Used to display the entire softkey group (was previously Read/Write SoftkeyGroup)
        BatchApplyDeleteSoftkeyGroup,

        LoadAllSoftkeyGroups = 255
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::ReadSoftkey> softkeyReadSoftkeyDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::WriteSoftkey> softkeyWriteSoftkeyDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::DeleteSoftkey> softkeyDeleteSoftkeyDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::SaveSoftkey> softkeySaveSoftkeyDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::CreateNewSoftkey> softkeyCreateNewSoftkeyDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::ReadSoftkeyFeature> softkeyReadSoftkeyFeatureDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::WriteSoftkeyFeature> softkeyWriteSoftkeyFeatureDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::SaveSoftkeyFeature> softkeySaveSoftkeyFeatureDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::ReadSoftkeyStyle> softkeyReadSoftkeyStyleDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::WriteSoftkeyStyle> softkeyWriteSoftkeyStyleDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::SaveSoftkeyStyle> softkeySaveSoftkeyStyleDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::ReadSoftkeyGroup> softkeyReadSoftkeyGroupDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::ReadAllSoftkeyGroups> softkeyReadAllSoftkeyGroupsDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::WriteSoftkeyGroup> softkeyWriteSoftkeyGroupDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::WriteAllSoftkeyGroups> softkeyWriteAllSoftkeyGroupsDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::DeleteSoftkeyGroup> softkeyDeleteSoftkeyGroupDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::SaveSoftkeyGroup> softkeySaveSoftkeyGroupDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::CreateNewSoftkeyGroup> softkeyCreateNewSoftkeyGroupDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Softkey, softkeyDeviceMsgEventType::LoadSoftkeyGroup> softkeyLoadSoftkeyGroupDeviceMsg;

/** @} */

#endif // DEVICEMSGSOFTKEY_H
