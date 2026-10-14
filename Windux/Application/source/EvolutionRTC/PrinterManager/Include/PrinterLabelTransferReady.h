/**
 * @file PrinterLabelTransferReady.h
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for signaling printer ready for label image transfer.
 * Created on: Jan 16, 2019
 */
#ifndef PRINTERLABELTRANSFERREADY_H_
#define PRINTERLABELTRANSFERREADY_H_
#include "EvolMessage.h"
#include "Printer.h"
#include "CEvolMsgReceiver.h"

class PrinterLabelTransferReady : public EvolMessage
{
	public:
	PrinterLabelTransferReady( Hobart::CEvolMsgReceiver *pHobThread = NULL, bool ready = false, unsigned int id = 99 );
	PrinterLabelTransferReady( const char *buffer );
	~PrinterLabelTransferReady();

	const char *serialize() const;
	unsigned int getStationId() { return stationId_; }


	private:
		bool ready_;
		unsigned int stationId_;
};
#endif
