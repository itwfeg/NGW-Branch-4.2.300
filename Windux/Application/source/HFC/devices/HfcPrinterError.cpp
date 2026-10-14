/**
* @file HfcPrinterError.cpp
* @author Alex Zavala
* @brief The source file for receiving and sending printer error messages from and to the UI respectively
* 
*/
#include "HfcPrinterError.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"
#include "Double.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterError::dataType = 0;
std::string HfcPrinterError::dataTypeName("HfcPrinterError");
#endif

/**
* @brief Constructor for sending the PrinterError message to and from the UI
* 
* @param data a char buffer of a PrinterError message
*/
HfcPrinterError::HfcPrinterError(const char *data) : PrinterError(data) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}

/**
* @brief Constructs a PrinterError message based on a certain format
* 
* @param data a tokenized buffer containing a PrinterError message
* @param len the length of the tokenized message
* @param formatType the format in which the message came over
*/
HfcPrinterError::HfcPrinterError(const char *data, int len, int formatType) : PrinterError() {
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
                    case fieldOffsetPEPrinterErrorType:
                        token.unQuoteForCSV();
                        setErrorType(token.c_str());
                        break;
                    case fieldOffsetPEPrinterErrorData:
                        token.unQuoteForCSV();
                        setErrorData(token.c_str());
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
* @brief the HFC PrinterError destructor
* 
*/
HfcPrinterError::~HfcPrinterError() {
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}

/**
* @brief Serializes the HfcPrinterError message into a char buffer
* 
* @param format the format for serializing the data(Internal, Browser, SM)
* @return a buffer of a PrinterError message in the specified format
*/
const char *HfcPrinterError::serialize(int format) const {
    const char *msgdata = NULL;
    int msglen = 0;
    HobString stringForQuoting;

    switch(format) {
        case InternalFormat: {
            msgdata = PrinterError::serialize();
            msglen = PrinterError::getSerializedLength();

            HfcObject::serializedData = PrinterError::serializedData;
            msglen = HfcObject::getSerializedLength();
            break;
        }
        case BrowserFormat: {
            short loopCnt = 1;

            HfcObject::serializedData = Integer::toString((int)getDeviceType());
            HfcObject::serializedData.append(BrowserFormatSeparator);
            HfcObject::serializedData.append(Integer::toString((int)getEventType()));

            while(loopCnt < endHfcPrinterErrorFieldOffsets) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch(loopCnt) {
                    case fieldOffsetPEPrinterErrorType:
                        stringForQuoting = getErrorType();
                        stringForQuoting.quoteForCSV();
                        HfcObject::serializedData.append(stringForQuoting);
                        break;
                    case fieldOffsetPEPrinterErrorData:
                        stringForQuoting = getErrorData();
                        stringForQuoting.quoteForCSV();
                        HfcObject::serializedData.append(stringForQuoting);
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
            HfcObject::serializedData = "Serialized CAN Printer Error : sm format";
            break;
        default:
            break;
    }
    return HfcObject::serializedData.data();
}