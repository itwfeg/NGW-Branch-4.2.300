
#include "OpModeEvent.h"
#include "ProductionEntryMode.h"

#include "PrinterManager.h"
#include "PrinterStatus.h"
#include "PrinterLabelComplete.h"
#include "PrinterLabelReady.h"
#include "PrinterRequestStatus.h"
#include  "PrinterCutterStatus.h"

#include "ProductUpdateMsg.h"
#include "ProductChangeMsg.h"
#include "ProductFinishedPrintingAllLabelsMsg.h"
#include "ProductStartPrintForTransactionMsg.h"
#include "ProductCompleteMsg.h"
#include "ProductPackageCompleteMsg.h"

#include "ProductTransaction.h"
#include "ProductManager.h"
#include "Configuration.h"
#include "SessionManager.h"
#include "Session.h"
#include "ResponseMessageManager.h"
#include "LabelPrintingCounts.h"
using namespace std;

#include "Integer.h"
#include "Hex.h"
#include "Bool.h"
#include "HobString.h"
#include "HobList.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductionEntryMode::dataType = 0;
std::string ProductionEntryMode::dataTypeName("ProductionEntryMode"); 
#endif

ProductionEntryMode::ProductionEntryMode(Product *pCurrentProduct,
								 //WeigherManager *pWeigherManager,
								 //PrinterManager *pFirstPrinter, 
								 //PrinterManager *pSecondPrinter,
								 //PrinterManager *pThirdPrinter,
								 //LabelApplierManager *pLabelApplierMananger,
								 //WrapperManager *pWrapperManager) :	OperatingMode("tPEWeigh",
								 Hobart::CEvolMsgReceiver *pWeigherManager,
								 Hobart::CEvolMsgReceiver *pFirstPrinter, 
								 Hobart::CEvolMsgReceiver *pSecondPrinter,
								 Hobart::CEvolMsgReceiver *pThirdPrinter,
								 Hobart::CEvolMsgReceiver *pLabelApplierMananger,
								 Hobart::CEvolMsgReceiver *pWrapperManager) :	OperatingMode("tPEWeigh",
																				  pCurrentProduct,
																				  pWeigherManager,
																				  pFirstPrinter,
																				  pSecondPrinter,
																				  pThirdPrinter,
																				  pLabelApplierMananger,
																				  pWrapperManager)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	registerEvent(ProductDev, ProductChange);
	registerEvent(ProductDev, ProductChangeSmart);
	registerEvent(ProductDev, ProductUpdate);
	registerEvent(ProductDev, ProductStartPrintAllLabels);
	registerEvent(ProductDev, ProductStopPrintAllLabels);
	registerEvent(ProductDev, ProductDontUpdateProductionPlanning);
	registerEvent(ProductDev, ProductStartPrintForTransaction);
	registerEvent(ProductDev, ProductRemovePreviousTransaction);

	registerEvent(Printer, PrStatus);
	registerEvent(Printer, PrLabelComplete);
	registerEvent(Printer, PrLabelReady);
	registerEvent(Printer, PrStockMetrics);
	registerEvent(Printer, PrConfig);
	registerEvent(Printer, PrCutterStatus);

	stateMachine = pemInitial;
	Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemInitial");

	///sendWrapperAbort();

	// set the CLA mode to Manual
	///setLabelApplierMode(Manual);
	///setWeigherMode(ScaleManualMode);
	///requestWeigherStatus();

	sendPrinterResetImmediate(FirstPrinter);
	sendPrinterResetImmediate(SecondPrinter);
	sendPrinterResetImmediate(ThirdPrinter);

	takeLabelTime = (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgProductionEntryContinuousPrint)) ? 0 : 5;
	setTakeLabelTime(FirstPrinter);
	setTakeLabelTime(SecondPrinter);
	setTakeLabelTime(ThirdPrinter);

	currentPackId = 0;

	receivedCancelCommand = false;
	startedPrintingLabels = false;
	pausedPrintingLabels = false;
	
	waitForInterlock = false;				//SOF-3885
		
	receivedPrStockMetricsAfterError = true;


	///if (pCurrentProduct != NULL)
	///{
	///	ProductChangeMsg pcMsg(this, (const Product *) pCurrentProduct);

	///	char *pCC = (char *) pcMsg.serialize();
	///	handleProductMessage((ProductEvent) pcMsg.getEventType(), pCC);
	///}

	requestLabelSize();
}

ProductionEntryMode::~ProductionEntryMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	
	emptyProductTransactionQ();

	// FT-1092
	//ProductFinishedPrintingAllLabelsMsg finishedMessage( 0, receivedCancelCommand ? false : true );
	//dispatchEvent( &finishedMessage );
	if (receivedCancelCommand)
	{
		receivedCancelCommand = false;
		sendFinishedPrintingMsg(false);
	}

	deregisterAllEvents();
	ShutdownNicely();

	///setLabelApplierMode(Conveyor);
}				

