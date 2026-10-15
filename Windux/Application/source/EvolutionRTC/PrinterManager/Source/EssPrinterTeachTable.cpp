//using namespace std;

#include "EssPrinterTeachTable.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EssPrinterTeachTable::dataType = 0;
std::string EssPrinterTeachTable::dataTypeName("EssPrinterTeachTable"); 
#endif

EssPrinterTeachTable *EssPrinterTeachTable::getInstance()
{
	if (instance == 0)
	{
		instance = new EssPrinterTeachTable;
	}
	return instance;
}

EssPrinterTeachTable *EssPrinterTeachTable::instance = (EssPrinterTeachTable *) NULL;

EssPrinterTeachTable::EssPrinterTeachTable()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// Specific Printer Command Lists
	Debug::trace("EssPrinterTeachTable", "Creating command list");
	createCommandList();
	Debug::trace("EssPrinterTeachTable", "Creating teach tables");
	createTeachTable();
}

EssPrinterTeachTable::~EssPrinterTeachTable()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void EssPrinterTeachTable::createGenericSize()
{
	genericSize.push_back(&disableCounterOp);
	genericSize.push_back(&resetCounterOp);
	genericSize.push_back(&syncBarOp);
	genericSize.push_back(&checkStockOp);
	genericSize.push_back(&abortSizeTestOp);
	genericSize.push_back(&enableCounterOp);
	genericSize.push_back(&peelLabelOp);
	genericSize.push_back(&syncBarOp);
	genericSize.push_back(&disableCounterOp);
	genericSize.push_back(&jammedLabelTestOp0);
	genericSize.push_back(&checkStockOp);
	genericSize.push_back(&idleOp);
}
