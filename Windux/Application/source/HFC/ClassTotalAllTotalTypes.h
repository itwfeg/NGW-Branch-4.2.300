/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CLASSTOTALALLTOTALTYPES_H
#define CLASSTOTALALLTOTALTYPES_H

#include "HobList.h"

class DBAccess;


class ClassTotalAllTotalTypes : public HfcObject
{
	public:
		ClassTotalAllTotalTypes(HobRecordNum classNum, DBAccess *dbAccess);
		~ClassTotalAllTotalTypes();

		int getListCount();
		const char *serialize(int format) const;


	private:
		int		listCount;
		HobRecordNum classNumber;
		HobString description;
		HobList	totalTypeNumber;
		HobList	totalWeight;
		HobList	totalPrice;
		HobList	totalPackages;
		HobList	totalRuns;
		HobList	totalItems;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CLASSTOTALALLTOTALTYPES_H