void ProductionEntryMode::weigherAttached(Hobart::CEvolMsgReceiver *pWeigherManager)
{
	_Mybase::weigherAttached( pWeigherManager );

	setWeigherMode(ScaleManualMode);
}

void ProductionEntryMode::firstPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	Debug::trace("ProductionEntryMode", "firstPrinterAttached() entered");
	_Mybase::firstPrinterAttached(pPrinterMgr);

	stateMachine = pemWaitingForProductUpdate;
	Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemWaitingForProductUpdate");
	Debug::trace("ProductionEntryMode", "firstPrinterAttached() finished");
}

void ProductionEntryMode::firstPrinterDetached()
{
	Debug::trace("ProductionEntryMode", "firstPrinterDetached() entered");
	_Mybase::firstPrinterDetached();

	stateMachine = pemInitial;
	Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemInitial");
	Debug::trace("ProductionEntryMode", "firstPrinterDetached() finished");
}

void ProductionEntryMode::secondPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	_Mybase::secondPrinterAttached( pPrinterMgr );

	stateMachine = pemWaitingForProductUpdate;
	Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemWaitingForProductUpdate");
}

void ProductionEntryMode::secondPrinterDetached()
{
	_Mybase::secondPrinterDetached();

	stateMachine = pemInitial;
	Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemInitial");
}

void ProductionEntryMode::thirdPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	_Mybase::thirdPrinterAttached( pPrinterMgr );

	stateMachine = pemWaitingForProductUpdate;
	Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemWaitingForProductUpdate");
}

void ProductionEntryMode::thirdPrinterDetached()
{
	_Mybase::thirdPrinterDetached();

	stateMachine = pemInitial;
	Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemInitial");
}

void ProductionEntryMode::labelApplierAttached(Hobart::CEvolMsgReceiver *pLabelApplierManager)
{
	_Mybase::labelApplierAttached( pLabelApplierManager );

	setLabelApplierMode(Manual);
}

ProductTransaction *ProductionEntryMode::findTransaction(const ProductTransaction *transactionToFind)
{
	ProductTransaction *foundTransaction = 0;

	if (!productTransactionQ.empty())
	{
		std::deque<ProductTransaction *>::iterator pos;
		pos = productTransactionQ.begin();
		while (pos != productTransactionQ.end())
		{
			if (*pos == transactionToFind)
			{
				foundTransaction = *pos;
				break;
			}
			pos++;
		}
	}
	return foundTransaction;
}


void ProductionEntryMode::getNextTransaction()
{
	pCurrentProductTransaction = 0;
	pCurrentProduct = 0;
	currentPackId = 0;

	if (productTransactionQ.empty())
	{
		Debug::trace("ProductionEntryMode", HobString("pem - getNextTransaction - no more transactions to print, sending finished printing message"));
		sendFinishedPrintingMsg();
	}
	else
	{
		// get the first transaction from the list
		pCurrentProductTransaction = productTransactionQ.front();
		productTransactionQ.pop_front();

		Debug::trace("ProductionEntryMode", HobString("pem - getNextTransaction - next transaction: ") + HobString(Hex::toString((long long) pCurrentProductTransaction)));

		// get the transaction's product
		pCurrentProduct = pCurrentProductTransaction->getProduct();
		if (pCurrentProduct)
		{
			ProductTransaction *pCPT = const_cast<ProductTransaction *>(pCurrentProductTransaction);
			Debug::trace("ProductionEntryMode.StateMachine", "Number of labels to print: " + Integer::toString( pCPT->getNumOfLabelsToPrint() ) );
			Debug::trace("ProductionEntryMode.StateMachine", "Number of labels printed: " + Integer::toString( pCPT->getNumOfLabelsPrinted() ) );
			if (pCPT->getNumOfLabelsPrinted() > 0)
			{
				if (pCPT->getNumOfLabelsToPrint() > pCPT->getNumOfLabelsPrinted())
				{
					pCPT->setNumOfLabelsToPrint( pCPT->getNumOfLabelsToPrint() - pCPT->getNumOfLabelsPrinted() );
					Debug::trace("ProductionEntryMode.StateMachine", "Number of labels remaining: " + Integer::toString( pCPT->getNumOfLabelsToPrint() ) );
					pCPT->setNumOfLabelsPrinted( 0 );
				}
			}

			ProductStartPrintForTransactionMsg msg(this, pCurrentProductTransaction);
			dispatchEvent(&msg);
		}
		else
		{
			Debug::trace("ProductionEntryMode", HobString("pem - getNextTransaction - don't have a product for the transaction, dumping this product transaction"));
			emptyProductTransactionQ();
			sendFinishedPrintingMsg(false);
		}
	}
}

