/**
 * @file PrinterLabelTransferStatus.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @brief The source for creating a PrinterLabelTransferStatus message
 * Created on: Aug 4, 2015 Updated, Formatted 11/6/15 by Alex Zavala
 */
#include "PrinterLabelTransferStatus.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterLabelTransferStatus::PrinterLabelTransferStatus(Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType)
        : PrinterGeneric(PrLabelTransferStatus, pHobThread, printerType), _statusBits(0) {
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterLabelTransferStatus::PrinterLabelTransferStatus(const char *buffer) : PrinterGeneric(buffer) {
    // index into buffer
    PrinterGeneric::serialize();

    int j = getSerializedLength();

    char *pChar = (char *) &_statusBits;
    for (unsigned int i = 0; i < sizeof(_statusBits); i++) {
        *pChar++ = buffer[j++];
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterLabelTransferStatus::~PrinterLabelTransferStatus() {
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *PrinterLabelTransferStatus::serialize() const {
    PrinterGeneric::serialize();

    char *pChar = (char *) &_statusBits;
    for (unsigned int i = 0; i < sizeof(_statusBits); i++) {
        serializedData.append(1, pChar[i]);
    }
    return serializedData.data();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char PrinterLabelTransferStatus::getLabelDataBankState(unsigned int dataBankNum) {
    return ((_statusBits >> dataBankNum) & 0x01);
}
