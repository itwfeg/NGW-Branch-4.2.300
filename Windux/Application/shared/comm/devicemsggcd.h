/**
 * @file devicemsggcd.h
 * GCD Device events and message classes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef DEVICEMSGGCD_H
#define DEVICEMSGGCD_H

#include "devicemsg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// GCD Device Event Types.
/**
 * Enumeration of events for the @c deviceMsgType::GCD device.
 */
class gcdDeviceMsgEventType
{
public:

    /// Event Types.
    enum type
    {
        Data                                = 11,
        LoadDisplayObjectXML,
        TestColor,
        ShutdownDevice,
        LanguageAndWeighMode,                       // 15
        SetVolume,
        Heartbeat,
        GetStatus,
        GetFileInfo,
        FileUpload,                                 // 20
        LoadPlaylist,
        StartFirmwareUpdate,
        StartPlaylistUpdate,
        SetPlaybackWaitTime,
        SetTimeInfo,                                // 25
        TransferStart,
        TransferDone,
        TransferError,
        SetUnitSeparator,
        SetCurrencySymbol,                          // 30
        NowServingCustomerNumber,
        DismissNowServingCustomerNumber,

        Error                               = 111,
        HeartbeatResponse,
        LoadDisplayObjectXMLResponse,
        LoadPlaylistResponse,
        Status,                                     // 115
        FileInfo,
        RequestFileUpload,
        FirmwareUpdateStaged,
        FirmwareUpdateCompleted,
        PlaylistUpdateStaged,                       // 120
        PlaylistUpdateCompleted
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // DEVICEMSGGCD_H
