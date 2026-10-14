/**
 * @file PrinterTestCassette.h
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for reporting results of print roller validation test.
 * Created on: Aug 28, 2020
 */
#ifndef PRINTERTESTCASSETTE_H
#define PRINTERTESTCASSETTE_H
#include "PrinterGeneric.h"
#include "Printer.h"


class PrinterTestCassette : public PrinterGeneric
{
	public:
		PrinterTestCassette( Hobart::CEvolMsgReceiver *pHobThread = NULL,
					        UINT16 measured	= 0,
					        UINT16 actual = 0 );
		PrinterTestCassette( const char *buffer );
		~PrinterTestCassette();
		 
		const char *serialize() const;

		void show();	

		UINT16 getMeasuredSize() const { return measuredSize_; } 
		void setMeasuredSize( UINT16 measured ) { measuredSize_ = measured; }

		UINT16 getActualSize() const { return actualSize_; } 
		void setActualSize( UINT16 actual ) { actualSize_ = actual; }

		bool getResult() const { return result_; }
		void setResult( bool result ) { result_ = result; }

		#define MAX_DELTA	60
	private:
	    UINT16	measuredSize_;
	    UINT16	actualSize_;
		bool 	result_;
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif