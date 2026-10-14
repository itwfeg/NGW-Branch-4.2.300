/**
 * @file PrinterError.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Alex Zavala
 * @brief The source for creating a PrinterError message
 * 
 */
#include <iostream>
#include "HobString.h"
#include "Integer.h"
#include "Debug.h"
#include "PrinterError.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
int PrinterError::dataType = 0;
std::string PrinterError::dataTypeName("PrinterError"); 
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterError::PrinterError(Hobart::CEvolMsgReceiver *pHobThread,
                               PrinterType printerType,
                               unsigned char errorType_,
                               unsigned char errorData_[7]) : PrinterGeneric(PrError, pHobThread, printerType), errorType(errorType_) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
    //Initialize our private variables
    for(unsigned int i=0;i<sizeof(errorData);i++) {
        errorData[i] = errorData_[i];
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterError::PrinterError(const char *buffer) : PrinterGeneric(buffer) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    PrinterGeneric::serialize();

    int j = getSerializedLength();

    // Now, for the derived class items
    char *pChar = (char *) &errorType;
    for(unsigned int i = 0; i < sizeof(errorType); i++) {
        *pChar++ = buffer[j++];
    }
    
    pChar = (char *) &errorData;
    for(unsigned int i = 0; i < sizeof(errorData); i++) {
        *pChar++ = buffer[j++];
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterError::~PrinterError() {
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *PrinterError::serialize() const {
    PrinterGeneric::serialize();

    // Now, for the derived class items
    char *pChar = (char *) &errorType;
    for(unsigned int i = 0; i < sizeof(errorType); i++) {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *) &errorData;
    for(unsigned int i = 0; i < sizeof(errorData); i++) {
        serializedData.append(1, pChar[i]);
    }
    return serializedData.data();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterError::show() {
    PrinterGeneric::show();

    Debug::trace("PrinterError", "PrinterError.show(): errorType:" + string(1,errorType));
    for(unsigned int i = 0; i < sizeof(errorData); i++) {
        Debug::trace("PrinterError", "PrinterError.show(): errorData["+HobString(Integer::toString(i))+"]:" + string(1,errorData[i]));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char PrinterError::getErrorType() const {
    return errorType;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterError::setErrorType(unsigned char errorType_) {
    errorType = errorType_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const unsigned char* PrinterError::getErrorData() const {
    return errorData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterError::setErrorData(unsigned char errorData_[7]) {
    for(unsigned int i=0;i<sizeof(errorData);i++) {
        errorData[i] = errorData_[i];
    }
}