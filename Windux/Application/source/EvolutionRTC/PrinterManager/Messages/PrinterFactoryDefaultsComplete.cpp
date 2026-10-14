/**
 * @file PrinterFactoryDefaultsComplete.cpp
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for signaling the completion of saving printer configuration defaults.
 * Created on: March 2, 2018
 */
#include "PrinterFactoryDefaultsComplete.h"
#include "PrinterEvent.h"
#include "Debug.h"
#include "Integer.h"

PrinterFactoryDefaultsComplete::PrinterFactoryDefaultsComplete(Hobart::CEvolMsgReceiver *pHobThread, bool saved ) : EvolMessage(Printer, PrResetFactoryDefaultsComplete, pHobThread)
{
	saved_ = saved;
}

PrinterFactoryDefaultsComplete::PrinterFactoryDefaultsComplete(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &saved_;
	for (unsigned int i = 0; i < sizeof(saved_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterFactoryDefaultsComplete::~PrinterFactoryDefaultsComplete()
{}

const char *PrinterFactoryDefaultsComplete::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &saved_;
	for (unsigned int i = 0; i < sizeof(saved_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}


