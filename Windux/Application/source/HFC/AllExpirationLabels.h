/* HLX Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ALLEXPIRATIONLABELS_H
#define ALLEXPIRATIONLABELS_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"

class DBAccess;

class AllExpirationLabels : public HfcObject
{
	public:

		AllExpirationLabels();
		~AllExpirationLabels();

		void findAll(DBAccess *dbAccess);

		const char *serialize(int format = BrowserFormat) const;


	private:

		HobList pluNumList;
		HobList daysExpiredList;
		HobList expiredTimeList;
		HobList descriptionList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLEXPIRATIONLABELS_H
