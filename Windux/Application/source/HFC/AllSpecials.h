/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ALLSPECIALS_H
#define ALLSPECIALS_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"

class DBAccess;

class AllSpecials : public HfcObject
{
	public:

		AllSpecials();
		~AllSpecials();

		void findAll(DBAccess *dbAccess);

		const char *serialize(int format = BrowserFormat) const;


	private:

		HobList numList;
		HobList descList;
		HobList firstGraphicList;
		HobList secondGraphicList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLSPECIALS_H
