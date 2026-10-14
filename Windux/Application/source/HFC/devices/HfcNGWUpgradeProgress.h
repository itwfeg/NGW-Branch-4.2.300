/**
 * @file HfcNGWUpgradeProgress.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI to the NGW upgrade progress
 * Created on: April 5, 2016
 */
#ifndef HFCNGWUPGRADEPROGRESS_H_
#define HFCNGWUPGRADEPROGRESS_H_
#include "HfcObject.h"
#include "NGWUpgradeProgress.h"

class HfcNGWUpgradeProgress : public NGWUpgradeProgress, public HfcObject
{
	public:
	HfcNGWUpgradeProgress(const char *data);

	~HfcNGWUpgradeProgress();

	virtual const char *serialize(int format = BrowserFormat) const;

	private:
};
#endif




