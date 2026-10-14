/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ALLCATEGORIES_H
#define ALLCATEGORIES_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"

class DBAccess;

class AllCategories : public HfcObject
{
	public:

		AllCategories();
		~AllCategories();

		void findAll(DBAccess *dbAccess, LanguageSelection lang);
		void findAllMainCategories(DBAccess *dbAccess, LanguageSelection lang);

		const char *serialize(int format = BrowserFormat) const;


	private:

		HobList recordNumList;
		HobList nameList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLCATEGORIES_H
