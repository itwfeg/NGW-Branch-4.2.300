/**
 * @file PrinterExecute.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Alex Zavala
 * @brief Class for creating a PrinterExecute message
 * 
 * @addtogroup CANMessaging
 * @{
 */
#ifndef PrinterExecute_H
#define PrinterExecute_H

#include "PrinterGeneric.h"

/**
 * @class PrinterExecute
 * @brief Class for creating a Printer Execute message object
 * Class for converting a buffer to a Printer execute message we can read or an execute message to a buffer to send to the UI
 */
class PrinterExecute : public PrinterGeneric {
    public:
        /**
         * @brief Printer execute constructor
         * 
         * @param pHobThread the message receiver class
         * @param printerType the printer number
         * @param executeCode_ the printer execute code
         */
        PrinterExecute(Hobart::CEvolMsgReceiver *pHobThread = NULL,
                                 PrinterType printerType = FirstPrinter,
                                 PrExecuteCode executeCode_ = EXECUTE_ENABLE_PRINTER);
        
        /**
         * @brief Printer execute constructor using a character array
         * 
         * @param buffer the character array
         */
        PrinterExecute(const char *buffer);
        
        /**
         * @brief Printer execute destructor
         * 
         */
        ~PrinterExecute();
        
        /**
         * Turns the printer execute object into buffer form
         * 
         * @return the printer execute in a buffer
         */
        const char *serialize() const;
        
        /**
         * @brief Shows the value of the printer execute variables (exec code)
         * 
         */
        void show();
        
        /**
         * @brief Getter for the printer execute code
         * 
         * @return the printer exec code
         */
        PrExecuteCode getExecuteCode() const;   
        
        /**
         * @brief Setter for the printer execute code
         * 
         * @param executeCode_ the exec code
         */
        void setExecuteCode(PrExecuteCode code_);

    private:
        PrExecuteCode executeCode;

#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};
/** @} */
#endif //PrinterExecute_H
