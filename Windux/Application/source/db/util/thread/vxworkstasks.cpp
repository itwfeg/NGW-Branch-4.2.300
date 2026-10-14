#include <algorithm>


#include "vxworkstasks.h"

#include "lock.h"
#include "mutex.h"
#include "timers.h"

#include "../e_string/utilstring.h"
#include "../../../util/Debug.h"



const int VxWorksTasks::MAX_TASK_SWITCHES = 4000;
Time VxWorksTasks::_firstTime_;
struct timespec VxWorksTasks::_lastClock_;
bool VxWorksTasks::_switchTaskRegistered_ = false;
int VxWorksTasks::_switchInfoIndex_ = 0;
int VxWorksTasks::_totalTaskSwitches_ = 0;
Mutex<> VxWorksTasks::_switchTaskInfoMutex_;
VxWorksTasks::switchTaskInfo_container VxWorksTasks::_switchTaskInfo_ = VxWorksTasks::switchTaskInfo_container(MAX_TASK_SWITCHES);

Mutex<> VxWorksTasks::_deleteHooksMutex_;
VxWorksTasks::deleteTaskHooks_container VxWorksTasks::_deleteHooks_ = VxWorksTasks::deleteTaskHooks_container();


int VxWorksTasks::currentThreadPriority(int p)
{
	int tid = taskIdSelf();
	int currentPriority = 0;
	
	if ( taskPriorityGet(tid, &currentPriority) == OK )
	{
		if ( p != -1 && taskPrioritySet(tid, p) != OK )
			currentPriority = -1;

		return currentPriority;
	}

	return -1;
}


int VxWorksTasks::incrementCurrentThreadPriority(int p)
{
	int tid = taskIdSelf();
	int currentPriority = 0;

	if ( taskPriorityGet(tid, &currentPriority) == OK )
	{
		if ( taskPrioritySet(tid, currentPriority - p) != OK )
			currentPriority = -1;

		return currentPriority;
	}

	return -1;
}


int VxWorksTasks::decrementCurrentThreadPriority(int p)
{
	int tid = taskIdSelf();
	int currentPriority = 0;

	if ( taskPriorityGet(tid, &currentPriority) == OK )
	{
		if ( taskPrioritySet(tid, currentPriority + p) != OK )
			currentPriority = -1;

		return currentPriority;
	}

	return -1;
}


bool VxWorksTasks::registerSwitchTaskHook()
{
	bool registered = false;

	if ( !_switchTaskRegistered_ )
	{
		Lock< Mutex<> > lock(_switchTaskInfoMutex_);

		if ( !_switchTaskRegistered_ )
		{
			_totalTaskSwitches_ = _switchInfoIndex_ = 0;
			_firstTime_ = Time();
			clock_gettime(CLOCK_REALTIME, &_lastClock_);

			if ( taskSwitchHookAdd((FUNCPTR)switchTaskHook) == OK )
				_switchTaskRegistered_ = true;
			else
				throw;

			registered = true;
		}
	}

	if ( registered )
		Debug::trace("TaskSwitching", 28, "Successfully registered switch task hook");

	return _switchTaskRegistered_;
}


bool VxWorksTasks::unregisterSwitchTaskHook()
{
	bool unregistered = false;

	if ( _switchTaskRegistered_ )
	{
		Lock< Mutex<> > lock(_switchTaskInfoMutex_);

		if ( _switchTaskRegistered_ )
		{
			if ( taskSwitchHookDelete((FUNCPTR)switchTaskHook) == OK )
				_switchTaskRegistered_ = false;
			else
				throw;

			clock_gettime(CLOCK_REALTIME, &_lastClock_);

			unregistered = true;
		}
	}

	if ( unregistered )
		Debug::trace("TaskSwitching", 28, "Successfully unregistered switch task hook");

	return !_switchTaskRegistered_;
}


STATUS VxWorksTasks::switchTaskHook(WIND_TCB* pOldTcb, WIND_TCB* pNewTcb)
{
	if ( pOldTcb && pNewTcb )
	{
		if ( _switchInfoIndex_ < MAX_TASK_SWITCHES )
		{
			SwitchTaskInfo& switchInfo = _switchTaskInfo_[_switchInfoIndex_];

			clock_gettime(CLOCK_REALTIME, &switchInfo.clock);
			if ( memcmp(&switchInfo.clock, &_lastClock_, sizeof(_lastClock_)) )
			{
				switchInfo.oldTaskID = (int)pOldTcb;
				memcpy(switchInfo.oldTaskName, pOldTcb->name, TASKNAME_LENGTH);
				switchInfo.oldTaskName[TASKNAME_LENGTH - 1] = '\0';
				switchInfo.newTaskID = (int)pNewTcb;
				memcpy(switchInfo.newTaskName, pNewTcb->name, TASKNAME_LENGTH);
				switchInfo.newTaskName[TASKNAME_LENGTH - 1] = '\0';

				++_switchInfoIndex_;
				_lastClock_ = switchInfo.clock;
			}
		}
	}

	++_totalTaskSwitches_;

	return OK;
}


