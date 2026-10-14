/**
* @file HfcPrinterRequest.cpp
* @author Alex Zavala
* @brief The source file for receiving and sending printer request messages from and to the UI respectively
* 
*/
#include "HfcPrinterRequest.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"
#include "Double.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterRequest::dataType = 0;
std::string HfcPrinterRequest::dataTypeName("HfcPrinterRequest"); 
#endif

/**
* @brief Constructor for sending the PrinterRequest message to and from the UI
* 
* @param data a buffer containing a PrinterRequest message
*/
HfcPrinterRequest::HfcPrinterRequest(const char *data) : PrinterRequest(data) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}

/**
* @brief Constructs a PrinterRequest message based on a certain format
* 
* @param data a tokenized buffer containing a PrinterRequest message
* @param len the length of the tokenized message
* @param formatType the format in which the message came over
*/
HfcPrinterRequest::HfcPrinterRequest(const char *data, int len, int formatType) : PrinterRequest() {
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
                    case fieldOffsetPrinterRequestCode:
                        setPrinterRequestCode((PrRequestCode) Integer::valueOf(token.c_str()));
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
HfcPrinterRequest::~HfcPrinterRequest() {
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
const char *HfcPrinterRequest::serialize(int format) const {
    const char *msgdata = NULL;
    int msglen = 0;
    HobString stringForQuoting;

    switch(format) {
        case InternalFormat: {
            msgdata = PrinterRequest::serialize();
            msglen = PrinterRequest::getSerializedLength();

            HfcObject::serializedData = PrinterRequest::serializedData;
            msglen = HfcObject::getSerializedLength();
            break;
        }
        case BrowserFormat: {
            short loopCnt = 1;

            HfcObject::serializedData = Integer::toString((int)getDeviceType());
            HfcObject::serializedData.append(BrowserFormatSeparator);
            HfcObject::serializedData.append(Integer::toString((int)getEventType()));

            while(loopCnt < endHfcPrinterRequestFieldOffsets) {
                HfcObject::serializedData.append(BrowserFormatSeparator);

                switch(loopCnt) {
                    case fieldOffsetPrinterRequestCode:
                        HfcObject::serializedData.append(Integer::toString(getRequestCode()));
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
            HfcObject::serializedData = "Serialized Printer Request : sm format";
            break;
        }
        default: {
            break;
        }
    }
    return HfcObject::serializedData.data();
}