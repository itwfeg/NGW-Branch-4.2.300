#include <iostream>
using namespace std;

//#include "vxWorks.h"
#include "Printer.h"
#include "PrinterTest.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterTest::dataType = 0;
std::string PrinterTest::dataTypeName("PrinterTest"); 
#endif

PrinterTest::PrinterTest(Hobart::CEvolMsgReceiver *pHobThread,
						 PrinterType printerType,
						 UINT8 data) : PrinterGeneric(PrTest, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->data = data;
}

PrinterTest::PrinterTest(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &data;
	for (unsigned int i = 0; i < sizeof(data); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterTest::~PrinterTest()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterTest::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &data;
	for (unsigned int i = 0; i < sizeof(data); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterTest::show()
{
	cout << "Printer Test: " << endl;

	cout << "data: " << getData() << endl;
}

UINT8 PrinterTest::getData()
{
	return data;
}

void PrinterTest::setData(UINT8 data)
{
	this->data = data;
}
