/* HLX Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HOBALLRECALLS_H
#define HOBALLRECALLS_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"

class DBAccess;

class AllRecalls : public HfcObject
{
	public:

		AllRecalls();
		~AllRecalls();

		void findAll(DBAccess *dbAccess);

		const char *serialize(int format = BrowserFormat) const;


	private:

		HobList recallNumList;
		HobList recallNameList;
		HobList receivedDateTimeList;
		HobList textList;
		HobList acknowledgedList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HOBALLRECALLS_H
