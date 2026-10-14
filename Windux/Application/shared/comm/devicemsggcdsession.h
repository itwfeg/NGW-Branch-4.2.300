/**
 * @file devicemsggcdsession.h
 * GCD Session Device events and message classes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef DEVICEMSGGCDSESSION_H
#define DEVICEMSGGCDSESSION_H

#include "devicemsg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// GCD Session Device Event Types.
/**
 * Enumeration of events for the @c deviceMsgType::GCDSession device.
 */
class gcdSessionDeviceMsgEventType
{
public:

    /// Event Types.
    enum type
    {
        Restart                             = 0,
        ShuttingDown,
        ColorTestOn,
        ColorTestOff,
        Status,
        English,                                    // 5
        French,
        Spanish,
        Avoir,
        Metric,
        Kind,                                       // 10

        SetVolume                           = 32,
        UpdateViaFtpNow,
        UpdateViaUsbNow,
        SetPlaybackWaitTime
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // DEVICEMSGGCDSESSION_H
