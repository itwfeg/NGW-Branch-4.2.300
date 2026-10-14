#include "PrinterGapCalComplete.h"
#include "PrinterEvent.h"
PrinterGapCalComplete::PrinterGapCalComplete( Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType, bool pass, float calPoint ) : PrinterGeneric( PrGapCalComplete, pHobThread, printerType )
{
	pass_       =  pass;
    calPoint_   = calPoint;
}

PrinterGapCalComplete::PrinterGapCalComplete( const char *buffer ) : PrinterGeneric(buffer)
{
	PrinterGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &pass_;
	for (unsigned int i = 0; i < sizeof(pass_); i++)
	{
		*pChar++ = buffer[j++];
	}
    pChar = (char *) &calPoint_;
	for (unsigned int i = 0; i < sizeof(calPoint_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterGapCalComplete::~PrinterGapCalComplete()
{
}

const char *PrinterGapCalComplete::serialize()const
{
	PrinterGeneric::serialize();

	char *pChar = (char *) &pass_;
	for (unsigned int i = 0; i < sizeof(pass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
    pChar = (char *) &calPoint_;
	for (unsigned int i = 0; i < sizeof(calPoint_); i++)
	{
		serializedData.append(1, pChar[i]);
	}    
	return serializedData.data();
}

