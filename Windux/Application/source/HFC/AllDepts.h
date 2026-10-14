/* HT Series Scale Project
 * COPYRIGHT (c) 2017.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#ifndef ALLDEPTS_H
#define ALLDEPTS_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"

class DBAccess;

class AllDepts : public HfcObject
{
	public:

		AllDepts();
		~AllDepts();

		const HobList getDeptNumList() const;

		void findAll(DBAccess *dbAccess);

		const char *serialize(int format = BrowserFormat) const;


	private:

		HobList numList;
		HobList name1List;
		HobList name2List;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLDEPTS_H
