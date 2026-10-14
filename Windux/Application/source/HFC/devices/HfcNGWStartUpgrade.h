/**
 * @file HfcNGWStartUpgrade.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI the number of nodes to present on upgrade page
 * Created on: April 5, 2016
 */
#ifndef HFCNGWSTARTUPGRADE_H_
#define HFCNGWSTARTUPGRADE_H_
#include "HfcObject.h"
#include "NGWStartUpgrade.h"

class HfcNGWStartUpgrade : public NGWStartUpgrade, public HfcObject
{
	public:
	HfcNGWStartUpgrade(const char *data);

	~HfcNGWStartUpgrade();

	virtual const char *serialize(int format = BrowserFormat) const;

	private:

};

#endif