void VxWorksTasks::reportSwitchTaskInfo()
{
	if ( !_switchTaskRegistered_ )
	{
		Debug::trace("TaskSwitching", 28, "*** Going to report switch task information:");

		int tempIndex;
		switchTaskInfo_container tempContainer;

		{
			Lock< Mutex<> > lock(_switchTaskInfoMutex_);
			tempIndex = _switchInfoIndex_;
			tempContainer = _switchTaskInfo_;
		}

		int milliseconds, totalMilliseconds = 0, largestMilliseconds = 0;
		struct tm time;
		Time lastTime = _firstTime_, newTime;
		for ( int i = 0; i < tempIndex; ++i )
		{
			SwitchTaskInfo& switchInfo = _switchTaskInfo_[i];

			localtime_r(&switchInfo.clock.tv_sec, &time);
			newTime.setTime(time.tm_hour, time.tm_min, time.tm_sec, switchInfo.clock.tv_nsec / 1000000);
			milliseconds = (newTime - lastTime).getTotalMilliseconds();

			if ( milliseconds > 17 )
			{
				Debug::trace("TaskSwitching", 28, switchInfo.oldTaskName + string("[") +
						ptrtostring((void*)switchInfo.oldTaskID) + "] run for " +
						inttostring(milliseconds) + "ms.");

				totalMilliseconds += milliseconds;
			}

			if ( milliseconds > largestMilliseconds )
				largestMilliseconds = milliseconds;

			lastTime = newTime;
		}

		if ( tempIndex )
		{
			SwitchTaskInfo& switchInfo = _switchTaskInfo_[_switchInfoIndex_ - 1];

			localtime_r(&_lastClock_.tv_sec, &time);
			newTime.setTime(time.tm_hour, time.tm_min, time.tm_sec, _lastClock_.tv_nsec / 1000000);
			milliseconds = (newTime - lastTime).getTotalMilliseconds();

			if ( milliseconds > 17 )
			{
				Debug::trace("TaskSwitching", 28, switchInfo.newTaskName + string("[") +
						ptrtostring((void*)switchInfo.newTaskID) + "] run for " +
						inttostring(milliseconds) + "ms.");

				totalMilliseconds += milliseconds;
			}

			if ( milliseconds > largestMilliseconds )
				largestMilliseconds = milliseconds;

			Debug::trace("TaskSwitching", 28, inttostring(tempIndex + 1) +
					" significant task switches (of a total of " + inttostring(_totalTaskSwitches_) +
					") and " + inttostring(totalMilliseconds) + " ms of significant execution." +
					" The longest execution period was " + inttostring(largestMilliseconds) + ".");
		}
		else
			Debug::trace("TaskSwitching", 28, "No tasks switches happened during the interval.");
	}
}


bool VxWorksTasks::registerDeleteTaskHook(DeleteTaskHook* th)
{
	Lock< Mutex<> > lock(_deleteHooksMutex_);

	if ( _deleteHooks_.empty() )
	{
		if ( taskDeleteHookAdd((FUNCPTR)deleteTaskHook) != OK )
			throw;
	}

	deleteTaskHooks_container::iterator iter = find(_deleteHooks_.begin(), _deleteHooks_.end(), th);
	if ( iter == _deleteHooks_.end() )
		_deleteHooks_.push_back(th);

	return true;
}


bool VxWorksTasks::unregisterDeleteTaskHook(DeleteTaskHook* th)
{
	Lock< Mutex<> > lock(_deleteHooksMutex_);

	deleteTaskHooks_container::iterator iter = find(_deleteHooks_.begin(), _deleteHooks_.end(), th);
	if ( iter != _deleteHooks_.end() )
		_deleteHooks_.remove(th);

	if ( _deleteHooks_.empty() )
		taskDeleteHookDelete((FUNCPTR)deleteTaskHook);

	return true;
}

#if 0
STATUS VxWorksTasks::deleteTaskHook(WIND_TCB* pTcb)
{
	VxWorksTasks::ThreadIDType tid = (VxWorksTasks::ThreadIDType)pTcb;//taskNameToId(pTcb->name);
	if ( tid != ERROR )
	{
		// In case calling back to report a task deleted causes that the callback be
		// unregistered, we do not perform the callbacks directly as we navigate the
		// registry, but indirectly by creating a temporary registry of the callbacks
		// we will be performing.
		deleteTaskHooks_container temp;

		{
			Lock< Mutex<> > lock(_deleteHooksMutex_);
			temp = _deleteHooks_;
		}

		// Perform callbacks if there is any object registered.
		if ( temp.size() )
		{
			deleteTaskHooks_container::iterator iter = temp.begin();
			while ( iter != temp.end() )
				(*iter++)->taskDeleted(tid);
		}
	}

	return OK;
}
#endif
STATUS VxWorksTasks::deleteTaskHook(WIND_TCB* pTcb)
{
	VxWorksTasks::ThreadIDType tid = (VxWorksTasks::ThreadIDType)pTcb;//taskNameToId(pTcb->name);
	if ( tid != ERROR )
	{
		Lock< Mutex<> > lock(_deleteHooksMutex_);

		// Perform callbacks if there is any object registered.
		if ( _deleteHooks_.size() )
		{
			deleteTaskHooks_container::iterator iter = _deleteHooks_.begin();
			while ( iter != _deleteHooks_.end() )
				(*iter++)->taskDeleted(tid);
		}
	}

	return OK;
}
