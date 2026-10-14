/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTTOTALALLTOTALTYPES_H
#define PRODUCTTOTALALLTOTALTYPES_H

#include "HobList.h"

class DBAccess;


class ProductTotalAllTotalTypes : public HfcObject
{
	public:
		ProductTotalAllTotalTypes(HobRecordNum productNum, DBAccess *dbAccess);
		~ProductTotalAllTotalTypes();

		int getListCount();
		const char *serialize(int format) const;


	private:
		int		listCount;
		HobRecordNum productNumber;
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

#endif //PRODUCTTOTALALLTOTALTYPES_H




