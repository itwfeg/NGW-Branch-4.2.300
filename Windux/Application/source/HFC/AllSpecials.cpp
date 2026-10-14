/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllSpecials.h"
#include "Configuration.h"
#include "util/filesys/filename.h"
#include "Integer.h"
#include "Long.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBSpecialProgramsRM.h"
#include "DBSpecialProgram.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllSpecials::dataType = 0;
std::string AllSpecials::dataTypeName("AllSpecials"); 
#endif


AllSpecials::AllSpecials()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	descList.setQuotingElements(true);
}

// private
AllSpecials::~AllSpecials()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


void AllSpecials::findAll(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	DBSpecialProgramsRM *dbRM = dbAccess->getDBSpecialsRM();
	if (!dbRM)
		return;

	HobString filename;
	bool traceOn = Debug::isTracing("AllSpecials");
	try
	{
		DBQuery<DBSpecialProgram>* allQuery = dbRM->retrieveAllForDept( Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum) );
		if (!allQuery)
			return;

		DBQuery<DBSpecialProgram>::const_iterator iter = allQuery->begin();
		while (iter != allQuery->end())
		{
			if (traceOn)
				Debug::trace("AllSpecials", string("findAll - num ") + iter->getNumber() + "  desc " + iter->getDescription());
			numList.push_back(iter->getNumber());
			descList.push_back(iter->getDescription());

			filename = iter->getImage1();
			Filename file1(filename);
			if (!filename.empty() && file1.getFileNameExtension().empty())
				filename.append(Configuration::getInstance()->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDEFAULTEXTENSION));
			firstGraphicList.push_back(filename);

			filename = iter->getImage2();
			Filename file2(filename);
			if (!filename.empty() && file2.getFileNameExtension().empty())
				filename.append(Configuration::getInstance()->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDEFAULTEXTENSION));
			secondGraphicList.push_back(filename);

			++iter;
		}
		delete allQuery;

		if (traceOn)
			Debug::trace("AllSpecials", string("findAll - numList size ") + inttostring(numList.size()) + "  descList size " + inttostring(descList.size()) +
			                            "  firstGraphicList size " + inttostring(firstGraphicList.size()) + "  secondGraphicList size " + inttostring(secondGraphicList.size()));
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "AllSpecials::findAll() ********** database exception **********");
		DBException::Type t = dbe.getType();
		Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
	}
}

const char *AllSpecials::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
			if (numList.size())
			{
				serializedData.append(numList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(descList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(firstGraphicList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(secondGraphicList.serialize(format));
			}
			break;

		default:
			break;
	}
	return serializedData.c_str();
}

