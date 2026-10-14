/*
 * KPCUpgradeProgress.h
 *
 *  Created on: Mar 14, 2019
 *      Author: Eric Landes
 */

#ifndef KPCUPGRADEPROGRESS_H_
#define KPCUPGRADEPROGRESS_H_
#include "EvolMessage.h"
#include "KpcBootUpgraderEvent.h"
#include "KpcBootEvent.h"

class KPCUpgradeProgress: public EvolMessage {

public:
	KPCUpgradeProgress(const KPCUpgradeProgress&);
	KPCUpgradeProgress(const char *buffer);
	KPCUpgradeProgress(EvolDeviceType deviceType, Hobart::CEvolMsgReceiver * pSource);
	~KPCUpgradeProgress();

	const char *serialize() const;

	void show();

    int getProgress() const {return progress;};

    void setProgress(int pro) {progress = pro;};

private:
	KPCUpgradeProgress();
    int progress;

#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif
};

#endif /* KPCUPGRADEProgress_H_ */
