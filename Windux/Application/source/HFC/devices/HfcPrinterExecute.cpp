/**
* @file HfcPrinterExecute.cpp
* @author Alex Zavala
* @brief The source file for receiving and sending printer execute messages from and to the UI respectively
* 
*/
#include "HfcPrinterExecute.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"
#include "Double.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterExecute::dataType = 0;
std::string HfcPrinterExecute::dataTypeName("HfcPrinterExecute"); 
#endif

/**
* @brief Constructor for sending the PrinterExecute message to and from the UI
* 
* @param data a char buffer of a PrinterExecute message
*/
HfcPrinterExecute::HfcPrinterExecute(const char *data) : PrinterExecute(data) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}

/**
* @brief Constructs a PrinterExecute message based on a certain format
* 
* @param data a tokenized buffer containing a PrinterExecute message
* @param len the length of the tokenized message
* @param formatType the format in which the message came over
*/
HfcPrinterExecute::HfcPrinterExecute(const char *data, int len, int formatType) : PrinterExecute() {
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
                    case fieldOffsetPrinterExecuteCode:
                        setPrinterExecuteCode((PrExecuteCode) Integer::valueOf(token.c_str()));
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
* @brief The destructor
* 
*/
HfcPrinterExecute::~HfcPrinterExecute() {
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}

/**
* @brief Serializes a message into a buffer based on the format passed in
* 
* @param format the format( InternalFormat, BrowserFormat, etc.)
* @return a buffer containing the serialized message in the format passed in
*/
const char *HfcPrinterExecute::serialize(int format) const {
    const char *msgdata = NULL;
    int msglen = 0;
    HobString stringForQuoting;

    switch(format) {
        case InternalFormat: {
            msgdata = PrinterExecute::serialize();
            msglen = PrinterExecute::getSerializedLength();

            HfcObject::serializedData = PrinterExecute::serializedData;
            msglen = HfcObject::getSerializedLength();
            break;
        }
        case BrowserFormat: {
            short loopCnt = 1;

            HfcObject::serializedData = Integer::toString((int)getDeviceType());
            HfcObject::serializedData.append(BrowserFormatSeparator);
            HfcObject::serializedData.append(Integer::toString((int)getEventType()));

            while(loopCnt < endHfcPrinterExecuteFieldOffsets) {
                HfcObject::serializedData.append(BrowserFormatSeparator);

                switch(loopCnt) {
                    case fieldOffsetPrinterExecuteCode:
                        HfcObject::serializedData.append(Integer::toString(getExecuteCode()));
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
        case SMFormat: {
            HfcObject::serializedData = "Serialized Printer Execute : sm format";
            break;
        }
        default: {
            break;
        }
    }
    return HfcObject::serializedData.data();
}