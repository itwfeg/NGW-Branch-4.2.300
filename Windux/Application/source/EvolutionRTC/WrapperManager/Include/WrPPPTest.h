/**
 * @file WrPPPTest.h
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate position test message
 * Created on: May 13, 2021
 */
#ifndef WRPPPTEST_H
#define WRPPPTEST_H
#include "WrapperGeneric.h"

class PPPTest : public WrapperGeneric
{
	public:
    PPPTest( Hobart::CEvolMsgReceiver *pHobThread = NULL, bool start = false );
    PPPTest( const char *buffer );
    ~PPPTest();
    
    bool getStartTest() const { return startTest_; }
    void setStartTest( bool start ) { startTest_ = start; }
    
    
    const char *serialize() const;
    
    void show();

	private:
    bool startTest_;
};
#endif