/**
* @file PrinterConfigurationWrite.cpp
* @author Alex Zavala
* @brief The class for creating a PrinterConfigurationWrite message
*/
#include <iostream>
#include "HobString.h"
#include "Bool.h"
#include "Integer.h"
#include "Debug.h"
#include "PrinterConfigurationWrite.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
int PrinterConfigurationWrite::dataType = 0;
std::string PrinterConfigurationWrite::dataTypeName("PrinterConfigurationWrite"); 
#endif

//------------------------------------------------------------------------CTORS/DTORS----------------------------------------------------------------
/**
* @brief Printer configuration write constructor with initialization lists
* 
* @param pHobThread the message receiver class
* @param printerType the printer number
*/
PrinterConfigurationWrite::PrinterConfigurationWrite(Hobart::CEvolMsgReceiver *pHobThread,
                                                    PrinterType printerType) : PrinterConfiguration(pHobThread, printerType) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
    setEventType(PrConfigurationWrite);
}

/**
* @brief Printer configuration write constructor using a character array
* 
* @param buffer the character array
*/
PrinterConfigurationWrite::PrinterConfigurationWrite(const char *buffer) : PrinterConfiguration(buffer) {
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}

/**
* @brief Printer Configuration Write destructor
* 
*/
PrinterConfigurationWrite::~PrinterConfigurationWrite() {
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif

}