void ProductionEntryMode::postImagePrinters()
{
	Debug::trace("ProductionEntryMode", "postImagePrinters() entered");

	if (printerScenario == PrimaryPrinterOnly)
	{
		postImagePrinter(FirstPrinter, pFirstPrinter, currentPackId++);

		stateMachine = pemPrintingPrimary;
		Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemPrintingPrimary");
	}
	else if (printerScenario == SecondPrinterOnly)
	{
		postImagePrinter(SecondPrinter, pSecondPrinter, currentPackId++);

		stateMachine = pemPrintingSecondary;
		Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemPrintingSecondary");
	}
	else if (printerScenario == PrimaryAndSecondPrinter)
	{
		postImagePrinter(FirstPrinter, pFirstPrinter, currentPackId++);
		postImagePrinter(SecondPrinter, pSecondPrinter, currentPackId++);

		stateMachine = pemPrintingPrimaryPrintingSecondary;
		Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemPrintingPrimaryPrintingSecondary");
	}
	else
	{
		Debug::trace("ProductionEntryMode.Error", "Invalid printerScenario");
	}
}


void ProductionEntryMode::handleProductMessage(ProductEvent productEvent, const char *recvBuffer)
{
	switch (productEvent)
	{
		// Operator has chosen a new product
		case ProductChange:
		{
			//stateMachine = pemWaitingForProductUpdate;
			Debug::trace("ProductionEntryMode", HobString("received ProductChange - stateMachine=") + HobString(Integer::toString(stateMachine)));
			break;
		}

		// Operator has chosen a new product
		case ProductChangeSmart:
		{
            ProductChangeMsg prodChange(recvBuffer);
			ProductManager *productManager = (ProductManager *) prodChange.getProductMgr();

			// FT-815 printing expanded text
			if ( productManager )
			{
				Util::CSmartPtr<Product> prod = productManager->getCurrentProductSmartPtr();

				if (( prod ) && ( prod->isDummyProduct() ))
				{
					pCurrentProductSmartPtr = productManager->getCurrentProductSmartPtr();
					Product *pProduct = pCurrentProductSmartPtr.get();

					emptyProductQ();

					if ( pProduct )
						productQ.push( pCurrentProductSmartPtr );
					else
						pCurrentProduct = 0;

					if (stateMachine == pemInitial || stateMachine == pemWaitingForProductUpdate)
					{
						if (getNewProduct() == true)
						{
							changeProduct();
							preImagePrinters();

							stateMachine = pemWaitingForProductUpdate;
							Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemWaitingForProductUpdate");
						}
					}
				}
			}

			break;
		}

		case ProductUpdate:
		{
			ProductUpdateMsg prodUpdate(recvBuffer);
			ProductTransaction *pNextProductTransaction = (ProductTransaction *) prodUpdate.getProductTransaction();
			Product *pNextProduct = (pNextProductTransaction ? pNextProductTransaction->getProduct() : NULL);

			// FT-815 printing expanded text
			if (( pNextProduct ) && ( pNextProduct->isDummyProduct() ))
			{
				Debug::trace("ProductionEntryMode", "handle DUMMY product update");

				if ( stateMachine == pemWaitingForProductUpdate )
				{
					// printing individual labels
					pCurrentProductTransaction = pNextProductTransaction;

					Debug::trace("ProductionEntryMode", "post image DUMMY product...");
					postImagePrinters();

					// get rid of dummy product
					pCurrentProductTransaction = 0;
					pCurrentProductSmartPtr.Release();
					pCurrentProduct = 0;
				}

				// ignore dummy product
				break;
			}

			Debug::trace("ProductionEntryMode", HobString("ProductUpdate received:  transaction=") + HobString(Hex::toString((long long) pNextProductTransaction)));

			// If we received a transaction, add it to the list.
			if ( pNextProductTransaction )
			{
				// Only add it to the list if it's not already in the list.
				if ( !findTransaction( pNextProductTransaction ) )
					productTransactionQ.push_back( pNextProductTransaction );
			}

			Debug::trace("ProductionEntryMode", "ProductUpdate handling finished");
			break;
		}

		case ProductStartPrintAllLabels:
		{
			if (!startedPrintingLabels || pausedPrintingLabels)
			{
				Debug::trace("ProductionEntryMode", "ProductStartPrintAllLabels received");

				// When starting to print the first time, count the total number of labels to be printed (summed up for each transaction in the queue)
				if (!startedPrintingLabels)
				{
					totalLabelsCount = 0;
					currentLabelCount = 0;
					if (!productTransactionQ.empty())
					{
						std::deque<ProductTransaction*>::iterator pos = productTransactionQ.begin();
						while (pos != productTransactionQ.end())
						{
							totalLabelsCount += (*pos)->getNumOfLabelsToPrint();
							pos++;
						}

						LabelPrintingCounts lpc(this, currentLabelCount, totalLabelsCount);
						dispatchEvent(&lpc);
					}
				}

				startedPrintingLabels = true;
				pausedPrintingLabels = false;

				getNextTransaction();
				if (pCurrentProductTransaction && pCurrentProduct)
				{
					stateMachine = pemWaitingForOKToPrintTransaction;
					Debug::trace("ProductionEntryMode.StateMachine", "pem - handling ProductStartPrintAllLabels - stateMachine = pemWaitingForOKToPrintTransaction");
				}
				else
				{
					stateMachine = pemWaitingForProductUpdate;
					Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemWaitingForProductUpdate");
				}

				Debug::trace("ProductionEntryMode", "ProductStartPrintAllLabels handling finished");
			}
			break;
		}

		case ProductStartPrintForTransaction:
		{
			Debug::trace("ProductionEntryMode", "ProductStartPrintForTransaction received");
			ProductStartPrintForTransactionMsg productStartPrint(recvBuffer);

			if (stateMachine == pemWaitingForOKToPrintTransaction)
			{
				if (productStartPrint.isOkToPrintTransaction() && stateMachine == pemWaitingForOKToPrintTransaction)
				{
					stateMachine = pemWaitingForProductUpdate;

					Debug::trace("ProductionEntryMode", HobString("pem - handling ProductStartPrintForTransaction - before calling changeProduct()"));
					changeProduct();

					if (printerScenario != NoPrinters)
					{
						if (pCurrentProductTransaction && pCurrentProduct)
						{
							if ( printerScenario == SecondPrinterOnly && pSecondPrinter )
							{
								if ( SessionManager::getInstance()->isPrinterStreamable(SecondPrinter) )
								{
									PrinterManager *mgr( dynamic_cast<PrinterManager*>( pSecondPrinter ) );

									if ( mgr )
										mgr->setStreamingLabels(true);
								}
							}
							else if ( printerScenario == ThirdPrinterOnly && pThirdPrinter )
							{
								if ( SessionManager::getInstance()->isPrinterStreamable(ThirdPrinter) )
								{
									PrinterManager *mgr( dynamic_cast<PrinterManager*>( pThirdPrinter ) );

									if ( mgr )
										mgr->setStreamingLabels(true);
								}
							}

							Debug::trace("ProductionEntryMode", HobString("handling ProductStartPrintForTransaction - before calling preImagePrinters()"));
							preImagePrinters();	
							Debug::trace("ProductionEntryMode", HobString("handling ProductStartPrintForTransaction - done with calling preImagePrinters()"));

							Debug::trace("ProductionEntryMode", HobString("handling ProductStartPrintForTransaction - got next transaction, calling postImagePrinters to print first label - ") +
												HobString(Integer::toString(pCurrentProductTransaction->getNumOfLabelsPrinted())) + HobString(" of ") + HobString(Integer::toString(pCurrentProductTransaction->getNumOfLabelsToPrint())));
							postImagePrinters();
						}
					}
					else
						Debug::trace("ProductionEntryMode", HobString("handling ProductStartPrintForTransaction - printerScenario is NoPrinters"));
				}
				else
				{
					productFailed();

					getNextTransaction();
					if (pCurrentProductTransaction && pCurrentProduct)
					{
						stateMachine = pemWaitingForOKToPrintTransaction;
						Debug::trace("ProductionEntryMode.StateMachine", "pem - handling PrLabelComplete - stateMachine = pemWaitingForOKToPrintTransaction");
					}
					else
					{
						stateMachine = pemWaitingForProductUpdate;
						Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemWaitingForProductUpdate");
					}
				}
			}
			break;
		}

		case ProductStopPrintAllLabels:
		{
			Debug::trace("ProductionEntryMode", "ProductStopPrintAllLabels received");

			// Set a flag so that after the next time we receive a PrLabelComplete, we will cancel printing
			// the remaining labels for the Production Entry group. 

			if (startedPrintingLabels)
				Debug::trace("ProductionEntryMode", "ProductStopPrintAllLabels - startedPrintingLabels=true");
			if (pausedPrintingLabels)
				Debug::trace("ProductionEntryMode", "ProductStopPrintAllLabels - pausedPrintingLabels=true");


			// If haven't started printing, just empty the list.
			if (!startedPrintingLabels || pausedPrintingLabels)
			{
				emptyProductTransactionQ();
				sendFinishedPrintingMsg(false); 
			}
			else
				receivedCancelCommand = true;

			Debug::trace("ProductionEntryMode", "ProductStopPrintAllLabels handling finished");
			break;
		}

		case ProductDontUpdateProductionPlanning:
		{
			Debug::trace("ProductionEntryMode", "ProductDontUpdateProductionPlanning received");

			if (!productTransactionQ.empty())
			{
				std::deque<ProductTransaction *>::iterator pos;
				pos = productTransactionQ.begin();
				while (pos != productTransactionQ.end())
				{
					const Product * prod = (*pos)->getProduct();
					if (prod && prod->getProductionPlanningType() != Product::PRODUCTIONPLANNING_NO)
						(*pos)->setUpdateProductionPlanning(false);
					pos++;
				}
			}

			Debug::trace("ProductionEntryMode", "ProductDontUpdateProductionPlanning handling finished");
			break;
		}

		case ProductRemovePreviousTransaction:
		{
			Debug::trace("ProductionEntryMode", "ProductRemovePreviousTransaction received");

			if ( productTransactionQ.empty() )
				Debug::trace("ProductionEntryMode", "no transactions exist");
			else
			{
				Debug::trace("ProductionEntryMode", "remove previous transaction...");
				productTransactionQ.pop_back();

				Debug::trace("ProductionEntryMode", HobString("transaction count: ") + Long::toString(productTransactionQ.size()));
			}

			Debug::trace("ProductionEntryMode", "ProductRemovePreviousTransaction handling finished");
			break;
		}

		default:
		{
			break;
		}
	}					
}

