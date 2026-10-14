/**
 * @file devicemsgerror.h
 * Error Device events and message classes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef DEVICEMSGERROR_H
#define DEVICEMSGERROR_H

#include "devicemsg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Error Device Event Types.
/**
 * Enumeration of events for the @c deviceMsgType::Error device.
 */
class errorDeviceMsgEventType
{
public:

    /// Event Types.
    enum type
    {
        ErrUnknownDevice                    = 0,
        ErrWeigherDevice                    = 1,
        ErrEventDevice                      = 3,
        ErrOperationModeDevice              = 4,
        ErrLabelApplierDevice               = 5,
        ErrPrinterDevice                    = 7,
        ErrWrapperDevice                    = 9,
        ErrWrapperStatusDevice              = 10,
        ErrTouchScreenDevice                = 14,
        ErrRFIDReader                       = 15,
        ErrBarcodeScanner                   = 16,
        ErrSessionDevice                    = 100,
        ErrProductDevice                    = 101,
        ErrOperatorDevice                   = 102,
        ErrTotalTypeDevice                  = 104,
        ErrGraphicDevice                    = 105,
        ErrExpandedTextDevice               = 106,
        ErrBackupRestoreDevice              = 109,
        ErrClassDevice                      = 110,
        ErrConfigurationDevice              = 112,
        ErrVersionManagerDevice             = 120,
        ErrCategoryDevice                   = 121,
        ErrNetworkDevice                    = 122,
        ErrTotalsDevice                     = 123,
        ErrSpecialsDevice                   = 124,
        ErrOperatorChangesDevice            = 131,
        ErrCOOLDevice                       = 135,
        ErrSoftkeyDevice                    = 136,
        ErrImportDevice                     = 144,
        ErrNetworkConnectivityDevice        = 145,
        ErrSynchronizationDevice            = 146,
        ErrTransactionDevice                = 151,
        ErrHourlyTotalDevice                = 153,
        ErrOperatorTotalDevice              = 154,
        ErrProductTotalDevice               = 156,
        ErrWindowsConfigurationDevice       = 158,
		ErrCutTestDev                       = 159,
		ErrCutTestTotalDev                  = 160,
        ErrGCDSessionDevice                 = 161,
        ErrGCDDev                           = 162,
        ErrHIDDev                           = 163,
        ErrMfgTestDev                       = 164,
        ErrCookingInstructionsDev           = 165,
        ErrRecipeDev                        = 166,
        ErrExpandedText2Dev                 = 167,
        ErrSuggestiveSellingDev             = 168,
        ErrAllergensDev                     = 169,
        ErrRecallsDev                       = 170,
        ErrDeviceDev                        = 171,
        ErrDiagnostics                      = 173,
	    ErrDepartments				        = 175,
        ErrEOFDevice                        = 254,
        ErrRemoveAnyModalDialogDevice       = 255
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef deviceMsgBiDirectional<deviceMsgType::Error, errorDeviceMsgEventType::ErrSoftkeyDevice> errorErrSoftkeyDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Error, errorDeviceMsgEventType::ErrRemoveAnyModalDialogDevice> errorErrRemoveAnyModalDialogDeviceMsg;

/** @} */

#endif // DEVICEMSGERROR_H
