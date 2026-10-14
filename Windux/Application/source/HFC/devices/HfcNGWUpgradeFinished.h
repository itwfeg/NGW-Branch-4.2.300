/**
 * @file HfcNGWUpgradeFinished.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI to close upgrade page
 * Created on: April 5, 2016
 */
#ifndef HFCNGWUPGRADEFINISHED_H_
#define HFCNGWUPGRADEFINISHED_H_
#include "HfcObject.h"
#include "NGWUpgradeFinished.h"

class HfcNGWUpgradeFinished : public NGWUpgradeFinished, public HfcObject
{
	public:
	HfcNGWUpgradeFinished(const char *data);

	~HfcNGWUpgradeFinished();

	virtual const char *serialize(int format = BrowserFormat) const;

	private:

};
#endif
