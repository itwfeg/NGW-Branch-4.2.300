/*
 * PrinterRequestVersion.h
 *
 *  Created on: Aug 5, 2015
 *      Author: aaron
 */

#ifndef PRINTERREQUESTVERSION_H_
#define PRINTERREQUESTVERSION_H_
#include "PrinterGeneric.h"

class PrinterRequestVersion : public PrinterGeneric
{

	public:
	PrinterRequestVersion(Hobart::CEvolMsgReceiver *pHobThread = NULL, PrinterType printerType = FirstPrinter);
	PrinterRequestVersion(const char *buffer);
	~PrinterRequestVersion();

	private:


};


#endif /* PRINTERREQUESTVERSION_H_ */
