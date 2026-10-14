/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ALLCOOLPRODUCTIONSTEPS_H
#define ALLCOOLPRODUCTIONSTEPS_H

#include "evolution.h"
#include "HobList.h"

class DBAccess;

class AllCoolProductionSteps : public HobList
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		AllCoolProductionSteps();												//	All countries in the database
		AllCoolProductionSteps(const char *data, int len, int formatType);		//	All countries linked to this category
		~AllCoolProductionSteps();

		HobRecordNum getCategoryNum();
		void setCategoryNum(HobRecordNum number);

		HobString getCategory();
		void setCategory(HobString name);

		void findAllProductionSteps(DBAccess *dbAccess);
		void findAllProductionStepsLinkedToCategory(DBAccess *dbAccess);
		void findAllProductionStepsLinkedToAllCategories(DBAccess *dbAccess);

		bool doExcludeAllCategories();
		void setExcludeAllCategories(bool value);

		void show();


	private:

		bool			forAllCategories;
		bool			excludeAllCategories;

		HobRecordNum	categoryNum;
		HobString		category;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLCOOLPRODUCTIONSTEPS_H
