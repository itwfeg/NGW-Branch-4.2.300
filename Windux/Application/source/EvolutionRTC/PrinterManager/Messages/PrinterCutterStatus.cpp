/**
 * @file PrinterCutterStatus.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief status of cutter home switch
 * Created on: Nov 9, 2017
 *
 **/
#include "PrinterCutterStatus.h"
#include "Debug.h"
#include "Integer.h"

PrinterCutterStatus::PrinterCutterStatus( Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType,
						                  bool home_, bool installed_, bool interlock_, bool jammed_) : PrinterGeneric( PrCutterStatus, pHobThread, printerType )
{
	home = home_;
	installed = installed_;
	interlock = interlock_;
	jammed = jammed_;
}

PrinterCutterStatus::PrinterCutterStatus(const char *buffer) : PrinterGeneric(buffer)
{
	PrinterGeneric::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &home;
	for (unsigned int i = 0; i < sizeof(home); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &installed;
	for (unsigned int i = 0; i < sizeof(installed); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &interlock;
	for (unsigned int i = 0; i < sizeof(interlock); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &jammed;
	for (unsigned int i = 0; i < sizeof(jammed); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterCutterStatus::~PrinterCutterStatus()
{

}

const char *PrinterCutterStatus::serialize() const
{
	PrinterGeneric::serialize();

	char *pChar = (char *) &home;
	for (unsigned int i = 0; i < sizeof(home); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &installed;
	for (unsigned int i = 0; i < sizeof(installed); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &interlock;
	for (unsigned int i = 0; i < sizeof(interlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &jammed;
	for (unsigned int i = 0; i < sizeof(jammed); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterCutterStatus::show() const
{
	if ( !Debug::isTracing( "PrinterCutterStatus" ) )
	return;

	Debug::trace("PrinterCutterStatus", "home: " + Integer::toString( home ) ); 
	Debug::trace("PrinterCutterStatus", "installed: " + Integer::toString( installed ) ); 
	Debug::trace("PrinterCutterStatus", "interlock: " + Integer::toString( interlock ) ); 
	Debug::trace("PrinterCutterStatus", "jammed: " + Integer::toString( jammed ) ); 
}
