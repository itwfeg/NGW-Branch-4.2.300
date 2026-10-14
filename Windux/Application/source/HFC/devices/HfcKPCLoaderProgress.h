/**
 * @file HfcKPCLoaderProgress.h
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Joe E Crew
 * @brief notifies the UI to the KPC Loader progress
 * Created on: August 19, 2019
 */
#ifndef HFCKPCLOADERPROGRESS_H_
#define HFCKPCLOADERPROGRESS_H_
#include "HfcObject.h"
#include "KPCUpgradeProgress.h"

class HfcKPCLoaderProgress : public KPCUpgradeProgress, public HfcObject
{
	public:
	HfcKPCLoaderProgress(const char *data);

	~HfcKPCLoaderProgress();

	virtual const char *serialize(int format = BrowserFormat) const;

	private:
};
#endif




