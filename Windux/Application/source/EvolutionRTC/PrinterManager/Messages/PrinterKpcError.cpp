/**
 * @file PrinterKpcError.cpp
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Aaron Swift
 * @brief Kpc engingeering debug Printer errors
 * Created on: Jan 8, 2019
 *
 **/
#include "PrinterKpcError.h"
#include "PrinterEvent.h"

PrinterKpcError::PrinterKpcError( Hobart::CEvolMsgReceiver *pHobThread, PrKpcWeigherErrors err, PrinterType type ) : PrinterGeneric( PrKPCError, pHobThread, type )
{
	error =  err;
}

PrinterKpcError::PrinterKpcError( const char *buffer ) : PrinterGeneric(buffer)
{
	PrinterGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &error;
	for (unsigned int i = 0; i < sizeof(error); i++)
	{
		*pChar++ = buffer[j++];
	}

}

PrinterKpcError::~PrinterKpcError()
{
}

const char *PrinterKpcError::serialize()const
{
	PrinterGeneric::serialize();

	char *pChar = (char *) &error;
	for (unsigned int i = 0; i < sizeof(error); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}

std::string PrinterKpcError::getErrorDescription()
{
	std::string description;

	switch(error)
	{
		case _PR_LABEL_IMAGE_QUEUE_FAILURE: {
			description = "Failed to queue label image transfer from RS485!";
			break;
		}
		case _PR_LABEL_IMAGE_BUFFER_FAILURE: {
			description = "Label image buffer is null!";
			break;
		}
		case _PR_SWAP_BUFFER_NULL: {
			description = "Byte swap buffer is null!";
			break;
		}
		case _PR_BARCODE_DATA_OUTOFRANGE: {
			description = "Barcode data is out of range!";
			break;
		}
		case _PR_RECTANGLE_DIM_INVALID: {
			description = "Rectangle dimensions out of range!";
			break;
		}
		case _PR_RECTANGLE_OUTOFBOUNDS: {
			description = "Rectangle dimensions out of blitting bounds!";
			break;
		}
		case _PR_CMD_QUEUE_FULL: {
			description = "Printer command is full!";
			break;
		}
		case _PR_LINE_TIMER_INIT_FAILURE: {
			description = "Failed to initialize print engine line timer!";
			break;
		}
		case _PR_STROBE_TIMER_INIT_FAILURE: {
			description = "Failed to initialize print strobe timer!";
			break;
		}
		case _PR_STROBE_TIMER_PWM_FAILURE: {
			description = "Failed to set print engine strobe PWM duty cycle!";
			break;
		}
		case _PR_HEAD_DATA_LOAD_FAILURE: {
			description = "DMA transfer of label data to the print head has failed!";
			break;
		}
		case _PR_ENG_TIMER_INIT_FAILURE: {
			description = "Failed to initialize print engine timer!";
			break;
		}
		case _PR_TEST_COND_UNKNOWN_FAILURE: {
			description = "Unknown compare condition!";
			break;
		}
		case _PR_CMD_QUEUE_NULL: {
			description = "Printer command queue is null!";
			break;
		}
		case _PR_HEAD_TYPE_UNKNOWN: {
			description = "Print head type is unknown!";
			break;
		}
		case _PR_OUT_OF_HEAD_DATA: {
			description = "Printer ran out of print head data while printing!";
			break;
		}
		case _PR_CFG_READ_FAILURE: {
			description = "Printer failed to read configuration from serial flash!";
			break;
		}
		case _PR_WAKE_TIMER_INIT_FAILURE: {
			description = "Printer wake up timer initialization failure!";
			break;
		}
		case _PR_MSG_QUEUE_NULL: {
			description = "Printer message queue is null!";
			break;
		}
		case _PR_MSG_QUEUE_FAILURE: {
			description = "Failed to get message from message queue!";
			break;
		}
		case _PR_DFLT_CFG_SAVE_FAILURE: {
			description = "Printer default configuration failed to save to serial flash!";
			break;
		}
		case _PR_CFG_SAVE_FAILURE: {
			description = "Printer configuration failed to save to serial flash!";
			break;
		}
		case _PR_CHKSUM_READ_FAILURE: {
			description = "Printer failed to read checksum page from serial flash!";
			break;
		}
		case _PR_RS485_OPEN_FAILURE: {
			description = "Printer failed to open RS485 interface for label image transfer!";
			break;
		}
		case _PR_ONESHOT_TIMER_DELETE_FAILURE: {
			description = "Printer failed to delete oneshot timer!";
			break;
		}
		case _PR_HEAD_DMA_XFER_FAILURE: {
			description = "DMA transfer to the print head has failed!";
			break;
		}
		case _PR_ENG_NULL: {
			description = "Printer print engine is null!";
			break;
		}
	}
	return description;
}





