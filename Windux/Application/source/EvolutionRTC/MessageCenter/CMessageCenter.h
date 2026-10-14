/**************************************************************************

TYPE:			Singleton

PURPOSE:		1). Keep track all the Message Receivers and their registered messages. 
				2). Deliver the incoming messages to all the receivers that register for them.

**************************************************************************/

#if !defined CMessageCenter_h
#define CMessageCenter_h

#include "EvolMessageWrapper.h"
#include "CSmartPtr.h"
#include "CCriticalSection.h"
#include "CSemStream.h"
#include "CTLSThread.h"
#include "CEvolMsgReceiver.h"

#include <map>

namespace Hobart
{	
	class CMessageCenter  : public Util::CRunnableClass
	{
	public:
		friend void CEvolMsgReceiver::registerEvent( EvolDeviceType evol_device, UINT8 evol_event, bool permanent );
		friend void CEvolMsgReceiver::deregisterEvent( EvolDeviceType evol_device, UINT8 evol_event );
		friend void CEvolMsgReceiver::deregisterAllEvents();
		friend void CEvolMsgReceiver::dispatchEvent( const EvolMessage * msg, CEvolMsgReceiver * receiver );
		friend void CEvolMsgReceiver::dispatchEvent( const char * msg, int length, CEvolMsgReceiver * receiver );
	
		//          dev << 16 + msg id, receiver's address, permanent
		//                       |                   |      |
		typedef std::multimap<uint32_t, std::pair< void*, bool> >					DispatchMsgMap;
		typedef std::multimap<uint32_t, std::pair< void*, bool> >::iterator		DispatchMsgMapIterator;
			
		//Singleton
		static  CMessageCenter & instance();
		virtual ~CMessageCenter();

		// Generic publich method to send messages to any receiver (you
		// do not need to be a message receiver to send a message).
		CMessageCenter& operator << ( EvolMessage * msg );
		
	private:
		//Do not allow to be initialized except using Instance()
		CMessageCenter();
		
		//Overriden function defined in CRunnableClass. 
		DWORD Run();

		//Forward message smart pointer to all the receivers that register it.
		void dispatch( Util::CSmartPtr< EvolMessageWrapper > & msg );
		
		//The following functions are only called by CEvolMsgReceiver
		void registerEvent( EvolDeviceType evol_device, UINT8 evol_event, CEvolMsgReceiver* receiver, bool permanent );
		void deregisterEvent( EvolDeviceType evol_device, UINT8 evol_event, CEvolMsgReceiver* receiver );
		void deregisterAllEvents( CEvolMsgReceiver* receiver );
		void dispatchEvent( const EvolMessage * msg, CEvolMsgReceiver * receiver = NULL );
		void dispatchEvent( const char * msg, int length, CEvolMsgReceiver * receiver = NULL );
	    		
	private:
		Util::CSmartPtr< Util::CTLSThread >	dispatchThread;
		Util::CSemStream< Util::CSmartPtr< EvolMessageWrapper > > incomingStream;
		DispatchMsgMap				dispatchMsgMap;
		static  CMessageCenter* 	messageCenter;
        static Util::CSmartCriticalSection critSec;	
		
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
	};

}//end of namespace

#endif //#if !defined CMessageCenter_h

