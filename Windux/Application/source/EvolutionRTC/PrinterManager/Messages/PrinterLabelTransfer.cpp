/**
 * @file PrinterLabelTransfer.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Alex Zavala
 * @brief The source for creating a PrinterLabelTransfer message
 * 
 */
#include <iostream>
#include "HobString.h"
#include "Bool.h"
#include "Integer.h"
#include "Debug.h"
#include "PrinterLabelTransfer.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
int PrinterLabelTransfer::dataType = 0;
std::string PrinterLabelTransfer::dataTypeName("PrinterLabelTransfer"); 
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterLabelTransfer::PrinterLabelTransfer(Hobart::CEvolMsgReceiver *pHobThread,
                               PrinterType printerType,
                               UINT32 size_,
                               bool applySecurityLabel_) : PrinterGeneric(PrLabelTransfer, pHobThread, printerType), size(size_), applySecurityLabel(applySecurityLabel_) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterLabelTransfer::PrinterLabelTransfer(const char *buffer) : PrinterGeneric(buffer) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    PrinterGeneric::serialize();

    int j = getSerializedLength();

    // Now, for the derived class items
    char *pChar = (char *) &size;
    for(unsigned int i = 0; i < sizeof(size); i++) {
        *pChar++ = buffer[j++];
    }
    
    pChar = (char *) &applySecurityLabel;
    for(unsigned int i = 0; i < sizeof(applySecurityLabel); i++) {
        *pChar++ = buffer[j++];
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterLabelTransfer::~PrinterLabelTransfer() {
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *PrinterLabelTransfer::serialize() const {
    PrinterGeneric::serialize();

    // Now, for the derived class items
    char *pChar = (char *) &size;
    for(unsigned int i = 0; i < sizeof(size); i++) {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *) &applySecurityLabel;
    for(unsigned int i = 0; i < sizeof(applySecurityLabel); i++) {
        serializedData.append(1, pChar[i]);
    }
    return serializedData.data();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterLabelTransfer::show() {
    PrinterGeneric::show();

    Debug::trace("PrinterLabelTransfer", "PrinterLabelTransfer.show(): size:" + HobString(Integer::toString( size )));
    Debug::trace("PrinterLabelTransfer", "PrinterLabelTransfer.show(): applySecurityLabel:" + HobString(Bool::toString( applySecurityLabel )));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 PrinterLabelTransfer::getSize() const {
    return size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterLabelTransfer::setSize(UINT32 size_) {
    size = size_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PrinterLabelTransfer::getApplySecurityLabel() const {
    return applySecurityLabel;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterLabelTransfer::setApplySecurityLabel(bool applySecurityLabel_) {
    applySecurityLabel = applySecurityLabel_;
}
