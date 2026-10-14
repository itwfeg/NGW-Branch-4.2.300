/**
 * @file BoardSiteTestCompleteUI.cpp
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message report the completion of all board site tests that can be run
 *        automatically.
 * Created on: March 5, 2020
 */
#include "BoardSiteTestCompleteUI.h"
#include "BoardSiteEvent.h"

BoardSiteTestCompleteUI::BoardSiteTestCompleteUI( Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( BoardSite, BrdSiteBoardTestCompleteUI, pHobThread )
{
    arkSerialFlashPass_ = false;
    arkSDSlot0Pass_ = false;
    arkSDSlot1Pass_ = false;
    arkRTCPass_ = false;
    arkUSBPass_ = false;
    arkWifiPass_ = false;
    arkBlueToothPass_ = false;
    arkEthernetPass_ = false;
    arkMemoryPass_ = false;
	arkCan0Pass_ = false;
	kpcCanPass_ = false;
    kpcModelPass_ = false;
    kpcSerialFlashPass_ = false;
	kpcValueMaxPass_ = false;
	kpcCutterPass_ = false;
	kpcDotWearPass_ = false;
}

BoardSiteTestCompleteUI::BoardSiteTestCompleteUI( const char *buffer ) : EvolMessage( buffer )
{
	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &arkSerialFlashPass_;
	for (unsigned int i = 0; i < sizeof(arkSerialFlashPass_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &arkSDSlot0Pass_;
	for (unsigned int i = 0; i < sizeof(arkSDSlot1Pass_); i++)
	{
		*pChar++ = buffer[j++];
	}


    pChar = (char *) &arkSDSlot1Pass_;
	for (unsigned int i = 0; i < sizeof(arkSDSlot1Pass_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &arkRTCPass_;
	for (unsigned int i = 0; i < sizeof(arkRTCPass_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &arkUSBPass_;
	for (unsigned int i = 0; i < sizeof(arkUSBPass_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &arkWifiPass_;
	for (unsigned int i = 0; i < sizeof(arkWifiPass_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &arkBlueToothPass_;
	for (unsigned int i = 0; i < sizeof(arkBlueToothPass_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &arkEthernetPass_;
	for (unsigned int i = 0; i < sizeof(arkEthernetPass_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &arkMemoryPass_;
	for (unsigned int i = 0; i < sizeof(arkMemoryPass_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &arkCan0Pass_;
	for (unsigned int i = 0; i < sizeof(arkCan0Pass_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &kpcCanPass_;
	for (unsigned int i = 0; i < sizeof(kpcCanPass_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &kpcModelPass_;
	for (unsigned int i = 0; i < sizeof(kpcModelPass_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &kpcSerialFlashPass_;
	for (unsigned int i = 0; i < sizeof(kpcSerialFlashPass_); i++)
	{
		*pChar++ = buffer[j++];
	}
	
    pChar = (char *) &kpcValueMaxPass_;
	for (unsigned int i = 0; i < sizeof(kpcValueMaxPass_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &kpcCutterPass_;
	for (unsigned int i = 0; i < sizeof(kpcCutterPass_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &kpcDotWearPass_;
	for (unsigned int i = 0; i < sizeof(kpcDotWearPass_); i++)
	{
		*pChar++ = buffer[j++];
	}


}

BoardSiteTestCompleteUI::~BoardSiteTestCompleteUI()
{
	
}

const char *BoardSiteTestCompleteUI::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &arkSerialFlashPass_;
	for (unsigned int i = 0; i < sizeof(arkSerialFlashPass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &arkSDSlot0Pass_;
	for (unsigned int i = 0; i < sizeof(arkSDSlot0Pass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &arkSDSlot1Pass_;
	for (unsigned int i = 0; i < sizeof(arkSDSlot1Pass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &arkRTCPass_;
	for (unsigned int i = 0; i < sizeof(arkRTCPass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &arkUSBPass_;
	for (unsigned int i = 0; i < sizeof(arkUSBPass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &arkWifiPass_;
	for (unsigned int i = 0; i < sizeof(arkWifiPass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &arkBlueToothPass_;
	for (unsigned int i = 0; i < sizeof(arkBlueToothPass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &arkEthernetPass_;
	for (unsigned int i = 0; i < sizeof(arkEthernetPass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &arkMemoryPass_;
	for (unsigned int i = 0; i < sizeof(arkMemoryPass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &arkCan0Pass_;
	for (unsigned int i = 0; i < sizeof(arkCan0Pass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &kpcCanPass_;
	for (unsigned int i = 0; i < sizeof(kpcCanPass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}


    pChar = (char *) &kpcModelPass_;
	for (unsigned int i = 0; i < sizeof(kpcModelPass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &kpcSerialFlashPass_;
	for (unsigned int i = 0; i < sizeof(kpcSerialFlashPass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
    
    pChar = (char *) &kpcValueMaxPass_;
	for (unsigned int i = 0; i < sizeof(kpcValueMaxPass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &kpcCutterPass_;
	for (unsigned int i = 0; i < sizeof(kpcCutterPass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &kpcDotWearPass_;
	for (unsigned int i = 0; i < sizeof(kpcDotWearPass_); i++)
	{
		serializedData.append(1, pChar[i]);
	}


	return serializedData.data();
}