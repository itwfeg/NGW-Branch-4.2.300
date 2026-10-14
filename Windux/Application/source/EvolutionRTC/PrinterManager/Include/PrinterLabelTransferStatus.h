/**
 * @file PrinterLabelTransferStatus.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Class for creating a PrinterLabelTransferStatus message
 * Created on: Aug 4, 2015 Updated, Formatted 11/6/15 by Alex Zavala
 * @addtogroup CANMessaging
 * @{
 */
#ifndef PRINTERLABELTRANSFERSTATUS_H_
#define PRINTERLABELTRANSFERSTATUS_H_
#include "PrinterGeneric.h"

/**
 * @class PrinterLabelTransferStatus
 * @brief Class for creating a PrinterLabelTransferStatus message object
 * 
 */
class PrinterLabelTransferStatus : public PrinterGeneric {
    public:
        /**
         * @brief Constructor using initialization lists
         * 
         * @param pHobThread the message receiver class
         * @param printerType the printer number
         */
        PrinterLabelTransferStatus(Hobart::CEvolMsgReceiver *pHobThread = NULL, PrinterType printerType = FirstPrinter);
        
        /**
         * @brief Constructor using a character array
         * 
         * @param buffer the char buffer containing a PrinterLabelTransferStatus message
         */
        PrinterLabelTransferStatus(const char *buffer);
        
        /**
         * @brief Destructor
         * 
         */
        ~PrinterLabelTransferStatus();
        
        /**
         * @brief Serializes the transfer status message into a char buffer
         * 
         * @return The transfer status message serialized into a char buffer
         */
        const char *serialize() const;
        
        /**
         * @brief Sets the status bits of the transfer status
         * 
         * @param status the status of the printer label data banks
         */
        void setStatusBits(UINT8 status) { _statusBits = status; }
        
        /**
         * @brief Gets the status bits from the transfer status
         * 
         * @return the status bits of the printer label data banks (Full/Empty)
         */
        UINT8 getStatusBits() { return _statusBits; }
        
        /**
         * @brief Gets the status bits from the transfer status
         * 
         * @param dataBankNum the data bank in question of being Full/Empty
         * @return the status of the printer label data bank (Full/Empty)
         */
        unsigned char getLabelDataBankState(unsigned int dataBankNum);
    private:
        UINT8 _statusBits;
};
/** @} */
#endif /* PRINTERLABELTRANSFERSTATUS_H_ */
