/**
 * @file WrPPPConfig.cpp
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate position configuration message
 * Created on: Mar 9, 2021
 */
#include "WrPPPConfig.h"
#include "Integer.h"
#include "Debug.h"

PPPConfig::PPPConfig( Hobart::CEvolMsgReceiver *pHobThread, unsigned long priDistance, unsigned long secDistance, unsigned short rate ) : WrapperGeneric( WrPPPConfig, pHobThread )
{
    priCalDistance_ = priDistance;
    secondCalDistance_ = secDistance;
    sampleRate_ 	= rate;
	
	#if 1 /* VL53L1X */
	x_ 				= 0;
	y_ 				= 0;
	centerSpad_ 	= 0;
	sensorAOffset_	= 0;
	sensorBOffset_	= 0;
	#endif
}

PPPConfig::PPPConfig( const char *buffer ) : WrapperGeneric( buffer )
{

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &priCalDistance_;
	for (unsigned int i = 0; i < sizeof(priCalDistance_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secondCalDistance_;
	for (unsigned int i = 0; i < sizeof(secondCalDistance_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sampleRate_;
	for (unsigned int i = 0; i < sizeof(sampleRate_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &primarySlope_;
	for (unsigned int i = 0; i < sizeof(primarySlope_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secondarySlope_;
	for (unsigned int i = 0; i < sizeof(secondarySlope_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &priPlateHomeDist_;
	for (unsigned int i = 0; i < sizeof(priPlateHomeDist_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secPlateHomeDist_;
	for (unsigned int i = 0; i < sizeof(secPlateHomeDist_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &mechanicalHomeSpec_;
	for (unsigned int i = 0; i < sizeof(mechanicalHomeSpec_); i++)
	{
		*pChar++ = buffer[j++];
	}

	#if 1 /* VL53L1X */
	pChar = (char *) &x_;
	for (unsigned int i = 0; i < sizeof(x_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &y_;
	for (unsigned int i = 0; i < sizeof(y_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &centerSpad_;
	for (unsigned int i = 0; i < sizeof(centerSpad_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sensorAOffset_;
	for (unsigned int i = 0; i < sizeof(sensorAOffset_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sensorBOffset_;
	for (unsigned int i = 0; i < sizeof(sensorBOffset_); i++)
	{
		*pChar++ = buffer[j++];
	}
	#endif
}

PPPConfig::~PPPConfig()
{

}

const char *PPPConfig::serialize() const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &priCalDistance_;
	for (unsigned int i = 0; i < sizeof(priCalDistance_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &secondCalDistance_;
	for (unsigned int i = 0; i < sizeof(secondCalDistance_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sampleRate_;
	for (unsigned int i = 0; i < sizeof(sampleRate_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &primarySlope_;
	for (unsigned int i = 0; i < sizeof(primarySlope_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secondarySlope_;
	for (unsigned int i = 0; i < sizeof(secondarySlope_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &priPlateHomeDist_;
	for (unsigned int i = 0; i < sizeof(priPlateHomeDist_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secPlateHomeDist_;
	for (unsigned int i = 0; i < sizeof(secPlateHomeDist_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &mechanicalHomeSpec_;
	for (unsigned int i = 0; i < sizeof(mechanicalHomeSpec_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	#if 1 /* VL53L1X */
	pChar = (char *) &x_;
	for (unsigned int i = 0; i < sizeof(x_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &y_;
	for (unsigned int i = 0; i < sizeof(y_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &centerSpad_;
	for (unsigned int i = 0; i < sizeof(centerSpad_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &sensorAOffset_;
	for (unsigned int i = 0; i < sizeof(sensorAOffset_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sensorBOffset_;
	for (unsigned int i = 0; i < sizeof(sensorBOffset_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	#endif
	
	return serializedData.data();
}

void PPPConfig::show()
{
    Debug::trace("PPPConfig","priCalDistance_: " + Integer::toString(priCalDistance_) );    
    Debug::trace("PPPConfig","secondCalDistance_: " + Integer::toString(secondCalDistance_) );    
    Debug::trace("PPPConfig","sampleRate_: " + Integer::toString(sampleRate_) );    
    Debug::trace("PPPConfig","primarySlope_: " + Integer::toString(primarySlope_) );    
    Debug::trace("PPPConfig","secondarySlope_: " + Integer::toString(secondarySlope_) );    
    Debug::trace("PPPConfig","priPlateHomeDist_: " + Integer::toString(priPlateHomeDist_) );    
    Debug::trace("PPPConfig","secPlateHomeDist_: " + Integer::toString(secPlateHomeDist_) );    
    Debug::trace("PPPConfig","mechanicalHomeSpec_: " + Integer::toString(mechanicalHomeSpec_) );
	#if 1 /* VL53L1X */
    Debug::trace("PPPConfig","X: " + Integer::toString(x_) );    
    Debug::trace("PPPConfig","Y: " + Integer::toString(y_) );    
    Debug::trace("PPPConfig","Center: " + Integer::toString(centerSpad_) );
    Debug::trace("PPPConfig","sensorAOffset_: " + Integer::toString(sensorAOffset_) );
    Debug::trace("PPPConfig","sensorBOffset_: " + Integer::toString(sensorBOffset_) );	
	#endif    

}

