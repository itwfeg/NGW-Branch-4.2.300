/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ALLCOOLPRETEXTS_H
#define ALLCOOLPRETEXTS_H

#include "evolution.h"
#include "HobList.h"

class DBAccess;

class AllCoolPretexts : public HobList
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		AllCoolPretexts();												//	All countries in the database
		AllCoolPretexts(const char *data, int len, int formatType);		//	All countries linked to this category
		~AllCoolPretexts();

		HobRecordNum getCategoryNum();
		void setCategoryNum(HobRecordNum number);

		HobRecordNum getDeptNum();
		void setDeptNum(HobRecordNum number);

		HobString getCategory();
		void setCategory(HobString name);

		void findAllPretexts(DBAccess *dbAccess);
		void findAllPretextsLinkedToCategory(DBAccess *dbAccess);
		void findAllPretextsLinkedToAllCategories(DBAccess *dbAccess);

		HobList *getRecordNumList();
		HobList *getNamesList();

		const char *serialize(int format = BrowserFormat) const;

		bool doExcludeAllCategories();
		void setExcludeAllCategories(bool value);

		void show();


	private:

		bool		forAllCategories;
		bool		excludeAllCategories;

		HobList		*recordNumList;
		HobList		*namesList;

		HobRecordNum	categoryNum;
		HobString		category;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLCOOLPRETEXTS_H
