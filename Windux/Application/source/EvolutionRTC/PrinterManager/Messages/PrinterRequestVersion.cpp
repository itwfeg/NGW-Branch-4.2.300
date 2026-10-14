/*
 * PrinterRequestVersion.cpp
 *
 *  Created on: Aug 5, 2015
 *      Author: aaron
 */
#include "PrinterRequestVersion.h"

PrinterRequestVersion::PrinterRequestVersion( Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType ) : PrinterGeneric(PrReqVersion, pHobThread, printerType) {}
PrinterRequestVersion::PrinterRequestVersion( const char *buffer ) : PrinterGeneric( buffer ) {}

PrinterRequestVersion::~PrinterRequestVersion() {}




