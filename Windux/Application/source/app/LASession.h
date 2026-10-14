/* This file is used for testing the application.
 * It is a test Label Applier Manager used for testing the application.
 * Larry
 */
 

#ifndef LASESSION_H
#define LASESSION_H

#include "evolution.h"

/* from comm */
#include "Session.h"
#include "HobThread.h"

#include "EvolMessage.h"
#include "DispatchEvent.h"
#include "EventManager.h"
#include "IEventManager.h"
#include "SystemEvent.h"
#include "LabelApplierEvent.h"

#include "ProductChangeMsg.h"
#include "ProductUpdateMsg.h"
#include "Product.h"
 
class LASession : public HobThread, public IEventManager
{
	public:
		LASession(HobThread *pMyLASession);
		LASession();
		~LASession();
		static int activeFunction(void *pObject);
		int activeFunctionTask();

	private:
/*		LASessionManager();
*/
		void handleLASessionEvent(LaEvent laEvent, char *recvBuffer);
		void handleLASysSessionEvent(SysEvent sysEvent, char *recvBuffer);
		void testLASession(int i);
        /**
         * @supplierCardinality 1
         * @clientCardinality 1
         * @label EvolutionMessage 
         */

//	    HobThread *pMyLASession;
		bool dispatchUpdates;

		short sendSpeed;

};
#endif //LASESSION_H




