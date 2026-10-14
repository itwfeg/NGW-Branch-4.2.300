/**
 * @file BoardSiteTest.h
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for functional testing of ARK board and KPC.
 * Created on: Feb 4, 2020
 */
#ifndef BOARDSITETEST_H
#define BOARDSITETEST_H
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"
#include "BoardSite.h"

class BoardSiteTest : public EvolMessage
{
	public:
	BoardSiteTest(Hobart::CEvolMsgReceiver *pHobThread = NULL, BoardSiteTests code = _ARK_NO_TEST );
    
    BoardSiteTest(Hobart::CEvolMsgReceiver *pHobThread = NULL, BoardSiteTests code = _ARK_NO_TEST,
    unsigned char testParam1 = 0, unsigned char testParam2 = 0, unsigned char testParam3 = 0,
    unsigned char testParam4 = 0, unsigned char testParam5 = 0, unsigned char testParam6 = 0, 
    unsigned char testParam7 = 0 ); 
	
    BoardSiteTest(const char *buffer);
	~BoardSiteTest();

    BoardSiteTests getTestCode() { return testCode_; }
    void setTestCode( unsigned char code ) { testCode_ = (BoardSiteTests)code; }
    
    unsigned char getTestParam1() { return testParam1_; }
    void setTestParam1( unsigned char param ) { testParam1_ = param; }
    unsigned char getTestParam2() { return testParam2_; }
    void setTestParam2( unsigned char param ) { testParam2_ = param; }
    unsigned char getTestParam3() { return testParam3_; }
    void setTestParam3( unsigned char param ) { testParam3_ = param; }
    unsigned char getTestParam4() { return testParam4_; }
    void setTestParam4( unsigned char param ) { testParam4_ = param; }
    unsigned char getTestParam5() { return testParam5_; }
    void setTestParam5( unsigned char param ) { testParam5_ = param; }
    unsigned char getTestParam6() { return testParam6_; }
    void setTestParam6( unsigned char param ) { testParam6_ = param; }
    unsigned char getTestParam7() { return testParam7_; }
    void setTestParam7( unsigned char param ) { testParam7_ = param; }

	const char *serialize() const;
    
    private:
    BoardSiteTests testCode_;
    unsigned char testParam1_;
    unsigned char testParam2_;
    unsigned char testParam3_;
    unsigned char testParam4_;
    unsigned char testParam5_;
    unsigned char testParam6_;
    unsigned char testParam7_;
};
#endif