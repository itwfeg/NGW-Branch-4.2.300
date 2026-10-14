/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ALLEMAILALERTS_H
#define ALLEMAILALERTS_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"

class DBAccess;

class AllEmailAlerts : public HfcObject
{
	public:

		AllEmailAlerts();
		~AllEmailAlerts();

		void findAll(DBAccess *dbAccess);

		const char *serialize(int format = BrowserFormat) const;


	private:

		HobList idList;
		HobList enableFailureNotificationList;
		HobList enableResolutionNotificationList;
		HobList subjectList;
		HobList delayList;
		HobList enableHandsFreeFailureNotificationList;
		HobList enableHandsFreeResolutionNotificationList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLEMAILALERTS_H
