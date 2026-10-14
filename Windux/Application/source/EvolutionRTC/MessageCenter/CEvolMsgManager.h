#ifndef CEVOLMSGMANAGER_H
#define CEVOLMSGMANAGER_H



#include "CEvolMsgReceiver.h"
#include "IEvolMsgHandler.h"
#include "EvolMessageWrapper.h"
#include "CSmartPtr.h"



namespace Hobart
{


	class CEvolMsgManager : public CEvolMsgReceiver
	{
	public:
		CEvolMsgManager(IEvolMsgHandler* handler, const char* name = "CEvolMsgManager",
			bool managedThread = false, bool superThread = false, bool startIt = false)
			: CEvolMsgReceiver(name, managedThread, superThread, startIt), handler_(handler) {}
		virtual ~CEvolMsgManager() {}


		/* --- Getters and setters --- */
		IEvolMsgHandler* getHandler() const { return handler_; }


		/* --- Instance function members --- */
		//Incoming message processing.
		void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
			{ if ( handler_ && msg.notNull() ) handler_->processEvent(msg); }


	private:
		mutable IEvolMsgHandler* handler_;
	};


}



#endif /* CEVOLMSGMANAGER_H */
