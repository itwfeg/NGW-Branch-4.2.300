/**
 * @file PrinterKpcError.h
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Aaron Swift
 * @brief Kpc engingeering debug Printer errors
 * Created on: Jan 8, 2019
 *
 **/
#ifndef PRINTERKPCERROR_H
#define PRINTERKPCERROR_H
#include <string>
#include "PrinterGeneric.h"
#include "CEvolMsgReceiver.h"

class PrinterKpcError : public PrinterGeneric
{
	public:

	//Pulled for KPC Printer firmware
	typedef enum
	{
	    _PR_LABEL_IMAGE_QUEUE_FAILURE,
	    _PR_LABEL_IMAGE_BUFFER_FAILURE,
	    _PR_SWAP_BUFFER_NULL,
	    _PR_BARCODE_DATA_OUTOFRANGE,
	    _PR_RECTANGLE_DIM_INVALID,
	    _PR_RECTANGLE_OUTOFBOUNDS,
	    _PR_CMD_QUEUE_FULL,
	    _PR_LINE_TIMER_INIT_FAILURE,
	    _PR_STROBE_TIMER_INIT_FAILURE,
	    _PR_STROBE_TIMER_PWM_FAILURE,
	    _PR_HEAD_DATA_LOAD_FAILURE,
	    _PR_ENG_TIMER_INIT_FAILURE,
	    _PR_TEST_COND_UNKNOWN_FAILURE,
	    _PR_CMD_QUEUE_NULL,
	    _PR_HEAD_TYPE_UNKNOWN,
	    _PR_OUT_OF_HEAD_DATA,
	    _PR_CFG_READ_FAILURE,
	    _PR_WAKE_TIMER_INIT_FAILURE,
	    _PR_MSG_QUEUE_NULL,
	    _PR_MSG_QUEUE_FAILURE,
	    _PR_DFLT_CFG_SAVE_FAILURE,
	    _PR_CFG_SAVE_FAILURE,
	    _PR_CHKSUM_READ_FAILURE,
	    _PR_RS485_OPEN_FAILURE,
	    _PR_ONESHOT_TIMER_DELETE_FAILURE,
	    _PR_HEAD_DMA_XFER_FAILURE,
	    _PR_ENG_NULL,
	    _PR_UNKNOWN
	}PrKpcWeigherErrors;

	PrinterKpcError( Hobart::CEvolMsgReceiver *pHobThread = NULL, PrKpcWeigherErrors err = _PR_UNKNOWN, PrinterType type = FirstPrinter );

	PrinterKpcError( const char *buffer );

	~PrinterKpcError();

	const char *serialize()const;

	PrKpcWeigherErrors getError() { return error; }
	std::string getErrorDescription();

	private:
	PrKpcWeigherErrors error;

};
#endif
