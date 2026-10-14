/**
* @file HfcCANPrinterSensors.h
* @author Alex Zavala
* @brief The header file for receiving and sending CAN printer sensor messages from and to the UI respectively
* 
*/
#ifndef HfcCANPrinterSensors_H
#define HfcCANPrinterSensors_H

#include "evolution.h"
#include "HfcObject.h"
#include "CANPrinterSensors.h"

typedef enum HfcCANPrinterSensorsFieldOffsets {
    fieldOffsetCANPrinterHeadupReading = 1,
    fieldOffsetCANPrinterHeadTemperatureReading,
    fieldOffsetCANPrinterLabelReading,
    fieldOffsetCANPrinterMediaReading,
    fieldOffsetCANPrinterHeadVoltageReading,
    endHfcCANPrinterSensorsFieldOffsets//MUST be last!
} HfcCANPrinterSensorsFieldOffsets_t;

class HfcCANPrinterSensors : public CANPrinterSensors, public HfcObject {
    public:
        HfcCANPrinterSensors(const char *data);
        HfcCANPrinterSensors(const char *data, int len, int formatType);
        ~HfcCANPrinterSensors();

        virtual const char *serialize(int format = InternalFormat) const;
    private:
#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};
#endif  //  HfcCANPrinterSensors_H