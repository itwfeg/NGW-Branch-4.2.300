/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PERIPHERALEVENT_H
#define PERIPHERALEVENT_H

#include "DataTypeEvent.h"

// Peripheral Generated Events - PeripheralDev
typedef enum PeripheralEvent
{
	// This data type uses the common data type events defined in DtEvent above.

	WriteLostPeripheralsList = 20,
	RemoveCheckingLostPeripheral,
	ReadCreatePeripheral
}   PeripheralEvent_t;

typedef enum PeripheralDevError
{
	PeripheralErrUnknownError = PeripheralDev * DEVICE_ERROR_OFFSET,
	PeripheralErrRecordNotFound,
	PeripheralErrDeleteAreYouSure
}   PeripheralDevError_t;

#endif // PERIPHERALEVENT_H
