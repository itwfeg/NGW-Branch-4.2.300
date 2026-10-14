/**
 * @file NGWEvent.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief NGW Wrapper Events
 * Created on: Jan 5, 2016
 *
 **/
#ifndef NGWEVENT_H_
#define NGWEVENT_H_

typedef enum NGWEvents
{
	_StartUpgrade,			// number of nodes for UI to present
	_UpgradeStatus,
	_UpgradeProgress,
	_UpgradeFinished,		// close UI upgrade page
	_UpgradeRequired,		// ask UI to open upgrade page
	_UpgradeUIReady,		// UI has signaled the upgrade page is ready
	_WrNGWScan,				// have session scan for AWS3 if not attached on power up or after detachment
	_UpgradeCancel,			// UI has signaled that the user wishes to cancel the upgrade
	_ReqUpgradeInfo,		// UI requests upgrade information
	_UpgradeInfo,			// upgrade information
	_WrNGWScanFinished,
	_WrStartKPC
}NGWEvents_t;


#endif
