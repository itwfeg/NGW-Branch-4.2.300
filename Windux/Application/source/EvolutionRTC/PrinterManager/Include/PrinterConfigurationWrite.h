/**
* @file PrinterConfigurationWrite.h
* @author Alex Zavala
* @brief The header file for the Printer Configuration Write class
* 
*/
#ifndef PrinterConfigurationWrite_H
#define PrinterConfigurationWrite_H

#include "PrinterConfiguration.h"

class PrinterConfigurationWrite : public PrinterConfiguration {
    public:
//-----------------------------------------------CTORS/DTORS------------------------------------------------------
        PrinterConfigurationWrite(Hobart::CEvolMsgReceiver *pHobThread = NULL,
                             PrinterType printerType = FirstPrinter);
        
        PrinterConfigurationWrite(const char *buffer);
        ~PrinterConfigurationWrite();
    private:
#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};
#endif //PrinterConfigurationWrite_H