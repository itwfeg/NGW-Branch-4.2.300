/**
 * @file BoardSiteTestComplete.cpp
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message report the completion of the functional test.
 * Created on: Feb 4, 2020
 */

#include "BoardSiteTestComplete.h"
#include "BoardSiteEvent.h"
BoardSiteTestComplete::BoardSiteTestComplete(Hobart::CEvolMsgReceiver *pHobThread, bool testResult ) : EvolMessage(BoardSite, BrdSiteBoardTestComplete, pHobThread)
{
    testResult_ = testResult;
	code_ = BSTNoError;
	expected_ = 0;
	result_ = 0;
}


BoardSiteTestComplete::BoardSiteTestComplete(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &testResult_;
	for (unsigned int i = 0; i < sizeof(testResult_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &code_;
	for (unsigned int i = 0; i < sizeof(code_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &expected_;
	for (unsigned int i = 0; i < sizeof(expected_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &result_;
	for (unsigned int i = 0; i < sizeof(result_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

BoardSiteTestComplete::~BoardSiteTestComplete()
{
	
}

const char *BoardSiteTestComplete::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &testResult_;
	for (unsigned int i = 0; i < sizeof(testResult_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &code_;
	for (unsigned int i = 0; i < sizeof(code_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &expected_;
	for (unsigned int i = 0; i < sizeof(expected_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &result_;
	for (unsigned int i = 0; i < sizeof(result_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

const std::string BoardSiteTestComplete::TranslateErrorCode( BSTstErrCode code )
{
	std::string text;
	switch( code ) 
	{
		case BSTNoError: {
			break;
		}
		case BSTCan0: {
			text = "ARK CAN 0 failure";
			break;
		}
		case BSTSFWrite: {
			text = "ARK serial flash failure";
			break;
		}
		case BSTETHERNET: {
			text = "Ethernet failed to obtain IP address";
			break;
		}
		case BSTMissSD0Card: {
			text = "Slot 0 Missing SD card";
			break;
		}
		case BSTSD0Erase: {
			text = "Slot 0 failed to erase SD card";
			break;
		}
		case BSTSD0Write: {
			text = "Slot 0 failed to write SD card";
			break;
		}
		case BSTMissSD1Card: {
			text = "Slot 1 Missing SD card";
			break;
		}
		case BSTSD1Erase: {
			text = "Slot 1 failed to erase SD card";
			break;
		}
		case BSTSD1Write: {
			text = "Slot 1 failed to write SD card";
			break;
		}
		case BSTUSB_1: {
			text = "Root hub missing";
			break;
		}
		case BSTUSB_2: {
			text = "Hub 1 missing";
			break;
		}
		case BSTUSB_3: {
			text = "WIFI card missing";
			break;
		}
		case BSTUSB_4: {
			 text = "TouchScreen missing"; 
			break;
		}
		case BSTUSB_5: {
			text = "Port 5 device missing";
			break;
		}
		case BSTUSB_6: {
			text = "Port 6 device missing";
			break;
		}
		case BSTUSB_7: {
			text = "Port 7 device missing";
			break;
		}
		case BSTUSB_8: {
			text = "Port 8 device missing";
			break;
		}
		case BSTWIFICARDNOTPRESENT: {
			text = "Missing WIFI card or bad PCIe interface";
			break;
		}
		case BSTWIFI: {
			text = "No access point found!";
			break;
		}
		case BSTMemoryFailure_1: {
			text = "Ark memory error allocating or locking memory, or invocation error";
			break;
		}
		case BSTMemoryFailure_2: {
			text = "Ark memory error during stuck address test";
			break;
		}
		case BSTMemoryFailure_3: {
			text = "Ark memory error during one of the other tests";
			break;
		}
		case BSTRTCFailure: {
			text = "ARK failed to set real time clock";
			break;
		}
		case BSTBlue_1: {
			text = "ARK bluetooth failed to find device during scan";
			break;
		}
		case BSTBlue_2: {
			text = "ARK bluetooth command failure";
			break;
		}
		case BSTBlue_3: {
			text = "ARK bluetooth device not up or missing";
			break;
		}
		case BSTKPCSerialFlash: {
			text = "KPC serial flash failure";
			break;			
		}
		case BSTKPCValueMax: {
			text = "KPC value max failure";
			break;
		}
		case BSTKPCModel: {
			text = "KPC ADC failure";
			break;
		}
		case BSTKPCCan: {
			text = "KPC Can interface failure. Is device programmed?";
			break;
		}
		case BSTKPCDotWear: {
			text = "KPC dotwear failure";
			break;
		}
		case BSTKPCCutter: {
			text = "KPC Cutter failure. Be sure jumper is installed";
			break;
		}
		//Phase 1 errors
		case BSTRs485Init:
        case BSTRs485Pattern:
        case BSTRs485NoTransfer:
        case BSTSFRead:        
        case BSTSFSector0:
        case BSTSFSector1:
        case BSTSFSector2:
        case BSTSFSector3:
		case BSTValueMaxID:
		case BSTValueMaxNotInst:
		case BSTValueMaxSerialEEprom:
		case BSTIoInitState:
		case BSTIoInitPin:
		case BSTIoInitPort: {
			text = "ARK unsupported test";
			break;
		}
	}
	return text;
}
