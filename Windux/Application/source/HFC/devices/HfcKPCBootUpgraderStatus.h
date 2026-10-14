/**
 * @file HfcKPCBootUpgraderStatus.h
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Joe Crew
 * @brief notifies the UI to start the KPC upgrade page
 * Created on: August 17, 2019
 */

#ifndef HFCKPCBOOTUPGRADERSTATUS_H_
#define HFCKPCBOOTUPGRADERSTATUS_H_
#include "HfcObject.h"
#include "KPCBootUpgraderStatus.h"

class HfcKPCBootUpgraderStatus : public KPCBootUpgraderStatus, public HfcObject
{
	public:
	HfcKPCBootUpgraderStatus(const char *data);

	~HfcKPCBootUpgraderStatus();

	virtual const char *serialize(int format = BrowserFormat) const;

	private:
};
#endif
