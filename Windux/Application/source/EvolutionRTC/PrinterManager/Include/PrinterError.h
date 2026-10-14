/**
 * @file PrinterError.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Alex Zavala
 * @brief Class for creating a PrinterError message
 * 
 * @addtogroup CANMessaging
 * @{
 */
#ifndef PrinterError_H
#define PrinterError_H

#include "PrinterGeneric.h"

/**
 * @class PrinterError
 * @brief Class for creating a Printer Error message object
 * Class for converting a buffer to a Printer error message we can read or a error message to a buffer to send to the UI
 */
class PrinterError : public PrinterGeneric {
    public:
        /**
         * @brief Printer error constructor
         * 
         * @param pHobThread the message receiver class
         * @param printerType the printer number
         * @param errorType_ the type of printer error
         * @param errorData_ the data being passed in for the printer error
         */
        PrinterError(Hobart::CEvolMsgReceiver *pHobThread = NULL,
                                 PrinterType printerType = FirstPrinter,
                                 unsigned char errorType_ = 0,
                                 unsigned char errorData_[7] = 0);
        
        /**
         * @brief Printer error constructor using a character array
         * 
         * @param buffer the character array
         */
        PrinterError(const char *buffer);
        
        /**
         * @brief Printer error destructor
         * 
         */
        ~PrinterError();
        
        /**
         * Serializes the printer error message into a buffer
         * 
         * @return the printer error message in a buffer
         */
        const char *serialize() const;
        
        /**
         * @brief Shows the printer error
         * 
         */
        void show();
        
        /**
         * @brief Getter for the printer error type
         * 
         * @return the error type
         */
        unsigned char getErrorType() const;   
        
        /**
         * @brief Setter for the printer error type
         * 
         * @param errorType_ the error type
         */
        void setErrorType(unsigned char errorType_);

        /**
         * @brief Getter for the printer error data
         * 
         * @return the error data
         */
        const unsigned char* getErrorData() const;
        
        /**
         * @brief Setter for the printer error data
         * 
         * @param errorData_ the error data
         */
        void setErrorData(unsigned char errorData_[7]);
        
    private:
        unsigned char errorType;
        unsigned char errorData[7];

#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};
/** @} */
#endif //PrinterError_H
