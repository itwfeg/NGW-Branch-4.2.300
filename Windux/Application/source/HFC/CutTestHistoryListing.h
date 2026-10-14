/* HT-Series Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#ifndef CUTTESTHISTORYLISTING_H
#define CUTTESTHISTORYLISTING_H

#include "evolution.h"
#include "HfcObject.h"

class HobList;
class DBAccess;

class CutTestHistoryListing : public HfcObject
{
	public:

		CutTestHistoryListing();
		~CutTestHistoryListing();

		const char *serialize(int format = BrowserFormat) const;

		void createHistoryListing(DBAccess *dbAccess, const HobString &operNum);

		HobList *getCutTestNumList() const { return cutTestNumList; }


	private:

		HobList *cutTestNumList;  
		HobList *productNumList;  
		HobList *operatorNameList;
		HobList *statusList;      
		HobList *vendorNumList;
		HobList *primalCostList;  
		HobList *primalWeightList;
		HobList *lotNumList;      
		HobList *packingDateList; 

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CUTTESTHISTORYLISTING_H
