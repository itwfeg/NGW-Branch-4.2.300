/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCGCDSYSTEMSTATUS_H
#define HFCGCDSYSTEMSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "SystemStatusInfo.h"
#include "GCDSystemStatus.h"

enum HfcGCDSysStatusFieldOffsets
{
	fieldOffsetHfcGCDSysStatusDDRTestPassed = 1,
	fieldOffsetHfcGCDSysStatusBoardVersion,
	fieldOffsetHfcGCDSysStatusWinCEVersion,
	fieldOffsetHfcGCDSysStatusGCDMGRSoftwareVersion,
	fieldOffsetHfcGCDSysStatusGCDDLLSoftwareVersion,

	endHfcGCDSysStatusFieldOffsets	//	MUST be last!
};

class HfcGCDSystemStatus : public GCDSystemStatus, public HfcObject
{
	public:

		HfcGCDSystemStatus();
		HfcGCDSystemStatus(const char *data);

		~HfcGCDSystemStatus();

		virtual const char *serialize(int format = BrowserFormat) const;
		
	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//HFCGCDSYSTEMSTATUS_H
