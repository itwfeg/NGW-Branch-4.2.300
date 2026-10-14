/**
* @file HfcCANPrinterSensors.cpp
* @author Alex Zavala
* @brief The source file for receiving and sending CAN printer sensor messages from and to the UI respectively
* 
*/
#include "HfcCANPrinterSensors.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"
#include "Double.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcCANPrinterSensors::dataType = 0;
std::string HfcCANPrinterSensors::dataTypeName("HfcCANPrinterSensors"); 
#endif

/**
* @brief The base constructor for sending the CANPrinterSensors message to and from the UI
* 
* @param data a char buffer of a CANPrinterSensors message
*/
HfcCANPrinterSensors::HfcCANPrinterSensors(const char *data) : CANPrinterSensors(data) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}

/**
* @brief Constructs a CANPrinterSensors message based on a certain format
* 
* @param data a tokenized buffer containing a CANPrinterSensors message
* @param len the length of the tokenized message
* @param formatType the format in which the message came over
*/
HfcCANPrinterSensors::HfcCANPrinterSensors(const char *data, int len, int formatType) : CANPrinterSensors() {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);
    util_acc::addInstance(dataType);
#endif
    switch(formatType) {
        case BrowserFormat: {
            HobString temp(data, len);
            HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
            HobString token;
            short loopCnt = 1;
            while(tok.next(&token) != -1) {
                setReceived(loopCnt, (token.length() == 0) ? false : true);
                switch(loopCnt) {
                    case fieldOffsetCANPrinterHeadupReading:
                        setHeadupReading(Integer::valueOf(token.c_str()));
                        break;
                    case fieldOffsetCANPrinterHeadTemperature:
                        setHeadTemperatureReading(Integer::valueOf(token.c_str()));
                        break;
                    case fieldOffsetCANPrinterLabelReading:
                        setLabelReading(Integer::valueOf(token.c_str()));
                        break;
                    case fieldOffsetCANPrinterMediaReading:
                        setMediaReading(Integer::valueOf(token.c_str()));
                        break;
                    case fieldOffsetCANPrinterHeadVoltageReading:
                        setHeadVoltageReading(Integer::valueOf(token.c_str()));
                        break;
                    default:
                        break;
                }
                loopCnt++;
            }
            break;
        }
        case InternalFormat:
        case SMFormat:
        default:
            break;
    }
}

/**
* @brief the HFC CANPrinterSensors destructor
* 
*/
HfcCANPrinterSensors::~HfcCANPrinterSensors() {
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}

/**
* @brief Serializes the HfcCANPrinterSensors message into a char buffer
* 
* @param format the format for serializing the data(Internal, Browser, SM)
* @return a buffer of a CANPrinterSensors message in the specified format
*/
const char *HfcCANPrinterSensors::serialize(int format) const {
    const char *msgdata = NULL;
    int msglen = 0;
    HobString stringForQuoting;

    switch(format) {
        case InternalFormat: {
            msgdata = CANPrinterSensors::serialize();
            msglen = CANPrinterSensors::getSerializedLength();

            HfcObject::serializedData = CANPrinterSensors::serializedData;
            msglen = HfcObject::getSerializedLength();
            break;
        }
        case BrowserFormat: {
            short loopCnt = 1;

            HfcObject::serializedData = Integer::toString((int)getDeviceType());
            HfcObject::serializedData.append(BrowserFormatSeparator);
            HfcObject::serializedData.append(Integer::toString((int)getEventType()));

            while(loopCnt < endHfcCANPrinterSensorsFieldOffsets) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch(loopCnt) {
                    case fieldOffsetCANPrinterHeadupReading:
                        HfcObject::serializedData.append(Bool::toString(isHeadDisengaged()));
                        break;
                    case fieldOffsetCANPrinterHeadTemperature:
                        HfcObject::serializedData.append(Integer::toString(getHeadTemperatureReading()));
                        break;
                    case fieldOffsetCANPrinterLabelReading:
                        HfcObject::serializedData.append(Integer::toString(getLabelReading()));
                        break;
                    case fieldOffsetCANPrinterMediaReading:
                        HfcObject::serializedData.append(Integer::toString(getMediaReading()));
                        break;
                    case fieldOffsetCANPrinterHeadVoltageReading:
                        HfcObject::serializedData.append(Integer::toString(getHeadVoltageReading()));
                        break;
                    default:
                        break;
                }
                loopCnt++;
            }
            msgdata = HfcObject::serializedData.data();
            msglen = HfcObject::getSerializedLength();
            break;
        }
        case SMFormat:
            HfcObject::serializedData = "Serialized CAN Printer Sensors : sm format";
            break;
        default:
            break;
    }
    return HfcObject::serializedData.data();
}