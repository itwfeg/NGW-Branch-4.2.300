/**
 * @file PrinterHeadWear.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Value Max print head dot wear
 * Created on: Jan 31, 2018
 *
 **/

#ifndef PRINTERHEADDOTACK_H
#define PRINTERHEADDOTACK_H
#include "PrinterGeneric.h"
#include "Printer.h"


class PrinterHeadDotAck : public PrinterGeneric
{
	public:
		PrinterHeadDotAck( Hobart::CEvolMsgReceiver *pHobThread = NULL, PrinterType printerType = FirstPrinter, uint16_t source = 0, uint16_t destination = 0, uint16_t dotsCount = 0);
		PrinterHeadDotAck( const char *buffer );
		~PrinterHeadDotAck();

		const char *serialize() const;

        void setSource(uint16_t source){this->source = source;}
        const uint16_t getSource(){return source;}

        void setDestination(uint16_t destination){this->destination = destination;}
		const uint16_t getDestination(){return destination;}

        void setDotCount(uint16_t dotsCount){this->dotsCount = dotsCount;}
		const uint16_t getDotCount(){return dotsCount;}
        
        void show();

	protected:
		uint16_t source;
        uint16_t destination;
        uint16_t dotsCount;
};


#endif //PRINTERHEADDOTACK_H