/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllOperators.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBOperatorsRM.h"
#include "DBOperator.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllOperators::dataType = 0;
std::string AllOperators::dataTypeName("AllOperators"); 
#endif


AllOperators::AllOperators()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	nameList.setQuotingElements(true);
}

AllOperators::~AllOperators()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


void AllOperators::findAll(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	DBOperatorsRM *dbRM = dbAccess->getDBOperatorsRM();
	if (!dbRM)
		return;

	bool traceOn = Debug::isTracing("AllOperators");
	try
	{
		DBQuery<DBOperator>* allQuery = dbRM->retrieveAll();
		if (!allQuery)
			return;

		DBQuery<DBOperator>::const_iterator iter = allQuery->begin();
		while (iter != allQuery->end())
		{
			if (traceOn)
				Debug::trace("AllOperators", string("findAll - num ") + iter->getNumber() + "  name " + iter->getName());
			idList.push_back(iter->getNumber());
			rfidList.push_back(iter->getRFID());
			accessLevelList.push_back(inttostring(iter->getClassification()));
			nameList.push_back(iter->getName());
			languageList.push_back(iter->getLanguage());
			++iter;
		}
		delete allQuery;

		if (traceOn)
			Debug::trace("AllOperators", string("findAll - idList size ") + inttostring(idList.size()) + "  rfidList size " + inttostring(rfidList.size()) +
			                             "  nameList size " + inttostring(nameList.size()) + "  accessLevelList size " + inttostring(accessLevelList.size()) + "  languageList size " + inttostring(languageList.size()));
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "AllOperators::findAll() ********** database exception **********");
		DBException::Type t = dbe.getType();
		Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
	}
}

const char *AllOperators::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
//			if (idList.size())
			{
				serializedData.append(idList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(rfidList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(nameList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(accessLevelList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(languageList.serialize(format));
			}
			break;

		default:
			break;
	}
	return serializedData.c_str();
}

