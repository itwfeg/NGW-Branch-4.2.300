/**
 * @file devicemsgsession.h
 * Session Device events and message classes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef DEVICEMSGSESSION_H
#define DEVICEMSGSESSION_H

#include "devicemsg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Session Device Event Types.
/**
 * Enumeration of events for the @c deviceMsgType::Session device.
 */
class sessionDeviceMsgEventType
{
public:

    /// Event Types.
    enum type
    {
        Unknown                             = 0,
        Register,
        VersionInfo,
        TimedOut,
        Terminate,
        ReadDateTime,                               // 5
        WriteDateTime,
        ContinuousDateTime,
        StopDateTime,
        ReadWeighModeTransactionType,
        WriteWeighModeTransactionType,              // 10
        ReadDirectoryList,
        WriteDirectoryList,
        ReadDevicesList,
        WriteDevicesList,
        SyncDOMRequest,                             // 15
        SyncDOM,
        MonitorModeOn,
        MonitorModeOff,
        ReadCurrentWebPage,
        WriteCurrentWebPage,                        // 20
        CommStart,
        CommEnd,
        TouchTypeSetting,
        AllFilesDeleted,
        PrintKeyPressed,                            // 25
        ClearKeyPressed,
        SelectCurrentLanguage,
        ReadLocaleSettings,
        WriteLocaleSettings,
        StartStreamingGraphic,                      // 30
        StopStreamingGraphic,
        ReadStreamingGraphic,
        WriteStreamingGraphic,
        CreateDirectory,
        InsertNewDirectory,                         // 35
        PlatterTareOn,
        PlatterTareOff,
        RequestPlatterTare,
        DesktopAccess,
        UpdateUIFonts,                              // 40
        TarUserDirectory,
        TarSystemDirectory,
        RestoreIndependentData,
        TFTPdbDate,
        CompactFlashMakeBootrom,                    // 45
        CompactFlashCopyInternal,
        UsingEmptyDatabase,
        ReadScaleStatus,
        WriteScaleStatus,
        ReadStatistics,                             // 50
        WriteStatistics,
        ContinuousStatistics,
        StopStatistics,
        BootupCompleted,
        DisableInputEvent,                          // 55
        EnableInputEvent,
        ShutdownTSInput,
        SimulateTouch,
        SoundBeep,
        KeyActivation,                              // 60
        KeyDeactivation,
        RequestKeyActivation,
        KeyActivationStates,
        DBIndependentBackup,
        RequestDBCompress,                          // 65
        DBCompressStarting,
        DBCompressDone,
        PrinterMetrics,
        UsbKbdAttachChange,
        SelectActiveWebPageFrame,                   // 70
        DisplayingModalPopupKeyPadOn,
        DisplayingModalPopupKeyPadOff,
        BrowserMenuChanging,
        AddNewSession,
        PopupVirtualKeyboard,                       // 75
        DismissVirtualKeyboard,
        DBClosingTemporarily,
        DBReopened,
        PopupIntranetBrowserWindow,
        PopupScaleBrowserWindow,                    // 80
        RemoveSessionStarter,
        SetFlashkeyInternetWebPage,
        SwitchToIntranetFrame,
        RequestIntranetAddress,
        UsingMirrorDatabase,            			// 85
        RefreshFPPFrame,
        ProductSwitchToManualWeighMode,
        ProductReturnToPriorOpMode,

        StopDialogs                         = 90,
        RestartDialogs,
        ShutdownNonPrimary,
        PrimaryToMonitoringSessionMsg1,
        PrimaryToMonitoringSessionMsg2,
        PrimaryToMonitoringSessionPopDialog,		// 95
        ShutdownComplete,
        ShutdownRequest,
        Terminated,
        Reboot,

        TestMessage                         = 100,
        Delay,
        DebugTrace,
        DebugWrite,
        WeigherFrequency,
        TestAllProducts,                            // 105
        ConfigUpdate,
        FileHandlesUsage,
        SetNewFirmwareVersionSearchTime,
        SendShutdownSystemRequest,
        LogoffRequest,                              // 110

        TestAddTimer                        = 120,
        TestDeleteAllSessionTimers,
        TestDisplayAllTimers,

        ShowAllTasks                        = 125,

        DispatchConfigurationSaved          = 130,
        TellBrowserGotoHomepage,

        AddSessionAsTraceListener           = 140,
        RemoveSessionAsTraceListener,

        CreateLostPeripheralsTimer          = 150,

        DisplayAllWebPageFrames             = 170,

        InitializeSynchronization           = 180,
        TerminateSynchronization,
        ChangeSynchronizationType,
        ChangePrimaryScale
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef deviceMsgBiDirectional<deviceMsgType::Session, sessionDeviceMsgEventType::Register> sessionRegisterDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Session, sessionDeviceMsgEventType::TestMessage> sessionTestMessageDeviceMsg;

/** @} */

#endif // DEVICEMSGSESSION_H
