/**
 * @file ScannerManager.h
 * @brief HfcScannerProductRead tracks all barcodes that have been scanned by the usb scanner.  Currently the
 * Scanner is intended to be attached to an elevated self service device. The customer will have
 * the ability to walk up to a elevated self service scale with their produce item and hold it under
 * the scanner and the barcode sticker attached to the item will be used to look up a PLU which the
 * customer can then print a label for purchase.
 *
 * @copyright Copyright (c) 2015, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup BarcodeScannerCsu
 * @{
*/


#ifndef SCANNERMANAGER_H
#define SCANNERMANAGER_H

#include "BarcodeScannerEvent.h"
#include "CEvolMsgReceiver.h"
#include "CHobartTimer.h"
#include "motorolassiscanner.h"

// should I include the motorola *.h files here? 

#include "util/thread/mutex.h"
#if defined( __linux__ )
    #include <termios.h>
    #include <unistd.h>
#endif

class ScannerManager : public Hobart::CEvolMsgReceiver, public Util::CRunnableClass, public Hobart::CHobartTimer
{
	public:

		void init();

		// This is in case we end up needing to change the COM port from 3<-->4

		void enableScanner();
		void disableScanner();

		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg ); //lcg rev 0.1
		virtual void timeoutProcess( __int32 timer_id );

		static ScannerManager *getInstance();
		static void deleteInstance();

	private:

		enum ScannerManagerTimer
		{
			NoIdTimer = 1
		};

		Util::CSmartPtr<Util::CTLSThread> ScannerManagerThread_;

		/* --- Constructors and destructor --- */
		ScannerManager();
		// Do not allow the default copy constructor.
		ScannerManager(const ScannerManager&);
		~ScannerManager();


		/* --- Instance Function Members --- */
		// Do not allow the default assignment operator.
		ScannerManager operator=(const ScannerManager& rhs);

		bool tracing_;
		bool tracingRun_;
		bool tracingEvent_;
		bool tracingError_;
		
		bool scannerActive_;

		virtual DWORD Run();

		// initialize it in the Run() now - motorolaScanner *scanner;
		
		// 1: Constructor change UINT8 scancode_[MAX_SCANNED_CODE_STRING];
		std::string scancode_;

		/* --- Class Data Members --- */
		// Singleton data members
		static ScannerManager *_instance_;
		static util_thread::Mutex<> _singletonMutex_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //SCANNERMANAGER_H
