/**
 * @file BoardSiteTestCompleteUI.h
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message report the completion of the functional test.
 * Created on: March 5, 2020
 */

#ifndef BOARSITETESTCOMPLETEUI_H
#define BOARSITETESTCOMPLETEUI_H

#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class BoardSiteTestCompleteUI : public EvolMessage
{
	public:
        BoardSiteTestCompleteUI( Hobart::CEvolMsgReceiver *pHobThread = NULL );
        BoardSiteTestCompleteUI( const char *buffer );
        ~BoardSiteTestCompleteUI();

        const char *serialize() const;

        void setArkSerialFlashTest( void )  { arkSerialFlashPass_ = true; }
        void clrArkSerialFlashTest( void )  { arkSerialFlashPass_ = false; }
        bool getArkSerialFlashResult( void ) const { return arkSerialFlashPass_; }

        void setArkSDSlot0Test( void ) { arkSDSlot0Pass_ = true; }
        void clrArkSDSlot0Test( void )  { arkSDSlot0Pass_ = false; }
        bool getArkSDSlot0Result( void ) const { return arkSDSlot0Pass_; }

        void setArkSDSlot1Test( void ) { arkSDSlot1Pass_ = true; }
        void clrArkSDSlot1Test( void ) { arkSDSlot1Pass_ = false; }
        bool getArkSDSlot1Result( void ) const { return arkSDSlot1Pass_; }

        void setArkRTCTest( void )  { arkRTCPass_ = true; }
        void clrArkRTCTest( void ) { arkRTCPass_ = false; }
        bool getArkRTCResult( void ) const { return arkRTCPass_; }

        void setArkUSBTest( void )  { arkUSBPass_ = true; }
        void clrArkUSBTest( void ) { arkUSBPass_ = false; }
        bool getArkUSBResult( void ) const { return arkUSBPass_; }

        void setArkWifiTest( void ) { arkWifiPass_ = true; }
        void clrArkWifiTest( void ) { arkWifiPass_ = false; }
        bool getArkWifiResult( void ) const { return arkWifiPass_; }

        void setArkBlueToothTest( void ) { arkBlueToothPass_ = true; }
        void clrArkBlueToothTest( void ) { arkBlueToothPass_ = false; }
        bool getArkBlueToothResult( void ) const { return arkBlueToothPass_; }

        void setArkEthernetTest( void ) { arkEthernetPass_ = true; }
        void clrArkEthernetTest( void ) { arkEthernetPass_ = false; }
        bool getArkEthernetResult( void ) const { return arkEthernetPass_; }

        void setArkMemoryTest( void ) { arkMemoryPass_ = true; }
        void clrArkMemoryTest( void ) { arkMemoryPass_ = false; }
        bool getArkMemoryResult( void ) const { return arkMemoryPass_; }

        void setArkCan0Test( void ) { arkCan0Pass_ = true; }
        void clrArkCan0Test( void ) { arkCan0Pass_ = false; }
        bool getArkCan0Result( void ) const { return arkCan0Pass_; }

        void setKpcCanTest( void ) { kpcCanPass_ = true; }
        void clrKpcCanTest( void ) { kpcCanPass_ = false; }
        bool getKpcCanResult( void ) const { return kpcCanPass_; }

        void setKpcModelTest( void ) { kpcModelPass_ = true; }
        void clrKpcModelTest( void ) { kpcModelPass_ = false; }
        bool getKpcModelResult( void ) const { return kpcModelPass_; }

        void setKpcSerialFlashTest( void )  { kpcSerialFlashPass_ = true; }
        void clrKpcSerialFlashTest( void )  { kpcSerialFlashPass_ = false; }
        bool getKpcSerialFlashResult( void ) const { return kpcSerialFlashPass_; }

        void setKpcValueMaxTest( void )  { kpcValueMaxPass_ = true; }
        void clrKpcValueMaxTest( void )  { kpcValueMaxPass_ = false; }
        bool getKpcValueMaxResult( void ) const { return kpcValueMaxPass_; }

        void setKpcCutterTest( void )  { kpcCutterPass_ = true; }
        void clrKpcCutterTest( void )  { kpcCutterPass_ = false; }
        bool getKpcCutterResult( void ) const { return kpcCutterPass_; }

        void setKpcDotWearTest( void )  { kpcDotWearPass_ = true; }
        void clrKpcDotWearTest( void )  { kpcDotWearPass_ = false; }
        bool getKpcDotWearResult( void ) const { return kpcDotWearPass_; }


    private:
        bool arkSerialFlashPass_;
        bool arkSDSlot0Pass_;
        bool arkSDSlot1Pass_;
        bool arkRTCPass_;
        bool arkUSBPass_;
        bool arkWifiPass_;
        bool arkBlueToothPass_;
        bool arkEthernetPass_;
        bool arkMemoryPass_;
        bool arkCan0Pass_;
        bool kpcCanPass_;
        bool kpcModelPass_;
        bool kpcSerialFlashPass_;
        bool kpcValueMaxPass_;
        bool kpcCutterPass_;
        bool kpcDotWearPass_;
};
#endif