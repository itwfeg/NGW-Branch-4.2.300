
/**
 * @file PrinterLabelImageSize.h
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Aaron Swift
 * @brief Class for creating a printer image size message object
 * Created on: May 10, 2019
 * @addtogroup CANMessaging
 * @{
 */

#ifndef PRINTERLABELIMAGESIZE_H_
#define PRINTERLABELIMAGESIZE_H_
#include "PrinterGeneric.h"

class PrinterLabelImageSize : public PrinterGeneric {
public:

	PrinterLabelImageSize(Hobart::CEvolMsgReceiver *pHobThread = NULL, PrinterType printerType = FirstPrinter, unsigned long size = 0 );

	PrinterLabelImageSize(const char *buffer);

    ~PrinterLabelImageSize();

    unsigned long getImageSize() { return size_; }

    const char *serialize() const;
private:
    unsigned long size_;
};



#endif
