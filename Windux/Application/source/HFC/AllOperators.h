/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ALLOPERATORS_H
#define ALLOPERATORS_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"

class DBAccess;

class AllOperators : public HfcObject
{
	public:

		AllOperators();
		~AllOperators();

		void findAll(DBAccess *dbAccess);

		const char *serialize(int format = BrowserFormat) const;


	private:

		HobList idList;
		HobList rfidList;
		HobList accessLevelList;
		HobList nameList;
		HobList languageList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLOPERATORS_H
