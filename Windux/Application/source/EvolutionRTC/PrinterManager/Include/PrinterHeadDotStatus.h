/**
 * @file PrinterHeadStatus.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Value Max print head dot wear
 * Created on: Jan 31, 2018
 *
 **/

#ifndef PRINTERHEADDOTSTATUS_H
#define PRINTERHEADDOTSTATUS_H
#include "PrinterGeneric.h"
#include "Printer.h"


class PrinterHeadDotStatus : public PrinterGeneric
{
	public:
		PrinterHeadDotStatus( Hobart::CEvolMsgReceiver *pHobThread = NULL, PrinterType printerType = FirstPrinter, uint16_t source = 0, uint16_t destination = 0, uint8_t status = 0);
		PrinterHeadDotStatus( const char *buffer );
		~PrinterHeadDotStatus();

		const char *serialize() const;

        void setSource(uint16_t source){this->source = source;}
        const uint16_t getSource() const {return source;}

        void setDestination(uint16_t destination){this->destination = destination;}
		const uint16_t getDestination() const {return destination;}

		typedef enum 
		{
			_AllDotsGood,
			_DotsMixed,
			_AllDotsBad,
			_TestFailed
		}TestStatus;

        void setStatus(uint8_t status){this->status = status;}
		const uint8_t getStatus() const {return status;}
        
        void show();

	protected:
		uint16_t source;
        uint16_t destination;
        uint8_t status;
};


#endif //PRINTERHEADDOTSTATUS_H