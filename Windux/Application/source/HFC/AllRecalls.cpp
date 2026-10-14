/* HLX Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllRecalls.h"
#include "HobDateTime.h"
#include "Integer.h"
#include "Bool.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBRecallsRM.h"
#include "DBRecall.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllRecalls::dataType = 0;
std::string AllRecalls::dataTypeName("AllRecalls"); 
#endif


AllRecalls::AllRecalls()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	recallNameList.setQuotingElements(true);
	receivedDateTimeList.setQuotingElements(true);
	textList.setQuotingElements(true);
}

AllRecalls::~AllRecalls()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


void AllRecalls::findAll(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	DBRecallsRM *dbRM = dbAccess->getDBRecallsRM();
	if (!dbRM)
		return;

	bool traceOn = Debug::isTracing("AllRecalls");
	try
	{
		DBQuery<DBRecall>* allQuery = dbRM->retrieveAll();
		if (!allQuery)
			return;

		DBQuery<DBRecall>::const_iterator iter = allQuery->begin();
		while (iter != allQuery->end())
		{
			if (traceOn)
				Debug::trace("AllRecalls", string("findAll - recall name is ") + iter->getName() + "  Rcvd Date/Time " + iter->getReceivedDateTime());
			recallNumList.push_back(iter->getNumber());
			recallNameList.push_back(iter->getName());
			receivedDateTimeList.push_back((iter->isReceivedDateTimeNull()) ? "" : iter->getReceivedDateTime());
			acknowledgedList.push_back(Bool::toString((iter->isAcknowledgedNull()) ? false : iter->getAcknowledged()));
			textList.push_back(iter->getText());
			++iter;
		}
		delete allQuery;

		if (traceOn)
			Debug::trace("AllRecalls", string("findAll - recallNumList size ") + inttostring(recallNumList.size()) + "  recallNameList size " + inttostring(recallNameList.size()) +
			                           "  receivedDateTimeList size " + inttostring(receivedDateTimeList.size()) + "  acknowledgedList size " + inttostring(acknowledgedList.size()) +
			                           "  textList size " + inttostring(textList.size()));
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "AllRecalls::findAll() ********** database exception **********");
		DBException::Type t = dbe.getType();
		Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
	}
}

const char *AllRecalls::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
			if ( recallNumList.size() )
			{
				//serializedData.append(recallNumList.serialize(format));
				//serializedData.append(BrowserFormatSeparator);
				serializedData.append(recallNameList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(receivedDateTimeList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(textList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(acknowledgedList.serialize(format));
			}
			break;

		default:
			break;
	}
	return serializedData.c_str();
}

