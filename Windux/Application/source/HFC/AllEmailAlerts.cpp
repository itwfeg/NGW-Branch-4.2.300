/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllEmailAlerts.h"
#include "HobEmailAlert.h"
#include "Configuration.h"
#include "Integer.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBEmailAlertsRM.h"
#include "DBEmailAlert.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllEmailAlerts::dataType = 0;
std::string AllEmailAlerts::dataTypeName("AllEmailAlerts"); 
#endif

AllEmailAlerts::AllEmailAlerts()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	subjectList.setQuotingElements(true);
}

AllEmailAlerts::~AllEmailAlerts()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


void AllEmailAlerts::findAll(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	DBEmailAlertsRM *dbRM = dbAccess->getDBEmailAlertsRM();
	if (!dbRM)
		return;

	bool traceOn = Debug::isTracing("AllEmailAlerts");
	try
	{
		DBQuery<DBEmailAlert>* allQuery = dbRM->retrieveAll();
		if (!allQuery)
			return;

		bool isPrepack = Configuration::getInstance()->isPrepackScale();
		int alertNum = -1;
		DBQuery<DBEmailAlert>::const_iterator iter = allQuery->begin();
		while (iter != allQuery->end())
		{
			if (traceOn)
				Debug::trace("AllEmailAlerts", string("findAll - num ") + iter->getNumber() + "  subject is " + iter->getSubject1());

			alertNum = (isPrepack) ? Integer::valueOf(iter->getNumber()) : -1;		//	HA-72: We only need to check this if Prepack.
			if ((alertNum == EMAILALERT_GCD_PLAYLIST_ERROR) || (alertNum == EMAILALERT_GCD_FTP_ERROR))
			{
				Debug::trace("AllEmailAlerts", "findAll():  Email alert # " + Integer::toString(alertNum) + " does not apply to Prepack.");
			}
			else
			{
				idList.push_back(iter->getNumber());
				enableFailureNotificationList.push_back(Bool::toString(iter->getNotifyOnFailure()));
				enableResolutionNotificationList.push_back(Bool::toString(iter->getNotifyOnResolution()));
				enableHandsFreeFailureNotificationList.push_back(Bool::toString(iter->getNotifyHandsFreeDeviceOnFailure()));
				enableHandsFreeResolutionNotificationList.push_back(Bool::toString(iter->getNotifyHandsFreeDeviceOnResolution()));
				subjectList.push_back(iter->getSubject1());
				delayList.push_back(Integer::toString(iter->getDelay()));
			}
			++iter;
		}
		delete allQuery;

		if (traceOn)
			Debug::trace("AllEmailAlerts", string("findAll - idList size ") + inttostring(idList.size()) + "  subjectList size " + inttostring(subjectList.size()) +
			                               "  enableFailureNotificationList size " + inttostring(enableFailureNotificationList.size()) +
			                               "  enableResolutionNotificationList size " + inttostring(enableResolutionNotificationList.size()) + "  delayList size " + inttostring(delayList.size()) + 
			                               "  enableHandsFreeFailureNotificationList size " + inttostring(enableHandsFreeFailureNotificationList.size()) +
			                               "  enableHandsFreeResolutionNotificationList size " + inttostring(enableHandsFreeResolutionNotificationList.size()));
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "AllEmailAlerts::findAll() ********** database exception **********");
		DBException::Type t = dbe.getType();
		Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
	}
}

const char *AllEmailAlerts::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
			if (idList.size())
			{
				serializedData.append(idList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(subjectList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(enableFailureNotificationList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(enableResolutionNotificationList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(enableHandsFreeFailureNotificationList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(enableHandsFreeResolutionNotificationList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(delayList.serialize(format));
			}
			break;

		default:
			break;
	}
	return serializedData.c_str();
}
