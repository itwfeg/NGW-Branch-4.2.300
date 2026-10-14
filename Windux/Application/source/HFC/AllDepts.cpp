/* HT Series Scale Project
 * COPYRIGHT (c) 2017.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#include "AllDepts.h"
#include "Integer.h"
#include "Long.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBDeptsRM.h"
#include "DBDept.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllDepts::dataType = 0;
std::string AllDepts::dataTypeName("AllDepts"); 
#endif


AllDepts::AllDepts()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

AllDepts::~AllDepts()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


const HobList AllDepts::getDeptNumList() const
{
	return numList;
}

void AllDepts::findAll(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	DBDeptsRM *dbRM = dbAccess->getDBDeptsRM();
	if (!dbRM)
		return;

	bool traceOn = Debug::isTracing("AllDepts");
	try
	{
		DBQuery<DBDept>* allQuery = dbRM->retrieveAll();
		if (!allQuery)
			return;

		DBQuery<DBDept>::const_iterator iter = allQuery->begin();
		while (iter != allQuery->end())
		{
			if (traceOn)
				Debug::trace("AllDepts", string("findAll - deptnum ") + iter->getNumber() + "  name " + iter->getName1());
			numList.push_back(iter->getNumber());
			name1List.push_back(iter->getName1());
			name2List.push_back(iter->getName2());
			++iter;
		}
		delete allQuery;

		if (traceOn)
			Debug::trace("AllDepts", string("findAll - numList size ") + inttostring(numList.size()) + "  name1List size " + inttostring(name1List.size()) + "  name2List size " + inttostring(name2List.size()));
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "AllDepts::findAll() ********** database exception **********");
		DBException::Type t = dbe.getType();
		Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
	}
}

const char *AllDepts::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
			if ( numList.size() )
			{
				serializedData.append(numList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(name1List.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(name2List.serialize(format));
			}
			break;

		default:
			break;
	}
	return serializedData.c_str();
}

