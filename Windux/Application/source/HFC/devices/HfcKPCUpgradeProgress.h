/**
 * @file HfcKPCUpgradeProgress.h
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Joe Crew
 * @brief notifies the UI to the KPC upgrade progress
 * Created on: August 18, 2019
 */
#ifndef HFCKPCUPGRADEPROGRESS_H_
#define HFCKPCUPGRADEPROGRESS_H_
#include "HfcObject.h"
#include "KPCUpgradeProgress.h"

class HfcKPCUpgradeProgress : public KPCUpgradeProgress, public HfcObject
{
	public:
	HfcKPCUpgradeProgress(const char *data);

	~HfcKPCUpgradeProgress();

	virtual const char *serialize(int format = BrowserFormat) const;

	private:
};
#endif




