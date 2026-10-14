#ifndef VXWORKSTASKS_H
#define VXWORKSTASKS_H



#include <list>
#include <vector>

#include "vxWorks.h"
#include "semLib.h"
#include "time.h"
#include "taskHookLib.h"

#include "../time/dbtime.h"


class VxWorksTasks;

#ifdef TORNADO_2_2
template <class ThreadingModel> class Mutex;
#else
template <class ThreadingModel = VxWorksTasks> class Mutex;
#endif



class VxWorksTasks
{
	public:
		// Types, subtypes
		typedef int ThreadIDType;
		typedef ThreadIDType ThreadHandle;
		typedef SEM_ID MutexType;
		typedef int ThreadExitCodeType;

		static const int TASKNAME_LENGTH = 20;
		static const int MAX_TASK_SWITCHES;
		struct SwitchTaskInfo
		{
			ThreadIDType oldTaskID;
			char oldTaskName[TASKNAME_LENGTH];
			ThreadIDType newTaskID;
			char newTaskName[TASKNAME_LENGTH];
			struct timespec clock;
		};
		typedef std::vector<SwitchTaskInfo> switchTaskInfo_container;

		struct DeleteTaskHook
		{
			virtual ~DeleteTaskHook() {}
			virtual void taskDeleted(ThreadIDType id) = 0;
		};
		typedef std::list<DeleteTaskHook*> deleteTaskHooks_container;


		// --- Processing methods ---
		// Thread methods
		static ThreadIDType currentThreadID() { return taskIdSelf(); }
		static int currentThreadPriority(int p = -1);
		static int incrementCurrentThreadPriority(int p = 1);
		static int decrementCurrentThreadPriority(int p = 1);

		// Switch task hook methods
		static bool registerSwitchTaskHook();
		static bool unregisterSwitchTaskHook();
		static void reportSwitchTaskInfo();

		// Delete task hook methods
		static bool registerDeleteTaskHook(DeleteTaskHook* th);
		static bool unregisterDeleteTaskHook(DeleteTaskHook* th);

		// Mutex methods
		static MutexType createMutex(MutexType* = 0)
				{ return semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE | SEM_DELETE_SAFE); }
		static void acquireMutex(MutexType* mutex) { semTake(*mutex, WAIT_FOREVER); }
		static void releaseMutex(MutexType* mutex) { semGive(*mutex); }
		static void deleleMutex(MutexType* mutex) { semDelete(*mutex); }


	private:
		static Time _firstTime_;
		static struct timespec _lastClock_;
		static bool _switchTaskRegistered_;
		static int _switchInfoIndex_;
		static int _totalTaskSwitches_;
		static switchTaskInfo_container _switchTaskInfo_;
		static Mutex<VxWorksTasks> _switchTaskInfoMutex_;

		static deleteTaskHooks_container _deleteHooks_;
		static Mutex<VxWorksTasks> _deleteHooksMutex_;

		static STATUS switchTaskHook(WIND_TCB* pOldTcb, WIND_TCB* pNewTcb);
		static STATUS deleteTaskHook(WIND_TCB* pTcb);
};



#endif // VXWORKSTASKS_H

