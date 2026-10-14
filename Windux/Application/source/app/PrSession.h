/* This file is used for testing the application.
 * It is a test Printer Manager used for testing the application.
 * Larry
 */
 

#ifndef PRSESSION_H
#define PRSESSION_H

#include <queue>
using namespace std;

#include "evolution.h"

/* from comm */
#include "Session.h"
#include "HobThread.h"

#include "EvolMessage.h"
#include "DispatchEvent.h"
#include "EventManager.h"
#include "IEventManager.h"

#include "SystemEvent.h"
#include "OpModeEvent.h"
#include "RendererEvent.h"
#include "PrinterEvent.h"
#include "ProductDevEvent.h"

#include "Printer.h"

class Product;
class ProductTransaction;
class LabelType;
class RendererManager;
 
class PrSession : public HobThread, public IEventManager
{
	public:
//		PrSession(HobThread *pMyPrSession, PrinterType printerType);
		PrSession();
		~PrSession();
		static int activeFunction(void *pObject);
		int activeFunctionTask();

		static int	index;
		static PrinterStateType printerOnOrOff;

	private:
		void initSensorsTracking(PrinterType printerType);
		void sendPrinterSensors();

		void handlePrSessionEvent(PrEvent prEvent, char *recvBuffer);
		void handlePrinterSystemEvent(SysEvent sysEvent, char *recvBuffer);
		void handleRendererMessage(RdEvent rdEvent, char *recvBuffer);
		void handleOperationModeMessage(OpModeEvent opModeEvent, char *recvBuffer);
		void handleProductEvent(ProductEvent prodEvent, char *recvBuffer);

		void preImageLabel();
		void testPrSession(int i);

	    /**
	     * @supplierCardinality 1
	     * @clientCardinality 1
	     * @label Evolution Messages
	     * @undirected*/

	    HobThread *pMyPrinter;
		PrinterType myPrinterType;

		RendererManager *pMyRenderer;
		UINT16 myRendererSessionId;


		bool preImaging;
		bool postImaging;

		UINT8 *pSourceData;
		UINT8 *pDestinationData;

		Product *pCurrentProduct;

		queue<ProductTransaction *> productTransactionQ;
		ProductTransaction *pCurrentProductTransaction;

		LabelType *pCurrentLabel;

		bool takingLabel;
		int takingLabelTimer;

		bool labelTaken;
		int labelTakenTimer;

		bool printComplete;
		int printCompleteTimer;

};
#endif //PRSESSION_H




