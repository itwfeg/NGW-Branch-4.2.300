#ifndef NETWORKCONNECTIVITYEVENTS_H
#define NETWORKCONNECTIVITYEVENTS_H



#include "Event.h"
#include "DataTypeEvent.h"



// NetworkConnectivityManager Handled Events - NetworkConnectivityDev
typedef enum NetworkConnectivityEvent
{
	GoodComm = 20,
	BadComm,
	RequestCurrentStatus,
	CurrentStatus,
	BadCommRepeat,
	NetworkAvailable,
	NetworkUnavailable,
	NetworkSyncStarted,
	ActivityNormal,
	ActivityBusy,
	NetworkActivity,
	SnWriteNetworkStatus,
	StartContinuousNetworkStatus,
	StopContinuousNetworkStatus,
	RequestNetworkType,
	NetworkType
	
} NetworkConnectivityEvent_t;


// for ErrNetworkConnectivityDev
typedef enum NetworkConnectivityDevError
{
	NetworkConnectivityManagerErrUnknownError = NetworkConnectivityDev * DEVICE_ERROR_OFFSET,
	NetworkConnectivityManagerErrLostComm
} NetworkConnectivityDevError_t;




#endif // NETWORKCONNECTIVITYEVENTS_H

