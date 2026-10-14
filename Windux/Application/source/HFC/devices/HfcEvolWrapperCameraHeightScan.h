/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERCAMERAHEIGHTSCAN_H
#define HFCEVOLWRAPPERCAMERAHEIGHTSCAN_H

#include "evolution.h"
#include "HfcObject.h"
//#include "../../EvolutionRTC/WrapperManager/Include/EvolWrapperCameraHeightScan.h"
#include "EvolWrapperCameraHeightScan.h"

class HfcEvolWrapperCameraHeightScan : public EvolWrapperCameraHeightScan, public HfcObject
{
	public:
		HfcEvolWrapperCameraHeightScan(UINT16 newScan = 0);
		HfcEvolWrapperCameraHeightScan(const char *data);

		~HfcEvolWrapperCameraHeightScan();

		virtual const char *serialize(int format = BrowserFormat)const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//	HFCEVOLWRAPPERCAMERAHEIGHTSCAN_H
