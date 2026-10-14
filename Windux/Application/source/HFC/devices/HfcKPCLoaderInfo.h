/**
 * @file HfcKPCLoaderInfo.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Job E. Crew
 * @brief notifies the UI to start the KPC Loader page
 * Created on: April 5, 2016
 */
#ifndef HFCKPCLOADERINFO_H_
#define HFCKPCLOADERINFO_H_
#include "HfcObject.h"
#include "KPCUpgradeInfo.h"

class HfcKPCLoaderInfo : public KPCUpgradeInfo, public HfcObject
{
	public:
	HfcKPCLoaderInfo(const char *data);

	~HfcKPCLoaderInfo();

	virtual const char *serialize(int format = BrowserFormat) const;

	private:
};
#endif
