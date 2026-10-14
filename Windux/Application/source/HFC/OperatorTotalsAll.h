/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef OPERATORTOTALSALL_H
#define OPERATORTOTALSALL_H

#include "HobList.h"

class DBAccess;
class OperatorManager;


class OperatorTotalsAll : public HfcObject
{
	public:
		OperatorTotalsAll(OperatorManager *operatorManager = NULL, DBAccess *dbAccess = (DBAccess *)NULL);
		~OperatorTotalsAll();

		int getListCount();
		const char *serialize(int format) const;


	private:
		int		listCount;
		HobList	operatorId;
		HobList	operName;
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

#endif //OPERATORTOTALSALL_H