void ProductionEntryMode::handlePrinterMessage(PrEvent prEvent, const char *recvBuffer)
{
	switch (prEvent)
	{
		case PrStatus: 
		{						
			Debug::trace("ProductionEntryMode", "PrStatus received");

			PrinterStatus prStatus(recvBuffer);
			PrinterType printerType = prStatus.getPrinterType();

			prStatus.show();

			if (prStatus.isHeadDisengaged() == true)
			{
				Debug::trace("ProductionEntryMode", "PrStatus: isHeadDisengaged is true");

				receivedPrStockMetricsAfterError = false;
				updatePrinterHeadUp(printerType);
					
				if (stateMachine != pemPrinterHeadUp)
				{
					stateMachine = pemPrinterHeadUp;
					Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemPrinterHeadUp");

					if (pCurrentProductTransaction)
						sendProductIncomplete();
				}
				Debug::trace("ProductionEntryMode", "PrStatus: isHeadDisengaged handling finished");
			}

			if (prStatus.isSizeCmdComplete() == true)
			{
				Debug::trace("ProductionEntryMode", "PrStatus: isSizeCmdComplete is true");

				updateSizeCmdComplete(printerType);

				Debug::trace("ProductionEntryMode", "PrStatus: isSizeCmdComplete handling finished");
			}
#if 0		
			//SOF-5366 handle intermittant label taken sensor
			if( prStatus.isPriorityError() &&  prStatus.isHardwareError() ) {
				Debug::trace("ProductionEntryMode",	"PrStatus: isHardwareError()"); 
				//handle this the same way we handle the cutter interlock 
				Debug::trace("ProductionEntryMode.StateMachine", "isHardwareError()!");
				if( pCurrentProductTransaction ) {
										
					HobList hbList;
					HobString s = Integer::toString( pCurrentProductTransaction->getNumOfLabelsToPrint() );
					hbList.push_back( s );		
					Debug::trace("ProductionEntryMode.StateMachine", "Number of labels to print: " + s );

					s = Integer::toString( pCurrentProductTransaction->getNumOfLabelsPrinted() );
					hbList.push_back( s );
					Debug::trace("ProductionEntryMode.StateMachine", "Number of labels printed: " + s );										
					
					s = Integer::toString( pCurrentProductTransaction->getNumOfLabelsToPrint() - pCurrentProductTransaction->getNumOfLabelsPrinted() );
					hbList.push_back( s );
					Debug::trace("ProductionEntryMode.StateMachine", "Number of labels remaining: " + s );					
			
					Session *pSession = SessionManager::getInstance()->getPrimaryConsoleSession();
					if( pSession ) {
						ResponseMessageManager *pResponseMgr = pSession->getResponseMessageManager();
						if( pResponseMgr ) {
							pResponseMgr->addMessage( ErrPrinter, PrinterErrProductionEntrySensor, HfcResponse::RESPONSESTYLE_OK, &hbList );
						} else {
							Debug::trace("ProductionEntryMode.Error", "Failed to get ResponseMessageManager.");	
						}
					} else {
						Debug::trace("ProductionEntryMode.Error", "Failed to get Primary Session.");	
					}
					
				}
				emptyProductTransactionQ();
				sendFinishedPrintingMsg(false);
				stateMachine = pemWaitingForProductUpdate;
				Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemWaitingForProductUpdate");

			}
#endif
			if ((prStatus.isPriorityError() || prStatus.isLabelMissing()) && startedPrintingLabels)
			{
				Debug::trace("ProductionEntryMode", "PrStatus: isPriorityError or isLabelMissing = true - setting pausedPrintingLabels to true");
				if (prStatus.isPriorityError())
					receivedPrStockMetricsAfterError = false;

				if (!pausedPrintingLabels)
				{
					pausedPrintingLabels = true;

					// Send this message to the Session, who will forward it on to the UI, to remove the dialog indicating that the Production Entry
					// labels are being printed.  The operator will either need to pop the cassette and reinsert it, or Press Print again (for a label missing error).
					EvolMessage msg(ProductDev, ProductPausePrintAllLabels);
					dispatchEvent(&msg);

					//PrinterRequestStatus prs(this, prStatus.getPrinterType());
					//dispatchEvent(&prs);
				}

				break;
			}

			if ( pausedPrintingLabels && !isPrinterHeadUp() && receivedPrStockMetricsAfterError )
			{
				Debug::trace("ProductionEntryMode", "pausedPrintingLabels was true so restart printing");
				if (startedPrintingLabels)
				{
					EvolMessage msg(ProductDev, ProductStartPrintAllLabels);
					// Need to send this message to myself, the ProductionEntryMode
					dispatchEvent(&msg, this);
					// Also need to send it to the Session so it can redisplay the "Printing" dialog
					dispatchEvent(&msg);
				}
			}

			break;
		}	
		case PrStockMetrics:
		{
			Debug::trace("ProductionEntryMode", "PrStockMetrics received");

			receivedPrStockMetricsAfterError = true;

			PrinterStockMetrics prMetrics(recvBuffer);

			PrinterRequestStatus prs(this, prMetrics.getPrinterType());
			dispatchEvent(&prs);
			break;
		}
		case PrLabelReady: 
		{						
			Debug::trace("ProductionEntryMode.StateMachine", HobString("pem - handling PrLabelReady - stateMachine upon entry is: ") + HobString(Integer::toString(stateMachine)));
			break;
		}
		case PrLabelComplete: 
		{						
			Debug::trace("ProductionEntryMode.StateMachine", HobString("pem - handling PrLabelComplete - stateMachine upon entry is: ") + HobString(Integer::toString(stateMachine)));

			PrinterLabelComplete prLabelComplete(recvBuffer);
			PrinterType printerType = prLabelComplete.getPrinterType();
			//const ProductTransaction *pPT = prLabelComplete.getProductTransaction();
			bool printOk = prLabelComplete.isPrintOk();

			ProductTransaction *pPT = const_cast<ProductTransaction *>(prLabelComplete.getProductTransaction());
			Product *prod = (pPT ? pPT->getProduct() : NULL);

			// FT-815 printing expanded text
			if (( prod ) && ( prod->isDummyProduct() ))
			{
				Debug::trace("ProductionEntryMode", "sending DUMMY product complete");
				sendProductComplete( pPT, printOk );

				stateMachine = pemWaitingForProductUpdate;
				Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemWaitingForProductUpdate");

				if (getNewProduct() == true)
				{
					Debug::trace("ProductionEntryMode", "changing product after DUMMY print");

					changeProduct();
					preImagePrinters();
				}

				Debug::trace("ProductionEntryMode", "done processing DUMMY label complete");
				break;
			}

			if ( pPT )
			{
				Debug::trace("ProductionEntryMode", HobString("pem - handling PrLabelComplete - transaction=") + HobString(Hex::toString((long long) pPT)) + HobString(" - current transaction=") + HobString(Hex::toString((long long) pCurrentProductTransaction)));
				Debug::trace("ProductionEntryMode", HobString("pem - handling PrLabelComplete - printOk=") + HobString(Bool::toString(printOk)));

				if (printOk)
				{
					LabelPrintingCounts lpc(this, ++currentLabelCount, totalLabelsCount);
					dispatchEvent(&lpc);

					if ( (((stateMachine == pemPrintingPrimary) || (stateMachine == pemWaitingForPrimary)) &&
							(printerType == FirstPrinter)) ||
						 (((stateMachine == pemPrintingSecondary) || (stateMachine == pemWaitingForSecondary)) &&
							(printerType == SecondPrinter)))
					{
						// Don't do this here anymore.  It is incremented by the PrinterManager.
						//Debug::trace("ProductionEntryMode", HobString("pem - handling PrLabelComplete - increment number of labels printed"));
						//pPT->incrementNumOfLabelsPrinted();

						if (!receivedCancelCommand && pPT->getNumOfLabelsPrinted() < pPT->getNumOfLabelsToPrint())
						{
							Debug::trace("ProductionEntryMode", HobString("pem - handling PrLabelComplete - before calling sendPackageComplete(), not all labels printed for current transaction, printing ") +
												HobString(Integer::toString(pPT->getNumOfLabelsPrinted())) + HobString(" of ") + HobString(Integer::toString(pPT->getNumOfLabelsToPrint())));

							sendPackageComplete(pPT);

							if (Configuration::getInstance()->toIntCfgI("productionEntryPrintDelay") > 0)
								PAL_absclass::PAL_sleep(Configuration::getInstance()->toIntCfgI("productionEntryPrintDelay"));

							postImagePrinters();
						}
						else
						{
							Debug::trace("ProductionEntryMode", HobString("pem - handling PrLabelComplete - before calling sendProductComplete() for this transaction"));
							Debug::trace("ProductionEntryMode", HobString("pem - PrLabelComplete - Printed ") +
								HobString(Integer::toString(pPT->getNumOfLabelsPrinted())) + HobString(" of ") + HobString(Integer::toString(pPT->getNumOfLabelsToPrint())));

							sendProductComplete(pPT, ((pPT->getNumOfLabelsPrinted() == pPT->getNumOfLabelsToPrint()) ? true : false));

							if (receivedCancelCommand)
							{
								receivedCancelCommand = false;
								emptyProductTransactionQ();
								sendFinishedPrintingMsg(false);
								stateMachine = pemWaitingForProductUpdate;
								Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemWaitingForProductUpdate");
							}
							else
							{
								getNextTransaction();
								if (pCurrentProductTransaction && pCurrentProduct)
								{
									stateMachine = pemWaitingForOKToPrintTransaction;
									Debug::trace("ProductionEntryMode.StateMachine", "pem - handling PrLabelComplete - stateMachine = pemWaitingForOKToPrintTransaction");
								}
								else
								{
									stateMachine = pemWaitingForProductUpdate;
									Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemWaitingForProductUpdate");
								}
							}
						}
					}
					else if ((stateMachine == pemPrintingPrimaryPrintingSecondary) && (printerType == FirstPrinter))
					{
						stateMachine = pemWaitingForSecondary;
						Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemWaitingForSecondary");
					}
					else if((stateMachine == pemPrintingPrimaryPrintingSecondary) && (printerType == SecondPrinter))
					{
						stateMachine = pemWaitingForPrimary;
						Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemWaitingForPrimary");
					}
				}
				else
				{
					Debug::trace("ProductionEntryMode", HobString("pem - handling PrLabelComplete - printOK is false - before sending product incomplete for this transaction"));

					Debug::trace("ProductionEntryMode", HobString("pem - handling PrLabelComplete - before calling sendProductIncomplete(), not all labels printed, only printed ") +
						HobString(Integer::toString(pPT->getNumOfLabelsPrinted())) + HobString(" of ") + HobString(Integer::toString(pPT->getNumOfLabelsToPrint())));

					sendProductIncomplete();
				}
			}
			else
			{
				stateMachine = pemWaitingForProductUpdate;
				Debug::trace("ProductionEntryMode.StateMachine", "stateMachine = pemWaitingForProductUpdate");
			}
				Debug::trace("ProductionEntryMode.StateMachine", "Number of labels to print: " + Integer::toString( pPT->getNumOfLabelsToPrint() ) );
				Debug::trace("ProductionEntryMode.StateMachine", "Number of labels printed: " + Integer::toString( pPT->getNumOfLabelsPrinted() ) );					
				Debug::trace("ProductionEntryMode.StateMachine", "Number of labels remaining: " + Integer::toString( pPT->getNumOfLabelsToPrint() - pPT->getNumOfLabelsPrinted() ) );

			Debug::trace("ProductionEntryMode.StateMachine", "pem - handling PrLabelComplete complete");
			break;
		}	

		case PrConfig:
			Debug::trace("ProductionEntryMode", "handlePrinterMessage(PrConfig)");
			handlePrinterConfigInOpMode(recvBuffer);
			break;
		case PrCutterStatus: {
			Debug::trace("ProductionEntryMode", "handlePrinterMessage(PrCutterStatus)");
			//SOF-3885 cancel printing if interlock is opened
			PrinterCutterStatus cStatus(recvBuffer);
			if( ( cStatus.getInstalled() ) && ( !cStatus.getInterlock() ) && ( !waitForInterlock ) ){				
				waitForInterlock = true;
				Debug::trace("ProductionEntryMode.StateMachine", "Cutter Interlock opened!");
				if( pCurrentProductTransaction ) {
										
					HobList hbList;
					HobString s = Integer::toString( pCurrentProductTransaction->getNumOfLabelsToPrint() );
					hbList.push_back( s );		
					Debug::trace("ProductionEntryMode.StateMachine", "Number of labels to print: " + s );

					s = Integer::toString( pCurrentProductTransaction->getNumOfLabelsPrinted() );
					hbList.push_back( s );
					Debug::trace("ProductionEntryMode.StateMachine", "Number of labels printed: " + s );										
					
					s = Integer::toString( pCurrentProductTransaction->getNumOfLabelsToPrint() - pCurrentProductTransaction->getNumOfLabelsPrinted() );
					hbList.push_back( s );
					Debug::trace("ProductionEntryMode.StateMachine", "Number of labels remaining: " + s );					
			
					Session *pSession = SessionManager::getInstance()->getPrimaryConsoleSession();
					if( pSession ) {
						ResponseMessageManager *pResponseMgr = pSession->getResponseMessageManager();
						if( pResponseMgr ) {
							pResponseMgr->addMessage( ErrPrinter, PrinterErrProductionEntryInterlockOpen, HfcResponse::RESPONSESTYLE_OK, &hbList );
						} else {
							Debug::trace("ProductionEntryMode.Error", "Failed to get ResponseMessageManager.");	
						}
					} else {
						Debug::trace("ProductionEntryMode.Error", "Failed to get Primary Session.");	
					}
					
				}
				// labels are being printed.  Cancel all remaining labels to print.
				EvolMessage msg(ProductDev, ProductStopPrintAllLabels);
				dispatchEvent(&msg);
			} else {
				if( cStatus.getInstalled() && cStatus.getInterlock() && waitForInterlock ) {
					Debug::trace("ProductionEntryMode.StateMachine", "Cutter Interlock closed!");
					waitForInterlock = false;
				}
			}
			break;
		}		
		default:
		{
			OperatingMode::handlePrinterMessage(prEvent, recvBuffer);
			break;
		}
	}
}

