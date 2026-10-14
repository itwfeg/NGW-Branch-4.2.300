/**
 * @file PrinterLabelTransferReady.cpp
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for signaling printer ready for label image transfer.
 * Created on: Jan 16, 2019
 */
#include "PrinterLabelTransferReady.h"
#include "PrinterEvent.h"
#include "Debug.h"
#include "Integer.h"


PrinterLabelTransferReady::PrinterLabelTransferReady( Hobart::CEvolMsgReceiver *pHobThread, bool ready, unsigned int id ) : EvolMessage(RS485Mgr, PrLabelTransferReady, pHobThread)
{
	ready_ = ready;
	stationId_ = id;
}

PrinterLabelTransferReady::PrinterLabelTransferReady( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &ready_;
	for( unsigned int i = 0; i < sizeof(ready_); i++ ) {
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &stationId_;
	for( unsigned int i = 0; i < sizeof(stationId_); i++ ) {
		*pChar++ = buffer[j++];
	}
}

PrinterLabelTransferReady::~PrinterLabelTransferReady()
{}

const char *PrinterLabelTransferReady::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &ready_;
	for( unsigned int i = 0; i < sizeof(ready_); i++ ) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &stationId_;
	for( unsigned int i = 0; i < sizeof(stationId_); i++ ) {
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}






