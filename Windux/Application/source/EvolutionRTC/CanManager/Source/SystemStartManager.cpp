/*
 * @file SystemStartManager.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Manages Startup of the AWS3 Wrapper
 * Created on: Oct 4, 2016
 */

#include "SystemStartManager.h"
#include "HfcRequestPeripheralList.h"
#include "NGWGeneric.h"
#include "NGWUpdateManager.h"
#include "RealTimeController.h"
#include "CanManager.h"
#include "FpmManager.h"
#include "KPCBootManager.h"
#include "KPCBootUpgrader.h"
#include "PrinterGeneric.h"
#include "WeigherGeneric.h"
#include "PrinterRequestHeadDotStatus.h"
#include "util/filesys/filename.h"
#include "UnitTest.h"
#include "Integer.h"
#include "Debug.h"
#include "WrapperManager.h"

#include "PrinterRequestWakeup.h"
#include "WeigherRequestWakeup.h"

std::mutex SystemStartManager::instanceMutex_;
SystemStartManager *SystemStartManager::instance_( nullptr );

///////////////////////////////////////////////////////////////////////////////////////////////////
SystemStartManager *SystemStartManager::getInstance()
{
	if ( !instance_ )
	{
		std::lock_guard<std::mutex> guard( instanceMutex_ );

		if ( !instance_ )
			instance_ = new SystemStartManager();
	}

	return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SystemStartManager::shutdown()
{
	Debug::trace("SystemStartManager", "shutdown()");

	if ( instance_ )
	{
		std::lock_guard<std::mutex> guard( instanceMutex_ );

		if ( instance_ )
		{
			delete instance_;
			instance_ = nullptr;
		}
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SystemStartManager::SystemStartManager() : Hobart::CEvolMsgReceiver("SystemStartManager")
{

	state_ = Start_;

	coldStart_ = false;
	finished_ = false;

	pNGWUpdateManager_ = NULL;
	if( FpmManager::getInstance() != NULL )
		scaleStyle_ = FpmManager::getInstance()->getMFGSystemStyle();
	else
		Debug::trace("SystemStartManager", "FpmManager is NULL" );

	kBootManager_ = NULL;
	kBootUpgrader_ = NULL;

	Debug::trace("SystemStartManager", "ScaleStyle: " + scaleStyle_ );

	if( ( scaleStyle_ != "HTK" ) && ( scaleStyle_ != "FSS" ) ) {
		mode_ = _Idle;
	} else {
		mode_ = _KPCBootloader;
	}

#if defined ( __linux__ )		// No windows support for CAN

	//start CanManager for upgrading or starting up the wrapper
	CanManager *pCanManager;

	if ( (pCanManager = CanManager::getInstance( CanManager::_Boot )) )
		pCanManager->initializeTranslator();
	else
		Debug::trace("SystemStartManager.Error", "CAN interface (boot) is not available!" );

	myRunThread = new Util::CTLSThread( *this, 1, 0, "SystemStartManager" );
#endif // __linux__

	Debug::trace("SystemStartManager", "mode: " + Integer::toString(mode_));

	registerEvent(KPCBootloader, KpcBootDone);
	registerEvent(KPCBootloader, KpcBootReady);
	registerEvent(KPCBootUpdater, KpcBootUpgraderDone);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
SystemStartManager::~SystemStartManager()
{
	Debug::trace("SystemStartManager", "~SystemStartManager()");

	if( FpmManager::getInstance()->isWrapperTwoPrinter() )
	{
		if( kBootManager_ )
		{
			Debug::trace("SystemStartManager", "shutting down the kpc boot manager");
			kBootManager_->shutdown();
			kBootManager_ = nullptr;
		}
	}

	deregisterAllEvents();

	ShutdownNicely();

	// tell the thread to exit
	m_ExitEvent.EventSet();

	// wait for the thread to exit before deleting all the resources!
	if ( myRunThread )
	{
		while ( myRunThread->IsRunning() )
			PAL_absclass::PAL_sleep( 1 );

		Debug::trace("SystemStartManager", "~SystemStartManager() Thread has exited.");

        delete myRunThread;
		myRunThread = NULL;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SystemStartManager::setModeToScan( bool coldStart )
{
	std::lock_guard<std::mutex> guard( mutex_ );

	if ( _Idle != mode_ )
		return false;

	mode_ = _Scan;
	state_ = Start_;
	coldStart_ = coldStart;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD SystemStartManager::Run()
{
	Debug::trace("SystemStartManager", "Entering thread");

	do
	{
		PAL_absclass::PAL_sleep(1);

		std::lock_guard<std::mutex> guard( mutex_ );

		switch ( mode_ )
		{
		case _Scan:
			handleScan();
			break;

		case _Idle:
			break;
		case _KPCBootloader:
			RealTimeController::getInstance()->setDelayPeripheralList();
			handleKpcBootloader();
			break;
		case _KPCBootUpgrader:
			handleKpcBootUpgrader();
			break;
		case _StartupKpc:
			handleKpcStartup();
			RealTimeController::getInstance()->clrDelayPeripheralList();
			break;
		default:
			Debug::trace( "SystemStartManager.Error", "Unhandled mode_: " + Integer::toString(mode_) );
			break;

		}

	} while ( !m_ExitEvent.IsEventSet() );
	
	Debug::trace("SystemStartManager", "Exiting thread");
	finished_ = true;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SystemStartManager::handleScan()
{
	switch ( state_ )
	{
	case Start_:
		Debug::trace("SystemStartManager", "Mode: _Scan  Start_: start NGWUpdateManager.");

		if ( coldStart_ )
		{
			// do not respond to the peripheral message until upgrade is finished
			RealTimeController::getInstance()->setDelayPeripheralList();
		}

		// check wrapper for firmware updates and start wrapper application
		pNGWUpdateManager_ = NGWUpdateManager::getInstance();

#if defined ( __linux__ )		// No windows support for CAN
		if( pNGWUpdateManager_ != NULL )
			CanManager::getInstance()->setTranslatorUpdaterManager( pNGWUpdateManager_ );
#endif

		state_ = WaitForFinish_;
		break;

	case WaitForFinish_:
		if ( pNGWUpdateManager_->isManagerFinished() )
			state_ = End_;

		break;

	case End_:
	{
		Debug::trace("SystemStartManager", "Mode: _Scan  End_: shutdown NGWUpdateManager.");

		bool detected_ = pNGWUpdateManager_->isWrapperDetected();
		// shutdown the update manager
		NGWUpdateManager::shutdown();

		if(!FpmManager::getInstance()->isWrapperTwoPrinter())
		{
		    if( !detected_ ) {
			    NGWGeneric wrDone(_WrNGWScanFinished, this);
			    dispatchEvent(&wrDone);
		    }
        }

		if ( coldStart_ )
		{
			// allow response to peripherals list message
			RealTimeController::getInstance()->clrDelayPeripheralList();

			// resend the request for peripherals list
			HfcRequestPeripheralList reqPeripheralList(this);
			dispatchEvent(&reqPeripheralList);
		}

#if defined ( __linux__ )		// No windows support for CAN
		if ( ( scaleStyle_ == "NGW3" ) || ( scaleStyle_ == "NGW2" ) )
		{
			//start CanManager for normal wrapper operations
			CanManager *pCanManager;

			if ( (pCanManager = CanManager::getInstance( CanManager::_PrimaryBus )) )
				pCanManager->initializeTranslator();
			else
				Debug::trace("SystemStartManager.Error", "CAN interface (primary) is not available!" );

			Debug::trace("SystemStartManager", "send: _WrStartKPC");
			NGWGeneric wrDone(_WrStartKPC, this);
			dispatchEvent(&wrDone);

			
		}
#endif


		// go back to idle mode
		mode_ = _Idle;

		break;
	}

	default:
		Debug::trace( "SystemStartManager.Error", "Mode: _Scan  Unhandled state: " + Integer::toString(state_) );
		break;
	}
}

void SystemStartManager::handleKpcBootloader(){
	Debug::trace("SystemStartManager", "handleKpcBootloader(): Starting KPC ");
	if(!kBootManager_){
		Debug::trace("SystemStartManager", "kBootManager was null ");
		kBootManager_ = KPCBootManager::getInstance();
	}
	else
	{
		Debug::trace("SystemStartManager", "KPC Already Started");
	}
	mode_ = _Idle;
}

void SystemStartManager::handleKpcBootUpgrader(){
	Debug::trace("SystemStartManager", "handleKpcBootUpgrader(): Starting KPC Upgrader ");
	if(!kBootUpgrader_){
		kBootUpgrader_ = KPCBootUpgrader::getInstance();
	}
	mode_ = _Idle;
}

void SystemStartManager::handleKpcStartup()
{
	Debug::trace("SystemStartManager", "handleKpcStartup(): Starting KPC ");

	WeigherRequestWakeup wMsg( NULL, 0 );
	PrinterRequestWakeup pMsg( NULL, FirstPrinter );

    //let's give the kpc time to initialize. We need it after a kpc upgrade
	PAL_absclass::PAL_sleep(500);

	dispatchEvent( &wMsg );
	dispatchEvent( &pMsg );

	mode_ = _Idle;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void SystemStartManager::processEvent(Util::CSmartPtr<EvolMessageWrapper> & msg)
{
	std::lock_guard<std::mutex> guard( mutex_ );

	if (!msg.get()) {
		Debug::trace("SystemStartManager.Error", "processEvent(): message received is NULL! Ignoring");
		return;
	}
	switch (msg->getDeviceType()) {
	case KPCBootloader:
		switch(msg->getEventType()){
		case KpcBootReady:{
			Debug::trace("SystemStartManager", "KpcBootReady");
			//TODO: Fix so that we check which printer
			PrinterGeneric msg( PrKPCDetached, this, FirstPrinter);
			dispatchEvent( &msg );
			WeigherGeneric msg2 ( WgKPCDetached, this );
			dispatchEvent( &msg2 ); 
			break;
		}
		case KpcBootDone:
			Debug::trace("SystemStartManager", "processEvent(): KpcBootDone. Killing Boot Manager");
			// SOF-5607 We no longer want the upgrade of a bootloader to be and automatic process.
			// We run the chance of bricking boards from a core upgrade if this is an automatic process.
			//mode_ = _KPCBootUpgrader;		//do not start instance of boot upgrader!
			kBootManager_->shutdown();
			kBootManager_ = NULL;
			//SOF-5607 added for this bug fix.
			mode_ = _StartupKpc;

			if(FpmManager::getInstance()->isWrapperTwoPrinter())
			{
				NGWGeneric wrDone(_WrNGWScanFinished);
				dispatchEvent(&wrDone);
			}
			break;
		default:
			Debug::trace("SystemStartManager.Error", "processEvent(): Unhandled Device Type DeviceType: " + Integer::toString(msg->getEventType()));
			break;
		}
		break;
	case KPCBootUpdater:
		switch(msg->getEventType()){
		case KpcBootUpgraderDone:
			Debug::trace("SystemStartManager", "processEvent(): KpcBootUpgraderDone. Killing Boot Upgrader");
			mode_ = _StartupKpc;
			kBootUpgrader_->shutdown();
			kBootUpgrader_ = NULL;
			break;
		default:
			Debug::trace("SystemStartManager.Error", "processEvent(): Unhandled Device Type DeviceType: " + Integer::toString(msg->getEventType()));
			break;
		}
		break;
	default:
		Debug::trace("SystemStartManager.Error", "processEvent(): Unhandled Device Type DeviceType: " + Integer::toString(msg->getDeviceType()));
		break;
	}

	return;
}
