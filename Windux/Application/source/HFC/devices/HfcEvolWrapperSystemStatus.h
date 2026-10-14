/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERSYSTEMSTATUS_H
#define HFCEVOLWRAPPERSYSTEMSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "EvolWrapperSystemStatus.h"
#include "HobList.h"

typedef enum HfcEvolWrapperSysStatusFieldOffsets
{
	fieldOffsetEvolWrSysStatusReportSysError = 1,
	fieldOffsetEvolWrSysStatusNode,
	fieldOffsetEvolWrSysStatusAppVersion,
	fieldOffsetEvolWrSysStatusDataVersion,
	fieldOffsetEvolWrSysStatusBootVersion,
	fieldOffsetEvolWrSysStatusConfigVersion,
	fieldOffsetEvolWrSysStatusDeviceInitFlags,
	fieldOffsetEvolWrSysStatusErrorFlags,
	fieldOffsetEvolWrSysStatusWrapBoxChecksum,

	endHfcEvolWrSysStatusFieldOffsets	//	MUST be last!
} HfcEvolWrapperSysStatusFieldOffsets_t;


class HfcEvolWrapperSystemStatus : public EvolWrapperSystemStatus, public HfcObject
{
	public:
		HfcEvolWrapperSystemStatus(const char *data);

		~HfcEvolWrapperSystemStatus();

		virtual const char *serialize(int format = BrowserFormat)const;


	private:
		HobList	nodeList;
		HobList	configVersionList;
		HobList	deviceInitFlagsList;
		HobList	errorFlagsList;
		HobList	appVersionList;
		HobList	dataVersionList;
		HobList	bootVersionList;
		HobList	wrapBoxChecksumList;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//	HFCEVOLWRAPPERSYSTEMSTATUS_H
