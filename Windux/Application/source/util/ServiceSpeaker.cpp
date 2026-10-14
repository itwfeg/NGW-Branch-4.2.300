#include "ServiceSpeaker.h"
#include "PAL_platformheader.h"
#include "Configuration.h"
#include "PythonScripter.h"

bool ServiceSpeaker::goodBeepEnabled	= true;
int ServiceSpeaker::goodBeepFrequency	= 0x0800;
int ServiceSpeaker::goodBeepPeriod		= 40;
int ServiceSpeaker::goodBeepDelay		= 20;

bool ServiceSpeaker::badBeepEnabled		= true;
int ServiceSpeaker::badBeepFrequency	= 0x0F00;
int ServiceSpeaker::badBeepPeriod		= 200;
int ServiceSpeaker::badBeepDelay		= 100;

bool ServiceSpeaker::offlineBeepEnabled		= true;
int ServiceSpeaker::offlineBeepFrequency	= 0x0F00;
int ServiceSpeaker::offlineBeepPeriod		= 200;
int ServiceSpeaker::offlineBeepDelay		= 100;
int ServiceSpeaker::offlineBeepVolume		= 100;


void ServiceSpeaker::goodBeep( int times )
{
	if (goodBeepEnabled)
	{
		for ( int i = 0; i < times; i++ )
		{
        #if defined(LINUX_FIX)
            PAL_absclass::PAL_beep( badBeepFrequency, badBeepPeriod );
        #else
			::Beep( goodBeepFrequency, goodBeepPeriod );
        #endif
			if ( i != times - 1 )
            {
            #if defined(LINUX_FIX)
                PAL_absclass::PAL_sleep( goodBeepDelay );
            #else
				Sleep( goodBeepDelay );
            #endif
            }
		}
	}
}

void ServiceSpeaker::badBeep( int times )
{
	if (badBeepEnabled)
	{
		for ( int i = 0; i < times; i++ )
		{
        #if defined(LINUX_FIX)
            PAL_absclass::PAL_beep( badBeepFrequency, badBeepPeriod );
        #else
			::Beep( badBeepFrequency, badBeepPeriod );
        #endif
			if ( i != times - 1 )
            {
            #if defined(LINUX_FIX)
                PAL_absclass::PAL_sleep( badBeepDelay );
            #else
				Sleep( badBeepDelay );
            #endif
            }
		}
	}
}


void ServiceSpeaker::offlineBeep( int times )
{
	if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED) )
	{
		const string commandOn( PythonScripter::buildPythonCall("beep.py", -1, " " + inttostring(offlineBeepVolume)) );
		const string commandOff( PythonScripter::buildPythonCall("beep.py", -1, " 0") );
		string result;

		for (int i = 0; i < times; i++)
		{
			//Debug::trace("ServiceSpeaker", " beepOn: " + commandOn);
			PAL_absclass::PAL_executeWithResults(commandOn, result);
            PAL_absclass::PAL_sleep( offlineBeepDelay );
			//Debug::trace("ServiceSpeaker", " beepOff: " + commandOff);
			PAL_absclass::PAL_executeWithResults(commandOff, result);
		}
	}
}


