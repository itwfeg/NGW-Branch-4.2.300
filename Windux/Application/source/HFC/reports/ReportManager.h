/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef REPORTMANAGER_H
#define REPORTMANAGER_H



#include "CTLSThread.h"
#include "IEvolMsgHandler.h"
#include "CEvolMsgManager.h"
#include "ReportEvent.h"

#include "Printer.h"



class SessionMessage;

class DBManager;

class Report;
class HfcStartReport;



//active function states
typedef enum ReportState
{
	IdleState,
	ReportingState,
	CancelState,
	DoneState
} ReportState_t;


class ReportManager : public Util::CRunnableClass, public Hobart::IEvolMsgHandler
{
	public:
		/* --- Constructors and destructor --- */
		ReportManager(SessionMessage* msg);
		~ReportManager();


		/* --- Accessor methods --- */
		Hobart::CEvolMsgManager* getMessageReceiver() { return msgManager_; }


		/* --- Processing methods --- */
		DWORD Run();

		void registerEvent( EvolDeviceType evol_device, UINT8 evol_event, bool permanent = true )
				{ msgManager_->registerEvent(evol_device, evol_event, permanent); }
		void registerEventTemporarily(EvolDeviceType evol_device, UINT8 evol_event)
				{ msgManager_->registerEvent(evol_device, evol_event, false); }
		void deregisterEvent( EvolDeviceType evol_device, UINT8 evol_event )
				{ msgManager_->deregisterEvent(evol_device, evol_event); }
		void deregisterAllEvents()
				{ msgManager_->deregisterAllEvents(); }

		void dispatchEvent( const EvolMessage * msg, Hobart::CEvolMsgReceiver * receiver = NULL )
				{ msgManager_->dispatchEvent(msg, receiver); }
//		void dispatchEvent( const char * msg, int length, Hobart::CEvolMsgReceiver * receiver = NULL );		
		virtual void processEvent( Util::CSmartPtr<EvolMessageWrapper> & msg );
		//		void handleMessage(SessionMessage *msg);


	private:
		// --- Object data members ---
		Util::CSmartPtr<Util::CTLSThread> thread_;
		Hobart::CEvolMsgManager* msgManager_;

		Report* report_;
		ReportState state_;   							//current state of the task
		ReportEventType event_;
		ReportType reportType_;
		PrinterType printerType_;
		int cutTestNumber_;
		int cutTestReportType_;
		HfcStartReport* requestMessage_;

		std::string reportName_;
		std::string reportContent_;

		// --- Processing methods ---
		void init();
		void handleCancel();
		void handleDone();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif // REPORTMANAGER_H
