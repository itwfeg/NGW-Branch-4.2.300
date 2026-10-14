/**
 * @file PrinterRequest.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Alex Zavala
 * @brief Class for creating a PrinterRequest message
 * 
 * @addtogroup CANMessaging
 * @{
 */
#ifndef PrinterRequest_H
#define PrinterRequest_H

#include "PrinterGeneric.h"

/**
 * @class PrinterRequest
 * @brief Class for creating a PrinterRequest message object
 * Class for converting a buffer to a PrinterRequest message we can read or a PrinterRequest message to a buffer to send to the UI
 */
class PrinterRequest : public PrinterGeneric {
    public:
        /**
         * @brief Printer request constructor
         * 
         * @param pHobThread the message receiver class
         * @param printerType the printer number
         * @param requestCode_ the printer request code
         */
        PrinterRequest(Hobart::CEvolMsgReceiver *pHobThread = NULL,
                                 PrinterType printerType = FirstPrinter,
                                 PrRequestCode requestCode_ = REQUEST_SENSORS);
        
        /**
         * @brief Printer request constructor using a character array
         * 
         * @param buffer the character array
         */
        PrinterRequest(const char *buffer);
        
        /**
         * @brief Printer request destructor
         * 
         */
        ~PrinterRequest();
        
        /**
         * Turns the printer request object into buffer form
         * 
         * @return the printer request in a buffer
         */
        const char *serialize() const;
        
        /**
         * @brief Shows the value of the printer request variables (request code)
         * 
         */
        void show();
        
        /**
         * @brief Getter for the printer request code
         * 
         * @return the printer request code
         */
        PrRequestCode getRequestCode() const;   
        
        /**
         * @brief Setter for the printer request code
         * 
         * @param requestCode_ the request code
         */
        void setRequestCode(PrRequestCode code_);

    private:
        PrRequestCode requestCode;

#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};
/** @} */
#endif //PrinterRequest_H