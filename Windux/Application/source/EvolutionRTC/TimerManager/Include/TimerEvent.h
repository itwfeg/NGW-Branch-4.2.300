#ifndef TIMEREVENT_H
#define TIMEREVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"

	// Timer Generated Events - TimerDev
	typedef enum TimerEvent
	{
		TimerUnknown,
		TimerAdd,
		TimerDelete,
		TimerExpired
	}   TimerEvent_t;

	typedef enum TimedFunction
	{
		timedFuncNone,
		timedFuncBackupDatabase,
		timedFuncInactivityAutoLogout,
		timedFuncStreamingGraphic,
		timedFuncLookForNewFirmware,
		timedFuncContinuousStatistics,
		timedFuncCheckMissingPeripherals,
		timedFuncDailyTimersCheck,
		timedFuncGeneralTimer3,
		timedFuncShutdownWaitingForRemotesSessionsToQuit,
		timedFuncWrSystemInfoSaved,
		timedFuncEmailTimer,
		timedFuncNotifyLocalBatchesTimer,
		timedFuncSynchronizeAllSecondariesTimer,
		timedFuncBootupSynchronization,
		timedFuncRetryCommand,
		timedFuncNetworkOnlineCheck,
		timedFuncWrTimingsComplete,
		timedFuncChangeRotationPlacement,
		timedFuncMonitorMemory,				//	Uses the Accountant [CCD Sep 2011]
		timedFuncContinuousNetworkStatus,
		timedFuncDownloadFilesCheck,
		timedFuncPeripheralBootupCheck,
		timedFuncOfflineBeep,
		timedFuncGetHandsFreeDevicesInLoginRange,
		timedFuncDumpWrapperData,
		endOfTimedFunction
	}   TimedFunction_t;

#endif // TIMEREVENT_H
