/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ALLGENERICLABELTEXTS_H
#define ALLGENERICLABELTEXTS_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"

class DBAccess;


class AllGenericLabelTexts : public HfcObject
{
	public:

		AllGenericLabelTexts();
		~AllGenericLabelTexts();

		void findAll(DBAccess *dbAccess);

		const char *serialize(int format = BrowserFormat) const;


	private:

		HobList idList;
		HobList textList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLGENERICLABELTEXTS_H
