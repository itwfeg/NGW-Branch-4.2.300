/**
* @file HfcCANPrinterWakeup.cpp
* @author Alex Zavala
* @brief The source file for receiving and sending CAN printer wakeup messages from and to the UI respectively
* 
*/
#include "HfcCANPrinterWakeup.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"
#include "Double.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcCANPrinterWakeup::dataType = 0;
std::string HfcCANPrinterWakeup::dataTypeName("HfcCANPrinterWakeup"); 
#endif

/**
* @brief The base constructor for sending the CANPrinterWakeup message to and from the UI
* 
* @param data a char buffer of a CANPrinterWakeup message
*/
HfcCANPrinterWakeup::HfcCANPrinterWakeup(const char *data) : CANPrinterWakeup(data) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}

/**
* @brief Constructs a CANPrinterWakeup message based on a certain format
* 
* @param data a tokenized buffer containing a CANPrinterWakeup message
* @param len the length of the tokenized message
* @param formatType the format in which the message came over
*/
HfcCANPrinterWakeup::HfcCANPrinterWakeup(const char *data, int len, int formatType) : CANPrinterWakeup() {
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
                    case fieldOffsetCANPrinterBufferSize:
                        setBufferSize(Integer::valueOf(token.c_str()));
                        break;
                    case fieldOffsetCANPrinterPrintheadSize:
                        setPrintheadSize(Integer::valueOf(token.c_str()));
                        break;
                    case fieldOffsetCANPrinterConfigPassed:
                        setConfigPassed(Bool::valueOf(token.c_str()));
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
* @brief the HFC CANPrinterWakeup destructor
* 
*/
HfcCANPrinterWakeup::~HfcCANPrinterWakeup() {
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}

/**
* @brief Serializes the HfcCANPrinterWakeup message into a char buffer
* 
* @param format the format for serializing the data(Internal, Browser, SM)
* @return a buffer of a CANPrinterWakeup message in the specified format
*/
const char *HfcCANPrinterWakeup::serialize(int format) const {
    const char *msgdata = NULL;
    int msglen = 0;
    HobString stringForQuoting;

    switch(format) {
        case InternalFormat: {
            msgdata = CANPrinterWakeup::serialize();
            msglen = CANPrinterWakeup::getSerializedLength();

            HfcObject::serializedData = CANPrinterWakeup::serializedData;
            msglen = HfcObject::getSerializedLength();
            break;
        }
        case BrowserFormat: {
            short loopCnt = 1;

            HfcObject::serializedData = Integer::toString((int)getDeviceType());
            HfcObject::serializedData.append(BrowserFormatSeparator);
            HfcObject::serializedData.append(Integer::toString((int)getEventType()));

            while(loopCnt < endHfcCANPrinterWakeupFieldOffsets) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch(loopCnt) {
                    case fieldOffsetCANPrinterBufferSize:
                        HfcObject::serializedData.append(Integer::toString(getBufferSize()));
                        break;
                    case fieldOffsetCANPrinterPrintheadSize:
                        HfcObject::serializedData.append(Integer::toString(getPrintheadSize()));
                        break;
                    case fieldOffsetCANPrinterConfigPassed:
                        HfcObject::serializedData.append(Bool::toString(getConfigPassed()));
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
            HfcObject::serializedData = "Serialized CAN Printer Wakeup : sm format";
            break;
        default:
            break;
    }
    return HfcObject::serializedData.data();
}