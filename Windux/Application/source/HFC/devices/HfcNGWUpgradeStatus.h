/**
 * @file HfcNGWUpgradeStatus.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI to start the NGW upgrade page
 * Created on: April 5, 2016
 */
#ifndef HFCNGWUPGRADESTATUS_H_
#define HFCNGWUPGRADESTATUS_H_
#include "HfcObject.h"
#include "NGWUpgradeStatus.h"

class HfcNGWUpgradeStatus : public NGWUpgradeStatus, public HfcObject
{
	public:
	HfcNGWUpgradeStatus(const char *data);

	~HfcNGWUpgradeStatus();

	virtual const char *serialize(int format = BrowserFormat) const;

	private:
};
#endif
