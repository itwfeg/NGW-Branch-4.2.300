/**
 * @file PrinterLabelTransfer.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Alex Zavala
 * @brief Class for creating a PrinterLabelTransfer message
 * 
 * @addtogroup CANMessaging
 * @{
 */
#ifndef PrinterLabelTransfer_H
#define PrinterLabelTransfer_H

#include "PrinterGeneric.h"

/**
 * @class PrinterLabelTransfer
 * @brief Class for creating a PrinterLabelTransfer message object
 * Class for converting a buffer to a PrinterLabelTransfer message we can read or a PrinterLabelTransfer message to a buffer to send to the UI
 */
class PrinterLabelTransfer : public PrinterGeneric {
    public:
        /**
         * @brief Printer label transfer constructor
         * 
         * @param pHobThread the message receiver class
         * @param printerType the printer number
         * @param size_ the size of the label transfer
         * @param applySecurityLabel_ true to apply a security label, false otherwise
         */
        PrinterLabelTransfer(Hobart::CEvolMsgReceiver *pHobThread = NULL,
                                 PrinterType printerType = FirstPrinter,
                                 UINT32 size = 0,
                                 bool applySecurityLabel = false);
        
        /**
         * @brief Printer label transfer constructor using a character array
         * 
         * @param buffer the character array
         */
        PrinterLabelTransfer(const char *buffer);
        
        /**
         * @brief Printer label transfer destructor
         * 
         */
        ~PrinterLabelTransfer();
        
        /**
         * Serializes the label transfer message into a buffer
         * 
         * @return the label transfer message in a buffer
         */
        const char *serialize() const;
        
        /**
         * @brief Shows the label transfer variable values
         * 
         */
        void show();
        
        /**
         * @brief Getter for the label transfer size
         * 
         * @return the label transfer size
         */
        UINT32 getSize() const;   
        
        /**
         * @brief Setter for the label transfer size
         * 
         * @param size_ the label transfer size
         */
        void setSize(UINT32 size_);
        
        /**
         * @brief Getter for the apply security label
         * 
         * @return true if we want to apply a security label, false otherwise
         */
        bool getApplySecurityLabel() const;
        
        /**
         * @brief Setter for the apply security label
         * 
         * @param applySecurityLabel_ whether to apply the security label or not
         */
        void setApplySecurityLabel(bool applySecurityLabel_);
        
    private:
        UINT32 size;
        bool applySecurityLabel;

#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};
/** @} */
#endif //PrinterLabelTransfer_H