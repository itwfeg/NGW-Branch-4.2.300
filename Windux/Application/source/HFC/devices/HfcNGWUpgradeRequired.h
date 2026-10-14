/**
 * @file HfcNGWUpgradeRequired.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI to start the NGW upgrade page
 * Created on: July 18, 2016
 */
#ifndef HFCNGWUPGRADEREQUIRED_H_
#define HFCNGWUPGRADEREQUIRED_H_
#include "HfcObject.h"
#include "NGWUpgradeRequired.h"

class HfcNGWUpgradeRequired : public NGWUpgradeRequired, public HfcObject
{
	public:
	HfcNGWUpgradeRequired(const char *data);

	~HfcNGWUpgradeRequired();

	virtual const char *serialize(int format = BrowserFormat) const;

};
#endif
