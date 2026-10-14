/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRODUCTDESCLOOKUP_H
#define HFCPRODUCTDESCLOOKUP_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"

namespace edb
{
class DBProductsRM;
}

class HfcProductDescLookup : public HfcObject
{
	public:

		HfcProductDescLookup(const char *data, int len, int formatType);

		~HfcProductDescLookup();

		virtual const char *serialize(int format) const;

		void lookupProductsByDesc(edb::DBProductsRM *dbProductsRM, LanguageSelection language = PrimaryLanguage);


	private:
		/* If the lookupMethod is 'lookupByDescription',
		 * then this is the string to look for in the Product's description.
		 * If the lookupMethod is 'lookupPluRange', then this is the Product number
		 * which we want to find products before (lower plu numbers) and after (higher plu numbers).
		 * CfgPrepack.pluSearchRange specifies how many plu's before and after the number
		 * indicated should be found.
		 */
		HobString	lookupString;
		short			lookupMethod;

		HobList		productNumList;
		HobList		descriptionList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCPRODUCTDESCLOOKUP_H




