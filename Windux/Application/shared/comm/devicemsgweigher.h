/**
 * @file devicemsgweigher.h
 * Weigher Device events and message classes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef DEVICEMSGWEIGHER_H
#define DEVICEMSGWEIGHER_H

#include "devicemsg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Weigher Device Event Types.
/**
 * Enumeration of events for the @c deviceMsgType::Weigher device.
 */
class weigherDeviceMsgEventType
{
public:

    /// Event Types.
    enum type
    {
        Wakeup                              = 0,
        Config,
        Inquire,
        Reset,
        Status,
        Enable,                                     // 5
        Disable,
        Mode,
        Rezero,
        Control,
        ColdReset,                                  // 10   //	Sent to the Atmel Sam-ba weigher. [PTR 387 & 388]

        Normal                               = 32,          // this is a command from rtc
        Lock,                                               // this is a command from rtc
        ReqWakeup,                                  // 35   // Request Wakeup message from Peripheral
        ReqStatus,                                          // Request Status Message From Peripheral
        ReqInquire,                                         // Request Inquire Message From Peripheral
        ReqKind,
        Kind,
        CachedStatus,                               // 40   // old, cached status reported to RTC

        StatusAndPrint                      = 99,
        CalibrationStep,                            // 100  // Used for messages between the browser and Session to calibrate weigher

        // the data within this message indicates if the calibration ends the steps portion of the calibration (0),
        // or if the whole calibration is ending (after the gain factor is checked) (1),
        // or if the user is cancelling the whole thing (2).
        CalibrationEnd,

        ReqInfo,                                            //	Request data for the Weigher Info page.
        Info,                                               //	The data for the Weigher Info page.
        StatusCounts,                                       // Used to send the raw counts to the Browser for the Weigher Info page.
        SaveInfo,                                   // 105  //	The modified data from the Weigher Info page.
        Attach,                                             // Set the weigher attached flag.
        DoneInfo,                                           //	The user is leaving the Weigher Info page.

        ConfigurationBegin                  = 110,
        ConfigurationEnd,

        TestInit                            = 200,
        TestRezero,
        TestReadConfig,

        TestUp1                             = 205,
        TestUp10th,
        TestUp100th,
        TestUp1000th,

        TestDown1                           = 210,
        TestDown10th,
        TestDown100th,
        TestDown1000th,

        TestSwitch                          = 215,
        TestSimulateWeigherStatus
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef deviceMsgBiDirectional<deviceMsgType::Weigher, weigherDeviceMsgEventType::Status> weigherStatusDeviceMsg;

/** @} */

#endif // DEVICEMSGWEIGHER_H
