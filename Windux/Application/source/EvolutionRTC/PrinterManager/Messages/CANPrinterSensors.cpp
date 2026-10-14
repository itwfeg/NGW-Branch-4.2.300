/**
 * @file CANPrinterSensors.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Alex Zavala
 * @brief The source for creating a CANPrinterSensors message
 * 
 */
#include <iostream>
#include "HobString.h"
#include "Bool.h"
#include "UInteger.h"
#include "Integer.h"
#include "Debug.h"
#include "CANPrinterSensors.h"

using namespace std;

//Constants for calculating the voltage and printhead temp readings on the printers
#define headVoltageFactor 0.2071;
#define headVoltageFactorEpp 0.02784;

#define headTempMultiplyConstant -0.490798
#define headTempConstant 114.355828

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
int CANPrinterSensors::dataType = 0;
std::string CANPrinterSensors::dataTypeName("CANPrinterSensors"); 
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
CANPrinterSensors::CANPrinterSensors(Hobart::CEvolMsgReceiver *pHobThread,
                               PrinterType printerType,
                               PrinterHeadUpType headupReading_,
                               UINT16 headTemperatureReading_,
                               UINT16 headVoltageReading_,
                               UINT16 labelHighAverage_,
                               UINT16 labelLowAverage_,
                               UINT16 labelThreshold_,
                               UINT16 labelReading_,
                               UINT16 mediaHighAverage_,
                               UINT16 mediaLowAverage_,
                               UINT16 mediaThreshold_,
                               UINT16 mediaReading_) : PrinterGeneric(PrCANSensors, pHobThread, printerType), headupReading(headupReading_),
                                                      headTemperatureReading(headTemperatureReading_), headVoltageReading(headVoltageReading_),
                                                      labelHighAverage(labelHighAverage_), labelLowAverage(labelLowAverage_), labelThreshold(labelThreshold_),
                                                      labelReading(labelReading_), mediaHighAverage(mediaHighAverage_), mediaLowAverage(mediaLowAverage_),
                                                      mediaThreshold(mediaThreshold_), mediaReading(mediaReading_) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CANPrinterSensors::CANPrinterSensors(const char *buffer) : PrinterGeneric(buffer) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    PrinterGeneric::serialize();

    int j = getSerializedLength();

    // Now, for the derived class items
    char *pChar = (char *) &headupReading;
    for(unsigned int i = 0; i < sizeof(headupReading); i++) {
        *pChar++ = buffer[j++];
    }
    pChar = (char *) &headTemperatureReading;
    for(unsigned int i = 0; i < sizeof(headTemperatureReading); i++) {
        *pChar++ = buffer[j++];
    }
    pChar = (char *) &headVoltageReading;
    for(unsigned int i = 0; i < sizeof(headVoltageReading); i++) {
        *pChar++ = buffer[j++];
    }
    pChar = (char *) &labelHighAverage;
    for(unsigned int i = 0; i < sizeof(labelHighAverage); i++) {
        *pChar++ = buffer[j++];
    }
    pChar = (char *) &labelLowAverage;
    for(unsigned int i = 0; i < sizeof(labelLowAverage); i++) {
        *pChar++ = buffer[j++];
    }
    pChar = (char *) &labelThreshold;
    for(unsigned int i = 0; i < sizeof(labelThreshold); i++) {
        *pChar++ = buffer[j++];
    }
    pChar = (char *) &labelReading;
    for(unsigned int i = 0; i < sizeof(labelReading); i++) {
        *pChar++ = buffer[j++];
    }
    pChar = (char *) &mediaHighAverage;
    for(unsigned int i = 0; i < sizeof(mediaHighAverage); i++) {
        *pChar++ = buffer[j++];
    }
    pChar = (char *) &mediaLowAverage;
    for(unsigned int i = 0; i < sizeof(mediaLowAverage); i++) {
        *pChar++ = buffer[j++];
    }
    pChar = (char *) &mediaThreshold;
    for(unsigned int i = 0; i < sizeof(mediaThreshold); i++) {
        *pChar++ = buffer[j++];
    }
    pChar = (char *) &mediaReading;
    for(unsigned int i = 0; i < sizeof(mediaReading); i++) {
        *pChar++ = buffer[j++];
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CANPrinterSensors::~CANPrinterSensors() {
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *CANPrinterSensors::serialize() const {
    PrinterGeneric::serialize();

    // Now, for the derived class items
    char *pChar = (char *) &headupReading;
    for(unsigned int i = 0; i < sizeof(headupReading); i++) {
        serializedData.append(1, pChar[i]);
    }
    pChar = (char *) &headTemperatureReading;
    for(unsigned int i = 0; i < sizeof(headTemperatureReading); i++) {
        serializedData.append(1, pChar[i]);
    }
    pChar = (char *) &headVoltageReading;
    for(unsigned int i = 0; i < sizeof(headVoltageReading); i++) {
        serializedData.append(1, pChar[i]);
    }
    pChar = (char *) &labelHighAverage;
    for(unsigned int i = 0; i < sizeof(labelHighAverage); i++) {
        serializedData.append(1, pChar[i]);
    }
    pChar = (char *) &labelLowAverage;
    for(unsigned int i = 0; i < sizeof(labelLowAverage); i++) {
        serializedData.append(1, pChar[i]);
    }
    pChar = (char *) &labelThreshold;
    for(unsigned int i = 0; i < sizeof(labelThreshold); i++) {
        serializedData.append(1, pChar[i]);
    }
    pChar = (char *) &labelReading;
    for(unsigned int i = 0; i < sizeof(labelReading); i++) {
        serializedData.append(1, pChar[i]);
    }
    pChar = (char *) &mediaHighAverage;
    for(unsigned int i = 0; i < sizeof(mediaHighAverage); i++) {
        serializedData.append(1, pChar[i]);
    }
    pChar = (char *) &mediaLowAverage;
    for(unsigned int i = 0; i < sizeof(mediaLowAverage); i++) {
        serializedData.append(1, pChar[i]);
    }
    pChar = (char *) &mediaThreshold;
    for(unsigned int i = 0; i < sizeof(mediaThreshold); i++) {
        serializedData.append(1, pChar[i]);
    }
    pChar = (char *) &mediaReading;
    for(unsigned int i = 0; i < sizeof(mediaReading); i++) {
        serializedData.append(1, pChar[i]);
    }

    return serializedData.data();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CANPrinterSensors::equals(CANPrinterSensors &prSensors) {

    return ((headupReading == prSensors.getHeadupReading()) &&
        (headTemperatureReading == prSensors.getHeadTemperatureReading()) &&
        (headVoltageReading == prSensors.getHeadVoltageReading()) &&
        (labelReading == prSensors.getLabelReading()) &&
        (mediaReading == prSensors.getMediaReading()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterSensors CANPrinterSensors::toPrinterSensorMessage( Hobart::CEvolMsgReceiver* pMyPrinter, PrinterType prType, CANPrinterSensors prSense ) {
    PrinterSensors prSensors(pMyPrinter, prType, prSense.getHeadupReading(),
                                                 UnknownLabel,//labelWidth
                                                 0,//regulatorTemperature
                                                 prSense.getHeadTemperatureReading(),
                                                 (double)(prSense.getHeadVoltageReading()*0.02784),
                                                 0,//headCurrent
                                                 prSense.getLabelHighAverage(),
                                                 prSense.getLabelLowAverage(),
                                                 prSense.getLabelThreshold(),
                                                 prSense.getLabelReading(),
                                                 prSense.getMediaHighAverage(),
                                                 prSense.getMediaLowAverage(),
                                                 prSense.getMediaThreshold(),
                                                 prSense.getMediaReading());
    return prSensors;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::show() {
    PrinterGeneric::show();
    
    Debug::trace("CANPrinterSensors", "CANPrinterSensors.show(): headupReading:" +HobString(UInteger::toString(headupReading)));
    Debug::trace("CANPrinterSensors", "CANPrinterSensors.show(): headTemperatureReading:" +HobString(UInteger::toString(headTemperatureReading)));
    Debug::trace("CANPrinterSensors", "CANPrinterSensors.show(): headVoltageReading:" +HobString(UInteger::toString(headVoltageReading)));
    Debug::trace("CANPrinterSensors", "CANPrinterSensors.show(): labelHighAverage:" +HobString(UInteger::toString(labelHighAverage)));
    Debug::trace("CANPrinterSensors", "CANPrinterSensors.show(): labelLowAverage:" +HobString(UInteger::toString(labelLowAverage)));
    Debug::trace("CANPrinterSensors", "CANPrinterSensors.show(): labelThreshold:" +HobString(UInteger::toString(labelThreshold)));
    Debug::trace("CANPrinterSensors", "CANPrinterSensors.show(): labelReading:" +HobString(UInteger::toString(labelReading)));
    Debug::trace("CANPrinterSensors", "CANPrinterSensors.show(): mediaHighAverage:" +HobString(UInteger::toString(mediaHighAverage)));
    Debug::trace("CANPrinterSensors", "CANPrinterSensors.show(): mediaLowAverage:" +HobString(UInteger::toString(mediaLowAverage)));
    Debug::trace("CANPrinterSensors", "CANPrinterSensors.show(): mediaThreshold:" +HobString(UInteger::toString(mediaThreshold)));
    Debug::trace("CANPrinterSensors", "CANPrinterSensors.show(): mediaReading:" +HobString(UInteger::toString(mediaReading)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CANPrinterSensors::isHeadDisengaged() const {
    return (headupReading == PrinterHeadDisengaged);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CANPrinterSensors::isHeadEngaged() const {
    return (headupReading == PrinterHeadEngaged);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterHeadUpType CANPrinterSensors::getHeadupReading() const {
    return headupReading;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::setHeadupReading(PrinterHeadUpType headupReading_) {
    if( headupReading_ >= PrinterHeadDisengagedThreshold ) {
        headupReading = PrinterHeadDisengaged;
    } else {
        headupReading = PrinterHeadEngaged;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::setHeadupReadingValue(UINT16 headupReading) {
    if(headupReading >= PrinterHeadDisengagedThreshold) {
        headupReading = PrinterHeadDisengaged;
    } else {
        headupReading = PrinterHeadEngaged;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT16 CANPrinterSensors::getHeadTemperatureReading() const {
    return headTemperatureReading;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::setHeadTemperatureReading(UINT16 headTemperatureReading_) {
    headTemperatureReading = headTemperatureReading_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::setHeadTemperatureCounts(UINT16 temperatureCounts) {
    UINT32 headTemp = (UINT32) (headTempMultiplyConstant*(double)temperatureCounts + headTempConstant);
    setHeadTemperatureReading((UINT16)headTemp);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT16 CANPrinterSensors::getHeadVoltageReading() const {
    return headVoltageReading;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::setHeadVoltageReading(UINT16 headVoltageReading_) {
    headVoltageReading = headVoltageReading_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::setHeadVoltageCounts(UINT16 voltageCounts, PrinterKind prKind) {
    if (voltageCounts == 65535) {
        headVoltageReading = -1.0;
    } else {
        if(prKind == ApcPrinter) {
            headVoltageReading = voltageCounts * headVoltageFactor;
        } else {
            headVoltageReading = voltageCounts * headVoltageFactorEpp;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT16 CANPrinterSensors::getLabelHighAverage() const {
    return labelHighAverage;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::setLabelHighAverage(UINT16 labelHighAverage_) {
    labelHighAverage = labelHighAverage_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT16 CANPrinterSensors::getLabelLowAverage() const {
    return labelLowAverage;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::setLabelLowAverage(UINT16 labelLowAverage_) {
    labelLowAverage = labelLowAverage_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT16 CANPrinterSensors::getLabelThreshold() const {
    return labelThreshold;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::setLabelThreshold(UINT16 labelThreshold_) {
    labelThreshold = labelThreshold_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT16 CANPrinterSensors::getLabelReading() const {
    return labelReading;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::setLabelReading(UINT16 labelReading_) {
    labelReading = labelReading_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT16 CANPrinterSensors::getMediaHighAverage() const {
    return mediaHighAverage;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::setMediaHighAverage(UINT16 mediaHighAverage_) {
    mediaHighAverage = mediaHighAverage_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT16 CANPrinterSensors::getMediaLowAverage() const {
    return mediaLowAverage;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::setMediaLowAverage(UINT16 mediaLowAverage_) {
    mediaLowAverage = mediaLowAverage_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT16 CANPrinterSensors::getMediaThreshold() const {
    return mediaThreshold;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::setMediaThreshold(UINT16 mediaThreshold_) {
    mediaThreshold = mediaThreshold_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT16 CANPrinterSensors::getMediaReading() const {
    return mediaReading;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CANPrinterSensors::setMediaReading(UINT16 mediaReading_) {
    mediaReading = mediaReading_;
}