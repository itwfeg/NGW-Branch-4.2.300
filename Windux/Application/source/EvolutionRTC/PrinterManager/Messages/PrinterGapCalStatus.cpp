#include "PrinterGapCalStatus.h"
PrinterGapCalStatus::PrinterGapCalStatus( Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType, 
                                          GapCalState state ) : PrinterGeneric( PrGapCalStatus, pHobThread, printerType )
{
   state_ = state;
   deflectionVoltage_ = 0.0;
   driveCurrent_ = 0.0;
   backingVoltage_ = 0.0; 
   labelBackingVoltage_ = 0.0;
}

PrinterGapCalStatus::PrinterGapCalStatus( const char *buffer ) : PrinterGeneric(buffer)
{
	PrinterGeneric::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &state_;
	for (unsigned int i = 0; i < sizeof(state_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &deflectionVoltage_;
	for (unsigned int i = 0; i < sizeof(deflectionVoltage_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &driveCurrent_;
	for (unsigned int i = 0; i < sizeof(driveCurrent_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &backingVoltage_;
	for (unsigned int i = 0; i < sizeof(backingVoltage_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &labelBackingVoltage_;
	for (unsigned int i = 0; i < sizeof(labelBackingVoltage_); i++)
	{
		*pChar++ = buffer[j++];
    }
}

PrinterGapCalStatus::~PrinterGapCalStatus()
{

}

const char *PrinterGapCalStatus::serialize() const
{
    PrinterGeneric::serialize();    
	char *pChar = (char *) &state_;
	for (unsigned int i = 0; i < sizeof(state_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &deflectionVoltage_;
	for (unsigned int i = 0; i < sizeof(deflectionVoltage_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &driveCurrent_;
	for (unsigned int i = 0; i < sizeof(driveCurrent_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &backingVoltage_;
	for (unsigned int i = 0; i < sizeof(backingVoltage_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &labelBackingVoltage_;
	for (unsigned int i = 0; i < sizeof(labelBackingVoltage_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}