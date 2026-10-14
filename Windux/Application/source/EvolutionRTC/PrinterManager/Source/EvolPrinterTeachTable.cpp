
#include "EvolPrinterTeachTable.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolPrinterTeachTable::dataType = 0;
std::string EvolPrinterTeachTable::dataTypeName("EvolPrinterTeachTable"); 
#endif

EvolPrinterTeachTable *EvolPrinterTeachTable::getInstance()
{
	if (instance == 0)
	{
		instance = new EvolPrinterTeachTable;
	}
	return instance;
}

EvolPrinterTeachTable *EvolPrinterTeachTable::instance = (EvolPrinterTeachTable *) NULL;

EvolPrinterTeachTable::EvolPrinterTeachTable()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// Specific Printer Command Lists
	Debug::trace("EvolPrinterTeachTable", "EvolPrinterTeachTable::CTOR() Creating Evol EPP command list CALL createCommandList()");
	createCommandList();

	Debug::trace("EvolPrinterTeachTable", "EvolPrinterTeachTable::CTOR() Creating Evol EPP teach tables call createTeachTable()");
	createTeachTable();
}

EvolPrinterTeachTable::~EvolPrinterTeachTable()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
