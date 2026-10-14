#ifndef WINDOWSCONFIGURATIONEVENTS_H
#define WINDOWSCONFIGURATIONEVENTS_H



#include "Event.h"
#include "DataTypeEvent.h"



// WindowsConfigurationManager Handled Events - WindowsConfigurationDev
typedef enum WindowsConfigurationEvent
{
	backupWirelessProfiles = 20
}   WindowsConfigurationEvent_t;


// for ErrNetworkConnectivityDev
typedef enum WindowsConfigurationDevError
{
	WindowsConfigurationErrUnknownError = WindowsConfigurationDev * DEVICE_ERROR_OFFSET,
	WindowsConfigurationErrBackupSuccessfull,
	WindowsConfigurationErrBackupFailed,
	WindowsConfigurationErrRestoreSuccessfull,
	WindowsConfigurationErrRestoreFailed
}   WindowsConfigurationDevError_t;




#endif // WINDOWSCONFIGURATIONEVENTS_H

