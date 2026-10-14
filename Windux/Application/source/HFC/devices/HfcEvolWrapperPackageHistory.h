/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERPACKAGEHISTORY_H
#define HFCEVOLWRAPPERPACKAGEHISTORY_H

#include "EvolWrapperPackageHistory.h"
#include "HobList.h"

class HfcWrapperConfig;


class HfcEvolWrapperPackageHistory : public EvolWrapperPackageHistory, public HfcObject
{
	public:

		HfcEvolWrapperPackageHistory();
		HfcEvolWrapperPackageHistory(const char *data);

		~HfcEvolWrapperPackageHistory();

		virtual const char *serialize(int format = BrowserFormat)const;


	private:
		HobList	idList;
		HobList	widthList;
		HobList	lengthList;
		HobList heightList;
		HobList	lipList;
		HobList filmSelectedList;
		HobList trayNameList;
		HobList usedWrapBoxBestFitList;
		HobList modifiedNetStretchList;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCEVOLWRAPPERPACKAGEHISTORY_H
