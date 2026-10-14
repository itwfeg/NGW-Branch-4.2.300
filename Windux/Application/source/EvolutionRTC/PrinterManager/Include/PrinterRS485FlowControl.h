/**
 * @file PrinterRS485FlowControl.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for signaling the success or failure of label image transfer.
 * Created on: Dec 7, 2018
 */
#ifndef PRINTERRS485FLOWCONTROL_H
#define PRINTERRS485FLOWCONTROL_H
#include "EvolMessage.h"
#include "Printer.h"
#include "CEvolMsgReceiver.h"

class PrinterRS485FlowControl : public EvolMessage
{
	public:
	PrinterRS485FlowControl(Hobart::CEvolMsgReceiver *pHobThread = NULL, bool resend = false );
	PrinterRS485FlowControl(const char *buffer);
	~PrinterRS485FlowControl();

	const char *serialize() const;

	bool isResend() const { return resend_; }

	private:
		bool resend_;
};

#endif
