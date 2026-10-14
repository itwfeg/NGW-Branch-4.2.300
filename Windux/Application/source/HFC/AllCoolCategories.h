/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ALLCOOLCATEGORIES_H
#define ALLCOOLCATEGORIES_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"

class DBAccess;

class AllCoolCategories : public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		AllCoolCategories();											//	All categories in the database
		AllCoolCategories(const char *data, int len, int formatType);	//	All categories linked to this country/production step/pretext
		~AllCoolCategories();

		void findAllCategories(DBAccess *dbAccess);
		void findAllCategoriesLinkedToCountry(DBAccess *dbAccess);
		void findAllCategoriesLinkedToProductionStep(DBAccess *dbAccess);
		void findAllCategoriesLinkedToPretext(DBAccess *dbAccess);

		HobList *getRecordNumList();
		HobList *getNamesList();

		const HobString &getLinkTo() const;
		void setLinkTo(HobString &name);

		const char *serialize(int format = BrowserFormat) const;


	private:

		HobList			*recordNumList;
		HobList			*namesList;
		
		HobString		linkTo;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLCOOLCATEGORIES_H
