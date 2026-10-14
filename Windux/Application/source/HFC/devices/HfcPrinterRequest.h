/**
* @file HfcPrinterRequest.h
* @author Alex Zavala
* @brief The header file for receiving and sending printer request messages from and to the UI respectively
* 
*/
#ifndef HfcPrinterRequest_H
#define HfcPrinterRequest_H

#include "evolution.h"
#include "HfcObject.h"
#include "PrinterRequest.h"

typedef enum HfcPrinterRequestFieldOffsets {
    fieldOffsetPrinterRequestCode = 1,
    endHfcPrinterRequestFieldOffsets        //  MUST be last!
} HfcPrinterRequestFieldOffsets_t;

class HfcPrinterRequest : public PrinterRequest, public HfcObject {
    public:
        HfcPrinterRequest(const char *data);
        HfcPrinterRequest(const char *data, int len, int formatType);
        ~HfcPrinterRequest();

        virtual const char *serialize(int format = InternalFormat) const;
    private:
#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};
#endif  //  HfcPrinterRequest_H