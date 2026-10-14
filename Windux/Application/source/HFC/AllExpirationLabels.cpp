/* HLX Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllExpirationLabels.h"
#include "HobDateTime.h"
#include "Configuration.h"
#include "Long.h"
#include "Integer.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBExpirationLabelsRM.h"
#include "DBExpirationLabel.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllExpirationLabels::dataType = 0;
std::string AllExpirationLabels::dataTypeName("AllExpirationLabels"); 
#endif

AllExpirationLabels::AllExpirationLabels()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	descriptionList.setQuotingElements(true);
}

AllExpirationLabels::~AllExpirationLabels()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


void AllExpirationLabels::findAll(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	DBExpirationLabelsRM *dbRM = dbAccess->getDBExpirationLabelsRM();
	if (!dbRM)
		return;

	bool traceOn = Debug::isTracing("AllExpirationLabels");
	try
	{
		DBQuery<DBExpirationLabel>* allQuery = dbRM->retrieveAllForDept( Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum) );
		if (!allQuery)
			return;

		HobString timeString;
		HobDateTime nowDateTime;
		HobDateTime expDateTime;
		DBQuery<DBExpirationLabel>::const_iterator iter = allQuery->begin();
		while (iter != allQuery->end())
		{
			if (!iter->isExpirationNull())
			{
				if (traceOn)
					Debug::trace("AllExpirationLabels", string("findAll - num ") + iter->getPLU() + "  Date/Time is " + iter->getExpiration());
				pluNumList.push_back(iter->getPLU());

				//	The number of days until expiration (-1 = expired - date already past, 0 = today, 1 = tomorrow, etc.
				expDateTime.convertFromDBFormat(iter->getExpiration());
				if (nowDateTime >= expDateTime)
				{
					Debug::trace("AllExpirationLabels", "expiration time is past, setting to -1");
					daysExpiredList.push_back("-1");
				}
				else
				{
					int daysDiff = nowDateTime.daysUntil(expDateTime);  // in seconds
					Debug::trace("AllExpirationLabels", string("# of days from now to expiration time is ") + inttostring(daysDiff));
					daysExpiredList.push_back(Integer::toString(daysDiff));
				}

				//	The date/time of the expiration label
				timeString = expDateTime.formatTime(FormatTime_HHMM, HourFormat_AMPM);
				expiredTimeList.push_back(timeString);

				descriptionList.push_back(iter->getProductDescription());
			}
			++iter;
		}
		delete allQuery;

		if (traceOn)
			Debug::trace("AllExpirationLabels", string("findAll - pluNumList size ") + inttostring(pluNumList.size()) + "  daysExpiredList size " + inttostring(daysExpiredList.size()) +
			                                    "  expiredTimeList size " + inttostring(expiredTimeList.size()) + "  descriptionList size " + inttostring(descriptionList.size()));
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "AllExpirationLabels::findAll() ********** database exception **********");
		DBException::Type t = dbe.getType();
		Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
	}
}

const char *AllExpirationLabels::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
			if (pluNumList.size())
			{
				serializedData.append(pluNumList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(daysExpiredList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(expiredTimeList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(descriptionList.serialize(format));
			}
			break;

		default:
			break;
	}
	return serializedData.c_str();
}
