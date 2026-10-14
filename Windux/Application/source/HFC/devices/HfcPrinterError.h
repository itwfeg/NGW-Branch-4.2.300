/**
* @file HfcPrinterError.h
* @author Alex Zavala
* @brief The header file for receiving and sending printer error messages from and to the UI respectively
* 
*/
#ifndef HfcPrinterError_H
#define HfcPrinterError_H

#include "evolution.h"
#include "HfcObject.h"
#include "PrinterError.h"

typedef enum HfcPrinterErrorFieldOffsets {
    fieldOffsetPEPrinterErrorType = 1,
    fieldOffsetPEPrinterErrorData,
    endHfcPrinterErrorFieldOffsets//MUST be last!
} HfcPrinterErrorFieldOffsets_t;

class HfcPrinterError : public PrinterError, public HfcObject {
    public:
        HfcPrinterError(const char *data);
        HfcPrinterError(const char *data, int len, int formatType);
        ~HfcPrinterError();

        virtual const char *serialize(int format = InternalFormat) const;
    private:
#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};
#endif  //  HfcPrinterError_H