void ProductionEntryMode::productFailed()
{
	Debug::trace("ProductionEntryMode", "entering productFailed()");
	sendProductComplete(pCurrentProductTransaction, false);
	pCurrentProductTransaction = NULL;

	unlockWeigher();
}


void ProductionEntryMode::sendProductIncomplete()
{
	Debug::trace("ProductionEntryMode", "sendProductIncomplete() started");

	if (pCurrentProductTransaction)
	{
		if (pCurrentProductTransaction->getNumOfLabelsPrinted() == pCurrentProductTransaction->getNumOfLabelsToPrint())
		{
			// complete
			Debug::trace("ProductionEntryMode", HobString("handle sendProductIncomplete gand calling sendProductComplete(): ") + HobString(Hex::toString(pCurrentProductTransaction->getTransactionNum())));
			sendProductComplete(pCurrentProductTransaction, true);
		}
		else
		{
			// incomplete
			if (pCurrentProductTransaction->getNumOfLabelsPrinted() > 0)
			{
				ProductCompleteMsg prodCompMessage(this, pCurrentProductTransaction->getTransactionNum(), pCurrentProductTransaction, false);
				prodCompMessage.setEventType(ProductIncomplete);
				dispatchEvent(&prodCompMessage);

				Debug::trace("ProductionEntryMode", HobString("sendProductIncomplete: ") + HobString(Hex::toString(pCurrentProductTransaction->getTransactionNum())));
			}
			else
			{
				Debug::trace("ProductionEntryMode", "handling sendProductIncomplete() incomplete - getNumOfLabelsPrinted() == 0");
				// This will be used and ONLY used if we are in PrintRemoteLabels mode and get a label status error (ex: outOfLabels)
				ProductCompleteMsg prodCompMessage(this, pCurrentProductTransaction->getTransactionNum(), pCurrentProductTransaction, false);
				prodCompMessage.setEventType(ProductIncomplete);
				dispatchEvent(&prodCompMessage);
			}

			productTransactionQ.push_front(const_cast<ProductTransaction *>(pCurrentProductTransaction));
		}
		pCurrentProductTransaction = 0;
	}
	unlockWeigher();

	Debug::trace("ProductionEntryMode", "sendProductIncomplete() finished");
}


