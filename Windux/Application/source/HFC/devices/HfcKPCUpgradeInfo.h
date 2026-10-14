/**
 * @file HfcKPCUpgradeInfo.h
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Joe E. Crew
 * @brief notifies the UI to start the KPC upgrade page
 * Created on: August 18, 2019
 */
#ifndef HFCKPCUPGRADEINFO_H_
#define HFCKPCUPGRADEINFO_H_
#include "HfcObject.h"
#include "KPCUpgradeInfo.h"

class HfcKPCUpgradeInfo : public KPCUpgradeInfo, public HfcObject
{
	public:
	HfcKPCUpgradeInfo(const char *data);

	~HfcKPCUpgradeInfo();

	virtual const char *serialize(int format = BrowserFormat) const;

	private:
};
#endif
