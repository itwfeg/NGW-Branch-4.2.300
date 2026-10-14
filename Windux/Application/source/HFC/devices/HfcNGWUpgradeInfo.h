/**
 * @file HfcNGWUpgradeInfo.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI to start the NGW upgrade page
 * Created on: April 5, 2016
 */
#ifndef HFCNGWUPGRADEINFO_H_
#define HFCNGWUPGRADEINFO_H_
#include "HfcObject.h"
#include "NGWUpgradeInfo.h"

class HfcNGWUpgradeInfo : public NGWUpgradeInfo, public HfcObject
{
	public:
	HfcNGWUpgradeInfo(const char *data);

	~HfcNGWUpgradeInfo();

	virtual const char *serialize(int format = BrowserFormat) const;

	private:
};
#endif
