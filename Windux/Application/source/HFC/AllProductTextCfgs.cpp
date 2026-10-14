/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllProductTextCfgs.h"
#include "Long.h"
#include "Integer.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBProductTextCfgsRM.h"
#include "DBProductTextCfg.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllProductTextCfgs::dataType = 0;
std::string AllProductTextCfgs::dataTypeName("AllProductTextCfgs"); 
#endif


AllProductTextCfgs::AllProductTextCfgs()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

AllProductTextCfgs::~AllProductTextCfgs()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


void AllProductTextCfgs::findAll(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	DBProductTextCfgsRM *dbRM = dbAccess->getDBProductTextCfgsRM();
	if (!dbRM)
		return;

	bool traceOn = Debug::isTracing("AllProductTextCfgs");
	try
	{
		DBQuery<DBProductTextCfg>* allQuery = dbRM->retrieveAll();
		if (!allQuery)
			return;

		DBQuery<DBProductTextCfg>::const_iterator iter = allQuery->begin();
		while (iter != allQuery->end())
		{
			if (traceOn)
				Debug::trace("AllProductTextCfgs", string("findAll - textType ") + iter->getTextType() + "  labelNum " + iter->getLabelNum() + "  printerNum " + iter->getPrinterNum());
			productTextNumsList.push_back(iter->getTextType());
			productTextWhichPrinterList.push_back(iter->getPrinterNum());
			productTextLabelNumList.push_back(iter->getLabelNum());
			++iter;
		}
		delete allQuery;

		if (traceOn)
			Debug::trace("AllProductTextCfgs", string("findAll - NumsList size ") + inttostring(productTextNumsList.size()) + "  WhichPrinterList size " + inttostring(productTextWhichPrinterList.size()) + "  LabelNumList size " + inttostring(productTextLabelNumList.size()));
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "AllProductTextCfgs::findAll() ********** database exception **********");
		DBException::Type t = dbe.getType();
		Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
	}
}

const char *AllProductTextCfgs::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
//			if (productTextNumsList.size())
			{
				serializedData.append(productTextNumsList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(productTextWhichPrinterList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(productTextLabelNumList.serialize(format));
			}
			break;

		default:
			break;
	}
	return serializedData.c_str();
}
