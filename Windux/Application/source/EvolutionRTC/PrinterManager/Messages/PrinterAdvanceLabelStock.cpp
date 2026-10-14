//#include "vxWorks.h"
#include "PrinterAdvanceLabelStock.h"

#include "Debug.h"
#include "HobString.h"
#include "UInteger.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterAdvanceLabelStock::dataType = 0;
std::string PrinterAdvanceLabelStock::dataTypeName("PrinterAdvanceLabelStock"); 
#endif

PrinterAdvanceLabelStock::PrinterAdvanceLabelStock(Hobart::CEvolMsgReceiver *pHobThread,
												   PrinterType printerType,
												   UINT16 numSteps) : PrinterGeneric(PrAdvLabelStock, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->numSteps = numSteps;
}

PrinterAdvanceLabelStock::PrinterAdvanceLabelStock(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &numSteps;
	for (unsigned int i = 0; i < sizeof(numSteps); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterAdvanceLabelStock::~PrinterAdvanceLabelStock()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterAdvanceLabelStock::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &numSteps;
	for (unsigned int i = 0; i < sizeof(numSteps); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterAdvanceLabelStock::show()
{
	bool isTracing = Debug::isTracing("PrinterAdvanceLabelStock");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterAdvanceLabelStock") );
#else
		Debug::addTraceCategory("PrinterAdvanceLabelStock");
#endif
	}

	Debug::trace("PrinterAdvanceLabelStock", "PrinterAdvanceLabelStock: ");

	PrinterGeneric::show();

	Debug::trace("PrinterAdvanceLabelStock", HobString("numSteps: ") +
											 HobString(UInteger::toString(numSteps)));

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrinterAdvanceLabelStock") );
#else
		Debug::removeTraceCategory("PrinterAdvanceLabelStock");
#endif
	}
}

UINT16 PrinterAdvanceLabelStock::getNumSteps() const
{
	return numSteps;
}

void PrinterAdvanceLabelStock::setNumSteps(UINT16 numSteps)
{
	this->numSteps = numSteps;
}
