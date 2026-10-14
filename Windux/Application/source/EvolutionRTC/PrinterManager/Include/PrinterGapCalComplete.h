/**
 * @file PrinterGapCalComplete.h
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Results of the Printer gap (shoot through) sensor calibration.
 * Created on: Nov 11, 2021
 * @addtogroup CANMessaging
 * @{
 */
#ifndef PRINTERGAPCALCOMLPETE_H
#define PRINTERGAPCALCOMLPETE_H
#include "PrinterGeneric.h"
#include "CEvolMsgReceiver.h"

class PrinterGapCalComplete : public PrinterGeneric
{
	public:
    PrinterGapCalComplete(Hobart::CEvolMsgReceiver *pHobThread = NULL, PrinterType printerType = FirstPrinter, bool pass = 0, float calPoint = 0.0 );
    PrinterGapCalComplete( const char *buffer );
    ~PrinterGapCalComplete();
	const char *serialize()const;

	private:
    bool pass_;
    float calPoint_;

};
#endif