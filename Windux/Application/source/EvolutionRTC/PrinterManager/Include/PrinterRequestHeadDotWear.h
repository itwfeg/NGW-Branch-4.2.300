/**
 * @file PrinterHeadWear.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Value Max print head dot wear
 * Created on: Jan 31, 2018
 *
 **/

#ifndef PRINTERREQUESTHEADDOTWEAR_H
#define PRINTERREQUESTHEADDOTWEAR_H
#include "PrinterGeneric.h"
#include "Printer.h"


class PrinterRequestHeadDotWear : public PrinterGeneric
{
	public:
		PrinterRequestHeadDotWear( Hobart::CEvolMsgReceiver *pHobThread = NULL, PrinterType printerType = FirstPrinter, uint16_t source = 0 , uint16_t destination = 0);
		PrinterRequestHeadDotWear( const char *buffer );
		~PrinterRequestHeadDotWear();

		const char *serialize() const;

        void setSource(uint16_t source){this->source = source;}
        const uint16_t getSource(){return source;}

        void setDestination(uint16_t destination){this->destination = destination;}
		const uint16_t getDestination(){return destination;}
        
        void show();

	protected:
		uint16_t source;
        uint16_t destination;
};


#endif //PRINTERREQUESTHEADDOTWEAR_H


