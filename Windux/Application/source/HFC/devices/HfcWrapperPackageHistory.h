/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERPACKAGEHISTORY_H
#define HFCWRAPPERPACKAGEHISTORY_H

#include "WrapperPackageHistory.h"
#include "HobList.h"

class HfcWrapperConfig;


class HfcWrapperPackageHistory : public WrapperPackageHistory, public HfcObject
{
	public:

		HfcWrapperPackageHistory();
		HfcWrapperPackageHistory(const char *data);

		~HfcWrapperPackageHistory();

		virtual const char *serialize(int format) const;

		void getPackCodeNames(HfcWrapperConfig *wrapConfig = 0);


	private:
		HobList	packCodeNameList;
		HobList	packageWidthList;
		HobList	packageLengthList;
		HobList packCodeNumList;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCWRAPPERPACKAGEHISTORY_H
