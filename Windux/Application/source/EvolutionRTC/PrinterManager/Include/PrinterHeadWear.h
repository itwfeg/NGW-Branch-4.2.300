/**
 * @file PrinterHeadWear.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Value Max print head dot wear
 * Created on: Jan 31, 2018
 *
 **/

#ifndef PRINTERHEADWEAR_H
#define PRINTERHEADWEAR_H
#include "PrinterGeneric.h"
#include "Printer.h"
#include <vector>


class PrinterHeadWear : public PrinterGeneric
{
	public:
		PrinterHeadWear( Hobart::CEvolMsgReceiver *pHobThread = NULL, PrinterType printerType = FirstPrinter, PrinterDotStatus * dots = NULL, uint8_t * counts = NULL, int headSize = 0);
		PrinterHeadWear( const char *buffer );
		~PrinterHeadWear();

		const char *serialize() const;

		const unsigned char getDotStatus(int x) { return headDotStatus_[x]; }
		const unsigned char getDotCounts(int x) { return headDotCounts_[x]; }

		const std::vector<PrinterDotStatus> getHeadDotStatus() const { return headDotStatus_; }
		void setHeadDotStatus( PrinterDotStatus* dots ) { headDotStatus_.assign(dots, dots + headSize_); }

		const std::vector<uint8_t> getHeadDotCounts() const  { return headDotCounts_; }
		void setHeadDotCounts( uint8_t * counts ) { headDotCounts_.assign(counts, counts + headSize_); }

		const int getHeadSize() const  { return headSize_; }
		void setHeadSize( int size ) { headSize_ = size; }

		void show();

	protected:
		std::vector<PrinterDotStatus>headDotStatus_;
	    std::vector<uint8_t>headDotCounts_;
		int headSize_;
};


#endif //PRINTERHEADWEAR_H


