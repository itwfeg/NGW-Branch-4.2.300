/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCVMSTATUSMSG_H
#define HFCVMSTATUSMSG_H

#include "VmStatusMsg.h"
#include "HfcObject.h"


class HfcVmStatusMsg : public VmStatusMsg, public HfcObject
{
	public:
		HfcVmStatusMsg();
		HfcVmStatusMsg(const char *data);
		~HfcVmStatusMsg();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCVMSTATUSMSG_H
