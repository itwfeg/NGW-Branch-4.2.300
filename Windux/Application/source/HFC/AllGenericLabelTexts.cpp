/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllGenericLabelTexts.h"
#include "Configuration.h"
#include "Long.h"
#include "Integer.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBLabelTextsRM.h"
#include "DBLabelText.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllGenericLabelTexts::dataType = 0;
std::string AllGenericLabelTexts::dataTypeName("AllGenericLabelTexts"); 
#endif


AllGenericLabelTexts::AllGenericLabelTexts()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	textList.setQuotingElements(true);
}

AllGenericLabelTexts::~AllGenericLabelTexts()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


void AllGenericLabelTexts::findAll(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	DBLabelTextsRM *dbRM = dbAccess->getDBLabelTextsRM();
	if (!dbRM)
		return;

	int count = 1;
	bool traceOn = Debug::isTracing("AllGenericLabelTexts");
	try
	{
		DBQuery<DBLabelText>* allQuery = dbRM->retrieveAll();
		if (!allQuery)
			return;

		DBQuery<DBLabelText>::const_iterator iter = allQuery->begin();
		while (iter != allQuery->end())
		{
			int num = stringtoint(iter->getNumber());
			if (traceOn)
				Debug::trace("AllGenericLabelTexts", string("findAll - num ") + inttostring(num) + "  text " + iter->getText() + "  count " + inttostring(count));
			while (num > count)
			{
				idList.push_back(inttostring(count++));
				textList.push_back("");
			}
			idList.push_back(inttostring(num));
			textList.push_back(iter->getText());
			count++;
			++iter;
		}
		delete allQuery;

		if (traceOn)
			Debug::trace("AllGenericLabelTexts", string("findAll - idList size ") + inttostring(idList.size()) + "  textList size " + inttostring(textList.size()));
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "AllGenericLabelTexts::findAll() ********** database exception **********");
		DBException::Type t = dbe.getType();
		Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
	}
}

const char *AllGenericLabelTexts::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
			if (idList.size())
			{
				serializedData.append(idList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(textList.serialize(format));
			}
			break;

		default:
			break;
	}
	return serializedData.c_str();
}

