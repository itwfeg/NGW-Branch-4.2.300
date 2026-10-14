/* Access Back-room Scale Project
 * COPYRIGHT (c) 2008.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCUPGRADELIST_H
#define HFCUPGRADELIST_H

#include "evolution.h"
#include "UpgradeList.h"
#include "HfcObject.h"
#include "HobList.h"

class HfcUpgradeList : public UpgradeList, public HfcObject
{
	public:
		HfcUpgradeList(const char *data);

		~HfcUpgradeList();

		virtual const char *serialize(int format = BrowserFormat) const;

		void add(std::list<UpgradeVersions>::iterator iter);

	private:
		HobList	components;
		HobList curVersions;
		HobList newVersions;
		HobList isPending;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCUPGRADELIST_H