void ProductionEntryMode::sendPackageComplete(const ProductTransaction *pCPT)
{
	Debug::trace("ProductionEntryMode", "sendPackageComplete() started");

	if (pCPT != NULL)
	{
		ProductPackageCompleteMsg packageCompMessage(this, pCPT->getTransactionNum(), pCPT);
		dispatchEvent(&packageCompMessage);

		Debug::trace("ProductionEntryMode", "sendPackageComplete() completed package successfully; sent PackageComplete for label " +
			util_string::inttostring(pCPT->getNumOfLabelsPrinted()) + " of " + util_string::inttostring(pCPT->getNumOfLabelsToPrint()));
	}

	Debug::trace("ProductionEntryMode", "sendPackageComplete() finished");
}


void ProductionEntryMode::emptyProductTransactionQ()
{
	Debug::trace("ProductionEntryMode", "emptyProductTransactionQ() started");

	if (!productTransactionQ.empty())
		Debug::trace("ProductionEntryMode", HobString("pem - emptyProductTransactionQ - queue was not empty"));

	ProductTransaction *foundTransaction = 0;

	while (!productTransactionQ.empty())
	{
		foundTransaction = productTransactionQ.front();
		productTransactionQ.pop_front();

		Debug::trace("ProductionEntryMode", HobString("pem - emptyProductTransactionQ - transaction removed: ") + HobString(Hex::toString((long long) foundTransaction)));
		sendProductComplete(foundTransaction, false);
	}

	Debug::trace("ProductionEntryMode", "emptyProductTransactionQ() finished");
}

void ProductionEntryMode::sendFinishedPrintingMsg(bool printOK)
{
	Debug::trace("ProductionEntryMode", "sendFinishedPrintingMsg() started");

	ProductFinishedPrintingAllLabelsMsg finishedMessage(0, printOK);
	dispatchEvent(&finishedMessage);

	LabelPrintingCounts lpc(this, totalLabelsCount + 1, totalLabelsCount);
	dispatchEvent(&lpc);

	pCurrentProductTransaction = 0;
	pCurrentProduct = 0;

	startedPrintingLabels = false;
	pausedPrintingLabels = false;

	unlockWeigher();
	Debug::trace("ProductionEntryMode", "sendFinishedPrintingMsg() finished");
}
