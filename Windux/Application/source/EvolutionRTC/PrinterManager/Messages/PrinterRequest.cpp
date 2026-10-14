/**
 * @file PrinterRequest.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Alex Zavala
 * @brief The source for creating a PrinterRequest message
 * 
 */
#include <iostream>
#include "HobString.h"
#include "Integer.h"
#include "Debug.h"
#include "PrinterRequest.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
int PrinterRequest::dataType = 0;
std::string PrinterRequest::dataTypeName("PrinterRequest"); 
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterRequest::PrinterRequest(Hobart::CEvolMsgReceiver *pHobThread,
                               PrinterType printerType,
                               PrRequestCode requestCode_) : PrinterGeneric(PrRequest, pHobThread, printerType), requestCode(requestCode_) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterRequest::PrinterRequest(const char *buffer) : PrinterGeneric(buffer) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    PrinterGeneric::serialize();

    int j = getSerializedLength();

    // Now, for the derived class items
    char *pChar = (char *) &requestCode;
    for (unsigned int i = 0; i < sizeof(requestCode); i++) {
        *pChar++ = buffer[j++];
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterRequest::~PrinterRequest() {
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *PrinterRequest::serialize() const {
    PrinterGeneric::serialize();

    // Now, for the derived class items
    char *pChar = (char *) &requestCode;
    for (unsigned int i = 0; i < sizeof(requestCode); i++) {
        serializedData.append(1, pChar[i]);
    }

    return serializedData.data();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterRequest::show() {
    PrinterGeneric::show();

    HobString msg = "";
    switch(requestCode) {
        case REQUEST_WAKEUP:
            msg += "RequestWakeup";
            break;
        case REQUEST_CONFIGURATION:
            msg += "RequestConfiguration";
            break;
        case REQUEST_STATUS:
            msg += "RequestStatus";
            break;
        case REQUEST_SENSORS:
            msg += "RequestSensors";
            break;
        case REQUEST_VERSION:
            msg += "RequestVersion";
            break;
        case REQUEST_ERROR:
            msg += "RequestError";
            break;
        default:
            msg += "UNKNOWN REQUEST CODE!";
            break;
    }
    Debug::trace("PrinterRequest", "PrinterRequest.show():" + msg + " code:" + HobString(Integer::toString(requestCode)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrRequestCode PrinterRequest::getRequestCode() const {
    return requestCode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterRequest::setRequestCode(PrRequestCode requestCode_) {
    requestCode = requestCode_;
}