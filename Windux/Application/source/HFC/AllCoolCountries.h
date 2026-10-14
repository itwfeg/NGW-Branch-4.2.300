/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ALLCOOLCOUNTRIES_H
#define ALLCOOLCOUNTRIES_H

#include "evolution.h"
#include "HobList.h"

class DBAccess;

class AllCoolCountries : public HobList
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		AllCoolCountries();												//	All countries in the database
		AllCoolCountries(const char *data, int len, int formatType);	//	All countries linked to this category
		~AllCoolCountries();

		HobRecordNum getCategoryNum();
		void setCategoryNum(HobRecordNum number);

		const HobString &getCategory() const;
		void setCategory(const HobString &name);

		void findAllCountries(DBAccess *dbAccess);
		void findAllCountriesLinkedToCategory(DBAccess *dbAccess);
		void findAllCountriesLinkedToAllCategories(DBAccess *dbAccess);

		bool doExcludeAllCategories();
		void setExcludeAllCategories(bool value);

		void show();


	private:

		bool forAllCategories;
		bool excludeAllCategories;

		HobRecordNum	categoryNum;
		HobString		category;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLCOOLCOUNTRIES_H
