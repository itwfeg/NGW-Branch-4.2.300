/* This file is used for testing the application.
 * It is a test Weigher Manager used for testing the application.
 * Larry
 */
 

#ifndef WSSESSION_H
#define WSSESSION_H

#include "evolution.h"
#include "CEvolMsgReceiver.h"

/* from comm */
#include "Session.h"

#include "EvolMessage.h"

#include "SystemEvent.h"
#include "WeigherEvent.h"
#include "ProductDevEvent.h"
#include "WeigherStatus.h"
//#include "ProductChangeMsg.h"
//#include "ProductUpdateMsg.h"
//#include "Product.h"
 
class WSSession : public Hobart::CEvolMsgReceiver
{
	public:
		WSSession();
		~WSSession();

		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

		static UINT32 counts;
		static UINT32 rawcounts;

	private:
		void sendWeight();
		void handleWSSessionEvent(WgEvent wgEvent, const char *recvBuffer);
		void handleWeigherSysEvent(SysEvent sysEvent, const char *recvBuffer);
//		void handleProductEvent(ProductEvent prEvent, const char *recvBuffer, int len);
//		void handleOperationModeEvent(OpModeEvent opModeEvent, char *recvBuffer);
		void testWSSession(int i);

		bool dispatchUpdates;

		short myScaleFactor;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif //WSSESSION_H




