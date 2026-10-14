/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "WSSession.h"
UINT32 WSSession::counts = 10000;
UINT32 WSSession::rawcounts = 10000;

#include "Printer.h"
#include "PrSession.h"
int PrSession::index = 0;
PrinterStateType PrSession::printerOnOrOff = PrinterOff;
