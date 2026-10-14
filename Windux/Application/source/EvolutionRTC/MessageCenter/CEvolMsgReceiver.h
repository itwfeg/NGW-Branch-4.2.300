/**************************************************************************

TYPE:			Abstract Classes

PURPOSE:		Define base classes of Message Receiver. 
					.It can register/deregister the messages derived from EvolMessage.
					.It create it own thread for checking the incomming message.
					.processEvent() must be overridden by subclass.

**************************************************************************/

#if !defined CEvolMsgReceiver_h
#define CEvolMsgReceiver_h





#include "IEvolMsgHandler.h"
#include "Event.h"
#include "EvolMessageWrapper.h"
#include "CSmartPtr.h"
#include "CCriticalSection.h"


#include "CSemStream.h"


#include "CTLSThread.h"



namespace Hobart
{
	class CEvolMsgReceiver : public IEvolMsgHandler
	{
	public:
		friend class CMessageCenter;
		
		//This inner class creates a sperate thread for checking the incoming message.
		class CMessageCheckingThread  : public Util::CRunnableClass
		{
		public:
			CMessageCheckingThread( CEvolMsgReceiver * receiver , bool managedThread = false, bool superThread = false, bool startIt = true);
			~CMessageCheckingThread();

			void RequestToShutdownNicely();
			void RestartThread() {if (checkingThread) checkingThread->RestartThread();}
			unsigned GetThreadID() {if (checkingThread) return checkingThread->GetThreadID();else return 0;}
			bool IsThreadManaged() {if (checkingThread) return checkingThread->IsThreadManaged();else return false;}
			bool IsASuperThread() {if (checkingThread) return checkingThread->IsASuperThread(); else return false;}

		private:
			virtual DWORD Run();	
			
		private:
			CEvolMsgReceiver * msgReceiver;
			Util::CSmartPtr<Util::CTLSThread>	checkingThread;	

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
		};


	public:	
		CEvolMsgReceiver( const char * name = "default receiver" , bool managedThread = false, bool superThread = false, bool startIt = true);
		virtual ~CEvolMsgReceiver();

		void ShutdownNicely() {if (msgCheckingThread) msgCheckingThread->RequestToShutdownNicely();}
		void RestartThread() {if (msgCheckingThread) msgCheckingThread->RestartThread();}
		unsigned GetThreadID() {if (msgCheckingThread) return msgCheckingThread->GetThreadID();else return 0;}
		bool IsThreadManaged() {if (msgCheckingThread) return msgCheckingThread->IsThreadManaged(); else return false;}
		bool IsASuperThread() {if (msgCheckingThread) return msgCheckingThread->IsASuperThread(); else return false;}

		//Send message to Message Center.
		//If receiver != NULL, the message will send to the receiver only; 
		//		Otherwise, the message will send to all the receivers that registered for it.
		void dispatchEvent( const EvolMessage * msg, CEvolMsgReceiver * receiver = NULL );
		void dispatchEvent( const char * msg, int length, CEvolMsgReceiver * receiver = NULL );		
			
		//Incoming message processing. [This is inherited from IEvolMsgHandler.]
//		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg ) = 0;

		const std::string getReceiverName() { return receiverName; };
		
		//Register message. If permanent == false, Message Center will deregister it after
		//		sending out one message.		
		void registerEvent( EvolDeviceType evol_device, UINT8 evol_event, bool permanent = true );
		
		//Deregister message.
		void deregisterEvent( EvolDeviceType evol_device, UINT8 evol_event );
		
		//Deregister all the messages for this receiver.
		void deregisterAllEvents();
		
		void send( EvolMessage * msg ); //send to itself
		void send(const char * ptr, uint32_t length );//send to itself

	private:
		//Called by Message Center
		void addEvolMessage( Util::CSmartPtr< EvolMessageWrapper > & msg );
		
	private:
		Util::CSmartPtr< CMessageCheckingThread > msgCheckingThread;		
		Util::CSemStream< Util::CSmartPtr< EvolMessageWrapper > >	msgStream;
		// not needed, stream already guards is std::deque
		//Util::CSmartCriticalSection  msgStreamCS;
		std::string receiverName;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
	};


}//end of namespace


#endif //#if !defined CEvolMsgReceiver_h



