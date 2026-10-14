/**
* @file HfcPrinterExecute.h
* @author Alex Zavala
* @brief The header file for receiving and sending printer execute messages from and to the UI respectively
* 
*/
#ifndef HfcPrinterExecute_H
#define HfcPrinterExecute_H

#include "evolution.h"
#include "HfcObject.h"
#include "PrinterExecute.h"

typedef enum HfcPrinterExecuteFieldOffsets {
    fieldOffsetPrinterExecuteCode = 1,
    endHfcPrinterExecuteFieldOffsets        //  MUST be last!
} HfcPrinterExecuteFieldOffsets_t;

class HfcPrinterExecute : public PrinterExecute, public HfcObject {
    public:
        HfcPrinterExecute(const char *data);
        HfcPrinterExecute(const char *data, int len, int formatType);
        ~HfcPrinterExecute();

        virtual const char *serialize(int format = InternalFormat) const;
    private:
#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};
#endif  //  HfcPrinterExecute_H