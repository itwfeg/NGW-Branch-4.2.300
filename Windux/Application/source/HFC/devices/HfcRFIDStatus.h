/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCRFIDSTATUS_H
#define HFCRFIDSTATUS_H

#include "evolution.h"
#include "RFIDStatus.h"
#include "HfcObject.h"

class HfcRFIDStatus : public RFIDStatus, public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HfcRFIDStatus(Hobart::CEvolMsgReceiver *pHobThread = NULL, UINT8 status = 0);
		HfcRFIDStatus(const char *data, int len, int formatType);

		~HfcRFIDStatus();

		virtual const char *serialize(int format) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCRFIDSTATUS_H
