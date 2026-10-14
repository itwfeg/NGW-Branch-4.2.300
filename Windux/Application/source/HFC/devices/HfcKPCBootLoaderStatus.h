/**
 * @file HfcKPCBootLoaderStatus.h
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Joe Crew
 * @brief notifies the UI to start the KPC upgrade page
 * Created on: August 17, 2019
 */

#ifndef HFCKPCBOOTLOADERSTATUS_H_
#define HFCKPCBOOTLOADERSTATUS_H_
#include "HfcObject.h"
#include "KPCBootStatus.h"

class HfcKPCBootLoaderStatus : public KPCBootStatus, public HfcObject
{
	public:
	HfcKPCBootLoaderStatus(const char *data);

	~HfcKPCBootLoaderStatus();

	virtual const char *serialize(int format = BrowserFormat) const;

	private:
};
#endif
