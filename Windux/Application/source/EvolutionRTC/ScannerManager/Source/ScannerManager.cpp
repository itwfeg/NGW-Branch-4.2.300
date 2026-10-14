//
/* Evolution Scale Project
 * Copyright (c) 2015, ITW Hobart Food Equipment Group, U.S.A.
 * Author: Joe E. Crew
 * All Right Reserved
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PAL_platformheader.h"

#include "ScannerManager.h"
#include "ScannerProductRead.h"
#include "BarcodeScannerEvent.h"
#include "motorolassiscanner.h"


#include "Debug.h"
#include "Configuration.h"

#include "util/thread/lock.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ScannerManager::dataType = 0;
std::string ScannerManager::dataTypeName("ScannerManager"); 
#endif

ScannerManager *ScannerManager::_instance_ = 0;
util_thread::Mutex<> ScannerManager::_singletonMutex_;

ScannerManager::ScannerManager()
	: ScannerManagerThread_(0), Hobart::CEvolMsgReceiver("ScannerManager")
{

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	tracing_        = Debug::isTracing("ScannerManager");
	tracingRun_     = Debug::isTracing("ScannerManager.Run");
	tracingEvent_   = Debug::isTracing("ScannerManager.processEvent");
	tracingError_   = Debug::isTracing("ScannerManager.Error");
    
	init();
}

ScannerManager::~ScannerManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	
	if ( ScannerManagerThread_ )
	{
		ScannerManagerThread_->StopThread();
		ScannerManagerThread_->WaitForCompletion();
	}

	shutdownTimerThreads();
	ShutdownNicely();

	if ( tracing_ )
		Debug::trace("ScannerManager", "~ScannerManager");

	deregisterAllEvents();
}

void ScannerManager::init()
{
	if(!ScannerManagerThread_)
		ScannerManagerThread_ = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "ScannerManager Thread" );
	else
		ScannerManagerThread_->RestartThread();
}

void ScannerManager::processEvent(Util::CSmartPtr<EvolMessageWrapper>& msg)
{
	if ( !msg.get() )
		return;

	Debug::trace("ScannerManager", "Process Event " +
		util_string::longtostring(msg->getDeviceType()) + " " +
		util_string::longtostring(msg->getEventType()) );

	if ( BarcodeScannerDev == msg->getDeviceType())
	{
		switch ( msg->getEventType() )
		{
		case BarcodeScannerEnable:
			enableScanner();
			break;
		case BarcodeScannerDisable:
			disableScanner();
			break;
		default:
			break;
		}
	}
}

void ScannerManager::enableScanner()
{
	if ( tracing_ )
		Debug::trace("ScannerManager.enableScanner", "The scanner is now ENABLED");

	scannerActive_ = true;
}
		
void ScannerManager::disableScanner()
{
	if ( tracing_ )
		Debug::trace("ScannerManager.disableScanner", "The scanner is now DISABLED");

	scannerActive_ = false;
}

DWORD ScannerManager::Run()
{
#ifdef __linux__
    Debug::trace("ScannerManager", "Run() - Install LINUX driver");
    motorolaScanner scanner( "/dev/ttySymbolScanner" );
#else
    Debug::trace("ScannerManager", "Run() - Install the Windows driver");    
    motorolaScanner scanner( "COM4" );								// NEEDS TO BE CONFIGURABLE, CHANGABLE
#endif

	if( !scanner.isValid() )
	{
		if ( tracing_ )
			Debug::trace("ScannerManager.Run", "!scanner->isValid() before the LOOP");
		return 0;
	}

    registerEvent(BarcodeScannerDev, BarcodeScannerEnable);
    registerEvent(BarcodeScannerDev, BarcodeScannerDisable);
    registerEvent(BarcodeScannerDev, BarcodeReadScannerCode);

    scannerActive_ = false;

    try
	{
		static const size_t idleTime( 2000 );
		static const size_t sleepTime( 250 );

		scanner.setDisableAfterScan(true);							// set initial functionality, and then just
		scanner.setLedEnabled(true);								// enable and disable moving forward

		for ( ;; )
		{
			motorolaScanner::barcodeType barcodeType;
			std::string barcode;

			if(!scanner.isValid())
			{
				if ( tracing_ )
					Debug::trace("ScannerManager.Run", "!scanner->isValid() in the LOOP");
				return 0;
			}

			if(scanner.read(barcodeType, barcode))				// read it
			{
				scancode_ = barcode;
			
				if ( tracing_ )
					Debug::trace("ScannerManager.Run", "Scanned barcode scancode_: " + scancode_);

				ScannerProductRead * scancodeMsg = new ScannerProductRead (this, scancode_);

				Util::CSmartPtr<EvolMessage> evolMsg ( scancodeMsg );

				if ( evolMsg.notNull() )
				{
					dispatchEvent( evolMsg.get() );						// pushes us to ScannerProductRead.serialize()
				}

				PAL_absclass::PAL_sleep( idleTime );
			}

			// ok now I need to see if we are still active 
			if( scannerActive_ )
				scanner.setEnabled(true);
			else
				scanner.setEnabled(false);

			PAL_absclass::PAL_sleep(sleepTime);

		}
	}
	catch (...)
	{
		Debug::trace("ScannerManager.Run", "In the catch() for the try()");;
	}

	Debug::trace("ScannerManager.Run", "Exiting run loop");;
	return 0;
}

void ScannerManager::timeoutProcess( __int32 timer_id )
{
	if ( timer_id == NoIdTimer )
	{
		scannerActive_ = false;	
	}
}


ScannerManager *ScannerManager::getInstance()
{
	if ( _instance_ == 0 )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_singletonMutex_);

		if ( _instance_ == 0 )
			_instance_ = new ScannerManager();
	}

	return _instance_;
}


void ScannerManager::deleteInstance()
{
	Debug::trace("ScannerManager", "deleteInstance");

	if ( _instance_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_singletonMutex_);

		if ( _instance_ )
		{
			delete _instance_;
			_instance_ = 0;
		}
	}
}
