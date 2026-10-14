/* This file is used for testing the application.
 * It is a test Wrapper Manager used for testing the application.
 * Larry
 */
 

#ifndef WRSESSION_H
#define WRSESSION_H

#include "evolution.h"

/* from comm */
#include "Session.h"
#include "HobThread.h"

#include "EvolMessage.h"
#include "DispatchEvent.h"
#include "EventManager.h"
#include "IEventManager.h"

//#include "WrapperStatus.h"
//#include "ProductChangeMsg.h"
//#include "ProductUpdateMsg.h"
//#include "Product.h"
#include "WrapperSensors.h"
#include "WrapperStatus.h"
#include "WrapperInquire.h"

class WRSession : public HobThread, public IEventManager
{
	public:
		WRSession(HobThread *pMyWRSession);
		WRSession();
		~WRSession();
		static int activeFunction(void *pObject);
		int activeFunctionTask();


	private:
/*		WRSessionManager();
*/
		void handleWRSessionEvent(WrEvent wrEvent, char *recvBuffer);
		void initializeWrapperConfig();
		void saveWrapperConfig(WrapperConfiguration *newConfig);
		void handleWrapperSysEvent(SysEvent sysEvent, char *recvBuffer);

/*		void handleOperationModeEvent(OpModeEvent opModeEvent, char *recvBuffer);
*/
		void testWRSession(int i);
        /**
         * @supplierCardinality 1
         * @clientCardinality 1
         * @label EvolutionMessage 
         */

	    HobThread *pMyWRSession;
		bool dispatchUpdates;

		//	To monitor Wrapper sensors
		bool 			monitorSensors;
		WrapperSensors	*wrapperSenseData;
		int				sensorLoopCount;
		bool 			sendWrapperStatusData;
		WrapperStatus	*wrapperStatusData;
		WrapperInquire	*wrapperConfiguration;
};
#endif //WRSESSION_H




