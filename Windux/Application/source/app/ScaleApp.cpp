
#include <PAL_platformheader.h>

#include "OutputLogger.h"
#include "DADFResourceUtils.h"
#include "CHobartScaleService.h"
#include "Main.h"
#include "UsbManager.h"
#include "Hex.h"
#include "HobString.h"

#include "SessionManagerHelper.h"

#include "MessageLogger.h"

#include <windows.h>

bool keepLooping( true );

///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI CtrlHandler( DWORD fdwCtrlType )
{
	switch ( fdwCtrlType )
	{
	// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		Debug::trace( HobString("ScaleApp"), HobString("Ctrl-C event") );
		Beep( 750, 300 );

		keepLooping = false;
		return TRUE;

	// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		Debug::trace( HobString("ScaleApp"), HobString("Ctrl-Close event") );
		Beep( 600, 200 );

		keepLooping = false;
		return TRUE;

	// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		Beep( 900, 200 );
		Debug::trace( HobString("ScaleApp"), HobString("Ctrl-Break event") );
		return TRUE;

	case CTRL_LOGOFF_EVENT:
		Beep( 1000, 200 );
		Debug::trace( HobString("ScaleApp"), HobString("Ctrl-Logoff event") );
		return FALSE;

	case CTRL_SHUTDOWN_EVENT:
		Beep( 750, 500 );
		Debug::trace( HobString("ScaleApp"), HobString("Ctrl-Shutdown event") );
		return FALSE;

	default:
		break;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
	PAL_absclass::PAL_OutputDbgStr( L"HobartApp: entry!\r\n" );

	// ===========================================================================
	// INIT LOGGING
	// ===========================================================================

#ifdef _DEBUG
	Debug::setEnabled( true );

	Debug::addTraceCategory( "ScaleApp" );
	Debug::trace( HobString("ScaleApp"), HobString("ScaleApp.exe") );

	//Debug::setTraceAllCategoriesOf("UsbManager");
	//Debug::setTraceAllCategoriesOf("CUsbPipe");
#endif

	// ===========================================================================
	// STARTUP SERVICE
	// ===========================================================================

	try
	{
		PAL_absclass::PAL_OutputDbgStr( L"HobartApp: calling EvolMain::startAll\r\n" );

		EvolMain main;
		main.startAll( 1, 0, 0 );

#ifdef _DEBUG
		// EvolMain::startAll loads the default configuration, which contains a debug enable 
		// property that forces setEnabled(false). Therefore, setEnabled(true) must be 
		// called here to override that.
#ifdef REENABLE_DEBUG_AFTER_CONFIG_LOAD
		Debug::setEnabled(true);
#endif

#endif

		// Enumerate all USB devices and launch required service classes.
		// Note: this must be done after registering for device notifications,
		// because the enumeration process may launch device firmware updaters 
		// which cause device resets after the firmware is downloaded, which in turn
		// causes removal/arrival notifications for the device in question. 
		Debug::trace( "ScaleApp", HobString("enumerating USB devices") );
		PAL_absclass::PAL_OutputDbgStr( L"HobartApp: enumerating USB devices\r\n" );

		UsbManager::getInstance()->EnumerateDevices();

		// wait forever...
		Debug::trace( "ScaleApp", HobString("waiting for termination" ) );
		PAL_absclass::PAL_OutputDbgStr( L"HobartApp: waiting for termination\r\n" );

		if ( SetConsoleCtrlHandler( CtrlHandler, TRUE ) )
		{
			while ( keepLooping )
				Sleep( 200 );
		}

		PAL_absclass::PAL_OutputDbgStr( L"HobartApp: shutdown!\r\n" );
	}
	catch (...)
	{
		Debug::trace( HobString("ScaleApp"), HobString("unspecified exception") );
		PAL_absclass::PAL_OutputDbgStr( L"HobartApp: unspecified exception.\r\n" );
		LogCatch;
	}

	// ===========================================================================
	// SHUTDOWN SERVICE
	// ===========================================================================

	try
	{
		PAL_absclass::PAL_OutputDbgStr( L"HobartApp: stopping UsbManager\r\n" );
		UsbManager::getInstance()->RemoveAllDevices();

		PAL_absclass::PAL_OutputDbgStr(L"HobartApp: shutdown\r\n");
		SessionManagerHelper::getInstance()->panicShutdown();
	}
	catch (...)
	{
		Debug::trace(HobString("ScaleApp"), HobString("exception"));
		PAL_absclass::PAL_OutputDbgStr(L"HobartApp: handled exception\r\n");
	}

	PAL_absclass::PAL_OutputDbgStr( L"HobartApp: exit!\r\n" );
	return 0;
}