/**
 * @file BoardSiteTestComplete.h
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message report the completion of the functional test.
 * Created on: Feb 4, 2020
 */
#ifndef BOARDSITETESTCOMPLETE_H
#define BOARDSITETESTCOMPLETE_H


#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class BoardSiteTestComplete : public EvolMessage
{
	public:

    typedef enum
    {
        BSTNoError,
        BSTIoInitPort,
        BSTIoInitPin,
        BSTIoInitState,
        BSTCan0,
        BSTValueMaxID,
        BSTValueMaxSerialEEprom,
        BSTValueMaxNotInst,
        BSTRs485Init,
        BSTRs485Pattern,
        BSTRs485NoTransfer,
        BSTSFRead,
        BSTSFWrite,
        BSTSFSector0,
        BSTSFSector1,
        BSTSFSector2,
        BSTSFSector3,
        BSTUSB_1,
        BSTUSB_2,
        BSTUSB_3,
        BSTUSB_4,
        BSTUSB_5,
        BSTUSB_6,
        BSTUSB_7,
        BSTUSB_8,
        BSTWIFICARDNOTPRESENT,
        BSTWIFI,
        BSTETHERNET,
        BSTMissSD0Card,
        BSTSD0Write,
        BSTSD0Erase,
        BSTMissSD1Card,
        BSTSD1Write,
        BSTSD1Erase,
        BSTRTCFailure,
        BSTMemoryFailure_1,
        BSTMemoryFailure_2,
        BSTMemoryFailure_3,
        BSTBlue_1,
        BSTBlue_2,
        BSTBlue_3,
        BSTKPCModel,
        BSTKPCValueMax,
        BSTKPCSerialFlash,
        BSTKPCCan,
        BSTKPCDotWear,
        BSTKPCCutter

    }BSTstErrCode;



	BoardSiteTestComplete( Hobart::CEvolMsgReceiver *pHobThread = NULL, bool testResult = false );
	BoardSiteTestComplete( const char *buffer );
	~BoardSiteTestComplete();

	const char *serialize() const;
    
    bool getTestResult() { return testResult_; }

    void setErrorCode( BSTstErrCode code ) { code_ = code; }
    BSTstErrCode getErrorCode() { return code_; }
    const std::string TranslateErrorCode( BSTstErrCode code ); 
    
    void setExpected( unsigned short expected ) { expected_ = expected; } 
    unsigned short getExpected() { return expected_; }
    
    void setResult( unsigned short result ) { result_ = result; }
    unsigned short getResult() { return result_; }


    private:

    bool testResult_;
    BSTstErrCode code_;
    unsigned short expected_;
    unsigned short result_;

};
#endif