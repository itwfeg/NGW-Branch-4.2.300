/**
 * @file BoardSitePercentComplete.h
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for displaying percentage complete on the UI.
 * Created on: April 49, 2020
 */
#ifndef BOARDSITEPERCENTCOMPLETE_H
#define BOARDSITEPERCENTCOMPLETE_H

#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"


class BoardSitePercentComplete : public EvolMessage
{
	public:

        typedef enum {
            ArkNoTest,
            ArkFlashTest,
            ArkSDSlot0Test,
            ArkSDSlot1Test,
            ArkUSBTest,
            ArkWifiTest,
            ArkEthernetTest,
            ArkCan0Test,
            ArkMemTest,
            ArkRTCTest,
            ArkBToothTest,
            KpcCan0Test,
            KpcModelTest,
            KpcSerialFlashTest,
            KpcValueMaxTest,
            KpcCutterTest,
            KpcDotWearTest,
        }BSTestComplete;

        BoardSitePercentComplete( Hobart::CEvolMsgReceiver *pHobThread = NULL, BSTestComplete test = ArkNoTest, int cmplt = 0 );
        BoardSitePercentComplete( const char *buffer );
        ~BoardSitePercentComplete();

        unsigned int getPercentComplete() const { return percent_; } 
        void setPercentComplete( int percent ) { percent_ = percent; }
        BSTestComplete getTest() const { return test_; } 
        void setTest( BSTestComplete test ) { test_ = test; }        

        const char *serialize() const;

    private:
        unsigned int percent_;
        BSTestComplete test_;

};
#endif