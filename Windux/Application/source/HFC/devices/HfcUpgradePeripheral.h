/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCUPGRADEPERIPHERAL_H
#define HFCUPGRADEPERIPHERAL_H

#include "evolution.h"
#include "UpgradePeripheral.h"
#include "HfcObject.h"

class HfcUpgradePeripheral : public UpgradePeripheral, public HfcObject
{
	public:
		HfcUpgradePeripheral(const char *data);
		HfcUpgradePeripheral(int event, const char *data, int len, int formatType = BrowserFormat);

		~HfcUpgradePeripheral();

		virtual const char *serialize(int format = BrowserFormat) const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCUPGRADEPERIPHERAL_H
