/*
 * KPCBootUpgraderEvent.h
 *
 *  Created on: Mar 11, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTUPGRADEREVENT_H_
#define KPCBOOTUPGRADEREVENT_H_

typedef enum KPCBootUpgraderEvent{
	KpcBootUpgraderGeneric,			//0
	KpcBootUpgraderStatus,
	KpcBootUpgraderVersion,
	KpcBootUpgraderAppVersion,
	KpcBootUpgraderAck,
	KpcBootUpgraderReady,			//5
	KpcBootUpgraderReqStatus,		
	KpcBootUpgraderCmd,
	KpcBootUpgraderFrame,
	KpcBootUpgraderFrame1,
	KpcBootUpgraderFrame2,			//10
	KpcBootUpgraderFrame3,			
	KpcBootUpgraderFrame4,
	KpcBootUpgraderFrame5,
	KpcBootUpgraderFrame6,
	KpcBootUpgraderFrame7,			//15
	KpcBootUpgraderFrame8,			
	KpcBootUpgraderFlush,
	KpcBootUpgraderReqVersion,
	KpcBootUpgraderReqAppVersion,
	KpcBootUpgraderDone,			//20
	KpcBootUpgraderRestart,			
	KpcBootUpgraderBulkMsg,
	KpcNoUpgraderEvent,

	KpcBootUpgraderStartUpgrade,		//    number of nodes for UI to present
	KpcBootUpgraderUpgradeStatus,		// 25 tell UI the status (nodeId,state,version,error) dont know what triggers it
	KpcBootUpgraderUpgradeProgress,     //    tell the UI the current progress. Prolly response to KpcStartUpgrade
	KpcBootUpgraderUpgradeFinished,		//    close UI upgrade page
	KpcBootUpgraderUpgradeRequired,		//    ask UI to open upgrade page
	KpcBootUpgraderUpgradeUIReady,		//    UI has signaled the upgrade page is ready
	KpcBootUpgraderReqUpgradeInfo,		// 30 UI requests upgrade information
	KpcBootUpgraderUpgradeInfo,			//    upgrade information

	KPCBootUpgraderEventEnd
}KPCBootUpgraderEvent_t;

#endif /* KPCBOOTUPGRADEREVENT_H_ */
