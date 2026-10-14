/**
 * @file PrinterExecute.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Alex Zavala
 * @brief The source for creating a PrinterExecute message
 * 
 */
#include <iostream>
#include "HobString.h"
#include "Integer.h"
#include "Debug.h"
#include "PrinterExecute.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
int PrinterExecute::dataType = 0;
std::string PrinterExecute::dataTypeName("PrinterExecute"); 
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterExecute::PrinterExecute(Hobart::CEvolMsgReceiver *pHobThread,
                               PrinterType printerType,
                               PrExecuteCode executeCode_) : PrinterGeneric(PrExecute, pHobThread, printerType), executeCode(executeCode_) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterExecute::PrinterExecute(const char *buffer) : PrinterGeneric(buffer) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    PrinterGeneric::serialize();

    int j = getSerializedLength();

    // Now, for the derived class items
    char *pChar = (char *) &executeCode;
    for (unsigned int i = 0; i < sizeof(executeCode); i++) {
        *pChar++ = buffer[j++];
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterExecute::~PrinterExecute() {
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *PrinterExecute::serialize() const {
    PrinterGeneric::serialize();

    // Now, for the derived class items
    char *pChar = (char *) &executeCode;
    for (unsigned int i = 0; i < sizeof(executeCode); i++) {
        serializedData.append(1, pChar[i]);
    }

    return serializedData.data();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterExecute::show() {
    PrinterGeneric::show();

    HobString msg = "";
    switch(executeCode) {
        case EXECUTE_ENABLE_PRINTER:
            msg += "ExecuteEnablePrinter";
            break;
        case EXECUTE_DISABLE_PRINTER:
            msg += "ExecuteDisablePrinter";
            break;
        case EXECUTE_RESET:
            msg += "ExecuteReset";
            break;
        case EXECUTE_PRINTHEAD_ON:
            msg += "ExecutePrintheadOn";
            break;
        case EXECUTE_PRINTHEAD_OFF:
            msg += "ExecutePrintheadOff";
            break;
        case EXECUTE_ENABLE_TAKEUP:
            msg += "ExecuteEnableTakeup";
            break;
        case EXECUTE_DISABLE_TAKEUP:
            msg += "ExecuteDisableTakeup";
            break;
        case EXECUTE_CALIBRATE_MEDIA_SENSOR:
            msg += "ExecuteCalibrateMediaSensor";
            break;
        case EXECUTE_CALIBRATE_LABEL_TAKEN_SENSOR:
            msg += "ExecuteCalibrateLabelTakenSensor";
            break;
        case EXECUTE_EJECT_LABEL:
            msg += "ExecuteEjectLabel";
            break;
        case EXECUTE_ADVANCE_LABEL:
            msg += "ExecuteAdvanceLabel";
            break;
        case EXECUTE_SIZE_LABEL:
            msg += "ExecuteSizeLabel";
            break;
        case EXECUTE_PRINT_LABEL:
            msg += "ExecutePrintLabel";
            break;
        default:
            msg += "UNKNOWN EXECUTE CODE!";
            break;
    }
    Debug::trace("PrinterExecute", "PrinterExecute.show():" + msg + " code:" + HobString(Integer::toString(executeCode)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrExecuteCode PrinterExecute::getExecuteCode() const {
    return executeCode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterExecute::setExecuteCode(PrExecuteCode executeCode_) {
    executeCode = executeCode_;
}