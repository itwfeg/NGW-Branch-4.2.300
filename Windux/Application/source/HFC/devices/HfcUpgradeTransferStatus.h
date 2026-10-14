/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCUPGRADETRANSFERSTATUS_H
#define HFCUPGRADETRANSFERSTATUS_H

#include "evolution.h"
#include "UpgradeTransferStatus.h"
#include "HfcObject.h"

class HfcUpgradeTransferStatus : public UpgradeTransferStatus, public HfcObject
{
	public:
		HfcUpgradeTransferStatus(const char *data);

		~HfcUpgradeTransferStatus();

		virtual const char *serialize(int format = BrowserFormat) const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCUPGRADETRANSFERSTATUS_H
