/**
* @file HfcCANPrinterWakeup.h
* @author Alex Zavala
* @brief The header file for receiving and sending CAN printer wakeup messages from and to the UI respectively
* 
*/
#ifndef HfcCANPrinterWakeup_H
#define HfcCANPrinterWakeup_H

#include "evolution.h"
#include "HfcObject.h"
#include "CANPrinterWakeup.h"

typedef enum HfcCANPrinterWakeupFieldOffsets {
    fieldOffsetCANPrinterBufferSize = 1,
    fieldOffsetCANPrinterPrintheadSize,
    fieldOffsetCANPrinterConfigPassed,
    endHfcCANPrinterWakeupFieldOffsets//MUST be last!
} HfcCANPrinterWakeupFieldOffsets_t;

class HfcCANPrinterWakeup : public CANPrinterWakeup, public HfcObject {
    public:
        HfcCANPrinterWakeup(const char *data);
        HfcCANPrinterWakeup(const char *data, int len, int formatType);
        ~HfcCANPrinterWakeup();

        virtual const char *serialize(int format = InternalFormat) const;
    private:
#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};
#endif  //  HfcCANPrinterWakeup_H