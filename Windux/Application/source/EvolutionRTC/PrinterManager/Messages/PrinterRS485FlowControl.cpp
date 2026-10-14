/**
 * @file PrinterRS485FlowControl.cpp
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for signaling the success or failure of label image transfer.
 * Created on: Dec 7, 2018
 */
#include "PrinterRS485FlowControl.h"
#include "PrinterEvent.h"
#include "Debug.h"
#include "Integer.h"

PrinterRS485FlowControl::PrinterRS485FlowControl(Hobart::CEvolMsgReceiver *pHobThread, bool resend ) : EvolMessage(RS485Mgr, PrRS485FlowControl, pHobThread)
{
	resend_ = resend;
}

PrinterRS485FlowControl::PrinterRS485FlowControl(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &resend_;
	for (unsigned int i = 0; i < sizeof(resend_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterRS485FlowControl::~PrinterRS485FlowControl()
{}

const char *PrinterRS485FlowControl::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &resend_;
	for (unsigned int i = 0; i < sizeof(resend_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}





