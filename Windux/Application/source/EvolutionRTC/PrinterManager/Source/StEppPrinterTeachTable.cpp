
#include "StEppPrinterTeachTable.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int StEppPrinterTeachTable::dataType = 0;
std::string StEppPrinterTeachTable::dataTypeName("StEppPrinterTeachTable"); 
#endif

StEppPrinterTeachTable * StEppPrinterTeachTable::getInstance()
{
	if (instance == 0)
	{
		instance = new StEppPrinterTeachTable;
	}
	return instance;
}

StEppPrinterTeachTable * StEppPrinterTeachTable::instance = (StEppPrinterTeachTable *) NULL;

StEppPrinterTeachTable::StEppPrinterTeachTable()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// Specific Printer Command Lists
	Debug::trace("StEppPrinterTeachTable", "StEppPrinterTeachTable::CTOR() Creating StEppPrinter command list call createSTEppCommandList()");
	createSTEppCommandList();

	Debug::trace("StEppPrinterTeachTable", "StEppPrinterTeachTable::CTOR() Creating StEppPrinter teach tables call createSTEppTeachTable()");
	createSTEppTeachTable();
}

StEppPrinterTeachTable::~StEppPrinterTeachTable()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
