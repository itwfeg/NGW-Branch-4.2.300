/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */
#include "ProductManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "Session.h"
#include "TransactionTypeManager.h"
#include "ProductDefaultManager.h"
#include "WeigherEvent.h"
#include "WeighModeTotalType.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "Product.h"
#include "OnlineOrderProduct.h"
#include "ProductDefault.h"
#include "ProductTransaction.h"
#include "ProductTransactionMaster.h"
#include "Nutrifacts.h"
#include "HfcProductChangeMsg.h"
#include "ProductChangeMsg.h"
#include "HfcProductUpdateValuesMsg.h"
#include "HfcProductSupportDataMsg.h"
#include "HfcProductLabelFormSizeMsg.h"
#include "HfcProductUpdateMsg.h"
#include "HfcProductCompleteMsg.h"
#include "HfcProductPackageCompleteMsg.h"
#include "Configuration.h"
#include "HfcErrorClass.h"
#include "HfcErrorIndexNum.h"
#include "HfcErrorGraphic.h"
#include "ClassEvent.h"
#include "ProductTree.h"
#include "HfcProductDescLookup.h"
#include "LabelApplierEvent.h"
#include "TransactionType.h"

#include "HobAllAccumulatedItems.h"

#include "WriteSingleField.h"

#include "HfcWeigherStatus.h"

#include "HfcResponse.h"
#include "ResponseMessageManager.h"

#include "CategoryManager.h"
#include "ClassManager.h"
#include "NutrifactsManager.h"
#include "ExpirationLabelManager.h"
#include "ProductTextManager.h"
#include "SpecialsManager.h"
//#include "COOLManager.h"
#include "TransactionManager.h"
#include "ProductTextCfg.h"
#include "Device.h"
#include "DeviceEvent.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBProductsRM.h"
#include "DBProductDefaultsRM.h"
#include "DBLabelsRM.h"
#include "DBTransactionType.h"

#include "DBROMLabelsRM.h"
#include "DBTransactionTypesRM.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#include <algorithm>
#include <vector>

#include "Category.h"
#include "GenericHfcObject.h"
#include "SoftkeyDevEvent.h"
#include "Softkey.h"
#include "SoftkeyBasics.h"
#include "SoftKeyLists.h"
#include "SoftkeyGroup.h"
#include "ReadSoftKeyParams.h"
#include "messages/VoidTransactionsMessage.h"

#include "DBQuery.h"
#include "DBProductStatisticsRM.h"
#include "DBProductStatistic.h"
#include "DBCategoriesRM.h"
#include "DBCategory.h"
#include "DBSpecialProgramsRM.h"
#include "DBSpecialProgram.h"
#include "DBClassesRM.h"


#include "ProductStatistic.h"
#include "PresentationMethod.h"

#include "HfcPrinterStatus.h"
#include "HfcPrinterStockMetrics.h"
#include "OperatorManager.h"
#include "GraphicFileNames.h"

#include "ServiceSpeaker.h"

#include "NumericGeneric.h"
#include "NumericField.h"
#include "NumericAnnunciator.h"

#include "MarqueeGeneric.h"
#include "MarqueeMode.h"
#include "MarqueeUpdate.h"

#include "LabelApplierPositionInfo.h"
#include "LabelApplier.h"

#include "time/datetime.h"

#include "WrapperEvent.h"

#include "LabelTypeManager.h"

#include "ProductStatusMsg.h"

#include "HFCManagerContainer.h"

#include "CMessageCenter.h"

#include "cgi/PerformCGICallMessage.h"
#include "cgi/CGICallPerformedMessage.h"
#include "cgi/http/HTTPURI.h"

#include "HfcProductStartPrintForTransactionMsg.h"
#include "ProductRemoveAccumItemMsg.h"
#include "ProductReadAllAccumulatedItemsMsg.h"
#include "ProductFinishedPrintingAllLabelsMsg.h"
#include "ProductTransactionMaster.h"

#include "HfcProductLookupMsg.h"

#include "GCDSession.h"

#include "CMFile.h"
#include "CMReaderWriterEvolTag.h"

#include "CutTestManager.h"
#include "ProductTotalManager.h"
#include "GenericHfcObject.h"
#include "TimerManager.h"

#include "PrinterRequestStatus.h"
#include "PythonScripter.h"
#include "OperationModeStatus.h"

#include "SystemPrinters.h"

#include "HobTokenizerCSV.h"
#include "bootParameters.h"

#include "CuttingTest.h"

#include "RemotePrintRequest.h"
#include "RemotePrintTemplateRequest.h"

#include "DiagnosticsValueMax.h"

#include "FpmManager.h"

#include "PrinterPreImage.h"
#include "PrinterPostImage.h"

#include <HobartCustomerHook.h>

using namespace std;


#define DUMMY_PRODUCTNUM -42
#define REMOTEPRINTTEMPLATE_PRODUCTNUM -28

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int ProductManager::_dataType_ = 0;
	string ProductManager::_dataTypeName_("ProductManager"); 
#endif


ProductManager::ProductManager()
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	init();
}

ProductManager::ProductManager(hfc::HFCManagerContainer *container)
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	init();
	dbAccess = container;
	mgrContainer_ = container;
}

ProductManager::~ProductManager()
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif

	//if (currentProduct)
	//	delete currentProduct;

	if (currentProductSmartPtr.notNull())
	{
		// The old currentProduct pointer is now wrapped into a smart ptr,
		// so delete the smart ptr.  We still have the old currentProduct pointer for easy access,
		// but we don't delete it anymore, just set it to null.
		currentProductSmartPtr = 0;
		currentProduct = 0;
	}

	destroyQueriesAndIters();

#if 0
	if (currentGridData)
		delete currentGridData;
#endif

	if (currentPresentationMethod)
	{
		delete currentPresentationMethod;
		currentPresentationMethod = NULL;
	}
}


void ProductManager::init()
{
	mySession = 0;

	currentProduct		= 0;
	dummyProduct 		= 0; // FT-873

	currentProductCanAcceptWeight = true;

	allProductsQuery = 0;
	allProductsIter  = 0;

//	currentGridData = (SoftKeyLists *)NULL;

	currentPresentationMethod = 0;
	currentPresentationMethodDataType = PresentationMethod::Type_Unknown;

	allProductStatisticsQuery = 0;
	allProductStatisticsIter = 0;

	waitingForStableWeightToPrint = false;
	waitingForLoginBeforeAddToAccum = false; // FT-809
	waitingForLoginBeforeAddToProductionEntry = false;
	productWaitingForProductionEntryLogin = 0;
	waitingForLoginBeforePrinting = false;
	waitingForLoginBeforeWriteSingleField = false;
	productUpdateWaitingForLogin = 0;

	productionEntryContainsFPPItems = false;

	processingKeyboardNetWt = false;

	accumulateItemsAdded = false;
	productionEntryItemsAdded = false;

	resubmitPrintJob = false;

	mainLabelTypeNum.insert(mainLabelTypeNum.begin(), MaxPrinterTypes, 0);
	altLabelTypeNum.insert(altLabelTypeNum.begin(), MaxPrinterTypes, 0);
//	altFamilyLabelTypeNum.insert(altFamilyLabelTypeNum.begin(), MaxPrinterTypes, 0);
//	activeLabelTypeNum.insert(activeLabelTypeNum.begin(), MaxPrinterTypes, 0);

	bamMaxSides = 0;
	bamSidesLeft = 0;
	bamAllSidesFree = false;
	bamUnlimitedSides = false;

	pick5ValueOutOfRange = false;

	sweetheartWeightTransaction = 0;

	//SOF-5092 Disable secondary label Flashkey
	secLabelTypeEnable	= true;
	origSecLabelTypeNum = 0;
}


DBSession *ProductManager::getMySession()
{
	return mySession;
}
void ProductManager::setMySession(DBSession *session)
{
	mySession = session;
}


void ProductManager::handleMessage(SessionMessage *msg)
{
	// Valid as long as no process calls this method with a message containing
	// some device other than ProductDev.
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	if (Debug::isTracing("ProductManager"))
	{
		HobString msgDebug(" handleMessage():  HfcObject = ");
		msgDebug.append(ptrtostring(msg->getHfcObject())).append("   device = ").append(Integer::toString(msg->getDeviceType()));
		msgDebug.append("   event = ").append(Integer::toString(event));
		Debug::trace("ProductManager", msgDebug);
	}
	
	switch (msg->getDeviceType())
	{
		case SessionDev:
		{
			Debug::trace("ProductManager", "debugRemotePrint - handling ProductManager case SessionDev");

			switch (event)
			{
				case SnPrintKeyPressed:
				{
					bool fppTestPrint = false;

					GenericHfcObject* gho = dynamic_cast<GenericHfcObject*>(msg->getHfcObject());
					if (gho)
					{
						std::string value(gho->getData());

						if (value.size())
							fppTestPrint = (util_string::stringtoint(value) == 2);
					}

					handleProductPrintKey1(fppTestPrint);

					break;
				}

				case SnBackendRemotePrint:
				{
					bool isTracing = Debug::isTracing("ProductManager.remotePrint");
					if (isTracing)
						Debug::trace("ProductManager.remotePrint", "ProductManager handling SessionDev | SnBackendRemotePrint (100,160)");

					GenericHfcObject* gho = dynamic_cast<GenericHfcObject*>(msg->getHfcObject());
					if (!gho)
						return;

					if (isTracing)
						Debug::trace("ProductManager.remotePrint", string("SnBackendRemotePrint - received:  ") + gho->getData());
					HobString rprData = gho->getData();
					RemotePrintRequest *rpr = new RemotePrintRequest(rprData.c_str(), (int) rprData.length(), BarFormat);
					if (!rpr)
						return;
					
					rpr->show();
					currentProductCanAcceptWeight = true;					// clear potential issues,reset fresh, and let it get tested

					int returnVal = handleRemotePrintProduct(rpr);	// START the remote print process - Lets get the remote printing party started
					if (isTracing)
						Debug::trace("ProductManager.remotePrint", string("handleRemotePrintProduct() returned ") + Integer::toString(returnVal));

					// We don't need this anymore, so delete it.
					if (rpr)
					{
						delete rpr;
						rpr = 0;
					}

					if (returnVal > RemotePrintSuccessfulPrint)
					{
						//	If the print method could not be initiated fall into here; otherwise, perform this action in productComplete. 
						Session *s = dynamic_cast<Session *> (mySession);
						if (s)
							s->remotePrintReturnMessage(returnVal, 1);				// |done msg, Send message back to the remote device who invoked this call.
					}

					break;
				}

				case SnBackendRemotePrintTemplateLabel:
				{
					bool isTracing = Debug::isTracing("ProductManager.remotePrint");
					if (isTracing)
						Debug::trace("ProductManager.remotePrint", "ProductManager handling SessionDev | SnBackendRemotePrintTemplateLabel (100,162)");

					GenericHfcObject* gho = dynamic_cast<GenericHfcObject*>(msg->getHfcObject());
					if (!gho)
						return;

					if (isTracing)
						Debug::trace("ProductManager.remotePrint", string("SnBackendRemotePrintTemplateLabel - received:  ") + gho->getData());
					HobString rptrData = gho->getData();
					// Note:  We won't delete this object - ownership is being passed to the handleProductRemotePrintTemplate(...) method
					RemotePrintTemplateRequest *rptr = new RemotePrintTemplateRequest(rptrData.c_str(), (int) rptrData.length(), BarFormat);
					if (!rptr)
						return;

					rptr->show();

					int returnVal = handleRemotePrintTemplate(rptr);	// START the remote print process - Lets get the remote printing party started
					if (isTracing)
						Debug::trace("ProductManager.remotePrint", string("handleRemotePrintTemplate() returned ") + Integer::toString(returnVal));

					if (returnVal > RemotePrintSuccessfulPrint)
					{
						//	If the print method could not be initiated fall into here; otherwise, perform this action in productComplete. 
						Session *s = dynamic_cast<Session *> (mySession);
						if (s)
							s->remotePrintReturnMessage(returnVal, 1);				// |done msg, Send message back to the remote device who invoked this call.
					}

					break;
				}

				case SnSelfServReimageLabels:
				{
					Debug::trace("ProductManager.TestScreenLabel", "handleMessage:: handling SessionDev | SnSelfServReimageLabels");


					currentProduct->setPrintLabelToPrinter(true);			// in product.h

					LabelType *currentProdPrintToScreenLT = currentProduct->getLabelType(FirstPrinter);

					ProductTransaction *myTrans = currentProduct->getCurrentTransaction();
					long transProdPrintToScreenLT = myTrans->getLabel1();

					if (currentProdPrintToScreenLT && (currentProdPrintToScreenLT != NULL))
						Debug::trace("ProductManager.TestScreenLabel", "SnSelfServReimageLabels() labelTypeNum: " + HobString(Long::toString(currentProdPrintToScreenLT->getLabelTypeNum())));
					else
						Debug::trace("ProductManager.TestScreenLabel", "SnSelfServReimageLabels() NO LABEL TYPE to report. LabelType is NULL");

					// BigY This did not work on its own.  I thought it would
					//Session *s = dynamic_cast<Session *> (mySession);
					//resetUpLabelTypes(s->getCurrentMode());

					HobRecordNum ltNum1 = currentProduct->getLabelTypeNum(Product::fieldOffsetDefaultLabelTypeNum1);
					HobRecordNum ltNum2 = currentProduct->getLabelTypeNum(Product::fieldOffsetDefaultLabelTypeNum2);
					HobRecordNum ltNum3 = currentProduct->getLabelTypeNum(Product::fieldOffsetDefaultLabelTypeNum3);

					LabelType *printToScreenLT1 = LabelType::findRecord(ltNum1, currentProduct->getDeptNum(), mySession);
					LabelType *printToScreenLT2 = LabelType::findRecord(ltNum2, currentProduct->getDeptNum(), mySession);
					LabelType *printToScreenLT3 = LabelType::findRecord(ltNum3, currentProduct->getDeptNum(), mySession);

					LabelType *lt1 = currentProduct->getLabelType(FirstPrinter);			// debug check
					LabelType *lt2 = currentProduct->getLabelType(SecondPrinter);
					LabelType *lt3 = currentProduct->getLabelType(ThirdPrinter);

					//lt1->setLabelMediaError(LabelOk);
					currentProduct->setLabelType(FirstPrinter, printToScreenLT1);
					currentProduct->setLabelType(SecondPrinter, printToScreenLT2);
					currentProduct->setLabelType(ThirdPrinter, printToScreenLT3);

					bool labelTypeError = setUpLabelTypes();

					// Send a Product Change message to the RTC so it knows the Imager needs to image the new label instead of the onscreen label in self service
					SessionMessage pluChg(ProductDev, ProductChange);
					pluChg.setFormatType(InternalFormat);
					HfcProductChangeMsg hpcm(mySession, currentProduct);
					pluChg.setHfcObject(&hpcm);
					mySession->send(&pluChg, Messenger::DEST_RTC);

					SessionMessage pluChg2(ProductDev, ProductChangeSmart);
					pluChg2.setFormatType(InternalFormat);
					HfcProductChangeMsg hpcm2(mySession, this);
					pluChg2.setHfcObject(&hpcm2);
					mySession->send(&pluChg2, Messenger::DEST_RTC);
				}
			}
			break;
		}
		case Error:
			if ( event == ErrRemoveAnyModalDialog )
			{
				HfcResponse *response = dynamic_cast<HfcResponse *> (msg->getHfcObject());
				if (!response)
					return;

				long code = response->getCode();
				short answer = response->getAnswer();
				Session *s = dynamic_cast<Session *> (mySession);
				if ( code == ProductErr && mySession )
				{
					//SessionMessage sMsg(Error, ErrRemoveAnyModalDialog);
					//sMsg.setFormatType(BrowserFormat);
					//HfcResponse m(response->getCode(), response->getStyle());
					//sMsg.setHfcObject(&m);
					//mySession->send(&sMsg, Messenger::DEST_UI);
					if (s && s->getCurrentMode() == OpModeSupervisor)
					{
						msg->setFormatType(BrowserFormat);
						mySession->send(msg, Messenger::DEST_UI);
					}
				}

				if ( code == WeigherErrScaleNotZero )
				{
					if (answer == HfcResponse::RESPONSEANSWER_CANCEL)  // Ignore was pressed
					{
                        currentProductCanAcceptWeight = true;
                        if (currentProduct && currentProduct->getCurrentTransaction())
						{
							currentProduct->getCurrentTransaction()->setWeigherStatus((HfcWeigherStatus *)0);
						}
						updateWeightAndNotify((HfcWeigherStatus *) NULL, true);
						if (mySession)
						{
							Session * s = dynamic_cast<Session *>(mySession);
							if (s)
								s->requestAWeigherStatus();
                        }
					}
					else if (mySession) // answer is Rezero was pressed
					{
						SessionMessage rezero(Weigher, WgRezero);
						s->handleMessage(&rezero);
					}
				}

				else if ((code == ProductErrScaleNotPresent || code == ProductErr) && mySession)
				{
					if ( currentProduct && !currentProduct->getItemIsMealSideItem() )
					{
						currentProductCanAcceptWeight = true;

						SessionMessage pluData(ProductDev, ProductWrite);
						pluData.setFormatType(BrowserFormat);
						pluData.setHfcObject(currentProduct);

						// Send the product to the UI (Browser)
						mySession->send(&pluData, Messenger::DEST_UI);
					}
//					else
//						clearCurrentProduct(true);
				}

				else if ( (code == ProductErrFPPSinglePLUUpdate || code == ProductErrFPPSinglePLUUpdateMultipleLabels) &&
				          currentProduct && currentProduct->getCurrentTransaction() )
				{
					switch ( currentProduct->getProductionPlanningType() )
					{
						case Product::PRODUCTIONPLANNING_ITEM:
						{
							switch (answer)
							{
								case HfcResponse::RESPONSEANSWER_OK:	   // ProductionPlanning: Item, Mixed = Update and Print; Ingredient = Update Only
								case HfcResponse::RESPONSEANSWER_THREE:	// ProductionPlanning: Item = RePrint; Mixed = Update Only
								{
									if ( answer == HfcResponse::RESPONSEANSWER_THREE )
										currentProduct->getCurrentTransaction()->setUpdateProductionPlanning(false);

									handleProductPrintKey3();

									break;
								}
								case HfcResponse::RESPONSEANSWER_CANCEL:
								default:
								{
									handleCancelPrinting();

									break;
								}
							}

							return;
						}
						case Product::PRODUCTIONPLANNING_INGREDIENT:
						{
							switch (answer)
							{
								case HfcResponse::RESPONSEANSWER_OK:	   // ProductionPlanning: Item, Mixed = Update and Print; Ingredient = Update Only
								{
									currentProduct->getCurrentTransaction()->setPrintLabels(false);

									handleProductPrintKey3();

									break;
								}
								case HfcResponse::RESPONSEANSWER_CANCEL:
								default:
								{
									handleCancelPrinting();

									break;
								}
							}

							return;
						}
						case Product::PRODUCTIONPLANNING_MIXED:
						{
							if ( answer == HfcResponse::RESPONSEANSWER_OK )
							{// ProductionPlanning: Item, Mixed = Update and Print; Ingredient = Update Only
								// do nothing, the default is print and collect totals
								currentProduct->getCurrentTransaction()->setPrintLabels(true);
							}
							else if ( answer == HfcResponse::RESPONSEANSWER_THREE )
							{// ProductionPlanning: Item = RePrint; Mixed = Update Only
								currentProduct->getCurrentTransaction()->setPrintLabels(false);
							}
							else if ( answer == HfcResponse::RESPONSEANSWER_FOUR )
							{// ProductionPlanning: Mixed = Test Print
								currentProduct->getCurrentTransaction()->setPrintLabels(true);
								currentProduct->getCurrentTransaction()->setUpdateProductionPlanning(false);
							}
							else
							{
								handleCancelPrinting();
								break;
							}

							handleProductPrintKey3();
							break;
						}
					#if defined(LINUX_FIX)
						default:
							// Nothing (handle the other cases)
							break;
					#endif
					}
				}

				else if ( code == ProductErrFPPMultiplePLUUpdate )
				{
					if ( answer == HfcResponse::RESPONSEANSWER_THREE )
					{
						//currentProduct->getCurrentTransaction()->setUpdateProductionPlanning(false);
						SessionMessage noUpdateMsg(ProductDev, ProductDontUpdateProductionPlanning);
						noUpdateMsg.setFormatType(InternalFormat);
						mySession->send(&noUpdateMsg, Messenger::DEST_RTC);
					}

					if ( answer == HfcResponse::RESPONSEANSWER_OK || answer == HfcResponse::RESPONSEANSWER_THREE )
						handleProductPrintKey3(); 

					else if ( answer == HfcResponse::RESPONSEANSWER_CANCEL )
					{
						Session *s = dynamic_cast<Session *> (mySession);
						if (s && (s->getCurrentMode() == OpModeProductionEntry || s->getCurrentMode() == OpModePrintMultipleLabels))
						{
							SessionMessage msg(ProductDev, ProductStopPrintAllLabels);
							msg.setFormatType(BrowserFormat);
							handleMessage(&msg);
						}
						else
							handleCancelPrinting();
					}
				}

				else if ( code == ProductErrNoExistingExpirationRecordForPrintExisting )
				{
					if ( answer == HfcResponse::RESPONSEANSWER_OK )
					{
						// The Operator wants to treat this as a new expiration label, instead of an existing one.
						Debug::trace("PrintExpirationLabel", HobString("response to ProductErrNoExistingExpirationRecordForPrintExisting - treat as new"));
						SessionMessage msg(ProductDev, ProductPrintNewExpirationLabel);
						msg.setFormatType(BrowserFormat);
						handleMessage(&msg);
					}
					else  // answer == HfcResponse::RESPONSEANSWER_CANCEL
					{
						// The Operator chose not to treat this as a new expiration label, so just cancel out.
						Debug::trace("PrintExpirationLabel", HobString("response to ProductErrNoExistingExpirationRecordForPrintExisting - cancel out"));
					}
				}

				else if ( code == ProductErrExistingExpirationRecordIsExpired )
				{
					if ( answer == HfcResponse::RESPONSEANSWER_OK )
					{
						// The Operator wants to print this expired expiration label.
						Debug::trace("PrintExpirationLabel", HobString("response to ProductErrExistingExpirationRecordIsExpired - print anyway"));
						mgrContainer_->getExpirationLabelManager()->handleProductPrintExistingExpirationLabel(currentProduct);
					}
					else  // answer == HfcResponse::RESPONSEANSWER_CANCEL
					{
						// The Operator chose not to print the expiration label for the expired product, so just cancel out.
						Debug::trace("PrintExpirationLabel", HobString("response to ProductErrExistingExpirationRecordIsExpired - cancel out"));
					}
				}

				else if ( code == ProductErrNewExpirationRecordAlreadyExists )
				{
					if ( answer == HfcResponse::RESPONSEANSWER_OK )
					{
						// The Operator wants to overwrite the existing expiration label record with a new one.
						Debug::trace("PrintExpirationLabel", HobString("response to ProductErrNewExpirationRecordAlreadyExists - print anyway"));
						mgrContainer_->getExpirationLabelManager()->handlePromptForExpirationDays();
					}
					else  // answer == HfcResponse::RESPONSEANSWER_CANCEL
					{
						// The Operator chose not to overwrite the currently existing expiration label record, so just cancel out.
						Debug::trace("PrintExpirationLabel", HobString("response to ProductErrNewExpirationRecordAlreadyExists - cancel out"));
					}
				}

				else if ( code == ProductErrAccumulateItemsRemoveAllAreYouSure )
				{
					if ( answer == HfcResponse::RESPONSEANSWER_OK )
					{
						// The Operator wants to remove all existing items in the Accumulated Items list.
						Session *s = dynamic_cast<Session *> (mySession);
						if (s)
						{
							SessionMessage msg(ProductDev, ProductRemoveAllAccumulatedItems);
							msg.setFormatType(BrowserFormat);
							handleMessage(&msg);
						}
					}
				}

				else if (code == ProductErrBelowMinWeight || code == ProductErrAboveMaxWeight || code == ProductErrBelowMinTotalPrice || code == ProductErrAboveMaxTotalPrice)
				{
					Debug::trace("Pick5", string("dialog response code: ") + inttostring((int)code) + "  answer: " + inttostring((int)answer));
					if ( answer == HfcResponse::RESPONSEANSWER_CANCEL )
						clearPick5ValueOutOfRange();
				}

				return;
			}
			break;

		case ProductDev:
            // TODO jhc testing only - DELETE when done
			//Debug::trace("ProductManager.remotePrint", "In ProductManager handleMessage() case ProductDev and EVT = " + HobString(Integer::toString(event)));
			switch (event)
			{
				case ProductRead:
				case ProductReadOnlineOrderingProduct:
				{
					Product *msgProduct = dynamic_cast<Product *> (msg->getHfcObject());
					handleProductReadMessage(msgProduct);
					break;
				}
				case ProductWrite:
				{
					Product *msgProduct = dynamic_cast<Product *> (msg->getHfcObject());
					handleProductWriteMessage(msgProduct);
					break;
				}

				case ProductDelete:
				case ProductBatchApplyDelete:
					handleDelete(msg);
					break;

				case ProductDeleteAll:
					handleDeleteAll();
					break;

				case ProductDeleteAllCommodityClass:
					handleDeleteAllCommodityClass();
					break;

				case ProductSave:
				{
					Product *msgProduct = dynamic_cast<Product *> (msg->getHfcObject());
					handleUpdateAndSave(msgProduct);
					break;
				}

				case ProductRestore:
				{
					Product *msgProduct = dynamic_cast<Product *> (msg->getHfcObject());
					if (msgProduct)
					{
						if (Configuration::getInstance()->isDontChangeCurrentCOOLViaComm())
						{
							if (mySession && mySession->isCommSession())
							{
								Debug::trace("Product", "ProductRestore: is Comm Session so don't accept the Current (MRU) COOL.");
								msgProduct->setReceived(Product::fieldOffsetProdCoolTextNum, false);
							}					
						}
						handleProductRestore(msg);
					}
					break;
				}

				case ProductCreate:
				{
					Product *msgProduct = dynamic_cast<Product *> (msg->getHfcObject());
					handleProductCreateMessage(msgProduct);
					break;
				}

				case ProductWriteSingleField:
				{
					handleProductWriteSingleFieldMessage(msg);
					break;
				}

				case ProductReadCurrentTotalValues:
					handleProductReadCurrentTotalValuesMessage();
					break;

				case ProductWriteCurrentTotalValues:
					handleProductWriteCurrentTotalValuesMessage(msg);
					break;

				case ProductSaveSupervisorTransaction:
					handleUpdateSupervisorTransaction();
					break;

				case ProductPrint:
					/* This is received from the Browser when it wants to print a label for the currently
					 * selected product.  This is only valid in Manual Weigh Mode.
					 */
					handleProductPrintKey2();
					break;

				case ProductComplete:
				case ProductIncomplete:
				{
					/* This is received from the RTC when it is completely done printing the label, and
					 * it has been taken, for a package.  It tells us the totals need updating for the
					 * transaction indicated within this message.
					 */

					HfcProductCompleteMsg *productCompleteMsg( dynamic_cast<HfcProductCompleteMsg*>( msg->getHfcObject() ) );
					if ( productCompleteMsg )
					{
						ProductTransaction *trans( productCompleteMsg->getProductTransaction() );

						UINT8 ev = productCompleteMsg->getEventType();
						Debug::trace("ProductManager", string("ProductManager handling: ") + ((ev == ProductComplete) ? "HandleProductComplete" : "HandleProductIncomplete"));

						if ( trans )
						{
							bool templateLabel = false;

							if ( trans->getProduct()->getRemotePrintTemplateRequest() )
								templateLabel = true;

							if (( trans->getProduct() ) && ( trans->getProduct()->isPrintingRemoteLabel() ))
								handleRemoteProductComplete( productCompleteMsg );
							else
								handleProductComplete(productCompleteMsg);

#if defined( __linux__ )
							if ( Configuration::getInstance()->getCustomerHooksEnabled() )
							{
								const int labelsToPrint( trans->getNumOfLabelsToPrint() );
								const int labelsPrinted( trans->getNumOfLabelsPrinted() );

								Debug::trace("ProductManager", "ProductComplete invoking hook... " +
									Long::toString( productCompleteMsg->isPrintOk() ) + " " +
									Long::toString( labelsToPrint ) + " " +
									Long::toString( labelsPrinted ) + " " +
									Long::toString( templateLabel ) );

								hobart_cust_hook_after_print( productCompleteMsg->isPrintOk(), labelsToPrint, labelsPrinted, templateLabel );
							}
#endif
						}
					}

					break;
				}
 
				case ProductPackageComplete:
				{
					Debug::trace("ProductManager", "handling ProductDev | ProductPackageComplete");

					handlePackageComplete(dynamic_cast<HfcProductPackageCompleteMsg *> (msg->getHfcObject()));

					break;
				}

				case ProductGetTree:
					handleProductTree();
					break;

				case ProductFindByDescription:
					handleProductGetByDescription(msg);
					break;

				case ProductReadLifeData:
					handleProductReadLifeDataMessage();
					break;

				case ProductStartPrintAllLabels:
				case ProductPausePrintAllLabels:
				{
					// We can receive a ProductStartPrintAllLabels message when the ProductionEntryMode automatically restarts printing after an event 
					//  such as thethe label cassette was removed and then reinserted.  When we get this, we need to notify the UI so it can display
					// the "Printing" dialog, which allows the user to cancel the printing.  Without this, the dialog is not redisplayed, so the
					// user couldn't cancel a print after the printing is paused and restarted.
					Session *s = dynamic_cast<Session *> (mySession);
					if (mySession && mySession->isPrimarySession() && s && (s->getCurrentMode() == OpModeProductionEntry || s->getCurrentMode() == OpModePrintMultipleLabels))
					{
						msg->setFormatType(BrowserFormat);
						mySession->send(msg, Messenger::DEST_UI);
					}
					break;
				}

				case ProductStopPrintAllLabels:
				{
					/*  This command is received when the browser sends a message to the session to stop printing the labels.
					 *  The user pressed the Cancel menu selection while the labels were being printed.
					 */
					Debug::trace("ProductManager", "received ProductStopPrintAllLabels");

					setHasProductionEntryItems(false);
					setHasAccumulateItems(false);

					Session *s = dynamic_cast<Session *> (mySession);
					if (mySession && mySession->isPrimarySession() && s && (s->getCurrentMode() == OpModeProductionEntry || s->getCurrentMode() == OpModePrintMultipleLabels))
					{
						msg->setFormatType(InternalFormat);
						mySession->send(msg, Messenger::DEST_RTC);
					}
					break;
				}

				case ProductFinishedPrintingAllLabels:
					// Received a message from the Production Entry Mode indicating that all labels have been printed.
					// Need to tell the browser this, so it can change the menu from the Cancel/Done menu back to Operate Mode menu.
					// Sends a message to the browser to tell it that all labels have been printed.
					Debug::trace("ProductManager.remotePrint", "received ProductFinishedPrintingAllLabels (101,44),  START Last Step of printing a streaming label");

					if (mySession)
					{
						Session *s = dynamic_cast<Session *> (mySession);
						if ( s )
						{
							if ( s->isInAnAccumulatingMode() )
							{
								setHasAccumulateItems(false);

								ProductFinishedPrintingAllLabelsMsg finishedMessage(msg->getOriginalBuff().c_str());
								
								if (finishedMessage.isPrintOk())
								{
									HobString formatStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgAccumItemsEANFormatString);
									int transNumChars = formatStr.countConsecutiveChars('T');
									if (transNumChars)
									{
										// If we printed the label, need to update the transaction number for the barcode.
										UINT32 transNum = (UINT32)(Configuration::getInstance()->toIntCfg(fieldOffsetCfgAccumItemsTransactionNumber)) + 1;
										// If this new transaction number is larger than the number of digits that can print,
										// in the EAN format string, then roll over the transaction back to 1.
										if ( (int) (HobString(Long::toString(transNum)).length()) > transNumChars )
											transNum = 1;
										Configuration::getInstance()->updateField(fieldOffsetCfgAccumItemsTransactionNumber, uinttostring(transNum));
										Configuration::getInstance()->save(mySession);
										Debug::trace("ProductManager", HobString("ProductFinishedPrintingAllLabels - saved updated transaction number: ") + HobString(Long::toString(transNum)));
									}
								}

								clearCurrentProduct(true, (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgAccumItemsReturnToStartScreen)) ? true : false);

								if ( s->getCurrentMode() == OpModeBuildAMeal )
								{
									// We are removing the main meal plu, so return to Manual Weigh Mode.
									s->leaveBuildAMealMode();
								}
							}

							if ( !s->isInAnAccumulatingMode() )
							{
								setHasProductionEntryItems(false);

								SessionMessage msg(ProductDev, ProductStopPrintAllLabels);
								msg.setFormatType(BrowserFormat);
								mySession->send(&msg, Messenger::DEST_UI);

								productionEntryContainsFPPItems = false;

								if (currentProduct)
									currentProduct->setNumOfLabelsEntered(false);

								if ( s->getCurrentMode() == OpModeProductionEntry )
								{
									clearCurrentProduct(true, (Configuration::getInstance()->isClearPluAfterPrinting()) ? true : false);
								}
							}
						}
					}
					
					Debug::trace("ProductManager.remotePrint", "LAST Step of printing a streaming label");

					break;

				case ProductStartPrintForTransaction:
				{
					HfcProductStartPrintForTransactionMsg *msgObject = dynamic_cast<HfcProductStartPrintForTransactionMsg *> (msg->getHfcObject());
					if (msgObject)
					{
						ProductTransaction *msgTrans = const_cast <ProductTransaction *> (msgObject->getProductTransaction());
						if (msgTrans)
						{
							Product *msgProd = msgTrans->getProduct();
							if (msgProd)
							{
								currentProduct = msgProd;

								Debug::trace("ProductManager", "ProductStartPrintForTransaction - before call to setUpLabelTypes()");
								bool labelTypeError = setUpLabelTypes();  // return is true if any label type is invalid
								if (labelTypeError)
								{
									Debug::trace("ProductManager", "ProductStartPrintForTransaction - after call to setUpLabelTypes() - labelTypeError");
									currentProduct = 0;
									msgObject->setOkToPrintTransaction(false);
									if (mySession)
										mySession->send(msg, Messenger::DEST_RTC);
								}
								else
								{
									Debug::trace("ProductManager", "ProductStartPrintForTransaction - after call to setUpLabelTypes() - no labelTypeError");
									currentProduct->setCurrentTransaction(msgTrans);
									currentProduct->setCoolVerifyForced(false);
									currentProduct->setLabelBeingPrinted(Product::PRINTING_PRODUCTION_LABEL);

									if (mySession)
									{
										// Send the product found back to the Browser.
										// Is this the behavior for ALL ProductRead messages the Session receives, to send the data back to the Browser?
										// Is there any situation where something else reads a PLU, but doesn't want the data sent back?
										// YES I just found it.  Remote Printing. And I am going to update this now because of it.
										
										
										if ( msgProd->isPrintingRemoteLabel() )
											Debug::trace("ProductManager.remotePrint", "ProductStartPrintForTransaction - isPrintingRemoteLabel = true so dont send product to UI screen." );
										else
										{
											bool isInOperateMode = false;
											Session *s = dynamic_cast<Session *> (mySession);
											if (s && s->isInOperateMode())
												isInOperateMode = true;

											Debug::trace("ProductManager.remotePrint", "Sending PLU to UI screen" );

											SessionMessage pluData(ProductDev, ProductWrite);
											currentProduct->setFormatType((isInOperateMode) ? BrowserFormatPLUOperateMode : BrowserFormat);
											pluData.setFormatType(BrowserFormat);
											pluData.setHfcObject(currentProduct);
											mySession->send(&pluData, Messenger::DEST_UI);

											// Send the total values to the browser (total price, total net weight, total ...)
											handleProductReadCurrentTotalValuesMessage();
										}
									}

									currentProduct->setPrintingInProductionEntryMode(true);

									Debug::trace("ProductManager", "ProductStartPrintForTransaction - after call to setUpLabelTypes() - no labelTypeError - sending message back to RTC");
									msgObject->setOkToPrintTransaction(true);
									if (mySession)
										mySession->send(msg, Messenger::DEST_RTC);
								}
							}
						}
					}
					break;
				}

				case ProductPrintMultipleLabels:
				{
					handlePrintMultipleLabels();
					break;
				}

				/**************************************************************************************************
				 ***                                                                                            ***
				 ***                         Accumulated Items Mode Support                                     ***
				 ***                                                                                            ***
				 **************************************************************************************************/

				case ProductAddItemToAccumulatedItems:
				{
					Configuration *cfg = Configuration::getInstance();
					if (!cfg)
						break;

					Session *s = dynamic_cast<Session *> (mySession);
					if ( s )
					{
						if ( s->isInAnAccumulatingMode() )
						{
							if (currentProduct)
							{
								if ( s->getCurrentMode() == OpModeAccumulateItems && 
								     (currentProduct->isRandomWeight() || currentProduct->isByCountWithWeight()) &&
								     currentProduct->getCurrentTransaction() &&
								     currentProduct->getCurrentTransaction()->getTotalNetWeight() <= cfg->getMinWeightToPrint() )
								{
									if (mySession)
										mySession->addDialogMessage(ErrProductDev, ProductErrNoWeightForAccumulateItems, HfcResponse::RESPONSESTYLE_OK);
									break;
								}
								else if ( s->getCurrentMode() == OpModeAccumulateItems &&
										  !cfg->toBoolCfg(fieldOffsetCfgEnableItemStripMode) &&
										  !cfg->toBoolCfg(fieldOffsetCfgEnableAccumItemsTotalsOnly) &&
										  labelMediaCheck() != LabelOk)
								{
									// FT-This detects
									break;
								}

								// Fixed weight verification
								else if ( s->getCurrentMode() == OpModeAccumulateItems &&
								          cfg->toBoolCfg(fieldOffsetCfgRequireFixedWeightVerification) &&
								          currentProduct->isFixedWeight() &&
								          currentProduct->getFixedWeight() > 0 &&
								          currentProduct->getCurrentTransaction() )
								{
									HobWeight weight = 0;
									if ( currentProduct->getCurrentTransaction()->getWeigherStatus() )
									{
										weight = currentProduct->getCurrentTransaction()->getWeigherStatus()->getWeight();
										Debug::trace( "ProductManager", string("handlePrintKey1 - fixedWeightVerify - current weight on platter is: ") + Long::toString(weight) );
									}

									if ( weight == 0 )
									{
										s->getResponseMessageManager()->addMessage(ErrProductDev, ProductErrFixedWeightVerificationNeedWeight, HfcResponse::RESPONSESTYLE_OK);
										break;
									}
									else 
									{
										HobWeight fweight = MetSig::fixedWeightToWeightCounts(currentProduct->getFixedWeight());
										Debug::trace( "ProductManager", string("handlePrintKey1 - fixedWeightVerify - fixed weight of product is: ") + Long::toString(fweight) );
										if ( weight < fweight )
										{
											HobList msgData;
											HobString data;

											HobWeight fweightCurr = MetSig::weightCountsToFixedWeight(weight);
											if (currentProduct->isFluidOunce())
												cfg->formatVolume(fweightCurr, data, FixedWeightText, true);
											else
												cfg->formatWeight(fweightCurr, data, true, FixedWeightText);

											msgData.push_back(data);
											s->getResponseMessageManager()->addMessage(ErrProductDev, ProductErrFixedWeightVerificationInsufficientWeight, HfcResponse::RESPONSESTYLE_OK, &msgData);
											break;
										}
									}
								}

								setHasAccumulateItems( true );
								waitingForLoginBeforeAddToAccum = false; // FT-809

								// Do we need to force a prompt for operator login? Added for Jira GCDR2-199
								// FT-807 prompt for login when tracking operator changes no matter what the configuration
								// FT-807 operator login required functionality to remain the same

								// THIS
								int isAccumMode = s->getCurrentMode();											// 17 AND
								bool isLoginReq = cfg->toBoolCfg(fieldOffsetCfgOperateLoginRequired);			// 1  AND
								bool isStripped = cfg->toBoolCfg(fieldOffsetCfgEnableItemStripMode);			// 0  AND
								bool isTotsOnly = cfg->toBoolCfg(fieldOffsetCfgEnableAccumItemsTotalsOnly);		// 0  AND
								// OR THIS
								bool isTracking = cfg->toBoolCfg(fieldOffsetCfgTrackingOperatorChanges);				// 1 AND
								bool hasChanges = currentProduct->getCurrentTransaction()->hasOperatorChangedData();	// 1
								// OR THIS (NEW CHANGE)
								bool isLognReq2 = cfg->toBoolCfg(fieldOffsetCfgOperateLoginRequired);			// 1 AND
								bool isTranReq2 = cfg->toBoolCfg(fieldOffsetCfgCollectingTransactions);			// 1
								// AND ALL TESTS MUST HAVE THIS
								int  numbrOpers = s->getOperatorManager()->countAllOperators();							// > 0					
								bool operNumber = false;
								if (s->getOperatorManager()->getLoggedInOperator() == 0)
									operNumber = true;																	// TRUE
								

								if( (( s->getCurrentMode() == OpModeAccumulateItems &&
										cfg->toBoolCfg(fieldOffsetCfgOperateLoginRequired) &&
										!cfg->toBoolCfg(fieldOffsetCfgEnableItemStripMode) &&
										!cfg->toBoolCfg(fieldOffsetCfgEnableAccumItemsTotalsOnly) ) ||
										( cfg->toBoolCfg(fieldOffsetCfgTrackingOperatorChanges ) && currentProduct->getCurrentTransaction()->hasOperatorChangedData() ) ||
									    ( cfg->toBoolCfg(fieldOffsetCfgOperateLoginRequired) && cfg->toBoolCfg(fieldOffsetCfgCollectingTransactions) ) ) &&
									s->getOperatorManager()->countAllOperators() > 0 && s->getOperatorManager()->getLoggedInOperator() == 0 )
								{
									waitingForLoginBeforeAddToAccum = true; // FT-809

									SessionMessage msg(OperatorDev, OpPromptForIDOrRFID);
									msg.setFormatType(BrowserFormat);
									mySession->send(&msg, Messenger::DEST_UI);
								}
								else
								{
									addCurrentItemToAccumOrMeal();

									if ( cfg->toBoolCfg(fieldOffsetCfgAccumItemsReturnToStartScreen) &&
											s->getCurrentMode() != OpModeBuildAMeal )
										clearCurrentProduct(true, true);
								}
							}
							else
								s->getResponseMessageManager()->addMessage(ErrProductDev, ProductErrNoProductForAccumulateItems, HfcResponse::RESPONSESTYLE_OK);
						}
					}
					break;
				}

				case ProductReadAndAddSideItem:
				{
					Product *msgProduct = dynamic_cast<Product *> (msg->getHfcObject());
					handleProductReadAndAddSideItemForBuildAMeal(msgProduct);
					break;
				}

				case ProductRemoveLastItemFromAccumulatedItems:
				{
					Session *s = dynamic_cast<Session *> (mySession);
					if ( s )
					{
						if ( s->isInAnAccumulatingMode() )
						{
							ProductRemoveAccumItemMsg removeIt(mySession, 0);
							mySession->send(&removeIt, Messenger::DEST_RTC);

							if ( s->getCurrentMode() == OpModeBuildAMeal )
							{
								if ( bamSidesLeft == bamMaxSides )
								{
									// We are removing the main meal plu, so return to Manual Weigh Mode.
									s->leaveBuildAMealMode();
								}
								else
								{
									// We are removing a side item, so add back to the number of sides left to add
									bamSidesLeft++;
								}

								// Send a message to the BuildAMeal mode to read the master trans so that the UI can be updated with a new total price,
								// for when sides are not free.
								SessionMessage readMasterTrans(ProductDev, ProductReadAccumulatedMasterTrans);
								readMasterTrans.setFormatType(InternalFormat);
								mySession->dispatchEvent(&readMasterTrans);
							}
							setHasAccumulateItems(false);
						}
					}
					break;
				}

				case ProductShowAccumulatedItems:
					// deprecated
					break;

				case ProductPrintAccumulatedItems:
				{
					Session *s = dynamic_cast<Session *> (mySession);
					if (s && s->getCurrentMode() == OpModeAccumulateItems)
						handleProductPrintKey1();
					break;
				}

				case ProductClearPrintingProductsList:
				{
					// bool isCleared = clearPrintingProductsList();	// I guess nothing - unused variable
					break;
				}

				case ProductRequestReadAllAccumulatedItems:
				{
					Session *s = dynamic_cast<Session *> (mySession);
					if ( s && s->isInAnAccumulatingMode() )
					{
						msg->setFormatType(InternalFormat);
						mySession->send(msg, Messenger::DEST_RTC);
					}
					break;
				}

				case ProductReadAllAccumulatedItems:
				{
					Session *s = dynamic_cast<Session *> (mySession);
					if ( s && s->isInAnAccumulatingMode() )
					{
						ProductReadAllAccumulatedItemsMsg readAllAccumulatedItemsMsg(msg->getOriginalBuff().c_str());
						const ProductTransactionMaster *prodTransMaster = readAllAccumulatedItemsMsg.getProductTransactionMaster();

						SessionMessage allAccumItemsMsg(ProductDev, ProductWriteAllAccumulatedItems);
						allAccumItemsMsg.setFormatType(BrowserFormat);
						HobAllAccumulatedItems theItems;

						if (prodTransMaster)
						{
							ProductTransaction *masterTrans = prodTransMaster->getMasterProductTransaction();
							if (masterTrans)
								theItems.setTotalPrice(masterTrans->getTotalPrice());

							for (int i = 1; i <= prodTransMaster->getTransactionCount(); i++)
							{
								const ProductTransaction *accumTrans = prodTransMaster->getTransactionPositionNum(i);
								const Product *accumItem = accumTrans->getProduct();
								theItems.addAccumulatedItem(accumItem, accumTrans);
							}
						}

						if( prodTransMaster->getTransactionCount() == 0 )
							setHasAccumulateItems(false);
	/*
						theItems.addAccumulatedItem(111, HobString("Description for one-one-one"), "2", 199, 398);
						theItems.addAccumulatedItem(222, HobString("Description for two-two-two"), "1", 249, 249);
						theItems.addAccumulatedItem(333, HobString("Description for three-three-three"), HobString("2.375"), 319, 678);
						theItems.addAccumulatedItem(4, HobString("Description for four"), "30090", 99999, 87699);
						theItems.addAccumulatedItem(5, HobString("Description for five"), "12345", 249, 249);
						theItems.addAccumulatedItem(6, HobString("Description for six"), HobString("2.375"), 319, 678);
						theItems.addAccumulatedItem(77, HobString("Description for seven"), "2", 199, 398);
						theItems.addAccumulatedItem(88, HobString("Description for eight"), "1", 249, 249);
						theItems.addAccumulatedItem(99, HobString("Description for nine"), HobString("2.375"), 319, 678);
						theItems.addAccumulatedItem(101010, HobString("Description for ten"), "2", 199, 398);
						theItems.addAccumulatedItem(111111, HobString("Description for eleven"), "1", 249, 249);
						theItems.addAccumulatedItem(121212, HobString("Description for twelve"), HobString("2.375"), 319, 678);
						theItems.addAccumulatedItem(131313, HobString("Description for thirteen"), "2", 199, 398);
						theItems.addAccumulatedItem(141414, HobString("Description for fourteen"), "1", 249, 249);
						theItems.addAccumulatedItem(151515, HobString("Description for fifteen"), HobString("2.375"), 319, 678);
	*/

						allAccumItemsMsg.setHfcObject(&theItems);
						mySession->send(&allAccumItemsMsg, Messenger::DEST_UI);
					}
					break;
				}

				case ProductRemoveAllAccumulatedItems:   // Removes all items from the list, but leaves the master transaction in the list
				{
					Session *s = dynamic_cast<Session *> (mySession);
					if ( s && s->isInAnAccumulatingMode() )
					{
						setHasAccumulateItems( false );
						// Just forward this on to the mode, it'll know how to handle it.
						msg->setFormatType(InternalFormat);
						mySession->send(msg, Messenger::DEST_RTC);

						SessionMessage msg(ProductDev, ProductRequestReadAllAccumulatedItems);
						msg.setFormatType(BrowserFormat);
						handleMessage(&msg);

						if (s && s->getCurrentMode() == OpModeBuildAMeal)
						{
							s->leaveBuildAMealMode();
						}
					}
					break;
				}

				case ProductRemoveAccumulatedItems:
				{
					Session *s = dynamic_cast<Session *> (mySession);
					if ( s )
					{
						if ( s->isInAnAccumulatingMode() )
						{
							Debug::trace("ProductManager", "ProductRemoveAccumulatedItems - start");
							hfc::VoidTransactionsMessage* listToRemoveMsg = dynamic_cast<hfc::VoidTransactionsMessage*>(msg->getHfcObject());
							if ( !listToRemoveMsg ) return;

							HobList listToRemove = listToRemoveMsg->getTaskIdentifiers();

							int numberOfItemsToRemove = listToRemove.getListCount();
							int lastOneRemoved = 9999;
							int currentOneToRemove = 0;
							HobString positionStr;

							// Need to remove them in reverse order, so if removing multiple items, and the list shrinks as they are removed, we don't start removing the wrong ones.
							// So go find the highest position number, that hasn't yet been removed.

							while (numberOfItemsToRemove)
							{
								currentOneToRemove = 0;
								int gotOne = listToRemove.first(positionStr);
								while (gotOne != -1)
								{
									int thisValue = Integer::valueOf(positionStr);
									if (thisValue > currentOneToRemove && thisValue < lastOneRemoved)
										currentOneToRemove = thisValue;
									gotOne = listToRemove.next(positionStr);
								}

								if (currentOneToRemove > 0)
								{
									numberOfItemsToRemove--;
									lastOneRemoved = currentOneToRemove;
									bool tellModeToRemoveItem = true;

									if ( s->getCurrentMode() == OpModeBuildAMeal )
									{
										if ( currentOneToRemove == 1 )
										{
											// We are removing the main meal plu, so return to Manual Weigh Mode.
											numberOfItemsToRemove = 0;
											tellModeToRemoveItem = false;
											s->leaveBuildAMealMode();
										}
										else
										{
											// We are removing a side item, so add back to the number of sides left to add
											bamSidesLeft++;
										}
									}
									if ( tellModeToRemoveItem )
									{
										Debug::trace("ProductManager", HobString("ProductRemoveAccumulatedItems - remove accum item in position: ") + HobString(Integer::toString(currentOneToRemove)));
										ProductRemoveAccumItemMsg removeIt(mySession, currentOneToRemove);
										mySession->send(&removeIt, Messenger::DEST_RTC);
									}
								}
							}

							if ( s->getCurrentMode() == OpModeBuildAMeal )
							{
								// Send a message to the BuildAMeal mode to read the master trans so that the UI can be updated with a new total price,
								// for when sides are not free.
								SessionMessage readMasterTrans(ProductDev, ProductReadAccumulatedMasterTrans);
								readMasterTrans.setFormatType(InternalFormat);
								mySession->dispatchEvent(&readMasterTrans);
							}
						}
					}
					break;
				}

				case ProductLeaveBuildAMealMode:
				{
					Session *s = dynamic_cast<Session *> (mySession);
					if ( s )
						s->leaveBuildAMealMode();
					break;
				}

				case ProductAddItemToAccumOrMeal:
				{
					// This code is provided to retry adding the current item to a meal,
					// but we had to display a Printer status message to the user, and they have
					// since dismissed that message (and hopefully responded to the printer status).
					addCurrentItemToAccumOrMeal();
					break;
				}

				case ProductReadToo:
				{
					HfcProductLookupMsg *prodLookup = dynamic_cast<HfcProductLookupMsg *> (msg->getHfcObject());
					//HfcProductLookupMsg prodLookup(recvBuffer);
					Product prod;
					prod.setProductNum(prodLookup->getPluNumber());
					handleProductReadMessage(&prod);
					
					break;
				}

				case ProductPrintProductSupportText:
				{
					bool isTracing = Debug::isTracing("ProductManager");

					ProductTextCfg* ptc = dynamic_cast<ProductTextCfg*>(msg->getHfcObject());
					if ( ptc )
					{
						// The data with this message used to be just the ProdSupportTextType (ProductText.h) to be printed,
						// in which we would just use a GenericHfcObject to hold it.
						// With the "second printer" project, a second parameter has been added.  The UI will send, in the second
						// parameter, in the printer to user chose to print to if the ProductTextCfg record for the ProdSupportTextType is
						// PromptForPrinter ("prompt").  If the second printer is not sent in, then use the default printer in the
						// ProductTextCfg db record for this ProdSupportTextType.  If that record doesn't exist, then just print to
						// the primary printer.  So now we use a ProductTextCfg object to hold the data.

						ProdSupportTextType whichSupportType = ptc->getTextType();
						if (isTracing)
							Debug::trace("ProductManager", "handling ProductPrintProductSupportText - whichSupportType:" + inttostring((int)whichSupportType));

						HobRecordNum whichLabelNum = Configuration::getInstance()->toIntCfgI(CfgInternal::PRODUCTSUPPORTTEXTLABELNUM);
						ProductTextCfg *dbptc = ProductTextCfg::findProductTextCfg(whichSupportType, dbAccess);
						if (dbptc)
							whichLabelNum = dbptc->getLabelNum();

						ProductTextPrinterChoices whichPrinter = PrinterChoice_PrimaryPrinter;

						if ( UnspecifiedPrinter == ptc->getPrinterNum() )
						{
							// The printer number was not passed in, so we have to go look in the ProductTextCfg record in the db to find out.
							if (dbptc)
							{
								if ( PrinterChoice_ThirdPrinter >= dbptc->getPrinterNum() )
								{
									whichPrinter = dbptc->getPrinterNum();
									if (isTracing)
										Debug::trace("ProductManager", "handling ProductPrintProductSupportText - whichPrinter set from db, to " + inttostring((int)whichPrinter));
								}
							}
						}
						else if ( PrinterChoice_ThirdPrinter >= ptc->getPrinterNum() )
						{
							whichPrinter = ptc->getPrinterNum();
							if (isTracing)
								Debug::trace("ProductManager", "handling ProductPrintProductSupportText - whichPrinter passed in and set to " + inttostring((int)whichPrinter));
						}
						// else
							// This is invalid, so just leave whichPrinter to be the primary printer, which it was initialized above.

						if (isTracing)
							Debug::trace("ProductManager", "handling ProductPrintProductSupportText - whichPrinter:" + inttostring((int)whichPrinter));

						if (whichSupportType > psttUnknown && whichSupportType < endProdSupportTextType)
							handlePrintProductSupportLabel( (ProdSupportTextType)whichSupportType, whichPrinter, whichLabelNum );
					}
					break;
				}


				/**************************************************************************************************
				 ***																							***
				 ***						 Product Support Data											   ***
				 ***																							***
				 **************************************************************************************************/

				case ProductShowLinksToSupportData:
					handleProductShowTextLinks(msg);
					break;

				case ProductReadSupportRecord:
					handleProductReadSupportRecord(msg);
					break;

				case ProductReadSupportData:
					handleProductReadSupportData(msg);
					break;

				case ProductWriteSupportData:
					handleProductWriteSupportData(msg);
					break;

				case ProductChangeSupportData:
					handleProductChangeSupportData(msg);
					break;

				case ProductReadLabelFormSize:
				{
					HfcProductLabelFormSizeMsg *labelFormSizeMsg =
							dynamic_cast<HfcProductLabelFormSizeMsg *> (msg->getHfcObject());
					if ( labelFormSizeMsg )
						handleProductLabelFormSize(labelFormSizeMsg);

					break;
				}

				case ProductReadNutriLabel:
					handleProductReadNutriLabelMessage();
					break;

				case ProductToggleLabelType:
					handleProductToggleLabelTypeMessage(msg);
					break;


				/**************************************************************************************************
				 ***																							***
				 ***						 Product Defaults												   ***
				 ***																							***
				 **************************************************************************************************/

				case ProductReadDefault:
				case ProductWriteDefault:
				case ProductSaveDefault:
					mgrContainer_->getProductDefaultManager()->handleMessage(msg);
					break;


				/**************************************************************************************************
				 ***																							***
				 ***						 Product Totals Related Methods									 ***
				 ***																							***
				 **************************************************************************************************/

				case ProductTransactionTypeChanged:
					Debug::trace("ProductManager", "handling ProductTransactionTypeChanged");
					handleProductTransactionTypeChanged();
					break;


				/**************************************************************************************************
				 ***																							***
				 ***						 Self-Service Related Methods									   ***
				 ***																							***
				 **************************************************************************************************/

				case ProductFindSoftKeysByDescription:
					Debug::trace("ProductManager", "handling ProductFindSoftKeysByDescription");
//					handleProductFindSoftKeysBySearchString(msg);
					break;

				case ProductReadSoftKeyList:
					Debug::trace("ProductManager", "handling ProductReadSoftKeyList");
//					handleProductReadSoftKeyList(msg);
					break;

				case ProductReadAllSoftKeyGraphicNames:
					Debug::trace("ProductManager", "handling ProductReadAllSoftKeyGraphicNames");
					handleProductReadAllProductGraphics();
					break;

				case ProductResetUnitPrice:
				{
					Debug::trace("ProductManager", "handling ProductResetPrice");
					handleResetPrice();
					break;
				}

				case ProductApplyDiscountCentsOff:
				{
					Debug::trace("ProductManager", "handling ProductApplyDiscountCentsOff");
					Product *msgProduct = dynamic_cast<Product *> (msg->getHfcObject());
					handleApplyPriceDiscount(msgProduct, event);
					break;
				}

				case ProductApplyDiscountPercentOff:
				{
					Debug::trace("ProductManager", "handling ProductApplyDiscountPercentOff");
					Product *msgProduct = dynamic_cast<Product *> (msg->getHfcObject());
					handleApplyPriceDiscount(msgProduct, event);
					break;
				}

				case ProductApplyDiscountPerPackage:
				{
					Debug::trace("ProductManager", "handling ProductApplyDiscountPerPackage");
					Product *msgProduct = dynamic_cast<Product *> (msg->getHfcObject());
					handleApplyPriceDiscount(msgProduct, event);
					break;
				}

				case ProductChangeRotationPlacement:
				{
					Debug::trace("ProductManager", "handling ProductChangeRotationPlacement");
					GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
					if (gho)
						handleChangeRotationPlacement(gho);
					break;
				}
				
				case Product2ndLabelApplyToggle://SOF-5092 Disable secondary label Flashkey
				{	
					handleProductToggleSecLabelTypeMessage(msg);
					break;
				}

				/**************************************************************************************************
				 ***																							***
				 ***						 Expiration Labels Related Methods								  ***
				 ***																							***
				 **************************************************************************************************/

				case ProductPrintNewExpirationLabel:
				case ProductPrintExistingExpirationLabel:
				case ProductReadAllExpirationLabels:
				case ProductDeleteAllExpirationLabels:
				case ProductRestoreExpirationLabel:
					mgrContainer_->getExpirationLabelManager()->handleMessage(msg);
					break;


				case ProductCompletePrintToScreen:
				{
					/* This is received from the UI when the user is completely finished with the label image on the screen
					 * for the SelfServePrintToScreen. The message received will contain a "0" if the user did not
					 * accept the label/product, or a "1" if they accepted it as a sold product.
					 */
					Debug::trace("ProductManager", "handling ProductCompletePrintToScreen - creating ProductDev | ProductComplete");
					HfcProductCompleteMsg productCompleteMsg;
					ProductTransaction *trans = ( getCurrentProduct() ) ? getCurrentProduct()->getFirstPrintingTransaction() : 0;
					if (trans)
						trans->setNumOfLabelsPrinted(0);
					productCompleteMsg.setProductTransaction(trans);
					productCompleteMsg.setPrintOk(false);

					GenericHfcObject* gho = dynamic_cast<GenericHfcObject*>(msg->getHfcObject());
					if (gho && gho->getData() == "1")
					{
						productCompleteMsg.setPrintOk(true);
						if (trans)
							trans->setNumOfLabelsPrinted(trans->getNumOfLabelsToPrint());
					}

					Debug::trace("ProductManager", "ProductCompletePrintToScreen - currentProductTrans* = " + ptrtostring(productCompleteMsg.getProductTransaction()));
					Debug::trace("ProductManager", "ProductCompletePrintToScreen - printOk = " + booltostring(productCompleteMsg.isPrintOk()));
					handleProductComplete(&productCompleteMsg);

					break;
				}

				default:
					Debug::trace("ProductManager", HobString("UNKNOWN ProductDev EVENT: ") + HobString(Integer::toString(event)));
					break;
			}
			break;


		case ProductDefaultsDev:
			mgrContainer_->getProductDefaultManager()->handleMessage(msg);
			break;


		case ProductStatisticsDev:
			switch (event)
			{
				case DtDeleteAll:
					handleDeleteAllProductStatistic();
					break;

				case DtRestore:
				{
					Debug::trace("ProductManager", "handling ProductStatisticsDev | DtRestore");
					ProductStatistic *msgProductStatistic = dynamic_cast<ProductStatistic *> (msg->getHfcObject());
					if (msgProductStatistic)
						handleRestoreProductStatistic(msgProductStatistic);
					break;
				}

				default:
					break;
			}
			break;

		default:
			break;
	}
}


void ProductManager::handlePrintProductSupportLabel(ProdSupportTextType supportType, ProductTextPrinterChoices whichPrinter, HobRecordNum whichLabelNum)
{
	bool isTracing = Debug::isTracing("ProductManager.supportPrint");

	if (isTracing)
		Debug::trace("ProductManager.supportPrint", "begin handlePrintProductSupportLabel()  texttype:" + inttostring((int)supportType));

	PrinterType thePrinter = (PrinterType)whichPrinter;
	if (isTracing)
		Debug::trace("ProductManager.supportPrint", "handlePrintProductSupportLabel() - printer:" + inttostring((int)whichPrinter) + "  labelnum:" + inttostring((int)whichLabelNum) );

	Debug::trace("ProductManager.supportPrint", "looking for Device ...");

	Device *printerDevice = Device::findDevice(Printer, thePrinter, mySession);
	if ( !printerDevice || printerDevice->isANetworkReportPrinter() || !printerDevice->isARealPrinter() )		//	This device does not exist
	{
		// Send error message to browser indicating Device not found.
		//if (mySession) 
		//	mySession->addDialogMessage(ErrDeviceDev, DeviceErrRecordNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);
		//return;

		// Instead of displaying an error, just set the printer to the primary printer.
		// Thus, if they restored/merged in the ProductTextCfg records from another scale, 
		// but this scaledoesn't have any of the external printers that were on the original scale,
		// then just print to the primary printer.
		thePrinter = FirstPrinter;
	}

	if ( currentProduct && 
	     ( ( supportType == psttNutrifacts && currentProduct->getNutrifacts() ) ||
	       ( supportType < psttLastProductText && currentProduct->getProductText(supportType) && currentProduct->getProductText(supportType)->getText().size() ) ) )
	{
		Product *myProd = new Product();
		if (myProd)
		{
			if (isTracing)
			{
				Debug::trace("ProductManager.supportPrint", HobString("handlePrintProductSupportLabel - created a dummy Product*=") + HobString(Hex::toString((long long) myProd)));
				Debug::trace("ProductManager.supportPrint", HobString("handlePrintProductSupportLabel - current Product*=") + HobString(Hex::toString((long long) currentProduct)));
			}

			myProd->setDBAccess(mySession);
			myProd->setProductNum42(DUMMY_PRODUCTNUM);
			myProd->setProductType(Product::PRODUCTTYPE_FIXEDWEIGHT);
			myProd->setDescription(currentProduct->getDescription());

			if ( supportType == psttNutrifacts )
			{
				HobRecordNum newNFNum = currentProduct->getNutrifacts()->getTextNum();
				if (isTracing)
					Debug::trace("ProductManager.supportPrint", HobString("  Will now find Nutrifact with Num:") + HobString(Long::toString(newNFNum)));
				Nutrifacts *newNutriFact = Nutrifacts::findRecord(newNFNum, currentProduct->getDeptNum(), dbAccess);
				if (!newNutriFact)
				{
					if (isTracing)
						Debug::trace("ProductManager.supportPrint", HobString("  DID NOT find Nutrifact with Num:") + HobString(Long::toString(newNFNum)));
					delete myProd;
					myProd = 0;
					if (printerDevice)
					{
						delete printerDevice;
						printerDevice = 0;
					}
					return;
				}

				myProd->setNutrifacts(newNutriFact);
			}
			else
			{	
				ProductText *dummyExpText = new ProductText(psttExpandedText);
				if (!dummyExpText)
				{
					if (isTracing)
						Debug::trace("ProductManager.supportPrint", "handlePrintProductSupportLabel - could not create dummy expanded text object");
					delete myProd;
					myProd = 0;
					if (printerDevice)
					{
						delete printerDevice;
						printerDevice = 0;
					}
					return;
				}
				
				dummyExpText->setText(currentProduct->getProductText(supportType)->getText());
				dummyExpText->setTextNum(DUMMY_PRODUCTNUM);
				myProd->setProductText(psttExpandedText, dummyExpText);
			}

			int ltnum = (int)whichLabelNum;
			LabelType *lt = LabelType::findRecord(ltnum, currentProduct->getDeptNum(), mySession);
			if (!lt)
			{
				ltnum = Configuration::getInstance()->toIntCfgI(CfgInternal::PRODUCTSUPPORTTEXTLABELNUM);
				lt = LabelType::findRecord(ltnum, currentProduct->getDeptNum(), mySession);
				if (!lt)
				{
					if (mySession)
						mySession->addDialogMessage(ErrProductDev, ProductErrPriLabelNotFound, HfcResponse::RESPONSESTYLE_OK);
					delete myProd;
					myProd = 0;
					if (printerDevice)
					{
						delete printerDevice;
						printerDevice = 0;
					}
					return;
				}
			}

			//bool labelTypeError = productManager->setUpLabelTypes();
			// The following comes from the call made directly above, which I can't use because it uses the ProductManager's currentProduct.
			bool setProductLabelType = false;
			if ( !printerDevice || printerDevice->isAHobartLabelPrinter() )
			{
				// Only look for a label in the family if we are not printing to the Zebra printer.
				LabelType *pCorrectedLabelType = 0;
				LabelMediaReturn result = labelTypeErrorTest(lt, &pCorrectedLabelType, thePrinter, currentProduct->getDeptNum(), true);
				if (result == LabelCorrected)
				{
					if (isTracing)
					{
						Debug::trace("ProductManager.supportPrint", HobString("handlePrintProductSupportLabel, corrected label - Found a label in the family to fit stock: ") + HobString("    OrigNum = ") + HobString(Long::toString(lt->getLabelTypeNum())) + HobString("    NewNum = ") + HobString(Long::toString(pCorrectedLabelType->getLabelTypeNum())));
						Debug::trace("ProductManager.supportPrint", HobString("handlePrintProductSupportLabel - set dummy product with corrected label type:") + HobString(Hex::toString((long long) pCorrectedLabelType)));
					}
					myProd->setLabelType(thePrinter, pCorrectedLabelType);
					setProductLabelType = true;
					delete lt;
				}
				else if (pCorrectedLabelType)
				{
					delete pCorrectedLabelType;
					pCorrectedLabelType = 0;
				}
			}

			if ( !setProductLabelType )
			{
				if (isTracing)
					Debug::trace("ProductManager.supportPrint", HobString("handlePrintProductSupportLabel - set dummy product with label type:") + HobString(Hex::toString((long long) lt)));
				myProd->setLabelType(thePrinter, lt);
			}

			ProductTransaction *myTrans = myProd->getCurrentTransaction();
			if (myTrans)
			{
				if (isTracing)
					Debug::trace("ProductManager.supportPrint", HobString("handlePrintProductSupportLabel - dummy Product's transaction*=") + HobString(Hex::toString((long long) myTrans)));

				currentProductSmartPtrSave = currentProductSmartPtr;

				setCurrentProduct(myProd);
				dummyProduct = myProd; // FT-873

				if ( printerDevice && printerDevice->isANonHobartLabelPrinter() )
				{
					myTrans->setLabelOutputFilename(Configuration::getInstance()->toStringCfgI(CfgInternal::USBPRINTEROUTPUTFILENAME));
					if (isTracing)
						Debug::trace("ProductManager.supportPrint", "handlePrintProductSupportLabel - label's output filename set to " + myTrans->getLabelOutputFilename() );
				}

				SessionMessage supportProductMsg(ProductDev, ProductChange);
				supportProductMsg.setFormatType(InternalFormat);
				HfcProductChangeMsg hpcm(mySession, myProd);
				supportProductMsg.setHfcObject(&hpcm);
				mySession->send(&supportProductMsg, Messenger::DEST_RTC);

				SessionMessage supportProductMsg2(ProductDev, ProductChangeSmart);
				supportProductMsg2.setFormatType(InternalFormat);
				HfcProductChangeMsg hpcm2(mySession, this);
				supportProductMsg2.setHfcObject(&hpcm2);
				mySession->send(&supportProductMsg2, Messenger::DEST_RTC);

				PAL_absclass::PAL_sleep(200);

				HfcProductUpdateMsg hpum(mySession, myTrans, true, true);	//	HA-160
				mySession->send(&hpum, Messenger::DEST_RTC);

				if (isTracing)
					Debug::trace("ProductManager.supportPrint", HobString("handlePrintProductSupportLabel - sent ProductChange and ProductUpdate prod*=") + HobString(Hex::toString((long long) myProd)));
			}
		}
	}

	if (printerDevice)
	{
		delete printerDevice;
		printerDevice = 0;
	}
}



/**************************************************************************************************
 **************************************************************************************************
 ***																							***
 ***						 Products														   ***
 ***																							***
 **************************************************************************************************
 **************************************************************************************************/

// If the first record was not read, this method will return NULL.
Product *ProductManager::firstProduct()
{
	if (!dbAccess)
		return (Product *)NULL;

	edb::DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
	if (!dbProductsRM)
		return (Product *)NULL;

	// If the user calls firstProduct() after they have already called it,
	// then delete the old query and create a new one.
	destroyProductsQueryAndIter();

	allProductsQuery = dbProductsRM->retrieveAll();

	// Couldn't get the query from which to access the products, just return false.
	if (!allProductsQuery)
		return (Product *)NULL;

	DBQuery<DBProduct>::const_iterator tempIter = allProductsQuery->begin();
	if (tempIter == allProductsQuery->end())
		return (Product *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextProduct()
	 * method call below).
	 */
	allProductsIter = new DBQuery<DBProduct>::const_iterator(tempIter);
	DBProduct *p = new DBProduct(*(*allProductsIter));
	Product *prod = new Product(p, dbAccess);
	return prod;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstProduct() method, the first call to this method will
 * result in the first record being returned.
 */
Product *ProductManager::nextProduct()
{
	if (!dbAccess)
		return (Product *)NULL;

	if (allProductsQuery && !allProductsQuery->isEnabled())
		destroyProductsQueryAndIter();

	if (!allProductsQuery || !allProductsIter)
		return firstProduct();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allProductsIter);
	if (*allProductsIter == allProductsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all products in the db. */
		destroyProductsQueryAndIter();
		return (Product *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBProduct copy constructor in order for the Hfc Product to
	 * control the destruction of the DBProduct record that the iterator contains; otherwise, the DBProduct will
	 * be deleted when the iterator goes out of scope.  The DBProduct copy will be deleted by the Product record
	 * created below.
	 */
	DBProduct *p = new DBProduct(*(*allProductsIter));
	Product *prod = new Product(p, dbAccess);
	return prod;
}


/* Provide a countAll method so the caller doesn't need to know about a dbProductsRM.
 */
int ProductManager::countAllProducts()
{
	if (!dbAccess)
		return -1;

	edb::DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
	if (!dbProductsRM)
		return -1;

	return dbProductsRM->countAll();
}

int ProductManager::calculateAverageSpace()
{
	if (!dbAccess)
		return -1;

	DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
	if (!dbProductsRM)
		return -1;

	return dbProductsRM->calculateAverageSpace();
}


Util::CSmartPtr<Product> ProductManager::getCurrentProductSmartPtr() const
{
	return currentProductSmartPtr;
}

CutTestManager *ProductManager::getCutTestManager()
{
	Debug::trace("ProductManager", "getCutTestManager()");

	CutTestManager *cutTestMgr = mgrContainer_->getCutTestManager();
	
	if (cutTestMgr)
	{
		Debug::trace("ProductManager", "getCutTestManager() got a cutTestMgr");
		return cutTestMgr;
	}
	else
	{
        Debug::trace("ProductManager", "getCutTestManager() didnt get a cutTestMgr return 0");
		return 0;
	}
}

Product *ProductManager::getCurrentProduct()
{
	return currentProduct;
}
void ProductManager::setCurrentProduct(Product *prod)
{
	Debug::trace("ProductManager.SetCurrentProduct", "currentProduct = " + ptrtostring(currentProduct));
	Debug::trace("ProductManager.SetCurrentProduct", "newProduct = " + ptrtostring(prod));

	// If we're changing to a new product, stop looking for sweethearting on the previous plu
	if (prod)
	{
		Debug::trace("sweetheartWeight", "*** ProductManager - setCurrentProduct to new product - calling stopRecordingSweetheartWeight() ***");
		stopRecordingSweetheartWeight();
	}

	if (currentProduct)
	{
		if (currentProduct->hasOutstandingTransactions())
		{
			Util::CSmartPtr<Product> printingProduct;
			if (!findPrintingProduct(currentProduct, printingProduct))
			{
				printingProducts.push_back(currentProductSmartPtr);  // Do we want to move this currentProduct pointer into here, or the smart ptr????
				Debug::trace("ProductManager", string("printingProducts.push_back1: new list len = ") + inttostring((int)printingProducts.size()));
			}
		}
		else if (currentProductSmartPtr.notNull())
		{
			// SOF-5071
			// Should we be sending a product change smart NULL here??

			// The old currentProduct pointer is now wrapped into a smart ptr,
			// so delete the smart ptr.  We still have the old currentProduct pointer for easy access,
			// but we don't delete it anymore, just set it to null.
			currentProductSmartPtr = 0;
			currentProduct = 0;
		}
	}

	//currentProductSmartPtr = Util::CSmartPtr<Product> (prod);
	currentProductSmartPtr = prod;
	currentProduct = currentProductSmartPtr.get();   //prod;

	//todo - may want to iterate through the list of products in the 'currently printing' list and
	//	   see if any have been there 'too long', and get rid of them?????????
}


void ProductManager::clearCurrentProduct(bool notifyUI, bool returnToStartScreen)
{
	Debug::trace("ProductManager.clearCurrentProduct", "begin - notifyUI=" + booltostring(notifyUI) + "  returnToStartScreen=" + booltostring(returnToStartScreen) + "  currentProduct="  + ptrtostring(currentProduct) );

	waitingForStableWeightToPrint = false;
	cancelWaitingForLogin();

	Session *s( dynamic_cast<Session*>( mySession ) );

	// SOF-4432 - As a safety measure, when we're clearing the active plu, clear the flag to ignore weigher statuses when in self-serve mode,
	// displaying the label on the screen, and the consumer has already finished the transaction by pressing "Scan or Print".
	if ( s )
		s->setSelfServeIgnoreWeightChangeForPrintToScreen(false);

	bool onlineOrderingProdLoaded( false );

	if ( !currentProduct )
		Debug::trace("ProductManager.clearCurrentProduct", "clearCurrentProduct - No currentProduct to delete");
	else
	{
		if ( OpModeProductionEntry == s->getCurrentMode() )
			if (( !currentProduct->isPrintingInProductionEntryMode() ) && ( !currentProduct->isDummyProduct() )) // FT-815
			{
				Debug::trace("ProductManager.clearCurrentProduct", "PRINT LABEL");
				printLabel();
			}

		try
		{
			OnlineOrderProduct *ooProd( currentProduct ? dynamic_cast<OnlineOrderProduct*>( currentProduct ) : NULL );
			if ( ooProd )
				onlineOrderingProdLoaded = true;
		}
		catch (...)
		{
			Debug::trace("ProductManager.clearCurrentProduct", "clearCurrentProduct - caught exception checking for OO product");
		}

		Debug::trace("ProductManager.clearCurrentProduct", "clearCurrentProduct - set currentProduct to NULL");
		setCurrentProduct( (Product*) NULL );

		if (( s ) && ( s->isPrimarySession() ) && ( s->isInOperateMode() ))
		{
			// This used to check 'config->isRFIDHWPresent()' as a condition for logging out, but starting with Fastrack, non-RFID scales behave like
			// HLX RFID scales, users only login when needed for a particular plu (operator login required, or they changed a field that has an
			// access level set for it, or they are tracking Operator changes and a field was changed), and we logout when we're done with that item.
			//	HA-185: We don't auto log out for a Prepack scale.
			if ( Configuration::getInstance()->isServiceCounterScale() && s->getOperatorManager() )
			{
				HobOperator *oper = s->getOperatorManager()->getLoggedInOperator();
				if (oper && oper->getLoginMethod() == HobOperator::LOGINMETHOD_OPNUM)
				{
					// FT-548: do not auto logout operator when in pre-pack mode
					bool cutTestOn = false;
                    bool initializingCutTest = false;

					CutTestManager *cutTestMgr = mgrContainer_->getCutTestManager();
					if (cutTestMgr)
                    {
						cutTestOn = cutTestMgr->isCuttingTestOn();
                        initializingCutTest = cutTestMgr->isWaitingToStartCutTest();
                    }

                    if ( ( OpModeAutoWeigh != s->getCurrentMode() ) && ( OpModeAutoManualApply != s->getCurrentMode() ) && ( !cutTestOn && !initializingCutTest ) )
					{
						SessionMessage logout(OperatorDev, OpLogout);
						s->getOperatorManager()->handleMessage(&logout);
					}
				}
			}

			// Send a Product Change message to the RTC so it knows it is dealing with a new plu.
			Debug::trace("ProductManager.clearCurrentProduct", "clearCurrentProduct - sending new plu message to RTC");
			SessionMessage noProductMsg(ProductDev, ProductChange);
			noProductMsg.setFormatType(InternalFormat);
			HfcProductChangeMsg hpcm(mySession, (Product *)NULL);
			noProductMsg.setHfcObject(&hpcm);
			mySession->send(&noProductMsg, Messenger::DEST_RTC);

			SessionMessage noProductMsg2(ProductDev, ProductChangeSmart);
			noProductMsg2.setFormatType(InternalFormat);
			HfcProductChangeMsg hpcm2(mySession, this);
			noProductMsg2.setHfcObject(&hpcm2);
			mySession->send(&noProductMsg2, Messenger::DEST_RTC);

			// reset the front-to-back and right-to-left film tensions.
			if (Configuration::getInstance()->isPrepackScale())
			{
				SessionMessage resetTensions(Wrapper, WrProductLoaded);
				resetTensions.setFormatType(InternalFormat);

				mySession->dispatchEvent(&resetTensions, mySession);
			}

			deleteSelfServeBarcodeLabel();
		}

		// If we're in Print Multiple Labels mode, leave that mode.
		Debug::trace("ProductManager.remotePrint", "CALL handleLeavePrintMultipleLabelsMode() VIA clearCurrentProduct()");

		handleLeavePrintMultipleLabelsMode();
	}

	if (( notifyUI ) && ( mySession ))
	{
		// Send a clear product message to the browser.
		// The browser now clears all plu fields in the dom frame (LAP - 11/29/07)
		SessionMessage clearProductMsg(ProductDev, ProductClearUI);
		clearProductMsg.setFormatType(BrowserFormat);  // passing in alternate format when done printing in Production Entry mode, but clear plu after printing is false.

		// if we have an online ordering product, don't return home, OO decides on its own
		GenericHfcObject returnToStartScreenFlag(Bool::toString(returnToStartScreen && !onlineOrderingProdLoaded));
		clearProductMsg.setHfcObject(&returnToStartScreenFlag);
		mySession->send(&clearProductMsg, Messenger::DEST_UI);

		ProductTransaction *clearedTransaction( new ProductTransaction() );
		if ( clearedTransaction )
		{
			HfcProductUpdateValuesMsg hpuvm(clearedTransaction);
			updateBrowserAndCustomerDisplay(hpuvm);

			if (mySession->isPrimarySession() && s && s->isInOperateMode())
			{
				MarqueeUpdate mqClearProduct(MqProduct, " ");
				mySession->send(&mqClearProduct, Messenger::DEST_RTC);

				MarqueeUpdate mqClearDaily(MqDaily, "");
				mySession->send(&mqClearDaily, Messenger::DEST_RTC);

				MarqueeMode mqMode(SuspendProductMode);
				mySession->send(&mqMode, Messenger::DEST_RTC);

				NumericGeneric reset(NmReset);
				mySession->send(&reset, Messenger::DEST_RTC);

				SessionMessage keypadOff(SessionDev, SnDisplayingModalPopupKeyPadOff);
				keypadOff.setFormatType(BrowserFormat);
				mySession->send(&keypadOff, Messenger::DEST_UI);
			}

			delete clearedTransaction;
		}
	}

	if (( s ) && ( s->isInOperateMode() ))
	{
		Debug::trace("ProductManager", "Sending request for weigher status");
		s->requestAWeigherStatus();
	}

	clearPick5ValueOutOfRange();
}


void ProductManager::clearPick5ValueOutOfRange()
{
	if (pick5ValueOutOfRange)
	{
		pick5ValueOutOfRange = false;

		Session *s = dynamic_cast<Session *> (mySession);
		if (s)
		{
			// Remove these in case they are displayed.
			s->getResponseMessageManager()->removeMessage(ErrProductDev, ProductErrBelowMinWeight, HfcResponse::RESPONSESTYLE_OK);
			s->getResponseMessageManager()->removeMessage(ErrProductDev, ProductErrAboveMaxWeight, HfcResponse::RESPONSESTYLE_OK);
			s->getResponseMessageManager()->removeMessage(ErrProductDev, ProductErrBelowMinTotalPrice, HfcResponse::RESPONSESTYLE_OK);
			s->getResponseMessageManager()->removeMessage(ErrProductDev, ProductErrAboveMaxTotalPrice, HfcResponse::RESPONSESTYLE_OK);
		}
	}
}


void ProductManager::handleProductReadMessage(Product *msgProduct)
{
	if (waitingForLoginBeforeAddToProductionEntry)
	{
		waitingForLoginBeforeAddToProductionEntry = false;
		msgProduct = productWaitingForProductionEntryLogin;
	}

	if (!msgProduct || !dbAccess || !mySession)
		return;

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	bool traceOn = Debug::isTracing("ProductManager");
	HobString myDebug("handleProductReadMessage():   ");

	Session *s = dynamic_cast<Session *> (mySession);
	
	if(!s)
		return;

	// SOF-4432 - As a safety measure, when we're looking up a plu, clear the flag to ignore weigher statuses when in self-serve mode,
	// displaying the label on the screen, and the consumer has already finished the transaction by pressing "Scan or Print".
	s->setSelfServeIgnoreWeightChangeForPrintToScreen(false);

	saveOffReadInPluHasForcedByCount = false;

	if (isPrintingRemoteLabel())
	{
		mySession->addDialogMessage(ErrProductDev, ProductErrPrintPrintingRemoteLabel, HfcResponse::RESPONSESTYLE_OK);
		return;
	}

	clearPick5ValueOutOfRange();
	
	/*	if (config->toBoolCfgI(CfgInternal::FAKEWEIGHERMANAGERENABLED))
	{
		SessionMessage msg(Weigher, WgTestRezero);
		msg.setFormatType(BrowserFormat);
		s->handleMessage(&msg);
	}
*/


	// We're supposed to leave BuildAMeal mode if the operator enters another plu number.
	if (s->getCurrentMode() == OpModeBuildAMeal )
		s->leaveBuildAMealMode();

	bool isInOperateMode = (s && s->isInOperateMode()) ? true : false;

	/* If in Manual weigh mode, and the user tries to change the plu before the current plu is finished
	 * being processed (done wrapping, printing label, updating totals, etc.), i.e. haven't yet received
	 * the ProductCompleteMsg from the RTC, then don't let them change the plu.  Just send back an error
	 * message.  In this situation, the Ultima just beeps.
	 */
/*
	if (s->getCurrentMode() == OpModeManualWeigh &&
		 currentProduct &&
		 currentProduct->hasOutstandingTransactions())
	{
		// Send error message to browser indicating product can't be looked up right now.
		SessionMessage errMsg(Error, ErrProductDev);
		errMsg.setFormatType(BrowserFormat);

		HfcError *hfcError = new HfcError(ProductErrCannotChangeProduct, HfcResponse::RESPONSESTYLE_OK, msgProduct->getProductNum(), msgProduct->getErrorCode());
		errMsg.setHfcObject(hfcError);

		// Send the error message to the UI (Browser)
		mySession->send(&errMsg, Messenger::DEST_UI);
		delete hfcError;
		return;
	}
*/

	if ( msgProduct->getProductNum() == (HobRecordNum)RECORD_NOT_FOUND )
	{
		clearCurrentProduct(true);
		if ( config->isPrepackScale() )
		{
			//	Now that the Product has been cleared, make sure we reset the TransactionType, if the prior PLU used Keyboard net weight.
			switchModeBackAfterKeyboardNetWt();			//	AWL-114
		}
		else if ( config->isServiceCounterScale() )
		{
            // HT-317 Keyboard Entered Weight will not print a label
			switchModeBackAfterKeyboardNetWt();
		}
		return;
	}

	bool switchToKeyboardNetWt = false;
	bool switchFromKeyboardNetWt = false;
	bool printingTransQueued = false;

	OnlineOrderProduct *msgOOProduct = dynamic_cast<OnlineOrderProduct *> (msgProduct);
	if (msgOOProduct)
		Debug::trace("OnlineOrderProduct", "handleProductReadMessage(): read is for an OnlineOrderProduct");

	Product *lookedUpProduct = Product::findRecord(msgProduct->getProductNum(), msgProduct->getDeptNum(), dbAccess, isInOperateMode, (msgOOProduct ? true : false));

	if ( !lookedUpProduct || lookedUpProduct->getErrorCode() != RECORD_OK )
	{
		if (s->getCurrentMode() == OpModeSupervisor)
		{
			// Send error message to browser indicating product_not_found.
			SessionMessage errMsg(Error, ErrProductDev);
			errMsg.setFormatType(BrowserFormat);

			HfcError hfcError = HfcError(ProductErr, HfcResponse::RESPONSESTYLE_NEWPRODUCT,
													msgProduct->getProductNum(), (lookedUpProduct ? lookedUpProduct->getErrorCode() : RECORD_NOT_FOUND));
			errMsg.setHfcObject(&hfcError);

			if (traceOn)
			{
				const char *msgdata = hfcError.serialize(BrowserFormat);
				int msglen = hfcError.getSerializedLength();
				Debug::trace("ProductManager", HobString("Product error: ") + HobString(msgdata, msglen));
			}

				// Send the error message to the UI (Browser)
				mySession->send(&errMsg, Messenger::DEST_UI);
		}
		else if ( config->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1 &&
			        mgrContainer_->getCutTestManager() && mgrContainer_->getCutTestManager()->getCurrentCuttingTest() &&
					( mgrContainer_->getCutTestManager()->getCurrentCuttingTest()->getStatus() == CuttingTest::Status_Initial || mgrContainer_->getCutTestManager()->getCurrentCuttingTest()->getStatus() == CuttingTest::Status_Suspended ) )
		{
			// Don't send a "Product Not Found" message if we are starting a cut test, in which case the operator is entering the primal plu number, or if we're resuming a cut test
			clearCurrentProduct(true);
		}
		else
		{
			currentProductCanAcceptWeight = false;
			mySession->addDialogMessage(ErrProductDev, ProductErr, HfcResponse::RESPONSESTYLE_OK);		// plu not found
			clearCurrentProduct(true);
		}

		if (lookedUpProduct)
			delete lookedUpProduct;

		return;
	}

	OnlineOrderProduct *lookedUpOOProduct = dynamic_cast<OnlineOrderProduct *> (lookedUpProduct);
	if (lookedUpOOProduct)
		lookedUpOOProduct->updateOnlineOrderingData(dynamic_cast<OnlineOrderProduct *> (msgProduct));

	if ( (s->getCurrentMode() == OpModeSupervisor) && (lookedUpProduct->isByCountForced()) )
		saveOffReadInPluHasForcedByCount = true;

	CutTestManager *cutTestMgr = mgrContainer_->getCutTestManager();
	if ( cutTestMgr && cutTestMgr->isCuttingTestOn() )
	{
		if ( config->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1 &&
		     !cutTestMgr->isPLUValidForCutTestOption1( lookedUpProduct->getProductNum() ) )
		{
			mySession->addDialogMessage(ErrProductDev, ProductErrProductNotValidForThisCutTest, HfcResponse::RESPONSESTYLE_OK);
			clearCurrentProduct(true);
			delete lookedUpProduct;
			return;
		}
		if ( lookedUpProduct->isFixedWtOrFluidOz() )
		{
			mySession->addDialogMessage(ErrProductDev, ProductErrProductNoFixedWeightInCutTest, HfcResponse::RESPONSESTYLE_OK);
			clearCurrentProduct(true);
			delete lookedUpProduct;
			return;
		}
	}

	if ( s && ( OpModeProductionEntry == s->getCurrentMode() || !SessionManager::getInstance()->isWeigherAttached() ) && 
	     config->toBoolCfg(fieldOffsetCfgRequireFixedWeightVerification) && 
		 lookedUpProduct->isFixedWeight() )
	{
		// In Production Entry mode, if fixed weight verification is required, don't allow looking up a fixed weight item.
		// If no weigher is attached and fixed weight verification is required, don't allow looking up a fixed weight item.
		if ( !SessionManager::getInstance()->isWeigherAttached() )
			mySession->addDialogMessage(ErrProductDev, ProductErrFixedWeightVerificationNoWeigher, HfcResponse::RESPONSESTYLE_OK);
		else
			mySession->addDialogMessage(ErrProductDev, ProductErrFixedWeightVerificationNoProductionEntry, HfcResponse::RESPONSESTYLE_OK);

		clearCurrentProduct(true);
		delete lookedUpProduct;
		return;
	}

	if ( OpModeSelfService == s->getCurrentMode())
	{
		if ( config->toBoolCfg(fieldOffsetCfgRequireFixedWeightVerification) && ( lookedUpProduct->isFixedWtOrFluidOz() ) )
		{
			// In self-serve mode, if fixed weight verification is turned on,
			// don't allow the user to select by count or fixed weight items.
			mySession->addDialogMessage(ErrProductDev, ProductErrProductNotValidInThisMode, HfcResponse::RESPONSESTYLE_OK);
			clearCurrentProduct(true);
			delete lookedUpProduct;
			return;
		}

		if ( config->toBoolCfg(fieldOffsetCfgSelfServePrintOnStableWeight) && lookedUpProduct->isFixedWtOrFluidOz() )  
		{
			// In self-serve mode, if a weight is required to select an item (Walmart Chile requirement),
			// don't allow the user to select fixed weight items.
			mySession->addDialogMessage(ErrProductDev, ProductErrProductNotValidInThisMode, HfcResponse::RESPONSESTYLE_OK);
			clearCurrentProduct(true);
			delete lookedUpProduct;
			return;
		}

		if ( lookedUpProduct->isByCount() && !config->toBoolCfg(fieldOffsetCfgSelfServeByCountItemsAllowed) )
		{
			// In self-serve mode, if item is a by count and by count items not allowed, don't allow it.
			mySession->addDialogMessage(ErrProductDev, ProductErrProductNotValidInThisMode, HfcResponse::RESPONSESTYLE_OK);
			clearCurrentProduct(true);
			delete lookedUpProduct;
			return;
		}
	}

	// Hookup the current transaction with the current transaction type
	Util::CSmartPtr<hfc::TransactionType> currentTransactionType = mgrContainer_->getTransactionTypeManager()->getOperatorModeTransactionType();
	if ( currentTransactionType.notNull() && lookedUpProduct->getCurrentTransaction() )
		lookedUpProduct->getCurrentTransaction()->associateTransactionType(currentTransactionType);


	if ( ( OpModeProductionEntry == s->getCurrentMode()) && ( !lookedUpProduct->isDummyProduct() ) )
	{
		if (lookedUpProduct->isRandomWeight() || lookedUpProduct->isByCountWithWeight())
		{
			mySession->addDialogMessage(ErrProductDev, ProductErrCannotSelectRandomWeightProduct, HfcResponse::RESPONSESTYLE_OK);
			clearCurrentProduct(true);
			delete lookedUpProduct;
			return;
		}
		else if (lookedUpProduct->getProductionPlanningType() == Product::PRODUCTIONPLANNING_INGREDIENT && config->toBoolCfg(fieldOffsetCfgEnableProductionPlanning))
		{
			mySession->addDialogMessage(ErrProductDev, ProductErrFPPIngredientNotAllowedInProdEntry, HfcResponse::RESPONSESTYLE_OK);
			clearCurrentProduct(true);
			delete lookedUpProduct;
			return;
		}

		// Send the previous product to the Production Entry mode to be added to the list.
		if (currentProduct &&
			config->isServiceCounterScale() &&		//	HA-185:  Does not apply to a Prepack scale.
			( config->toBoolCfg(fieldOffsetCfgOperateLoginRequired) ||
			  ( config->toBoolCfg(fieldOffsetCfgTrackingOperatorChanges) && currentProduct->getCurrentTransaction()->hasOperatorChangedData() ) ) &&
			s->getOperatorManager()->countAllOperators() > 0 && s->getOperatorManager()->getLoggedInOperator() == 0 &&
			currentProduct->getCurrentTransaction())
		{
			waitingForLoginBeforeAddToProductionEntry = true;
			if (productWaitingForProductionEntryLogin)
				delete productWaitingForProductionEntryLogin;
			productWaitingForProductionEntryLogin = new Product(msgProduct);

			delete lookedUpProduct;

			SessionMessage msg(OperatorDev, OpPromptForIDOrRFID);
			msg.setFormatType(BrowserFormat);
			mySession->send(&msg, Messenger::DEST_UI);
			return;
		}	

		if (productWaitingForProductionEntryLogin)
		{
			// This is where it's safe to delete this object.  It was assigned to the variable msgProduct, but msgProduct
			// isn't referenced anymore in this method; we've already looked it up into the lookedUpProduct object.
			delete productWaitingForProductionEntryLogin;
			productWaitingForProductionEntryLogin = 0;
		}

		if ( currentProduct )
		{
			const int numTransactions( currentProduct->numOutstandingTransactions() );

			Debug::trace("ProductManager", HobString("PRINT LABEL"));
			printLabel();

			// SOF-5499
			if ( numTransactions != currentProduct->numOutstandingTransactions() )
			{
				Debug::trace("ProductManager", HobString("Transaction queued for printing!"));
				printingTransQueued = true;
			}
		}
	
		lookedUpProduct->setNumOfLabelsForced(true);
	}

	if ( s->isInAnAccumulatingMode() )
	{
/*
		if (lookedUpProduct->isRandomWeight() || lookedUpProduct->isByCountWithWeight())
		{
			mySession->addDialogMessage(ErrProductDev, ProductErrCannotSelectRandomWeightForAccumulateItems, HfcResponse::RESPONSESTYLE_OK);
			clearCurrentProduct(true);
			delete lookedUpProduct;
			return;
		}
		else */ 
		if (lookedUpProduct->getProductionPlanningType() == Product::PRODUCTIONPLANNING_INGREDIENT)
		{
			mySession->addDialogMessage(ErrProductDev, ProductErrFPPIngredientNotAllowedInAccumulateItems, HfcResponse::RESPONSESTYLE_OK);
			clearCurrentProduct(true);
			delete lookedUpProduct;
			return;
		}
	}

	// If this is a random weight product, and the configuration indicates that no weigher is present, send an error.
	if (isInOperateMode && lookedUpProduct->isRandomWeight() && !SessionManager::getInstance()->isWeigherAttached() && !config->toBoolCfgI(CfgInternal::FAKEWEIGHERMANAGERENABLED))
	{
		mySession->addDialogMessage(ErrProductDev, ProductErrScaleNotPresent, HfcResponse::RESPONSESTYLE_OK);
		delete lookedUpProduct;
		return;
	}

	// Cannot look up fixed weight or fluid ounce items in Self-Serve mode.
	if ( config->isSelfServiceScale() )
	{
		if ( lookedUpProduct->isFixedWtOrFluidOz() )
		{
			mySession->addDialogMessage(ErrProductDev, ProductErrInvalidInSelfServeMode, HfcResponse::RESPONSESTYLE_OK);
			delete lookedUpProduct;
			clearCurrentProduct(true);
			return;
		}
		if ( lookedUpProduct->isUnitPriceForced() || lookedUpProduct->getTareA().isForced() || lookedUpProduct->isCoolVerifyForced() )
		{
			mySession->addDialogMessage(ErrProductDev, ProductErrForcedFieldEntryInvalidInSelfService, HfcResponse::RESPONSESTYLE_OK);
			delete lookedUpProduct;
			clearCurrentProduct(true);
			return;
		}
	}

	// For 'label types by mode' project
	// When Walmart Cake Label (branch-walmart-cake-label) was merged into trunk "isRemotePrintMsgReceived" no longer existed,
	// so the last parameter was removed.
	//lookedUpProduct->selectLabelTypesForMode(s->getCurrentMode(), isRemotePrintMsgReceived);
	lookedUpProduct->selectLabelTypesForMode(s->getCurrentMode());
	lookedUpProduct->setupLabelTypesForMode();
	//Done within call to previous line      lookedUpProduct->getBarCodeNum().determineBarcodeTypeDeterminant(lookedUpProduct);


	// Check if the product looked up is a Build-A-Meal product (the product's MealSideItemsGroupNum != 0).
	// If so, the change modes to BuildAMealMode.
	if ( lookedUpProduct->isMealBuilderItem()  &&
		 OpModeManualWeigh == s->getCurrentMode() &&
		 config->isPackageLicensed("SCALE_MEAL_BUILDER") &&
	     !lookedUpProduct->isUnitPriceForced() )
	{
		// First check if there are any outstanding printer statuses.  Accumulate Items mode needs to know how many items
		// fit on the label stock, and if the printer cassette is not in, (or other printer status) then we can't determine
		// that.  So check up front before selecting the item, especially in Build-A-Meal mode, since the operator doesn't
		// press a button to 'add' the item.
		if ( config->toBoolCfgI(CfgInternal::PRINTERSTATUSNONREPEATINGMSGSTOUI) )
		{
			if ( checkPrinterStatuses() )
			{
				delete lookedUpProduct;
				return;
			}
		}

		// Change to Build-A-Meal mode
		SessionMessage mode(SessionDev, SnProductSwitchToBuildAMealMode);
		s->handleMessage(&mode);

		PAL_absclass::PAL_sleep(100);

		// Request the new mode be sent to the UI.
		SessionMessage newMode(SessionDev, SnReadWeighModeTransactionType);
		s->handleMessage(&newMode);

		// Check if the product looked up is a Build-A-Meal product send the list of sides to the UI,
		// by looking up the flashkey group that is configured in the item record.
		SoftkeyGroup *mySoftkeyGroup = new SoftkeyGroup(dbAccess);
		if (!mySoftkeyGroup)
		{
			delete lookedUpProduct;
			clearCurrentProduct(true);
			return;
		}

		mySoftkeyGroup->setFeatureNumber(Reserved_BuildAMealSideItemButtons);
		mySoftkeyGroup->setGroupNumber(lookedUpProduct->getMealSideItemsGroupNum());

		SessionMessage loadSidesGroup(SoftkeyDev, LoadSoftkeyGroup);
		loadSidesGroup.setFormatType(SoftkeyGroupFormat);
		loadSidesGroup.setHfcObject(mySoftkeyGroup);
		s->handleMessage(&loadSidesGroup);

		bamMaxSides = lookedUpProduct->getMealNumSidesIncluded();
		bamSidesLeft = lookedUpProduct->getMealNumSidesIncluded();
		bamAllSidesFree = lookedUpProduct->getMealSideItemsAreFree();
		bamUnlimitedSides = (bamMaxSides == config->toIntCfgI(CfgInternal::BAMNUMOFSIDESREPRESENTINGUNLIMITEDSIDES));

		if ( lookedUpProduct->isByCountForced() )
		{
			lookedUpProduct->setByCountForced(false); 
			lookedUpProduct->setByCount(1);
		}
		if ( lookedUpProduct->getTareA().isForced() )
			(Configuration::getInstance()->toBoolCfg( fieldOffsetCfgUseSecondTare )) ? lookedUpProduct->getTareB().setForced(false) : lookedUpProduct->getTareA().setForced(false);
	}


	if (!isInOperateMode && !mySession->isCommSession())
	{
		if (lookedUpProduct->getProductText(psttExpandedText))
			lookedUpProduct->getProductText(psttExpandedText)->testIfUsedByMany();

		if (lookedUpProduct->getProductText(psttSpecialMessage))
			lookedUpProduct->getProductText(psttSpecialMessage)->testIfUsedByMany();

		if (lookedUpProduct->getProductText(psttProductNote))
			lookedUpProduct->getProductText(psttProductNote)->testIfUsedByMany();

		if (lookedUpProduct->getProductText(psttProductText5))
			lookedUpProduct->getProductText(psttProductText5)->testIfUsedByMany();

		if (lookedUpProduct->getProductText(psttProductText6))
			lookedUpProduct->getProductText(psttProductText6)->testIfUsedByMany();

		if (lookedUpProduct->getProductText(psttProductText7))
			lookedUpProduct->getProductText(psttProductText7)->testIfUsedByMany();

		if (lookedUpProduct->getProductText(psttProductText8))
			lookedUpProduct->getProductText(psttProductText8)->testIfUsedByMany();

		if (lookedUpProduct->getProductText(psttProductText9))
			lookedUpProduct->getProductText(psttProductText9)->testIfUsedByMany();

		if (lookedUpProduct->getNutrifacts())
			lookedUpProduct->getNutrifacts()->testIfUsedByMany();
	}

	if ( s->isPrimarySession() && isInOperateMode && config->isPrepackScale() )
	{
		if (SessionManager::getInstance()->isEvolutionWrapperAttached())
		{
			//	Set up the intakeMode from the current Product.
			HobString rpwc;

			short pIntakeMode = lookedUpProduct->getIntakeMode();
			if (pIntakeMode == WrapperIntakeMode::DefaultIntake)
				pIntakeMode = config->getDefaultIntakeMode();
			rpwc.append(Integer::toString(pIntakeMode));

			SessionMessage wrControl(Wrapper, WrProductLoaded);
			GenericHfcObject gho(rpwc);
			wrControl.setHfcObject(&gho);
			wrControl.setFormatType(InternalFormat);

			//	Send the WrProductLoaded change to this Session.
			mySession->dispatchEvent(&wrControl, mySession);
			if (traceOn)
			{
				Debug::trace("ProductManager", myDebug + "dispatchEvent(&wrControl, mySession)");
				Debug::trace("ProductManager", myDebug + "WrProductLoaded message sent to DEST_RTC.");
			}
		}
		adjustLabelRotations(lookedUpProduct);
	}

	// If the previous product was in Print Multiple Labels mode, then return to the previous mode.

	handleLeavePrintMultipleLabelsMode();

	// We have a new current product.
	setCurrentProduct(lookedUpProduct);


	// PJR-383
	if ( currentProduct && config->isSelfServiceScale() && config->toBoolCfg(fieldOffsetCfgSelfServeDisplayBarcodeLabel) )
	{
		if (!currentProduct->getLabelType(ExtraPrinter_UsedForSelfServeScreenBarcodeLabel))
		{
			LabelType *lt = LabelType::findRecord(Configuration::getInstance()->toIntCfgI(fieldOffsetCfgIPrintToScreenBarcodeLabelType), 0, dbAccess);
			if (lt && lt->getErrorCode() != RECORD_NOT_FOUND)
			{
				currentProduct->setLabelType(ExtraPrinter_UsedForSelfServeScreenBarcodeLabel, lt);
				
				PrinterPreImage preImage(0, FirstPrinter /*ExtraPrinter_UsedForSelfServeScreenBarcodeLabel*/, &currentProductSmartPtr);
				preImage.setEventType(PrPreImageBarcode);
				mySession->dispatchEvent(&preImage);
				Debug::trace("SelfServeBarcodeLabel", "handleProductReadMessage(): Found barcode labelType; Sent pre-image message");
				//PAL_absclass::PAL_sleep(500);
			}
		}
	}

	//SOF-5092 Flashkey to disable secondary printing
	
	//save off original sec Label Type num incase we need to set it back with Flashkey toggle
	LabelType *MySecLabelType = currentProduct->getLabelType(SecondPrinter);
	if(MySecLabelType)
	{
		origSecLabelTypeNum = MySecLabelType->getLabelTypeNum();
		Debug::trace("ProductManager", "PLU has Sec Label - setting Original 2nd Label to: " + Long::toString(origSecLabelTypeNum));
	}
	else
	{
		origSecLabelTypeNum = 0;
		Debug::trace("ProductManager", "PLU has No Sec Label - setting Original 2nd Label to: " + Long::toString(origSecLabelTypeNum));
	}

	//check to see if operator has disabled the second label
	if(secLabelTypeEnable == false && MySecLabelType)
	{
		currentProduct->setLabelType(SecondPrinter, 0);
		Debug::trace("ProductManager", "Sec Label is Disabled, calling: currentProduct->setLabelType(SecondPrinter, 0);");
	}
	//END - SOF-5092 Flashkey to disable secondary printing
	

	currentProductCanAcceptWeight = true;
	Debug::trace("ProductManager.Product", "handleProductReadMessage(): currentProduct = " + ptrtostring(currentProduct));

	if ( mySession->isPrimarySession() && isInOperateMode )
	{
		//	AWL-52: We have a Softkey that can switch to a specified TransactionType.
		//	The TransactionType will revert back when a new PLU is entered. The TransactionTypeManager will handle this.
		//	Make sure this is done before we set backdateDaysForced (Sept. 2013)
		mgrContainer_->getTransactionTypeManager()->revertToPrevTransactionType();

		//	Does the current total type allow backdating?  If so, the current product sets it as a forced field.
		if (mgrContainer_->getTransactionTypeManager()->currentTypeAllowsBackdating())
			currentProduct->setBackDateDaysForced(true);

		DBProductStatisticsRM *dbProductStatisticsRM = dbAccess->getDBProductStatisticsRM();
		if (dbProductStatisticsRM)
		{
			std::string hour;
			std::string lang;

			if (config->isSelfServiceScale())
			{
				// determine the hour for the statistics record to be updated
				hour = Integer::toString(config->determineHourForQuery());
				LanguageSelection langNum = (s && !config->toBoolCfgI(CfgInternal::ONLYPRILANGFORPRODSTATS)) ? s->getCurrentLanguageSelection() : PrimaryLanguage;
				lang = (langNum == PrimaryLanguage) ? config->toStringCfg(fieldOffsetCfgPrimaryLanguageCode) : config->toStringCfg(fieldOffsetCfgSecondaryLanguageCode);
			}
			else
			{
				//starting with Fastrack, these statistics aren't just for self-serve anymore

				// For Fastrack, we're not using a productStatistics hour method, use 0, which means to just accumulate all product stats into one bucket (for the plu)
				hour = "0";
				// For Fastrack, (non-selfServe), we're not separating the statistics for the language, so just use the primary language.
				lang = config->toStringCfg(fieldOffsetCfgPrimaryLanguageCode);
			}

			if (traceOn)
				Debug::trace("ProductManager", "readProduct() - looking for prodStats record  hour:" + hour + "  lang:" + lang);

			if (hour.size() && lang.size())
			{
				edb::DBProductStatistic *myProductStatistic = dbProductStatisticsRM->find(currentProduct->getMyDbRecord(), lang, hour);
				if (!myProductStatistic)
				{
					HobDateTime dt;

					if (traceOn)
						Debug::trace("ProductManager", "readProduct() - prodStats record not found - creating it");
					myProductStatistic = dbProductStatisticsRM->createRecord(currentProduct->getMyDbRecord());
					myProductStatistic->setLanguage(lang);
					myProductStatistic->setHourNumber(hour);
					myProductStatistic->setTotalLabelsPrinted(0);
					myProductStatistic->setRecordedDateTime(dt.convertToDBFormat());
				}

				if (myProductStatistic)
				{
					// Have a statistics record.  Update it to indicate the number of times the record was read.
					// Save back to the database now in case the user doesn't print a label for this plu.
					int selections = (myProductStatistic->isTotalSelectionsNull()) ? 1 : myProductStatistic->getTotalSelections() + 1;
					if (traceOn)
						Debug::trace("ProductManager", HobString("readProduct() - prodStats updating selected:") + HobString(Integer::toString(selections)));
					myProductStatistic->setTotalSelections(selections);
					dbProductStatisticsRM->saveRecord(myProductStatistic);

					delete myProductStatistic;
				}
			}
		}
	}
	
	if (traceOn)
		Debug::trace("ProductManager", "Sending request for weigher status");

	if (s)
		s->requestAWeigherStatus();
	
	bool labelTypeError = setUpLabelTypes();

	if ( ( config->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) || config->toBoolCfg(fieldOffsetCfgSelfServeSearchKeypadFlashkeys) ) && config->isSelfServiceScale())
	{
		// The PrintToScreen functionality (right now for self-serve mode) only shows a single label.
		// If the Product happens to have multiple label types, find out if any contain frequent shopper fields,
		// and if so, set up to use the internally-configured print-to-screen label type with frequent shopper fields.
		// If not, use the internally-configured print-to-screen label type without frequent shopper fields.
		// Then clear all other label types in the product record.

		Debug::trace("ProductManager.TestScreenLabel", "PrintToScreen - something about multiple label types");

		currentProduct->setPrintLabelToPrinter(false);

		int printerNumber = FirstPrinter;
		bool foundFS = false;
		while ( printerNumber < MaxPrinterTypes && !foundFS )
		{
			if ( currentProduct->getLabelType(printerNumber) && currentProduct->getLabelType(printerNumber)->labelHasFrequentShopper() )
				foundFS = true;
			++printerNumber;
		}

		HobRecordNum printToScreenLTNum = (foundFS) ? config->toIntCfgI(CfgInternal::SELFSERVEPRINTTOSCREENLABELTYPEFS) : config->toIntCfgI(CfgInternal::SELFSERVEPRINTTOSCREENLABELTYPE);

		LabelType *printToScreenLT = LabelType::findRecord(printToScreenLTNum, currentProduct->getDeptNum(), mySession);
		
		// Want to print the regular PLU label to the screen if we are doing BigY
		// adding the additional test to the IF lets us print the currentPlu->labelType to the screen, and then print it.
		if ( printToScreenLT && (!Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServeDisplayPluLabel)) )				
		{
			if (Configuration::getInstance()->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors) == SelfServOption_PrintToLabel)
			{
				// Option1 for BigY must come here if we enter this block
				// displays the assigned currentProduct Label to the screen
				if (currentProduct->getLabelType(FirstPrinter))
					;
				else if (currentProduct->getLabelType(SecondPrinter))
					currentProduct->setLabelType(FirstPrinter, currentProduct->getLabelType(SecondPrinter));
				else if (currentProduct->getLabelType(ThirdPrinter))
					currentProduct->setLabelType(FirstPrinter, currentProduct->getLabelType(ThirdPrinter));

				currentProduct->setLabelType(SecondPrinter, 0);
				currentProduct->setLabelType(ThirdPrinter, 0);
			}
			else
			{
				// This option displays the default label to the screen
				printToScreenLT->setLabelMediaError(LabelOk);
				currentProduct->setLabelType(FirstPrinter, printToScreenLT);
				currentProduct->setLabelType(SecondPrinter, 0);
				currentProduct->setLabelType(ThirdPrinter, 0);

				// now that we have the label type set up, we can set up the barcode data
				printToScreenLT->generateEmbeddedLabels();

				//	Update the barcode information, since the label type changed.
				BarCodeNumber tempBarCode = currentProduct->getBarCodeNum();
				tempBarCode.getFromDatabase(currentProduct->getMyDbRecord(), currentProduct, dbAccess);
				currentProduct->setBarCodeNum(tempBarCode);
			}
		}
		else
		{
			// Option1 for BigY must come here
			// displays the assigned currentProduct Label to the screen
			if ( currentProduct->getLabelType(FirstPrinter) )
				;
			else if ( currentProduct->getLabelType(SecondPrinter) )
				currentProduct->setLabelType( FirstPrinter, currentProduct->getLabelType(SecondPrinter) );
			else if ( currentProduct->getLabelType(ThirdPrinter) )
				currentProduct->setLabelType( FirstPrinter, currentProduct->getLabelType(ThirdPrinter) );

			currentProduct->setLabelType(SecondPrinter, 0);
			currentProduct->setLabelType(ThirdPrinter, 0);
		}
	}

	//if (config->isSelfServiceScale() && isInOperateMode && labelTypeError && (!Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && (SelfServOption_PrintToScreen != Configuration::getInstance()->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors))) )
	if (config->isSelfServiceScale() && isInOperateMode && labelTypeError && (!Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) ) && (!Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServeDisplayBarcodeLabel) ) )
	{
		Debug::trace("ProductManager.TestScreenLabel", "PrintToScreen - call clearCurrentProduct()");
		clearCurrentProduct(true);
		return;
	}
	else
	{
		Debug::trace("ProductManager.TestScreenLabel", "PrintToScreen - do not call clearCurrentProduct()");

		//	New Product, clear out the label type info to toggle labels and set up for the new PLU.
		for (short printerNum = FirstPrinter; printerNum < MaxPrinterTypes; printerNum++)
		{
			altLabelTypeNum[printerNum] = 0;
		}
		setupForToggleLabelTypeMessage();
		
		currentProduct->testForFrequentShopperLabel();

		if (isInOperateMode)
		{
			currentProduct->testForKbdNetWeightLabel();
			if (currentProduct->hasKbdNetWeightLabel())
			{
				if (currentProduct->isRandomWeight())
				{
					if (config->isPrepackScale())
					{
						switchToKeyboardNetWt = true;
						Debug::trace("ProductManager.KeyboardNetWeight", "handleProductReadMessage():  switchToKeyboardNetWt = " + booltostring(switchToKeyboardNetWt));
					}
				    else if (s->getCurrentMode() != OpModeAutoManualApply)
					{
                        // HT-317 Keyboard Entered Weight will not print a label
						//	This error only applies to a non-Prepack scale.
						s->addDialogMessage(ErrProductDev, ProductErrOnlyForPrepackMode, HfcResponse::RESPONSESTYLE_OK);

						// FT-503 clear plu from system
						clearCurrentProduct(true);
						return;
					}
				}
				else
				{
					s->addDialogMessage(ErrProductDev, ProductErrLabelOnlyForRandomWeight, HfcResponse::RESPONSESTYLE_OK);

					clearCurrentProduct(true);
					return;
				}
			}
			else if ( config->isPrepackScale() || config->isServiceCounterScale() )
			{
				// HT-317 Keyboard Entered Weight will not print a label
				switchFromKeyboardNetWt = true;
				Debug::trace("ProductManager.KeyboardNetWeight", "handleProductReadMessage():  switchFromKeyboardNetWt = " + booltostring(switchFromKeyboardNetWt));
			}
		}
	}

	
	//	See if the Total type can manually or automatically change the most significant UPC digit.
	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("Checking the currentTransactionType.");
		Debug::trace("ProductManager", myDebug2);
	}
	if (mySession->isPrimarySession() && isInOperateMode)
	{
		applyTransactionTypeToUpcNumber();		//	Potentially adjust the UPC number for Auto change UPC digit.

		// Send a Product Change message to the RTC so it knows it is dealing with a new plu.
		SessionMessage pluChg(ProductDev, ProductChange);
		pluChg.setFormatType(InternalFormat);
		HfcProductChangeMsg *hpcm = new HfcProductChangeMsg(mySession, currentProduct);

		//	We will send the Product whether or not we can print it.
		if (hpcm)
		{
			pluChg.setHfcObject(hpcm);

			// Generate the embedded labels
			Debug::trace("ProductManager.embeddedLabels", "Generating Embedded labels for current product ...");
			currentProduct->generateEmbeddedLabels();

			// Send the product to the RTC (it's current operating mode) for pre-imaging
			mySession->send(&pluChg, Messenger::DEST_RTC);
			delete hpcm;
		}


		// Send a Product Change message to the RTC so it knows it is dealing with a new plu.
		SessionMessage pluChg2(ProductDev, ProductChangeSmart);
		pluChg2.setFormatType(InternalFormat);
		HfcProductChangeMsg *hpcm2 = new HfcProductChangeMsg(mySession, this);

		//	We will send the ProductManager as well, so others can get the smart pointer to the product.
		if (hpcm2)
		{
			pluChg2.setHfcObject(hpcm2);
			mySession->send(&pluChg2, Messenger::DEST_RTC);
			delete hpcm2;
		}

		if (config->isPrepackScale())
		{
			//	Now that the ProductChange message has been sent to the RealTimeController, check to see if we need
			//	to switch modes because of a Keyboard net weight label type.
			if (switchToKeyboardNetWt)
				switchModeForKeyboardNetWt();
			else if (switchFromKeyboardNetWt)
				switchModeBackAfterKeyboardNetWt();
		}
		else if ( config->isServiceCounterScale() )
		{
            // HT-317 Keyboard Entered Weight will not print a label
			//  Even though this is the same as the IF, I kept apart in case I need to add additional code here to handle Service Counter differences. 
            //	Now that the ProductChange message has been sent to the RealTimeController, check to see if we need
			//	to switch modes because of a Keyboard net weight label type.
			if (switchToKeyboardNetWt)
				switchModeForKeyboardNetWt();
			else if (switchFromKeyboardNetWt)
				switchModeBackAfterKeyboardNetWt();
		}
	}

	//----------------------------
	// HT-365: Variable Shelf Life
	//----------------------------
	if( isInOperateMode && currentProduct && ( currentProduct->getShelfLifeDays() > 0 ) )
	{
		// Do NOT bump the PLU's Shelf Life Days if its Shelf Life Days are 0

		bool isActive = Configuration::getInstance()->toBoolCfg(fieldOffsetCfgEnableVariableShelfLifeTime);
		HobString varShelfLifeTime = Configuration::getInstance()->toStringCfg( fieldOffsetCfgVariableShelfLifeTime );

		if (isActive)
		{
			if (varShelfLifeTime != "00:00 AM")
		    {
				// Parse the Variable Shelf Life Time from Normal Scale Ops
				HobString hourStr, minuteStr, amPmStr;
				HobTokenizer tokens( &varShelfLifeTime, ":" );
				tokens.next( &hourStr );
	
				HobString rightStr;
				tokens.next( &rightStr );
				HobTokenizer tokens2( &rightStr, " " );
				tokens2.next( &minuteStr );
				tokens2.next( &amPmStr );
	
				int adjustHour = 0;
				amPmStr.toUpper();
				if( amPmStr == "PM" )
					adjustHour = 12;

				HobString msgDebug0 = "VariableShelfLife-Setup:  ";
				msgDebug0.append("RawShelfLifeTime = ").append( varShelfLifeTime );
				msgDebug0.append("  Hour (24) = ").append( Integer::toString( util_string::stringtoint( hourStr ) + adjustHour ) );
				msgDebug0.append("  Minute = ").append( minuteStr );
				msgDebug0.append("  AM/PM = ").append( amPmStr );
				Debug::trace("ProductManager.VariableShelfLife", msgDebug0);
	
				// Get the current time
				time_t currentTime;
				struct tm *localTime;
				time( &currentTime );
				localTime = localtime( &currentTime );
	
				HobString msgDebug = "VariableShelfLife-Current: ";
				msgDebug.append("currentHour = ").append( Integer::toString( localTime->tm_hour ) );
				msgDebug.append("  currentMinute = ").append( Integer::toString( localTime->tm_min ) );
				Debug::trace("ProductManager.VariableShelfLife", msgDebug);

				// Check if current time is later then the variable shelf life time by comparing the number of minutes.
				// If so add an extra day to Shelf Life (and Product Life)
				if( ( ( localTime->tm_hour * 60) + localTime->tm_min ) >
					( ( ( ( util_string::stringtoint( hourStr ) ) + adjustHour ) * 60 ) + util_string::stringtoint( minuteStr ) ) )
				{
					HobString msgDebug = "VariableShelfLife: ";
					msgDebug.append("Shelf Life Days incremented from: ").append( Integer::toString( currentProduct->getShelfLifeDays() ) );
					msgDebug.append(" to: ").append( Integer::toString( currentProduct->getShelfLifeDays() + 1 ) );
					Debug::trace("ProductManager.VariableShelfLife", msgDebug);
	
					// Calling adjustShelfLifeDays() modifies Shelf Life AND Product Life
					currentProduct->adjustShelfLifeDays( currentProduct->getShelfLifeDays() + 1 );
				}
			}
			else
			{
				Debug::trace("ProductManager.VariableShelfLife", "pop a dialog saying the time is 0");
				mySession->addDialogMessage(ErrProductDev, ProductErrExtraDayTimeIsZero, HfcResponse::RESPONSESTYLE_OK);
			}
		}
		/*
		else if( ( varShelfLifeTime != "00:00 AM" ) && !isActive) // existing
		    {
			    // Parse the Variable Shelf Life Time from Normal Scale Ops
			    HobString hourStr, minuteStr, amPmStr;
			    HobTokenizer tokens( &varShelfLifeTime, ":" );
			    tokens.next( &hourStr );

			    HobString rightStr;
			    tokens.next( &rightStr );
			    HobTokenizer tokens2( &rightStr, " " );
			    tokens2.next( &minuteStr );
			    tokens2.next( &amPmStr );

			    int adjustHour = 0;
			    amPmStr.toUpper();
			    if( amPmStr == "PM" )
				    adjustHour = 12;

			    HobString msgDebug0 = "VariableShelfLife-Setup:  ";
			    msgDebug0.append("RawShelfLifeTime = ").append( varShelfLifeTime );
			    msgDebug0.append("  Hour (24) = ").append( Integer::toString( util_string::stringtoint( hourStr ) + adjustHour ) );
			    msgDebug0.append("  Minute = ").append( minuteStr );
			    msgDebug0.append("  AM/PM = ").append( amPmStr );
			    Debug::trace("ProductManager.VariableShelfLife", msgDebug0);

			    // Get the current time
			    time_t currentTime;
			    struct tm *localTime;
			    time( &currentTime );
			    localTime = localtime( &currentTime );

			    HobString msgDebug = "VariableShelfLife-Current: ";
			    msgDebug.append("currentHour = ").append( Integer::toString( localTime->tm_hour ) );
			    msgDebug.append("  currentMinute = ").append( Integer::toString( localTime->tm_min ) );
			    Debug::trace("ProductManager.VariableShelfLife", msgDebug);

			    // Check if current time is later then the variable shelf life time by comparing the number of minutes.
			    // If so add an extra day to Shelf Life (and Product Life)
			    if( ( ( localTime->tm_hour * 60) + localTime->tm_min ) >
				    ( ( ( ( util_string::stringtoint( hourStr ) ) + adjustHour ) * 60 ) + util_string::stringtoint( minuteStr ) ) )
			    {
				    HobString msgDebug = "VariableShelfLife: ";
				    msgDebug.append("Shelf Life Days incremented from: ").append( Integer::toString( currentProduct->getShelfLifeDays() ) );
				    msgDebug.append(" to: ").append( Integer::toString( currentProduct->getShelfLifeDays() + 1 ) );
				    Debug::trace("ProductManager.VariableShelfLife", msgDebug);

				    // Calling adjustShelfLifeDays() modifies Shelf Life AND Product Life
				    currentProduct->adjustShelfLifeDays( currentProduct->getShelfLifeDays() + 1 );
			    }
		}
		*/
	}

	if (!currentProduct)
	{
		Debug::trace("ProductManager", "handleProductReadMessage - we have no currentPLU");
		return;
	}

	// This is a standard lookup made by the scale itself.  Send the results back to the scale to display PLU on screen
	SessionMessage pluData(ProductDev, ProductWrite);
	currentProduct->setFormatType((isInOperateMode) ? BrowserFormatPLUOperateMode : BrowserFormat);
	pluData.setFormatType(BrowserFormat);
	pluData.setHfcObject(currentProduct);
	mySession->send(&pluData, Messenger::DEST_UI);

	if (mySession->isPrimarySession() && isInOperateMode)
	{
		//	PT # 349 & 564 - Clear the Run totals AFTER we send the ProductWrite message to the browser.
		if (mgrContainer_->getProductTotalManager())
			mgrContainer_->getProductTotalManager()->clearAndDisplayProductRunTotal();

		//	We have no marquee if Prepack
		if (!config->isPrepackScale())
		{
			// Based on the Marquee configuration, send the product description
			// and/or the product's Marquee to the customer display.

			short descMqCtr = config->getProductDescMarqueeCounter();
			if (descMqCtr > (short)SuspendProductMode && descMqCtr <= (short)OnlyProductMode)
			{
				HobString mqDesc = "\\F\\R";
				mqDesc.append(currentProduct->getDescription());
				mqDesc.stripFontSequences();
				string::size_type lfPos = mqDesc.find_first_of(0x0A);
				while (lfPos != string::npos)
				{
					mqDesc.replace(lfPos, 1, " ");
					lfPos = mqDesc.find_first_of(0x0A);
				}
				lfPos = mqDesc.find_first_of(0x0D);
				while (lfPos != string::npos)
				{
					mqDesc.replace(lfPos, 1, " ");
					lfPos = mqDesc.find_first_of(0x0D);
				}
				if (mqDesc.size() >= MAX_MARQUEE_STRING)
					mqDesc.resize(MAX_MARQUEE_STRING-1);

				MarqueeMode mqMode((MqProdMode)descMqCtr);
				mySession->send(&mqMode, Messenger::DEST_RTC);

				MarqueeUpdate mqUpdate(MqProduct, mqDesc.c_str());
				mySession->send(&mqUpdate, Messenger::DEST_RTC);
			}

			if (descMqCtr != (short)OnlyProductMode && config->isProductMarqueeDisplayed())
			{
				ProductText *mq = currentProduct->getProductText(psttMarqueeText);
				if (mq)
				{
					std::string mqText = mq->getText().substr(0, MAX_MARQUEE_STRING-1);
					MarqueeUpdate mqUpdate(MqDaily, mqText.c_str());
					mySession->send(&mqUpdate, Messenger::DEST_RTC);
				}
				else
				{
					MarqueeUpdate mqUpdate(MqDaily, "");
					mySession->send(&mqUpdate, Messenger::DEST_RTC);
				}
			}
		}
	}

	//	If the Product is not random weight, send the currentTotalValues
	//	so the Total price field can be updated on the browser.
	if ( !currentProduct->isRandomWeight() && isInOperateMode )
		updateWeightAndNotify((HfcWeigherStatus *) NULL, true);

	/* Don't automatically send this message back to the browser.
	 * Make it ask for the data by passing a ProductDev|ProductReadSupport message.
	{
		// Send the ProductNum, Special Message, Expanded Text, Nutritional Info, ... back to the Browser.
		SessionMessage pluSupport(ProductDev, ProductWriteSupportData);
		pluSupport.setFormatType(BrowserFormat);

		HfcProductSupportDataMsg hpsdm(currentProduct);
		pluSupport.setHfcObject(&hpsdm);

		// Send the product data to the UI (Browser)

		mySession->send(&pluSupport, Messenger::DEST_UI);
	}
	*/


	if ( lookedUpOOProduct && msgOOProduct && msgOOProduct->isReceived(Product::fieldOffsetByCount) )
	{
		SessionMessage updateByCountMsg(ProductDev, ProductWriteSingleField);
		updateByCountMsg.setFormatType(BrowserFormat);
		WriteSingleField fieldUpdate( lookedUpOOProduct->getProductNum(), Product::fieldOffsetByCount, inttostring(msgOOProduct->getByCount()) );
		fieldUpdate.setFieldOrigValue( inttostring( lookedUpOOProduct->getByCount() ) );
		updateByCountMsg.setHfcObject(&fieldUpdate);
		handleMessage(&updateByCountMsg);
		//mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
	}


	if (!isInOperateMode)
	{
		//	Check the Reporting Class - Is it good? Are the links good?
		checkForReportingClassError();

		//	Check all of the Graphics - Report any errors
		checkForPLUGraphicsError();
	}

	// We only require the weigher to be zero when the user enters a new product for the Prepack scale,
	// not for SelfServe or ServiceCounter.
	if ( mySession->isPrimarySession() && config->isPrepackScale() )
	{
		HfcWeigherStatus *currentWeight = (s) ? s->getCurrentWeight() : 0;
		if (currentWeight && currentWeight->isPeripheralComm() && !currentWeight->isManagerDeleted() &&
			 isInOperateMode && !(currentWeight->isZeroStableWeight() || (currentWeight->isSmallMotion() && currentWeight->isWithinZeroQuarterGrad() && !currentWeight->isScaleWarmup())))
		{
			currentProductCanAcceptWeight = false;

			ProductTransaction *clearedTransaction = new ProductTransaction();
			if (clearedTransaction)
			{
				// Display the current weight on the platter in the Net weight field.  This will allow the operator
				// to see how much weight the scale is registering.  If they press 'ignore', the net weight will
				// then be updated with the total net weight for the product (i.e. this current weight less the
				// product's tare).
				clearedTransaction->setWeigherStatus(currentWeight);  // a copy of the currentWeight object is put into the trans., not the actual currentWeight pointer
				clearedTransaction->setTotalNetWeight(currentWeight->getWeight());

				// Send the product data back to the Browser and the customer display.
				HfcProductUpdateValuesMsg hpuvm(clearedTransaction);
				updateBrowserAndCustomerDisplay(hpuvm);

				NumericGeneric reset(NmReset);
				mySession->send(&reset, Messenger::DEST_RTC);

				delete clearedTransaction;
			}

			mySession->addDialogMessage(ErrWeigher, WeigherErrScaleNotZero, HfcResponse::RESPONSESTYLE_REZEROIGNORE);
		}
		else
			currentProductCanAcceptWeight = true;
	}
/*
	if (isInOperateMode)
	{
		//	Does the current total type allow backdating?
		if (mgrContainer_->getTransactionTypeManager()->currentTypeAllowsBackdating())
		{
			//	Request the # of days to backdate this product.
			SessionMessage bdMsg(ProductDev, ProductGetBackDateDays);
			bdMsg.setFormatType(BrowserFormat);
			mySession->send(&bdMsg, Messenger::DEST_UI);
		}
	}
*/

	if (s->getCurrentMode() == OpModeBuildAMeal )
	{
		EvolMessage msg(ProductDev, ProductAddItemToAccumulatedItems);
		//s->handleMessage(&msg);
		s->dispatchEvent(&msg, s);
	}

	//Debug::trace("ProductManager", "FINISHED handleProductReadMessage");
}




int ProductManager::handleRemoteReadProductMessage(Product *msgProduct)
{
	if (!msgProduct || !dbAccess || !mySession)
		return ProductErrPrintCantFindMainSession;

	Configuration *config = Configuration::getInstance();
	if (!config)
		return ProductErrPrintCantFindConfiguration;

	bool traceOn = Debug::isTracing("ProductManager.remotePrint");
	HobString myDebug("handleProductRemoteReadMessage():   ");

	Session *s = dynamic_cast<Session *> (mySession);

	bool isInOperateMode = ( s && s->isInOperateMode() ) ? true : false;
	saveOffReadInPluHasForcedByCount = false;

	if (msgProduct->getProductNum() == (HobRecordNum)RECORD_NOT_FOUND)
		return ProductErrPrintNoProduct;

	Product *lookedUpProduct = Product::findRecord(msgProduct->getProductNum(), msgProduct->getDeptNum(), dbAccess, isInOperateMode);

	if ( !lookedUpProduct || lookedUpProduct->getErrorCode() != RECORD_OK )
	{
		if (lookedUpProduct)
			delete lookedUpProduct;
		return ProductErrPrintNoProduct;
	}

	if ( !lookedUpProduct->getCurrentTransaction() )
	{
		delete lookedUpProduct;
		return ProductErrPrintTransactionMissingToPrint;
	}

	// If this is a random weight product, and the configuration indicates that no weigher is present, send an error.
	if ( isInOperateMode && lookedUpProduct->isRandomWeight() && !SessionManager::getInstance()->isWeigherAttached() && !config->toBoolCfgI(CfgInternal::FAKEWEIGHERMANAGERENABLED) )
	{
		mySession->addDialogMessage(ErrProductDev, ProductErrScaleNotPresent, HfcResponse::RESPONSESTYLE_OK);
		delete lookedUpProduct;
		return ProductErrPrintWeigherIssue;
	}

	if (lookedUpProduct->getTareA().isForced())
	{
		delete lookedUpProduct;
		return ProductErrPrintBadTare;			// NOT TESTED
	}

	if (lookedUpProduct->getTareB().isForced())
	{
		delete lookedUpProduct;
		return ProductErrPrintBadTare;			// NOT TESTED
	}

	if (lookedUpProduct->isCoolRequired() || lookedUpProduct->isCoolVerifyForced())
	{
		delete lookedUpProduct;
		return ProductErrPrintBadCool;			// NOT TESTED
	}

	if (lookedUpProduct->getProductionPlanningType() == Product::PRODUCTIONPLANNING_INGREDIENT)     // In Print Multiple Labels mode, cant print FPP items.
	{
		delete lookedUpProduct;
		return ProductErrCannotPrintMultLabelsForFPPType2;
	}

	// Check if the product looked up is a Build-A-Meal product (the product's MealSideItemsGroupNum != 0).
	// If so, the change modes to BuildAMealMode.
	//if (lookedUpProduct->isMealBuilderItem() &&

	if (lookedUpProduct->isFixedWeight() && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgRequireFixedWeightVerification))
	{
		// NOT TESTED
		// TODO remote print support - Fixed weight verification (but only relevant for a fixed weight item) - requires a weight >= the net weight set up for the item
		// Only do this code if PHASE 1 - For Phase 2 we will support fixed weight verification
		delete lookedUpProduct;
		return returnRemotePrintError(ProductErrCannotPrintInFixedWeightVerificationMode);
	}

	lookedUpProduct->testForKbdNetWeightLabel();
	if (lookedUpProduct->hasKbdNetWeightLabel())
	{
		delete lookedUpProduct;
		return ProductErrCannotPrintInKeyboardNetWeightMode;
	}

	// see if it fits on the label stock
	// Use this for phase 1, use ProductManager::labelTypeErrorTest() when we start changing the label in P2
	LabelType *labelType = lookedUpProduct->getLabelType(0);
	if ( !labelType || labelType->getLabelTypeNum() <= 0 )
	{
		delete lookedUpProduct;
		return ProductErrPriLabelNotFound;
	}

	/*  Don't need this check when looking up the remote product.  The label type will be validated in the call to setUpLabelTypes() below
	int status_code = checkForRemoteSizingStatus(labelType, LabelTypeManager::BEST_LABELFIT);
	if (status_code > 0)
	{
		delete lookedUpProduct;
		return status_code;
	}
	*/

	// We have a new current product.
	setCurrentProduct(lookedUpProduct);
	currentProductCanAcceptWeight = true;
	Debug::trace("ProductManager.Product", "handleProductReadMessage(): currentProduct = " + ptrtostring(currentProduct));

	if (!currentProduct)
	{
		Debug::trace("ProductManager", "handleProductReadMessage - we have no currentPLU");
		return ProductErrPrintNoProduct;
	}


	// This can only be called after the call to setCurrentProduct(), since it uses 'currentProduct'
	bool labelTypeError = setUpLabelTypes();
	if (labelTypeError)
	{
		setCurrentProduct(0);
		return ProductErrPriLabelWontFit;     // used to be ProductErrPriLabelNotSized;
	}

	// Hookup the current transaction with the current transaction type
	Util::CSmartPtr<hfc::TransactionType> currentTransactionType = mgrContainer_->getTransactionTypeManager()->getOperatorModeTransactionType();
	if (currentTransactionType.notNull())
		lookedUpProduct->getCurrentTransaction()->associateTransactionType(currentTransactionType);

	//	AWL-52: We have a Softkey that can switch to a specified TransactionType.
	//	The TransactionType will revert back when a new PLU is entered. The TransactionTypeManager will handle this.
	//	Make sure this is done before we set backdateDaysForced (Sept. 2013)
	mgrContainer_->getTransactionTypeManager()->revertToPrevTransactionType();

	//	Does the current total type allow backdating?  If so, the current product sets it as a forced field.
	//if (mgrContainer_->getTransactionTypeManager()->currentTypeAllowsBackdating())
	//	currentProduct->setBackDateDaysForced(true);

	DBProductStatisticsRM *dbProductStatisticsRM = dbAccess->getDBProductStatisticsRM();
	if (dbProductStatisticsRM)
	{
		std::string hour;
		std::string lang;

			//starting with Fastrack, these statistics aren't just for self-serve anymore

			// For Fastrack, we're not using a productStatistics hour method, use 0, which means to just accumulate all product stats into one bucket (for the plu)
			hour = "0";
			// For Fastrack, (non-selfServe), we're not separating the statistics for the language, so just use the primary language.
			lang = config->toStringCfg(fieldOffsetCfgPrimaryLanguageCode);

		if (traceOn)
			Debug::trace("ProductManager.remotePrint", "readProduct() - looking for prodStats record  hour:" + hour + "  lang:" + lang);

		if (hour.size() && lang.size())
		{
			edb::DBProductStatistic *myProductStatistic = dbProductStatisticsRM->find(currentProduct->getMyDbRecord(), lang, hour);
			if (!myProductStatistic)
			{
				HobDateTime dt;

				if (traceOn)
					Debug::trace("ProductManager.remotePrint", "readProduct() - prodStats record not found - creating it");
				myProductStatistic = dbProductStatisticsRM->createRecord(currentProduct->getMyDbRecord());
				myProductStatistic->setLanguage(lang);
				myProductStatistic->setHourNumber(hour);
				myProductStatistic->setTotalLabelsPrinted(0);
				myProductStatistic->setRecordedDateTime(dt.convertToDBFormat());
			}

			if (myProductStatistic)
			{
				// Have a statistics record.  Update it to indicate the number of times the record was read.
				// Save back to the database now in case the user doesn't print a label for this plu.
				int selections = (myProductStatistic->isTotalSelectionsNull()) ? 1 : myProductStatistic->getTotalSelections() + 1;
				if (traceOn)
					Debug::trace("ProductManager.remotePrint", HobString("readProduct() - prodStats updating selected:") + HobString(Integer::toString(selections)));
				myProductStatistic->setTotalSelections(selections);
				dbProductStatisticsRM->saveRecord(myProductStatistic);

				delete myProductStatistic;
			}
		}
	}

	if (traceOn)
		Debug::trace("ProductManager.remotePrint", "Sending request for weigher status");
	if (s)
		s->requestAWeigherStatus();

	currentProduct->testForFrequentShopperLabel();

	applyTransactionTypeToUpcNumber();		//	Potentially adjust the UPC number for Auto change UPC digit.

	// Generate the embedded labels
	Debug::trace("ProductManager.embeddedLabels", "Generating Embedded labels for current product ...");
	currentProduct->generateEmbeddedLabels();


	//----------------------------
	// HT-365: Variable Shelf Life
	//----------------------------
	if (currentProduct->getShelfLifeDays() > 0)
	{
		// Do NOT bump the PLU's Shelf Life Days if its Shelf Life Days are 0

		HobString varShelfLifeTime = Configuration::getInstance()->toStringCfg(fieldOffsetCfgVariableShelfLifeTime);
		if (varShelfLifeTime != "00:00 AM") // i.e. Enabled
		{
			// Parse the Variable Shelf Life Time from Normal Scale Ops
			HobString hourStr, minuteStr, amPmStr;
			HobTokenizer tokens(&varShelfLifeTime, ":");
			tokens.next(&hourStr);

			HobString rightStr;
			tokens.next(&rightStr);
			HobTokenizer tokens2(&rightStr, " ");
			tokens2.next(&minuteStr);
			tokens2.next(&amPmStr);

			int adjustHour = 0;
			amPmStr.toUpper();
			if (amPmStr == "PM")
				adjustHour = 12;

			HobString msgDebug0 = "VariableShelfLife-Setup:  ";
			msgDebug0.append("RawShelfLifeTime = ").append(varShelfLifeTime);
			msgDebug0.append("  Hour (24) = ").append(Integer::toString(util_string::stringtoint(hourStr) + adjustHour));
			msgDebug0.append("  Minute = ").append(minuteStr);
			msgDebug0.append("  AM/PM = ").append(amPmStr);
			Debug::trace("ProductManager.VariableShelfLife", msgDebug0);

			// Get the current time
			time_t currentTime;
			struct tm *localTime;
			time(&currentTime);
			localTime = localtime(&currentTime);

			HobString msgDebug = "VariableShelfLife-Current: ";
			msgDebug.append("currentHour = ").append(Integer::toString(localTime->tm_hour));
			msgDebug.append("  currentMinute = ").append(Integer::toString(localTime->tm_min));
			Debug::trace("ProductManager.VariableShelfLife", msgDebug);

			// Check if current time is later then the variable shelf life time by comparing the number of minutes.
			// If so add an extra day to Shelf Life (and Product Life)
			if (((localTime->tm_hour * 60) + localTime->tm_min) >
				((((util_string::stringtoint(hourStr)) + adjustHour) * 60) + util_string::stringtoint(minuteStr)))
			{
				HobString msgDebug = "VariableShelfLife: ";
				msgDebug.append("Shelf Life Days incremented from: ").append(Integer::toString(currentProduct->getShelfLifeDays()));
				msgDebug.append(" to: ").append(Integer::toString(currentProduct->getShelfLifeDays() + 1));
				Debug::trace("ProductManager.VariableShelfLife", msgDebug);

				// Calling adjustShelfLifeDays() modifies Shelf Life AND Product Life
				currentProduct->adjustShelfLifeDays(currentProduct->getShelfLifeDays() + 1);
			}
		}
	}

	if (traceOn)
		Debug::trace("ProductManager.remotePrint", "FINISHED handleProductRemoteReadMessage");
	return 0;
}




void ProductManager::handleProductReadAndAddSideItemForBuildAMeal(Product *msgProduct)
{
	if (!msgProduct || !dbAccess || !mySession)
		return;

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	bool traceOn = Debug::isTracing("ProductManager");
	HobString myDebug("handleProductReadMessage():   ");

	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return;

	if ( s->getCurrentMode() != OpModeBuildAMeal )
 		return;

	if (Configuration::getInstance()->toBoolCfgI(CfgInternal::PRINTERSTATUSNONREPEATINGMSGSTOUI))
	{
		if ( checkPrinterStatuses() )
			return;
	}

	if ( bamSidesLeft == 0 && !bamUnlimitedSides )
	{
		mySession->addDialogMessage(ErrProductDev, ProductErrMaxSidesAlreadyAddedToMeal, HfcResponse::RESPONSESTYLE_OK);
		return;
	}

	Product *lookedUpProduct = Product::findRecord(msgProduct->getProductNum(), msgProduct->getDeptNum(), dbAccess);

	if (!lookedUpProduct || lookedUpProduct->getErrorCode() != RECORD_OK)
	{
		mySession->addDialogMessage(ErrProductDev, ProductErr, HfcResponse::RESPONSESTYLE_OK);

		if (lookedUpProduct)
		{
			delete lookedUpProduct;
			lookedUpProduct = 0;
		}
		return;
	}

	if ( lookedUpProduct->isRandomWeight() && !bamAllSidesFree )
	{
		mySession->addDialogMessage(ErrProductDev, ProductErrInvalidSideProductType, HfcResponse::RESPONSESTYLE_OK);

		if (lookedUpProduct)
		{
			delete lookedUpProduct;
			lookedUpProduct = 0;
		}
		return;
	}

	// Hookup the current transaction with the current transaction type
	Util::CSmartPtr<hfc::TransactionType> currentTransactionType = mgrContainer_->getTransactionTypeManager()->getOperatorModeTransactionType();
	if ( currentTransactionType.notNull() && lookedUpProduct->getCurrentTransaction() )
		lookedUpProduct->getCurrentTransaction()->associateTransactionType(currentTransactionType);


	if ( s->isInAnAccumulatingMode() )
	{
		if (lookedUpProduct->getProductionPlanningType() == Product::PRODUCTIONPLANNING_INGREDIENT)
		{
			if (mySession)
				mySession->addDialogMessage(ErrProductDev, ProductErrFPPIngredientNotAllowedInAccumulateItems, HfcResponse::RESPONSESTYLE_OK);
			clearCurrentProduct(true);
			delete lookedUpProduct;
			lookedUpProduct = 0;
			return;
		}
	}

	// Check if the product looked up is a Build-A-Meal product (the product's MealSideItemsGroupNum != 0).
	// If so, the change modes to BuildAMealMode.
	if ( lookedUpProduct->isMealBuilderItem() )
	{
		// Per the design spec:
		// 11.	If a build a meal PLU has been selected as a side (i.e. in an invalid configuration),
		// it will not be added as a side and instead start the operator all over with the new build a meal PLU.
		// This is the same as key entering, pressing a plu flashkey on a different screen, or searching for a new PLU item.

		if ( !lookedUpProduct->isUnitPriceForced() )
		{
			EvolMessage msg(ProductDev, ProductLeaveBuildAMealMode);
			//s->handleMessage(&msg);
			s->dispatchEvent(&msg, s);

			ProductLookupMsg lookup(s, lookedUpProduct->getProductNum());
			//s->handleMessage(&msg);
			s->dispatchEvent(&lookup, s);

			//handleProductReadMessage(msgProduct);
		}

		delete lookedUpProduct;
		lookedUpProduct = 0;
		return;
	}

	// We have a new current product.
	setCurrentProduct(lookedUpProduct);

	currentProduct->setItemIsMealSideItem(true);

	currentProductCanAcceptWeight = true;
	Debug::trace("ProductManager.Product", "handleProductReadMessage(): currentProduct = " + ptrtostring(currentProduct));


	if ( bamAllSidesFree )
	{
		currentProduct->setUnitPrice(0);
										
		// This is from Product::initProductTransaction() and is needed to set the total price when the price is changed on a fixed weight item.
		if ( currentProduct->isByCount() || currentProduct->isFixedWtOrFluidOz() )
		{
			// SOF-2857 - Now that we've changed the unitPrice, so that the side item is free, we will call setOpModeByCount() for bycount items, to set the total price and update the transaction records.
			// The successful call to setOpModeByCount() here depends on the unitPriceChanged variable not being set to true, even though the unit price was just changed.
			// So don't change the code to call setUnitPriceChanged(true) here!

			if ( !currentProduct->isByCountForced() || (currentProduct->isByCountForced() && currentProduct->getByCount() > 0) )
				currentProduct->setOpModeByCount();

			if (currentProduct->isFixedWtOrFluidOz() && !currentProduct->isByCountForced())
				currentProduct->setOpModeFixedWt();
		}

		if ( currentProduct->getTareA().isForced() )
			(Configuration::getInstance()->toBoolCfg( fieldOffsetCfgUseSecondTare )) ? currentProduct->getTareB().setForced(false) : currentProduct->getTareA().setForced(false);
		if ( currentProduct->isUnitPriceForced() )
			currentProduct->setUnitPriceForced(false);
		if ( currentProduct->isCoolVerifyForced() )
			currentProduct->setCoolVerifyForced(false);
	}


	if ( mySession && mySession->isPrimarySession() )
	{
		DBProductStatisticsRM *dbProductStatisticsRM = dbAccess->getDBProductStatisticsRM();
		if (dbProductStatisticsRM)
		{
			std::string hour;
			std::string lang;

			//starting with Fastrack, these statistics aren't just for self-serve anymore

			// For Fastrack, we're not using a productStatistics hour method, use 0, which means to just accumulate all product stats into one bucket (for the plu)
			hour = "0";
			// For Fastrack, (non-selfServe), we're not separating the statistics for the language, so just use the primary language.
			lang = config->toStringCfg(fieldOffsetCfgPrimaryLanguageCode);

			if (traceOn)
				Debug::trace("ProductManager", "readProduct() - looking for prodStats record  hour:" + hour + "  lang:" + lang);

			if (hour.size() && lang.size())
			{
				edb::DBProductStatistic *myProductStatistic = dbProductStatisticsRM->find(currentProduct->getMyDbRecord(), lang, hour);
				if (!myProductStatistic)
				{
					HobDateTime dt;

					if (traceOn)
						Debug::trace("ProductManager", "readProduct() - prodStats record not found - creating it");
					myProductStatistic = dbProductStatisticsRM->createRecord(currentProduct->getMyDbRecord());
					myProductStatistic->setLanguage(lang);
					myProductStatistic->setHourNumber(hour);
					myProductStatistic->setTotalLabelsPrinted(0);
					myProductStatistic->setRecordedDateTime(dt.convertToDBFormat());
				}

				if (myProductStatistic)
				{
					// Have a statistics record.  Update it to indicate the number of times the record was read.
					// Save back to the database now in case the user doesn't print a label for this plu.
					int selections = (myProductStatistic->isTotalSelectionsNull()) ? 1 : myProductStatistic->getTotalSelections() + 1;
					if (traceOn)
						Debug::trace("ProductManager", HobString("readProduct() - prodStats updating selected:") + HobString(Integer::toString(selections)));
					myProductStatistic->setTotalSelections(selections);
					dbProductStatisticsRM->saveRecord(myProductStatistic);

					delete myProductStatistic;
				}
			}
		}

		// Send a Product Change message to the RTC so it knows it is dealing with a new plu.
		SessionMessage pluChg2(ProductDev, ProductChangeSmart);
		pluChg2.setFormatType(InternalFormat);
		HfcProductChangeMsg *hpcm2 = new HfcProductChangeMsg(mySession, this);

		//	We will send the ProductManager as well, so others can get the smart pointer to the product.
		if (hpcm2)
		{
			pluChg2.setHfcObject(hpcm2);
			mySession->send(&pluChg2, Messenger::DEST_RTC);
			delete hpcm2;
		}

		bamSidesLeft--;


		// Now add the side item to Meal
		// addCurrentItemToAccumOrMeal();
		Debug::trace("ProductManager", "addCurrentItemToAccumOrMeal() - calling printLabel");
		printLabel();


		// Send a message to the BuildAMeal mode to read the master trans so that the UI can be updated with a new total price,
		// for when sides are not free.
		SessionMessage readMasterTrans(ProductDev, ProductReadAccumulatedMasterTrans);
		readMasterTrans.setFormatType(InternalFormat);
		mySession->dispatchEvent(&readMasterTrans);
	}
}


void ProductManager::addCurrentItemToAccumOrMeal()
{
	// If there are any outstanding printer statuses, for any of the printers (and we are internally configured to not keep displaying them),
	// then redisplay those statuses, and leave; since we can't print with any printer being in some "error" state.
	Debug::trace("ProductManager", "addCurrentItemToAccumOrMeal() - will check status of printers");
	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return;

	if (Configuration::getInstance()->toBoolCfgI(CfgInternal::PRINTERSTATUSNONREPEATINGMSGSTOUI))
	{
		if ( checkPrinterStatuses() )
		{
			if ( s->getCurrentMode() == OpModeBuildAMeal )
				s->setCurrentModeOp(Session::OP_MEAL_WAITINGTORESOLVEPRINTERSTATUS);
			return;
		}
	}

	Debug::trace("ProductManager", "addCurrentItemToAccumOrMeal() - calling printLabel");
	printLabel();
}


void ProductManager::handleProductWriteMessage(Product *modifiedProduct)
{
	if ((!modifiedProduct) || (!currentProduct))
		return;

	Debug::trace("ProductManager.Product", "handleProductWriteMessage(): currentProduct = " + ptrtostring(currentProduct) + "  , modifiedProduct = " + ptrtostring(modifiedProduct));
  	currentProduct->updateRecord(modifiedProduct, mySession, true);

	Session *s = dynamic_cast<Session *> (mySession);
	if (s && s->isInOperateMode() && mySession->isPrimarySession())
	{
		// Send a Product Change message to the RTC so it knows it is dealing with a modified plu record.
		SessionMessage pluData(ProductDev, ProductChange);
		pluData.setFormatType(InternalFormat);
		HfcProductChangeMsg hpcm(mySession, currentProduct);
		pluData.setHfcObject(&hpcm);
		mySession->send(&pluData, Messenger::DEST_RTC);

		// Send a Product Change message to the RTC so it knows it is dealing with a new plu.
		SessionMessage pluData2(ProductDev, ProductChangeSmart);
		pluData2.setFormatType(InternalFormat);
		HfcProductChangeMsg *hpcm2 = new HfcProductChangeMsg(mySession, this);
		//	We will send the ProductManager as well, so others can get the smart pointer to the product.
		if (hpcm2)
		{
			pluData2.setHfcObject(hpcm2);
			mySession->send(&pluData2, Messenger::DEST_RTC);
			delete hpcm2;
		}
	}
}

void ProductManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	if (!dbAccess)
		return;

	bool usingCurrent = false;
	Product *msgProduct = dynamic_cast<Product *> (msg->getHfcObject());
	if (!msgProduct)
	{
		if (currentProduct)
			usingCurrent = true;
		else
			return;
	}

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Products))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		HobString serString = "";
		if (usingCurrent)
			serializeForSynchronization(serString, currentProduct, getDeleteEvent());
		else
			serString = msg->getOriginalBuff();
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getDeleteEvent(), serString));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		Product *existingProduct = (usingCurrent ? currentProduct : Product::findRecord(msgProduct->getProductNum(), msgProduct->getDeptNum(), dbAccess));
		if (existingProduct)
		{
			if (existingProduct->getProductNum() != RECORD_NOT_FOUND)
			{
				existingProduct->deleteRecord();
				Debug::trace("ProductManager", HobString("handleDelete - product deleted - productnum:") +
														HobString(Long::toString((long)existingProduct->getProductNum())));

				if (msg->getEventType() != ProductBatchApplyDelete)
				{
					// if received via UI, delete it and batch the record being deleted.
					if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Products))
					{
						std::string synchSer = "";
						serializeForSynchronization(synchSer, existingProduct, getDeleteEvent());

						synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

						if (proxy)
							proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
								hfc::SynchronizationChange(getDevice(), getDeleteEvent(), synchSer)));
					}
				}
			}
		}
		else
			Debug::trace("ProductManager", HobString("handleDelete - product to be deleted was not in the database to delete - productnum:") +
													HobString((existingProduct) ? Long::toString((long)existingProduct->getProductNum()) : "???"));

		if (existingProduct)
		{
			if (usingCurrent)
				clearCurrentProduct();
			else
				delete existingProduct;
		}
	}
}


void ProductManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void ProductManager::handleDeleteAll(HobRecordNum deptNum)
{
	Debug::trace("ProductManager", "handleDeleteAll - deleting all products");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Products))
	{
		// batch the delete all command; don't actually do it right now.
		SessionMessage message(getDevice(), getDeleteAllEvent());
		message.setFormatType(SynchronizationTaggedFormat);
		const char * buffer = message.serialize();

		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getDeleteAllEvent(), std::string(buffer, message.getSerializedLength())));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		if (!dbAccess)
			return;

		DBProductsRM *dbRM = dbAccess->getDBProductsRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));


		// Need to go through the Product Statistics records and delete any records for Product records that have been deleted,
		// since the Prouct Statistic's primary key field is the PLU_ID (prid), the internally generated primary key from the Products table.
		// And since we just deleted some Product records, there's no way to determine the PLU number (prNum) for which a particular Product Statistic
		// record was associated with, since that Product record no longer exists.
		DBProductStatisticsRM *dbProductStatisticsRM = dbAccess->getDBProductStatisticsRM();
		if (dbProductStatisticsRM)
		{
			try
			{
				DBQuery<DBProductStatistic>* allQuery = dbProductStatisticsRM->retrieveAll();
				if (!allQuery)
					return;

				DBQuery<DBProductStatistic>::const_iterator iter = allQuery->begin();
				while (iter != allQuery->end())
				{
					DBProductStatistic *dbPS = new DBProductStatistic(*iter);
					++iter;

					if (dbPS)
					{
						DBProduct *dbProd = dbRM->findLinkedRecord(dbPS);
						if (dbProd)
							// Product record exists in the database, so don't delete this ProductStatistic from the database.
							delete dbProd;
						else
						{
							// Product record DOES NOT exist in the database, so delete this ProductStatistic from the database.
							dbProductStatisticsRM->deleteRecord(dbPS);
							Debug::trace("ProductManager", string("handleDeleteAll - DBProductStatistic record deleted because it's corresponding DBProduct record was not found - prId ") + dbPS->getKey3());
						}
						delete dbPS;
					}
				}
				delete allQuery;
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "ProductManager::handleDeleteAll() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
			}
		}

		destroyProductsQueryAndIter();

#if 0
// This code existed when the SessionManager was responsible for handling the deleteAll in which we replaced the db file(s)
// for the record type with those from the empty database.  With the db's "zap" interface, this is no longer required.

		// Let the SessionManager know that this is to be done.
		if (mySession)
		{
			SessionMessage delAllMsg(ProductDev, ProductDeleteAll);
			delAllMsg.setFormatType(InternalFormat);
			mySession->send(&delAllMsg, Messenger::DEST_RTC);
		}

#endif
	}

	Debug::trace("ProductManager", "handleDeleteAll - leaving.");
}


void ProductManager::handleDeleteAllCommodityClass()
{
	Debug::trace("ProductManager", "handleDeleteAllCommodityClass - deleting all products and classes");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Products))
	{
		// batch the delete all command; don't actually do it right now.
		SessionMessage message(getDevice(), ProductDeleteAllCommodityClass);
		message.setFormatType(SynchronizationTaggedFormat);
		const char * buffer = message.serialize();

		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), ProductDeleteAllCommodityClass, std::string(buffer, message.getSerializedLength())));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		if (!dbAccess)
			return;

		DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
		if (!dbProductsRM)
			return;

		dbProductsRM->deleteAll();
		destroyProductsQueryAndIter();

		ClassManager* classManager = mgrContainer_->getClassManager();
		if ( classManager )
			classManager->handleDeleteAll();
	}

	Debug::trace("ProductManager", "handleDeleteAllCommodityClass - leaving.");
}

void ProductManager::handleUpdateSupervisorTransaction()
{
	Session *s = dynamic_cast<Session *> (mySession);
	bool isInSupervisorMode = ( s->isInSupervisor() ) ?  true : false;

	if (s && isInSupervisorMode && mySession->isPrimarySession() )
	{
		Configuration* config = Configuration::getInstance();
		bool isTrackingSuperChanges = config->toBoolCfg(fieldOffsetCfgTrackingSupervisorChanges);
		bool isInOperateMode    = ( s->isInOperateMode() ) ? true : false;

		if( isTrackingSuperChanges )
		{
			// we received  a 101,7, ie, ProductWrite, followed by 101,22 to Save and we are in Super and we are
			// tracking changes so lets make a call to save any transactional data if it has been changed.
			
			if( currentProduct == NULL )
				return;

			ProductTransaction *trans = (ProductTransaction *)NULL;
			trans = currentProduct->getCurrentTransaction(); 
				
			if (!trans || !trans->getProduct() )
				return;			

			Debug::trace("ProductManager.handleUpdateSupervisorTransaction", HobString("The message has a transaction") + HobString(Long::toString(trans->getTransactionNum())));
			Debug::trace("ProductManager.handleUpdateSupervisorTransaction", "The transaction has a product *");

			//	PTR 1662:	For a prepack scale, this is 1 package. Two or more printers is still 1 package.
			//				The printer manager increments the # of pkgs. for each label taken (HLX feature). 
			if (Configuration::getInstance()->isPrepackScale())
				trans->setNumberPackages(1);
				
			int modechange = 0;
			if( isInOperateMode )
	            modechange = hfc::OperatorChange::MC_OperateModeChange;		// 0 = Operate Mode Change
            else if( isInSupervisorMode )
	            modechange = hfc::OperatorChange::MC_SupervisorModeChange;	// 1 = Supervisor Mode Change

			int new_bc = currentProduct->getByCount();
			bool new_forcedbc = currentProduct->isByCountForced();
			HobPrice new_price = currentProduct->getUnitPrice();
            HobWeight new_fw = currentProduct->getFixedWeight();
			HobWeight new_tareA = currentProduct->getTareA().getValue();
			HobWeight new_tareB = currentProduct->getTareB().getValue();
			
			HobOperator* oper = s->getOperatorManager()->getLoggedInOperator();
			if ( trans && oper )
				trans->associateOperator(oper);

			trans->updateTransactionTotalValues();
			trans->updateSupervisorChangesValues(saveOffReadInPluHasForcedByCount, new_bc, new_forcedbc, new_tareA, new_tareB, new_fw, new_price, modechange);

			// attempt #2 - This was copied from handleUpdateAndSave and it was commented out there
			if( trans->getOperatorChangeType() > 0 )
				mgrContainer_->getTransactionManager()->handleSaveTransaction(trans);
		}
	}
}

void ProductManager::handleUpdateAndSave(Product *modifiedProduct)
{
	Debug::trace("ProductManager", "handleUpdateAndSave(): currentProduct = " + ptrtostring(currentProduct));
	
	if (currentProduct)
	{
		if (modifiedProduct)
		  	currentProduct->updateRecord(modifiedProduct);

		handleSaveRecord(currentProduct);

		//Session *s = dynamic_cast<Session *> (mySession);
	    //bool isInSupervisorMode = ( s->isInSupervisor() ) ?  true : false;
        //
		//if (s && isInSupervisorMode && mySession->isPrimarySession() )
		//{
		//	Configuration* config = Configuration::getInstance();
		//	bool isTrackingSuperChanges = config->toBoolCfg(fieldOffsetCfgTrackingSupervisorChanges);
        //
		//	if( isTrackingSuperChanges )
		//	{
		//		// we received  a 101,7, ie, ProductWrite, followed by 101,22 to Save and we are in Super and we are
		//		// tracking changes so lets make a call to save any transactional data if it has been changed.
		//	
		//		ProductTransaction *trans = (ProductTransaction *)NULL;
		//		trans = currentProduct->getCurrentTransaction(); 
		//		
		//		if (!trans || !trans->getProduct() )
		//			return;
		//		
		//		// test if there have been changes before saving. We dont want every transaction saved.
		//		if( trans->getOperatorChangeType() > 0 )
		//			mgrContainer_->getTransactionManager()->handleSaveTransaction(trans);
		//	}
		//}
	}
}

void ProductManager::handleSaveRecord(Product *recordToSave)
{
	Debug::trace("ProductManager", "handleSaveRecord(): recordToSave = " + ptrtostring(recordToSave));
	if (recordToSave)
	{
		if (recordToSave->getCategory() && recordToSave->getCategory()->getCategoryNum() != RECORD_NOT_FOUND)
			mgrContainer_->getCategoryManager()->handleSaveRecord(recordToSave->getCategory());

		if (recordToSave->getSpecialProgram() && recordToSave->getSpecialProgram()->getSpecialNum() != RECORD_NOT_FOUND)
			mgrContainer_->getSpecialsManager()->handleSaveRecord(recordToSave->getSpecialProgram());

		ProductText *prodText = 0;

		if (recordToSave->getNutrifacts() && recordToSave->getNutrifacts()->getTextNum() != RECORD_NOT_FOUND && recordToSave->getNutrifacts()->getErrorCode() == RECORD_OK)
			mgrContainer_->getNutrifactsManager()->handleSaveRecord(recordToSave->getNutrifacts());

		prodText = recordToSave->getProductText(psttExpandedText);
		if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
			mgrContainer_->getProductTextManager(psttExpandedText)->handleSaveRecord(prodText);

		prodText = recordToSave->getProductText(psttSpecialMessage);
		if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
			mgrContainer_->getSpecialMessageManager()->handleSaveRecord(prodText);

		prodText = recordToSave->getProductText(psttProductNote);
		if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
			mgrContainer_->getProductNoteManager()->handleSaveRecord(prodText);

		prodText = recordToSave->getProductText(psttMarqueeText);
		if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
			mgrContainer_->getMarqueeTextManager()->handleSaveRecord(prodText);

		prodText = recordToSave->getProductText(psttProductText5);
		if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
			mgrContainer_->getProductTextManager(psttProductText5)->handleSaveRecord(prodText);

		prodText = recordToSave->getProductText(psttProductText6);
		if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
			mgrContainer_->getProductTextManager(psttProductText6)->handleSaveRecord(prodText);

		prodText = recordToSave->getProductText(psttProductText7);
		if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
			mgrContainer_->getProductTextManager(psttProductText7)->handleSaveRecord(prodText);

		prodText = recordToSave->getProductText(psttProductText8);
		if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
			mgrContainer_->getProductTextManager(psttProductText8)->handleSaveRecord(prodText);

		prodText = recordToSave->getProductText(psttProductText9);
		if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
			mgrContainer_->getProductTextManager(psttProductText9)->handleSaveRecord(prodText);

		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Products))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, recordToSave, getWriteEvent());

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
		}
	}
}


void ProductManager::handleProductRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	Product *restoreProduct = dynamic_cast<Product *> (msg->getHfcObject());
	if (!restoreProduct || !dbAccess)
		return;

	if (restoreProduct->hasErrors())
	{
		if (mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = restoreProduct->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = restoreProduct->nextError();
			}
		}
		return;
	}

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Products))
	{
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		if ( msg->getFormatType() == UltimaTaggedFormat )
		{
			std::string reserialization;
			serializeForSynchronization(reserialization, restoreProduct);

			receivedChange = new hfc::SynchronizationChange(getDevice(), getWriteEvent(), reserialization);
		}
		else
			receivedChange = new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff());

		// batch the record being restored; don't actually write it to the database right now.
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		restoreProduct->setDBAccess(dbAccess);
		bool needToCreateNew = false;
		Product *usedProduct = (Product *)NULL;

		//find the product in the database and merge the fields
		Product *existingProduct = Product::findRecord(restoreProduct->getProductNum(), restoreProduct->getDeptNum(), dbAccess);
		if (existingProduct)
		{
			if (existingProduct->isRecordValid())
				usedProduct = existingProduct;
			else
			{
				//record not valid, delete and create a new record from defaults and save to database
				existingProduct->deleteRecord();
				needToCreateNew = true;
			}
		}
		else
			needToCreateNew = true;

		Product *newProduct = (Product *)NULL;
		if (needToCreateNew)
		{
			//product not found, create new product from defaults, merge and save to database
			Product *defProduct = ProductDefault::findRecord(restoreProduct->getProductType(), restoreProduct->getDeptNum(), dbAccess);
			if (!defProduct)
			{
				// Instead of not saving this new Product record to the database just because we don't have a ProductDefault record to set the Product's
				// attributes with default values which could happen with the multi-dept project, when this incoming, new Product is associated with
				// a dept that isn't in the database, just create a ProductDefault record from the default constructor, essentially a default ProductDefault.
				defProduct = new ProductDefault();
			}

			if (defProduct)		//don't do anything if we do not get a default
			{
				newProduct = new Product(defProduct, dbAccess, mySession);
				if (newProduct)
					usedProduct = newProduct;
				delete defProduct;
			}
		}

		if (usedProduct)
		{
			usedProduct->updateRecord(restoreProduct);

			if (usedProduct->getCategory() && usedProduct->getCategory()->getCategoryNum() != RECORD_NOT_FOUND)
				usedProduct->getCategory()->saveRecord();

			if (usedProduct->getSpecialProgram() && usedProduct->getSpecialProgram()->getSpecialNum() != RECORD_NOT_FOUND)
				usedProduct->getSpecialProgram()->saveRecord();

			if (usedProduct->getNutrifacts() && usedProduct->getNutrifacts()->getTextNum() != RECORD_NOT_FOUND && usedProduct->getNutrifacts()->getErrorCode() == RECORD_OK)
				usedProduct->getNutrifacts()->saveRecord();

			ProductText *prodText = 0;

			prodText = usedProduct->getProductText(psttExpandedText);
			if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
				prodText->saveRecord();

			prodText = usedProduct->getProductText(psttSpecialMessage);
			if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
				prodText->saveRecord();

			prodText = usedProduct->getProductText(psttProductNote);
			if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
				prodText->saveRecord();

			prodText = usedProduct->getProductText(psttMarqueeText);
			if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
				prodText->saveRecord();

			prodText = usedProduct->getProductText(psttProductText5);
			if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
				prodText->saveRecord();

			prodText = usedProduct->getProductText(psttProductText6);
			if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
				prodText->saveRecord();

			prodText = usedProduct->getProductText(psttProductText7);
			if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
				prodText->saveRecord();

			prodText = usedProduct->getProductText(psttProductText8);
			if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
				prodText->saveRecord();

			prodText = usedProduct->getProductText(psttProductText9);
			if (prodText && prodText->getTextNum() != RECORD_NOT_FOUND && prodText->getErrorCode() == RECORD_OK)
				prodText->saveRecord();

			usedProduct->saveRecord();
		}

		if (newProduct)
			delete newProduct;
		if (existingProduct)
			delete existingProduct;
	}
}


void ProductManager::handleProductCreateMessage(Product *msgProduct)
{
	if (!msgProduct || !dbAccess)
		return;

	//	Read that product default from the database based on the product type from the message.
	ProductDefault *productDefault = ProductDefault::findRecord(msgProduct->getProductType(), msgProduct->getDeptNum(), dbAccess);
	if (Debug::isTracing("New.ProductDefault"))
		Debug::trace("New.ProductDefault", "	ProductDefault: " + ptrtostring(productDefault) + " ProductManager::handleProductCreateMessage");

	if (productDefault)
	{
		//	Set the new product number
		productDefault->setProductNum(msgProduct->getProductNum());

		//	Create a new product with the Product default values.
		Product *newProduct = new Product(productDefault, dbAccess, mySession);
		if (newProduct)
		{
			newProduct->setErrorCode(RECORD_OK);
			newProduct->setDBAccess(dbAccess);

			//	Set this new product as the current product.
			setCurrentProduct(newProduct);

			Session *s = dynamic_cast<Session *> (mySession);
			if (s && s->isInOperateMode() && mySession->isPrimarySession())
			{
				// Send a Product Change message to the RTC so it knows it is dealing with a new plu.
				SessionMessage pluData(ProductDev, ProductChange);
				pluData.setFormatType(InternalFormat);
				HfcProductChangeMsg hpcm(mySession, currentProduct);
				pluData.setHfcObject(&hpcm);
				mySession->send(&pluData, Messenger::DEST_RTC);

				// Send a Product Change message to the RTC so it knows it is dealing with a new plu.
				SessionMessage pluData2(ProductDev, ProductChangeSmart);
				pluData2.setFormatType(InternalFormat);
				HfcProductChangeMsg *hpcm2 = new HfcProductChangeMsg(mySession, this);
				//	We will send the ProductManager as well, so others can get the smart pointer to the product.
				if (hpcm2)
				{
					pluData2.setHfcObject(hpcm2);
					mySession->send(&pluData2, Messenger::DEST_RTC);
					delete hpcm2;
				}
			}

			if (mySession)
			{
				// Send the new product to the Browser.
				SessionMessage pluData(ProductDev, ProductWrite);
				pluData.setFormatType(BrowserFormat);
				pluData.setHfcObject(currentProduct);
				mySession->send(&pluData, Messenger::DEST_UI);
			}

			handleSaveRecord(currentProduct);
		}

		delete productDefault;
	}
}


int ProductManager::determineRequiredAccessLevel(int fieldCode)
{
	int requiredAccessLevel = 1;
	Configuration* config = Configuration::getInstance();
	if (config)
	{
		if (fieldCode == Product::fieldOffsetUnitPrice)
			return config->toIntCfg(fieldOffsetCfgOAPriceAccessLevel);
		else if (fieldCode == Product::fieldOffsetByCount)
			return config->toIntCfg(fieldOffsetCfgOAByCountAccessLevel);
		else if (fieldCode == Product::fieldOffsetTareAValue || fieldCode == Product::fieldOffsetTareBValue)
			return config->toIntCfg(fieldOffsetCfgOATareAccessLevel);
		else if (fieldCode == Product::fieldOffsetFixedWeight)
			return config->toIntCfg(fieldOffsetCfgOAFixedWeightAccessLevel);
		else if (fieldCode == Product::fieldOffsetShelfLifeDays || fieldCode == Product::fieldOffsetShelfLifeHours || fieldCode == Product::fieldOffsetShelfLifeMinutes )
			return config->toIntCfg(fieldOffsetCfgOAShelfLifeAccessLevel);
		else if (fieldCode == Product::fieldOffsetProductLifeDays || fieldCode == Product::fieldOffsetProductLifeHours || fieldCode == Product::fieldOffsetProductLifeMinutes )
			return config->toIntCfg(fieldOffsetCfgOAProductLifeAccessLevel);
		else if (fieldCode == Product::fieldOffsetLabelTypeNum1 || fieldCode == Product::fieldOffsetLabelTypeNum2 || fieldCode == Product::fieldOffsetLabelTypeNum3)
			return config->toIntCfg(fieldOffsetCfgOALabelTypeAccessLevel);
		else if (fieldCode == Product::fieldOffsetManualLabelTypeNum1 || fieldCode == Product::fieldOffsetManualLabelTypeNum2 || fieldCode == Product::fieldOffsetManualLabelTypeNum3)
			return config->toIntCfg(fieldOffsetCfgOALabelTypeAccessLevel);
		else if (fieldCode == Product::fieldOffsetPrepackLabelTypeNum1 || fieldCode == Product::fieldOffsetPrepackLabelTypeNum2 || fieldCode == Product::fieldOffsetPrepackLabelTypeNum3)
			return config->toIntCfg(fieldOffsetCfgOALabelTypeAccessLevel);
		else if (fieldCode == Product::fieldOffsetProdEntryLabelTypeNum1 || fieldCode == Product::fieldOffsetProdEntryLabelTypeNum2 || fieldCode == Product::fieldOffsetProdEntryLabelTypeNum3)
			return config->toIntCfg(fieldOffsetCfgOALabelTypeAccessLevel);
		else if (fieldCode == Product::fieldOffsetSelfServLabelTypeNum1 || fieldCode == Product::fieldOffsetSelfServLabelTypeNum2 || fieldCode == Product::fieldOffsetSelfServLabelTypeNum3)
			return config->toIntCfg(fieldOffsetCfgOALabelTypeAccessLevel);

		else if (fieldCode == Product::fieldOffsetPortions)
			return config->toIntCfg(fieldOffsetCfgOAPortionsAccessLevel);
	}
	return requiredAccessLevel;
}

void ProductManager::continueProcessingWriteSingleField()
{
	// We were waiting for the user to login before processing the field update.
	// Now that they have logged in, go process the field update (saved in productUpdateWaitingForLogin) again.
	SessionMessage continueMsg(ProductDev, ProductWriteSingleField);
	continueMsg.setFormatType(BrowserFormat);
	continueMsg.setHfcObject(productUpdateWaitingForLogin);

	// Set up the message above, then make this call before processing the message,
	// to clear out the ProductManager's pointer for the saved WriteSingleField object;
	// the actual memory for the saved WriteSingleField object will be deleted when continueMsg goes out of scope - at the end of this function
	cancelWaitingForLogin();

	handleProductWriteSingleFieldMessage(&continueMsg);
}

void ProductManager::handleProductWriteSingleFieldMessage(SessionMessage *msg)
{
	if (!currentProduct || !dbAccess || !mySession)
		return;

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return;

	/* If we were waiting to get the data update to print the label, regardless of whether we now print or not,
	 * keep that we were in this mode, then indicate in the Session that we are no longer waiting to print the label.
	 */
	bool printKeyWasPressed = false;
	if (s->getCurrentModeOp() == Session::OP_PRINTKEYPRESSED)
	{
		printKeyWasPressed = true;
		s->setCurrentModeOp(Session::OP_UNKNOWN);
		Debug::trace("ProductManager", "printKeyWasPressed is true");
	}

	WriteSingleField *productUpdate = dynamic_cast<WriteSingleField *> (msg->getHfcObject());
	if (!productUpdate || ( productUpdate->getRecordNum() != currentProduct->getProductNum() && s->getCurrentMode() != OpModeBuildAMeal ) )
		return;

	HobString origValueString = productUpdate->getFieldOrigValue();
	HobString newValueString = productUpdate->getFieldNewValue();
	short field = productUpdate->getFieldCode();
	bool updateSuccessful = true;
	HobString productValue = "";

	/* The browser will send back a WriteSingleField message with offset of 0 to indicate that no field
	 * has changed when the PRINT key was pressed and the Session sent the browser the message to send
	 * over any field that was updated before the label is printed.  If this is the case, go ahead and print.
	 */

	bool isInOperateMode    = ( s->isInOperateMode() ) ? true : false;
	bool isInSupervisorMode = ( s->isInSupervisor() ) ?  true : false;
	bool isPrimarySession   = ( mySession->isPrimarySession() ) ? true : false;

	bool configTrackingSuperChanges   = config->toBoolCfg(fieldOffsetCfgTrackingSupervisorChanges);
	bool configTrackingOperateChanges = config->toBoolCfg(fieldOffsetCfgTrackingOperatorChanges);

	bool haveOutstandingForcedFields = (!currentProduct->areForcedFieldsEntered()) ? true : false;
	
	// If this field is one that could require an operator to be logged in and have a certain access level to change it,
	// and an operator is not logged in, tell the UI to ask for a login.
	// We save the productUpdate so it can be applied to the current product once the operator logs in and we validate their access level.
	// I am taking possession of the productUpdate object from the SessionMessage msg, so it doesn't delete it when the SessionMessage goes out of scope.
	// If I don't apply the productUpdate later, I will delete it.

	int requiredAccessLevel = determineRequiredAccessLevel(productUpdate->getFieldCode());

	// FT-545: the forced login prompt should only happen in operate mode and only when no more forced fields.
	if (( isInOperateMode ) && ( !haveOutstandingForcedFields ) )
	{
		if (requiredAccessLevel > 1 && s->getOperatorManager()->countAllOperators() > 0)
		{
			if (s->getOperatorManager()->getLoggedInOperator() == 0)
			{
				waitingForLoginBeforeWriteSingleField = true;
				productUpdateWaitingForLogin = productUpdate;
				msg->setHfcObjectCreatedInternally(false);

				SessionMessage msg(OperatorDev, OpPromptForIDOrRFIDForFieldAccess);
				msg.setFormatType(BrowserFormat);
				mySession->send(&msg, Messenger::DEST_UI);
				return;
			}
			else if (s->getOperatorManager()->getLoggedInOperator()->getAccessLevel() < requiredAccessLevel)
			{
				mySession->addDialogMessage(ErrOperatorDev, OperatorErrAccessDenied, HfcResponse::RESPONSESTYLE_PROMPTOK);
				return;
			}
		}
	}

	bool fieldUpdated = true;
	bool clearingByCount = false;

	if (field > 0)
	{
		if (field == Product::fieldOffsetSellByDate || field == Product::fieldOffsetBestBeforeDate)
		{
			//determine the date passed in (could be in one of two formats
			origValueString = "";
			currentProduct->getFieldAsString(field, origValueString, BrowserFormat);

			HobDateTime newDate;
			HobString newDateString = newValueString;
			Debug::trace("ProductManager", HobString("updating sell by date/best before date - string passed in is: ") + newDateString);
			// Let the HobDateTime class determine the format.
			newDate.setDateParts(newDateString);

			if (!newDate.isValid())
			{
				Debug::trace("ProductManager", HobString("updating sell by date/best before date - invalid date string passed in: ") + newDateString);
				SessionMessage fieldUpdatedMsg(ProductDev, ProductWriteSingleField);
				fieldUpdatedMsg.setFormatType(BrowserFormat);
				WriteSingleField fieldUpdate(0, field, origValueString);
				fieldUpdatedMsg.setHfcObject(&fieldUpdate);
				mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
				return;
			}

			newDateString = newDate.formatDate(FormatDate_MMDDYY);
			Debug::trace("ProductManager", HobString("updating sell by date/best before date - new string determined to be: ") + newDateString);
			Debug::trace("ProductManager", HobString("updating sell by date/best before date - sell by data about to be updated to: ") + newDateString);
			newValueString = newDateString;
		}
		else if (field == Product::fieldOffsetExceptionPrice || field == Product::fieldOffsetUnitPrice ||
				field == Product::fieldOffsetDiscountValue1 || field == Product::fieldOffsetDiscountValue2 || field == Product::fieldOffsetDiscountValue3)
		{
			if (!newValueString.empty())
			{
				string::size_type pos = newValueString.find(config->getMonetarySymbol());
				if (pos == 0)
					newValueString = newValueString.substr(config->getMonetarySymbol().size());
			}
		}
		else if (field == Product::fieldOffsetByCount &&
					(currentProduct->isByCount() || currentProduct->isFixedWtOrFluidOz()) &&
					Integer::valueOf(newValueString) == 0)
		{
			if ( config->isSelfServiceScale() )
			{
				clearingByCount = true;

				// Get the by count from the database record again, so we can reset it in the current product to the db value.
				currentProduct->setByCount(currentProduct->getOriginalByCount());
				currentProduct->setByCountEntered(false);

				// Set up the writeSingleField object with the by count from the database and let it fall through
				// the remainder of this method so the product data and the pricing information get updated.
				currentProduct->getFieldAsString(Product::fieldOffsetByCount, origValueString, BrowserFormat);
				productUpdate->setFieldOrigValue(origValueString);
				productUpdate->setFieldNewValue(origValueString);
			}
			else
			{
				// by count can't be 0 for bycount and fixed weight items.
				Debug::trace("ProductManager", "by count can't be 0 for bycount and fixed weight items");
				SessionMessage fieldUpdatedMsg(ProductDev, ProductWriteSingleField);
				fieldUpdatedMsg.setFormatType(BrowserFormat);
				WriteSingleField fieldUpdate(0, field, origValueString);
				fieldUpdatedMsg.setHfcObject(&fieldUpdate);
				mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
				
				if( isInOperateMode )				// in Supervisor we want to track this. Likely its forced
					return;
			}
		}
		else if (field == Product::fieldOffsetBackDateDate)
		{
			//determine the date passed in (could be in one of two formats
			origValueString = "";
			currentProduct->getFieldAsString(field, origValueString, BrowserFormat);

			HobDateTime newDate;
			HobString newDateString = newValueString;

			// Let the HobDateTime class determine the format.
			newDate.setDateParts(newDateString);

			if (!newDate.isValid())
			{
				SessionMessage fieldUpdatedMsg(ProductDev, ProductWriteSingleField);
				fieldUpdatedMsg.setFormatType(BrowserFormat);
				WriteSingleField fieldUpdate(0, field, origValueString);
				fieldUpdatedMsg.setHfcObject(&fieldUpdate);
				mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
				return;
			}

			newDateString = newDate.formatDate(FormatDate_MMDDYY);
			newValueString = newDateString;
		}
		else if ( field == Product::fieldOffsetLabelTypeNum1 || field == Product::fieldOffsetLabelTypeNum2 || field == Product::fieldOffsetLabelTypeNum3 )
		{
			// We will still receive these 3 field offsets when the operator changes a label type number in Operate mode,
			// so we need to remap them to the appropriate active label type offset for the current mode
			PrinterType printerNum = FirstPrinter;

			if (field == Product::fieldOffsetLabelTypeNum2)
				printerNum = SecondPrinter;
			else if (field == Product::fieldOffsetLabelTypeNum3)
				printerNum = ThirdPrinter;

			field = currentProduct->getActiveLabelTypeNumOffsetForMode(printerNum);
		}

		Debug::trace("ProductManager", HobString("updating field: ") + HobString(Integer::toString(field)) + HobString("  with new value: *") + newValueString + HobString("*"));
		// The updateField() method will return non-0 if an error occurred updating the field.
		if (field == Product::fieldOffsetShelfLifeDays)
			currentProduct->adjustShelfLifeDays(Integer::valueOf(newValueString));
		else if (field == Product::fieldOffsetShelfLifeHours)
		{
			if (isInOperateMode)
			currentProduct->adjustShelfLifeHours(Integer::valueOf(newValueString));
			else
				currentProduct->setShelfLifeHours(Integer::valueOf(newValueString));
		}
		else if (field == Product::fieldOffsetShelfLifeMinutes)
		{
			// This IF is true if we implement in OperateMode - For now comment out
			//if ( isInOperateMode )
			//	currentProduct->adjustShelfLifeMinutes( Integer::valueOf(newValueString) );
		    // else
			currentProduct->setShelfLifeMinutes( Integer::valueOf(newValueString) );
		}
		else if ( clearingByCount )
			;	// I've already taken care of updating the product, so don't call updateField(); but I want it to fall through to the rest of the processing.
		else if ((field == Product::fieldOffsetTareAValue || field == Product::fieldOffsetTareBValue) && isInOperateMode)
		{
			//	PTR 827, 828, 831: We need to take into account a TareEntryPrecision of 2 also.
			//		It is the ProductErrTareInvalid error message that triggers the browser to put up the keypad
			//		for Tare entry again. That error message is sent only from this section of code.
			//		NOTE: These same bugs also happen on the original Windows HLX. [CCD 7/2011]
			Debug::trace("ProductManager.updateTare", HobString("handleProductWriteSingleFieldMessage - updating field: ") + HobString(Integer::toString(field)) + HobString("  with new value: *") + newValueString + HobString("*"));
			if (config->getTareEntryPrecision() == 2)
			{
				Debug::trace("ProductManager.updateTare", "handleProductWriteSingleFieldMessage - TareEntryPrecision is 2, so just update the tare value ");
				fieldUpdated = currentProduct->updateField(field, newValueString, true);
			}
			else
			{
				// check if passing in 3 decimal places for tare, but the thousandth's place is not a 0 or 5; if so, then error
				// I didn't want to put this in the updateField method in Product, because we also want to set the tare to
				// whatever value is sent in via comm, or backup/restore, without rejecting it.

				//	PTR 827, 828, 831: Note also that if a Prepack weigher with 2 decimal precision is attached
				//		and TareEntryPrecision is set to 3, that third precision digit must always be 0. It is
				//		better to change TareEntryPrecision to 2 if that is the case. [CCD 7/2011]
				//char lastDigit = newValueString.at(newValueString.size()-1); -- JHC: on gcd merge with UnitedPMR1, added next line. Keep since could be issue.
				const char lastDigit( newValueString.at( newValueString.size() - 1 ) );
				const size_t digits( (std::string::npos != newValueString.find( "." )) ? newValueString.size() - newValueString.find( "." ) - 1 : 0 );
				const bool isScaleMetric(MetSig::isScaleMetric() );
				
				UINT32 scaleFactorUsedForValidation( 1 );
				if ( currentProduct->getCurrentTransaction() && currentProduct->getCurrentTransaction()->getWeigherStatus() )
					scaleFactorUsedForValidation = currentProduct->getCurrentTransaction()->getWeigherStatus()->getEffectiveScaleFactor();
				else
					scaleFactorUsedForValidation = config->getEffectiveWeigherScaleFactor();

				Debug::trace( "ProductManager.updateTare", string("handleProductWriteSingleFieldMessage - lastDigit: ") + lastDigit + "  digits: " + inttostring(digits) + "  isScaleMetric: " + booltostring(isScaleMetric) + "  scaleFactor: " + inttostring((int)scaleFactorUsedForValidation) );

				// FT-1073: only validate when 3 digits out and _not_ dual range down, otherwise will round
				//
				// mode    digits factor validate? notes
				// ------- ------ ------ --------- -------------------------------------------------------------
				// avoir   3      5      YES       dual range UP
				// avoir   3      1      no        dual range DOWN (will be rounded)
				// avoir   2      n/a    no        okay in both dual range UP/DOWN, and for non-dual range scale
				// metric  3      2      YES       dual range UP
				// metric  3      5      no        dual range DOWN (will be rounded), non-dual range scale
				//
				if (isScaleMetric)
				{
					// Note, for metric, digits should always be 3!
					if ( (scaleFactorUsedForValidation == 5 && (lastDigit == '0' || lastDigit == '5')) ||
					     (scaleFactorUsedForValidation == 2 && (lastDigit == '0' || lastDigit == '2' || lastDigit == '4' || lastDigit == '6' || lastDigit == '8')) )
						fieldUpdated = currentProduct->updateField(field, newValueString, true);
					else
						fieldUpdated = false;
				}
				else  // avoir
				{
					if ( digits == 2 || 
					     (digits == 3 && scaleFactorUsedForValidation == 5 && (lastDigit == '0' || lastDigit == '5')) ||
					     (digits == 3 && scaleFactorUsedForValidation == 1 && lastDigit == '0') )
						fieldUpdated = currentProduct->updateField(field, newValueString, true);
					else
						fieldUpdated = false;
				}

				/* the old code that was replaced with what is directly above
				if (( digits == 3 && scaleFactorUsedForValidation == 5 && (lastDigit == '0' || lastDigit == '5' || isScaleMetric) ) ||
					( digits == 3 && scaleFactorUsedForValidation == 2 && (lastDigit == '0' || lastDigit == '2' || lastDigit == '4' || lastDigit == '6' || lastDigit == '8') ) ||
					( digits == 3 && scaleFactorUsedForValidation == 1 && !isScaleMetric ) ||
					( digits == 2 && !isScaleMetric ))
				{
					fieldUpdated = currentProduct->updateField( field, newValueString, true );
				}
				else
				{
					fieldUpdated = false;
					// Send a message to browser indicating the Tare is invalid.
				}
				*/
			}

			if (fieldUpdated == false)
			{
				Debug::trace("ProductManager.updateTare", "handleProductWriteSingleFieldMessage - value did not pass validation");
				mySession->addDialogMessage(ErrProductDev, ProductErrTareInvalid, HfcResponse::RESPONSESTYLE_OK);
			}
		}
		else
			fieldUpdated = currentProduct->updateField(field, newValueString, true);

		if (field == Product::fieldOffsetSellByDate)
			productValue = currentProduct->getShelfLife().formatDate(FormatDate_MMDDYY);
		else if (field == Product::fieldOffsetBestBeforeDate)
			productValue = currentProduct->getProductLife().formatDate(FormatDate_MMDDYY);
		else if (field == Product::fieldOffsetBackDateDate)
			productValue = currentProduct->getBackdateDate().formatDate(FormatDate_MMDDYY);
		else  
			currentProduct->getFieldAsString(field, productValue, ((isInOperateMode) ? BrowserFormatPLUOperateMode : BrowserFormat));
		
		//PTR #608
		if ( (field >= Product::fieldOffsetGraphicName1) && (field <= Product::fieldOffsetGraphicName4) )
		{
			if ( newValueString == config->toStringCfgI(CfgInternal::NOGRAPHICSELECTEDNAME) )
			{
				newValueString = ""; //clear it out, so it will save correctly below	
			}
		}

		Debug::trace("ProductManager", HobString("updated field: ") + HobString(Integer::toString(field)) + HobString("  with new value: *") + productValue + HobString("*"));
	}

	if (field == 0 || productValue == newValueString || fieldUpdated || field == Product::fieldOffsetKeyboardNetWeight /*|| DBG_SLA_CHECK*/)
	{
		// The new value was successfully changed in the current product.

		if (s
			// Too many fields affect the product's total price, net weight, etc,
			// so just send the request for the weigher status.
			/*&& (field == fieldOffsetByCount || field == fieldOffsetExceptionPrice || field == fieldOffsetUnitPrice)*/)
		{
			/*
						if (isInOperateMode)
						{
							s->requestAWeigherStatus();
						}
			*/

			/* Tied to allowing keyboard net weight Plu to switch to random weight by pressing Cancel in the "Enter keyboard net weight" dialog,
			 * (fieldOffsetAllowKeyboardNetWeightItemAsRandomWeight), but we're not currently moving forware with that right now, so I'm commenting this out.
			if ( isInOperateMode && field == Product::fieldOffsetKeyboardNetWeight )
				s->requestAWeigherStatus();
			 */

			 // User entered a unit price; don't care if it's different, it was entered.
			if (field == Product::fieldOffsetUnitPrice)
				currentProduct->setUnitPriceChanged(true);

			if (isInOperateMode && field == Product::fieldOffsetNumOfLabels && currentProduct->getCurrentTransaction() && currentProduct->getCurrentTransaction()->getNumOfLabelsToPrint() == 0)
			{
				// If we're in Print Multiple Labels mode, leave that mode.
				Debug::trace("ProductManager.remotePrint", "CALL handleLeavePrintMultipleLabelsMode() via handleSingleFieldWrite()");

				handleLeavePrintMultipleLabelsMode();
			}

			if (isInOperateMode || isInSupervisorMode)
			{
				if (isPrimarySession && !config->isSelfServiceScale())
				{

					if (isInOperateMode && configTrackingOperateChanges)
					{
						ProductTransaction* currentTransaction = currentProduct->getCurrentTransaction();

						//	Record the operator change.
						//	The only exception to this is if the PLU is a bycount or fixed weight and the field is Unit price.
						//	That's because the total price was really changed and we don't need a duplication of changes.
						if (field > 0 && !(currentProduct->isByCount() && field == Product::fieldOffsetUnitPrice) &&
							!(currentProduct->isFixedWtOrFluidOz() && field == Product::fieldOffsetUnitPrice) &&
							!(currentProduct->isUnitPriceForced() && field == Product::fieldOffsetUnitPrice) &&
							!(currentProduct->isByCountForced() && field == Product::fieldOffsetByCount) &&
							!(currentProduct->getTareA().isForced() && (field == Product::fieldOffsetTareAValue || field == Product::fieldOffsetTareBValue)))
						{
							// This is the standard before I added tracking Supervisor Changes.  Operator Changes, generally, should hit this.
							if (currentTransaction)
								currentTransaction->addOperatorChange((Product::ProductFieldOffsets)productUpdate->getFieldCode(), productUpdate->getFieldOrigValue(), productValue);
						}
					}

					if (isInSupervisorMode && configTrackingSuperChanges)
					{
						ProductTransaction* currentTransaction = currentProduct->getCurrentTransaction();

						//	Record the operator change.
						// dont track everything all willy nilly
						if (field == Product::fieldOffsetUnitPrice || field == Product::fieldOffsetTareAValue || field == Product::fieldOffsetTareBValue ||
							field == Product::fieldOffsetByCount || field == Product::fieldOffsetFixedWeight)
						{
							if (currentTransaction)
								currentTransaction->addOperatorChange((Product::ProductFieldOffsets)productUpdate->getFieldCode(), productUpdate->getFieldOrigValue(), productValue);
						}
					}

				}

				//	Did any of the label types change?
				if (Product::isLabelTypeNumField(field))
				{
					if (isInOperateMode) // SOF-2999: Un-commented this 'if' statement based on Larry's comments below ...
					{
						// We receive Write Single Field messages when editing PLU's in Supervisor mode, particularly for label type numbers.
						// We don't need to do all of the setup for label type changes as done for Operate mode; we just need to check if the
						// label type exists or not, which was done in the call to Product->updateField above, so we can skip all of this.

						// TODO - Larry - done - depends on how we group the new labels with the one to one printer:label thing?
						// A label type number has changed, so re-setup the LabelType objects that will be used for printing.

						//currentProduct->selectLabelTypesForMode(s->getCurrentMode(), isRemotePrintMsgReceived);
						//currentProduct->setupLabelTypesForMode();

						short printer = getPrinterNumFromFieldOffset(field);		//	Printer 0, 1, or 2

						HobRecordNum newLabelTypeNum = currentProduct->getLabelTypeNum(field);
						LabelType* newLabelType = LabelType::findRecord(newLabelTypeNum, currentProduct->getDeptNum(), dbAccess);
						currentProduct->setLabelType(printer, newLabelType);

						bool needToReImageLabels = true;
						if (newLabelType)
						{
							LabelType* pCorrectedLabelType = 0;
							bool labelInvalid = false;
							bool oldHasFrequentShopperLabel = currentProduct->hasFrequentShopperLabel();

							LabelMediaReturn result = labelTypeErrorTest(newLabelType, &pCorrectedLabelType, printer, currentProduct->getDeptNum());

							if (result == LabelOk || result == LabelCorrected)
							{
								if (result == LabelCorrected)
								{
									//	Set the corrected label type in the product record before we check the fields on the label.
									Debug::trace("ProductManager.ToggleLabel", HobString("handleProductWriteSingleFieldMessage():  LabelCorrected  - Found a label in the family to fit stock: PrinterNum = ") + HobString(Integer::toString(printer)) + HobString("	OrigNum = ") + HobString(Long::toString(newLabelType->getLabelTypeNum())) + HobString("	NewNum = ") + HobString(Long::toString(pCorrectedLabelType->getLabelTypeNum())));

									currentProduct->setLabelType(printer, pCorrectedLabelType);
								}
								//	Check for Portions and/or a keyboard net weight label (Has FORCED_KB_WEIGHT instead of NET_WEIGHT)
								currentProduct->checkSpecialFieldsOnLabel();

								if (currentProduct->hasKbdNetWeightLabel())
								{
									if (currentProduct->isRandomWeight())
									{
										if (config->isPrepackScale())
										{
											switchModeForKeyboardNetWt();
										}
										else if ((config->isServiceCounterScale()) && (s->getCurrentMode() == OpModeAutoManualApply) && (s->getCurrentMode() != OpModeAutoManualApply))
										{
											// HT-317 Keyboard Entered Weight will not print a label
											switchModeForKeyboardNetWt();
										}
										else if ((s->getCurrentMode() != OpModeAutoManualApply) && (s->getCurrentMode() != OpModeAutoManualApply))
										{
											// HT-317 kinda overrides FT-503 some Keyboard Entered Weight will not print a label
											//	This error only applies to a non-Prepack scale.
											// FT-503 this should be auto manual (prepack) as per Nigel's interpretation of W&M
											mySession->addDialogMessage(ErrProductDev, ProductErrOnlyForPrepackMode, HfcResponse::RESPONSESTYLE_OK);
										}
									}
									else
									{
										mySession->addDialogMessage(ErrProductDev, ProductErrLabelOnlyForRandomWeight, HfcResponse::RESPONSESTYLE_OK);
										labelInvalid = true;
										needToReImageLabels = false;
									}
								}
								else if (config->isPrepackScale())
								{
									switchModeBackAfterKeyboardNetWt();
								}
								else if (config->isServiceCounterScale())
								{
									// HT-317 Keyboard Entered Weight will not print a label
									switchModeBackAfterKeyboardNetWt();
								}

								if (labelInvalid)
								{
									currentProduct->setLabelType(printer, 0);
									needToReImageLabels = false;

									//	Update productUpdate
									productValue = "0";
									productUpdate->setFieldNewValue(productValue);
								}
								else if (result == LabelOk || result == LabelCorrected)
								{
									if (result == LabelCorrected)
									{
										//	Update productUpdate with the corrected label type
										currentProduct->getFieldAsString(field, productValue, BrowserFormat);
										productUpdate->setFieldNewValue(productValue);

										// Send a message to browser indicating the Label Type was changed.
										mySession->addDialogMessage(ErrProductDev, (ProductErrPriLabelTypeChanged + printer), HfcResponse::RESPONSESTYLE_OK);
									}

									currentProduct->testForFrequentShopperLabel();
									if (oldHasFrequentShopperLabel != currentProduct->hasFrequentShopperLabel())
									{
										// Send the modified hasFreqShopper flag back to the browser.
										HobString boolString = "";
										currentProduct->getFieldAsString(Product::fieldOffsetLabelHasFreqShopper, boolString, BrowserFormat);
										WriteSingleField* changedFlagField = new WriteSingleField(1, Product::fieldOffsetLabelHasFreqShopper, boolString);
										if (changedFlagField)
										{
											SessionMessage fieldUpdatedMsg(ProductDev, ProductWriteSingleField);
											fieldUpdatedMsg.setFormatType(BrowserFormat);
											fieldUpdatedMsg.setHfcObject(changedFlagField);
											mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);

											delete changedFlagField;
										}
									}
								}
							}
							else
							{
								//	There is an error but the Error message was displayed already in labelTypeErrorTest().
								updateSuccessful = false;
								needToReImageLabels = false;
							}
						}

						if (needToReImageLabels)
						{
							if (isPrimarySession && isInOperateMode)
							{
								newLabelType = currentProduct->getLabelType(printer);
								if (newLabelType)
								{
									Debug::trace("ProductManager.embeddedLabels", "Generating Embedded labels for current product's label " +
										inttostring(printer + 1) + " because it changed");
									newLabelType->generateEmbeddedLabels();
									Debug::trace("ProductManager.HandleWriteSingleField", HobString("sending ProductChange message to load a new label   mode=") + HobString(Integer::toString(s->getCurrentMode())));
								}

								// Send a Product Change message to the RTC so it knows the Imager needs to image the new label.
								SessionMessage pluChg(ProductDev, ProductChangeSmart);
								pluChg.setFormatType(InternalFormat);
								HfcProductChangeMsg hpcm(mySession, this);
								pluChg.setHfcObject(&hpcm);
								mySession->send(&pluChg, Messenger::DEST_RTC);
							}
						}
					}

				}

				saveCOOLInfoToProduct(field);	//	Check to see if we need to save COOL Product changes to the database.
			}

			if (field > 0 && field != Product::fieldOffsetSellByDate && field != Product::fieldOffsetBestBeforeDate)
			{
				// Send the FIELD-UPDATED-SUCCESSFULLY message back to the browser.
				SessionMessage fieldUpdatedMsg(ProductDev, ProductWriteSingleField);
				fieldUpdatedMsg.setFormatType(BrowserFormat);

				// The record number (plu number), in the ProductWriteSingleField message sent back to the browser,
				// is being used to indicate whether the change was successful(1) or not(0).
				if (updateSuccessful)
					productUpdate->setRecordNum(1);
				else
					productUpdate->setRecordNum(0);

				if (field == Product::fieldOffsetNumOfLabels && updateSuccessful)
				{
					if (util_string::stringtoint(newValueString) > 0)
						setHasProductionEntryItems(true);
				}

				productUpdate->setFieldNewValue(productValue);

				fieldUpdatedMsg.setHfcObject(productUpdate);
				mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);

				if (!updateSuccessful)
					ServiceSpeaker::badBeep(1);
			}

			if (isInOperateMode)
			{
				if (field == Product::fieldOffsetExpirationDays && isPrimarySession)
				{
					// Number of days for a new expiration label have been entered.
					// Can now go ahead and print the expiration label.
					mgrContainer_->getExpirationLabelManager()->handleProductPrintNewExpirationLabel(currentProduct);
				}

				if (field == Product::fieldOffsetByCount)
				{
					Debug::trace("ProductManager", HobString("handleProductWriteSingleFieldMessage()1 - unitprice: ") + HobString(Long::toString(currentProduct->getUnitPrice())));
					if (currentProduct->getCurrentTransaction())
					{
						if ((currentProduct->isByCount() || currentProduct->isFixedWtOrFluidOz()))
						{
							currentProduct->setOpModeByCount();
							Debug::trace("ProductManager", HobString("handleProductWriteSingleFieldMessage()2 - unitprice: ") + HobString(Long::toString(currentProduct->getUnitPrice())));
							if (currentProduct->isFixedWtOrFluidOz())
							{
								currentProduct->setOpModeFixedWt();
								Debug::trace("ProductManager", HobString("handleProductWriteSingleFieldMessage()3 - unitprice: ") + HobString(Long::toString(currentProduct->getUnitPrice())));
							}
						}
					}
					//	Update the page with the new ByCount value
					if (currentProduct->isByCountWithWeight())
					{
						if (s)
							s->requestAWeigherStatus();
					}
					//					else
					{
						ProductTransaction* currentTransaction = currentProduct->getCurrentTransaction();
						if (currentTransaction)
						{
							//	ByCount has changed, so update the FS discounts.
							currentTransaction->calculateFSDiscounts(currentProduct);

							// Send the product total data back to the Browser.
							HfcProductUpdateValuesMsg hpuvm(currentTransaction);
							updateBrowserAndCustomerDisplay(hpuvm);

							// PJR-383
							// Use this block if we only want to tell the UI to display a barcode image when the weight is stable (no small motion)
							if (mySession->isPrimarySession())
								imageSelfServeBarcodeLabel(hpuvm);
						}
					}
				}
				else
					updateWeightAndNotify();
			}

			if (field == Product::fieldOffsetByCount && currentProduct->isFixedWtOrFluidOz() && !isInSupervisorMode)
			{
				//	Update the product with the new Fixed weight.
				SessionMessage fieldUpdateMsg(ProductDev, ProductWriteSingleField);
				fieldUpdateMsg.setFormatType(BrowserFormat);

				HobString temp;
				int format = BrowserFormat;
				if (isInOperateMode)
					format = BrowserFormatPLUOperateMode;
				currentProduct->getFieldAsString(Product::fieldOffsetFixedWeight, temp, format);

				WriteSingleField fixedWtUpdate(1, Product::fieldOffsetFixedWeight, temp);
				fieldUpdateMsg.setHfcObject(&fixedWtUpdate);
				mySession->send(&fieldUpdateMsg, Messenger::DEST_UI);
			}

			if (isInOperateMode)
			{
				// SOF-4904 Add shelfLifeMinutes to product and transactions 
				// TODO jhc - might have to rip all of the minutes code out here as this might be restricted to editing in Operate Mode. 
				// Or leave it here for future implementations just not implemented in Operate yet

				if (field == Product::fieldOffsetShelfLifeDays || field == Product::fieldOffsetSellByDate || field == Product::fieldOffsetShelfLifeHours || field == Product::fieldOffsetShelfLifeMinutes ||
					field == Product::fieldOffsetProductLifeDays || field == Product::fieldOffsetBestBeforeDate || field == Product::fieldOffsetProductLifeHours || field == Product::fieldOffsetProductLifeMinutes ||
					field == Product::fieldOffsetBackDateDays || field == Product::fieldOffsetBackDateDate)
				{
					SessionMessage fieldUpdatedMsg(ProductDev, ProductWriteSingleField);
					fieldUpdatedMsg.setFormatType(BrowserFormat);
					HobString temp;

					if (field == Product::fieldOffsetSellByDate || field == Product::fieldOffsetShelfLifeHours)
					{
						//	Send the updated Shelf life days
						currentProduct->getFieldAsString(Product::fieldOffsetShelfLifeDays, temp, BrowserFormat);
						WriteSingleField shelfLifeDaysUpdate(1, Product::fieldOffsetShelfLifeDays, temp);
						fieldUpdatedMsg.setHfcObject(&shelfLifeDaysUpdate);
						mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
					}

					if (field == Product::fieldOffsetShelfLifeDays || field == Product::fieldOffsetSellByDate || field == Product::fieldOffsetShelfLifeHours || field == Product::fieldOffsetShelfLifeMinutes ||
						field == Product::fieldOffsetBackDateDays || field == Product::fieldOffsetBackDateDate)
					{
						//	Send the updated Sell by date
						currentProduct->getFieldAsString(Product::fieldOffsetSellByDate, temp, BrowserFormat);
						WriteSingleField fieldUpdate(1, Product::fieldOffsetSellByDate, temp);
						fieldUpdatedMsg.setHfcObject(&fieldUpdate);
						mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
					}

					if (field == Product::fieldOffsetShelfLifeHours || field == Product::fieldOffsetShelfLifeMinutes)
					{
						//	Send the updated Sell by time
						currentProduct->getFieldAsString(Product::fieldOffsetSellByTime, temp, BrowserFormat);
						WriteSingleField sellByTimeUpdate(1, Product::fieldOffsetSellByTime, temp);
						fieldUpdatedMsg.setHfcObject(&sellByTimeUpdate);
						mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
					}

					if (field == Product::fieldOffsetShelfLifeDays || field == Product::fieldOffsetSellByDate || field == Product::fieldOffsetShelfLifeHours || field == Product::fieldOffsetShelfLifeMinutes ||
						field == Product::fieldOffsetBestBeforeDate || field == Product::fieldOffsetProductLifeHours || field == Product::fieldOffsetProductLifeMinutes)
					{
						//	Send the updated Product life days
						currentProduct->getFieldAsString(Product::fieldOffsetProductLifeDays, temp, BrowserFormat);
						WriteSingleField productLifeDaysUpdate(1, Product::fieldOffsetProductLifeDays, temp);
						fieldUpdatedMsg.setHfcObject(&productLifeDaysUpdate);
						mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
					}

					if (field == Product::fieldOffsetShelfLifeDays || field == Product::fieldOffsetSellByDate || field == Product::fieldOffsetShelfLifeHours || field == Product::fieldOffsetShelfLifeMinutes ||
						field == Product::fieldOffsetProductLifeDays || field == Product::fieldOffsetBestBeforeDate || field == Product::fieldOffsetProductLifeHours || field == Product::fieldOffsetProductLifeMinutes ||
						field == Product::fieldOffsetBackDateDays || field == Product::fieldOffsetBackDateDate)
					{
						//	Send the updated Best before date
						currentProduct->getFieldAsString(Product::fieldOffsetBestBeforeDate, temp, BrowserFormat);
						WriteSingleField bestBeforeDateUpdate(1, Product::fieldOffsetBestBeforeDate, temp);
						fieldUpdatedMsg.setHfcObject(&bestBeforeDateUpdate);
						mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
					}

					if (field == Product::fieldOffsetShelfLifeHours)
					{
						//	Send the updated Product life hours - jhc - why are we testing for shelf life hours, and sending product life hours?
						currentProduct->getFieldAsString(Product::fieldOffsetProductLifeHours, temp, BrowserFormat);
						WriteSingleField productLifeHoursUpdate(1, Product::fieldOffsetProductLifeHours, temp);
						fieldUpdatedMsg.setHfcObject(&productLifeHoursUpdate);
						mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
					}

					if (field == Product::fieldOffsetShelfLifeMinutes)
					{
						//	Send the updated Product life minutes
						currentProduct->getFieldAsString(Product::fieldOffsetProductLifeMinutes, temp, BrowserFormat);
						WriteSingleField productLifeMinutesUpdate(1, Product::fieldOffsetProductLifeMinutes, temp);
						fieldUpdatedMsg.setHfcObject(&productLifeMinutesUpdate);
						mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
					}

					if (field == Product::fieldOffsetShelfLifeHours || field == Product::fieldOffsetProductLifeHours || field == Product::fieldOffsetProductLifeMinutes)
					{
						//	Send the updated Best before time
						currentProduct->getFieldAsString(Product::fieldOffsetBestBeforeTime, temp, BrowserFormat);
						WriteSingleField bestBeforeTimeUpdate(1, Product::fieldOffsetBestBeforeTime, temp);
						fieldUpdatedMsg.setHfcObject(&bestBeforeTimeUpdate);
						mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
					}

					if (field == Product::fieldOffsetBackDateDays)
					{
						//	Send the updated Packed on time
						currentProduct->getFieldAsString(Product::fieldOffsetBackDateDate, temp, BrowserFormat);
						WriteSingleField backdateDateUpdate(1, Product::fieldOffsetBackDateDate, temp);
						fieldUpdatedMsg.setHfcObject(&backdateDateUpdate);
						mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
					}

					if (field == Product::fieldOffsetShelfLifeDays || field == Product::fieldOffsetSellByDate || field == Product::fieldOffsetShelfLifeHours || field == Product::fieldOffsetShelfLifeMinutes)
					{
						currentProduct->setDontPrintSellBy(false);
						currentProduct->setDontPrintSellByOverridden(true);

						currentProduct->getFieldAsString(Product::fieldOffsetDontPrintSellBy, temp, BrowserFormat);
						WriteSingleField dontPrintFlagUpdate(1, Product::fieldOffsetDontPrintSellBy, temp);
						fieldUpdatedMsg.setHfcObject(&dontPrintFlagUpdate);
						mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
					}

					if (field == Product::fieldOffsetProductLifeDays || field == Product::fieldOffsetBestBeforeDate || field == Product::fieldOffsetProductLifeHours || field == Product::fieldOffsetProductLifeMinutes)
					{
						currentProduct->setDontPrintBestBefore(false);
						currentProduct->setDontPrintBestBeforeOverridden(true);

						currentProduct->getFieldAsString(Product::fieldOffsetDontPrintBestBefore, temp, BrowserFormat);
						WriteSingleField dontPrintFlagUpdate(1, Product::fieldOffsetDontPrintBestBefore, temp);
						fieldUpdatedMsg.setHfcObject(&dontPrintFlagUpdate);
						mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
					}
				}

				//	We need to re-preimage if we are in one of the Operate modes.
				if ((isPrimarySession && isInOperateMode))
				{
					if (Product::isFieldPreImage(field))
					{
						Debug::trace("ProductManager.HandleWriteSingleField", HobString("sending message to re-preimage   mode=") + HobString(Integer::toString(s->getCurrentMode())));
						// Send a Product Change message to the RTC so it knows the Imager needs to be updated.
						//	A new label type - the Imager needs to image the new label.
						//	A new graphic - the Imager needs to pre-image the new graphic.
						SessionMessage pluChg(ProductDev, ProductChange);
						pluChg.setFormatType(InternalFormat);
						HfcProductChangeMsg hpcm(mySession, currentProduct);
						pluChg.setHfcObject(&hpcm);
						mySession->send(&pluChg, Messenger::DEST_RTC);

						SessionMessage pluChg2(ProductDev, ProductChangeSmart);
						pluChg2.setFormatType(InternalFormat);
						HfcProductChangeMsg hpcm2(mySession, this);
						pluChg2.setHfcObject(&hpcm2);
						mySession->send(&pluChg2, Messenger::DEST_RTC);
					}

					//	LabelPlacement/Rotation
					if (
						field == Product::fieldOffsetLabelRotation1 ||
						field == Product::fieldOffsetLabelRotation2 ||
						field == Product::fieldOffsetLabelPlacement1 ||
						field == Product::fieldOffsetLabelPlacement2
						)
					{
						// check if we're on a two printer system
						if (FpmManager::getInstance()->isWrapperTwoPrinter())
						{
							Debug::trace("ProductManager.HandleWriteSingleField", HobString("Label/Rotation Change on Two Label Applier System"));

							UINT16 primaryLength = 0;
							UINT16 secondaryLength = 0;
							UINT   secondaryLabelRotated = 0;
							LaPosLoc primaryLocation = (LaPosLoc)currentProduct->getLabelPlacement(FirstPrinter);
							LaPosLoc secondaryLocation = (LaPosLoc)currentProduct->getLabelPlacement(SecondPrinter);

							HfcPrinterStockMetrics* primaryPrinterStockMetrics = s->getPrinterStockMetrics(0);
							HfcPrinterStockMetrics* secondaryPrinterStockMetrics = s->getPrinterStockMetrics(1);

							if (primaryPrinterStockMetrics)
							{
								if (primaryPrinterStockMetrics->getLabelLengthInLines() > 0)
								{
									primaryLength = primaryPrinterStockMetrics->getLabelLengthInLines();
									Debug::trace("ProductManager.HandleWriteSingleField", HobString("Primary Label Length: ") + HobString(Integer::toString(primaryLength)));
								}
								else
								{
									Debug::trace("ProductManager.HandleWriteSingleField", "Primary Label Length <= 0");
								}
							}
							else
							{
								Debug::trace("ProductManager.HandleWriteSingleField", "primaryPrinterStockMetrics is NULL");
							}

							if (secondaryPrinterStockMetrics)
							{
								if (secondaryPrinterStockMetrics->getLabelLengthInLines() > 0 && secLabelTypeEnable)
								{
									if (currentProduct->getLabelType(SecondPrinter) == 0)
										secondaryLength = 0;
									else
										secondaryLength = secondaryPrinterStockMetrics->getLabelLengthInLines();

									secondaryLabelRotated = Configuration::getInstance()->toIntCfg(fieldOffsetCfgSecondaryLabelStockRotated);
									Debug::trace("ProductManager.HandleWriteSingleField", HobString("Secondary Label Length final check: ") + HobString(Integer::toString(secondaryLength)));
								}
								else
								{
									Debug::trace("ProductManager.HandleWriteSingleField", "Secondary Label Length <= 0");
								}
							}
							else
							{
								Debug::trace("ProductManager.HandleWriteSingleField", "secondaryPrinterStockMetrics is NULL");
							}

							LabelApplierPositionInfo newMsg(
								mySession,
								0, // could really be anything here
								currentProduct->getLabelRotation(FirstPrinter),
								currentProduct->getLabelRotation(SecondPrinter),
								primaryLocation,
								secondaryLocation,
								primaryLength,
								secondaryLength,
								secondaryLabelRotated
							);

							mySession->send(&newMsg, Messenger::DEST_RTC);
						}
						else
						{
							Debug::trace("ProductManager.HandleWriteSingleField", "Label/Rotation Change on One Printer System");
							short printer = ((field == Product::fieldOffsetLabelRotation2) ? SecondPrinter : FirstPrinter);
							LabelApplierPositionInfo newMsg(mySession, printer,
								currentProduct->getLabelRotation(FirstPrinter),
								currentProduct->getLabelRotation(SecondPrinter));

							if (field == Product::fieldOffsetLabelPlacement1)
							{
								//	Only set label placement if it has changed here. More so for the case of the AWS.
								//	Any CLA or UWS LA must set up label placement in Label applier configuration in Service.
								newMsg.setLabelPosition((LaPosLoc)currentProduct->getLabelPlacement(FirstPrinter));
							}

							mySession->send(&newMsg, Messenger::DEST_RTC);
						}
					}
				}


				if (field == Product::fieldOffsetKeyboardNetWeight)
				{
					Debug::trace("ProductManager", "fieldOffset KeyboardNetWeight");
					if ( !currentProduct->hasKbdNetWeightLabel() )	//fieldOffsetAcceptKbdNetWeight:
					{
						Debug::trace("ProductManager", "fieldOffset AcceptKbdNetWeight");

/*  I tried to just send an update to the fieldOffsetAcceptKbdNetWeight field to tell the UI to not treat the plu as if it was a keyboard net weight PLU,
*   so that it would redisplay the "Manual Weight" field as just "Net Weight", but this didn't work, so I had to resend the entire PLU record back to the UI
						// The record number (plu number), in the ProductWriteSingleField message sent back to the browser,
						// is being used to indicate whether the change was successful(1) or not(0).
						productUpdate->setRecordNum(1);
						productUpdate->setFieldCode(Product::fieldOffsetAcceptKbdNetWeight);
						productUpdate->setFieldNewValue("0");

						// Send the FIELD-UPDATED-SUCCESSFULLY message back to the browser.
						SessionMessage fieldUpdatedMsg(ProductDev, ProductWriteSingleField);
						fieldUpdatedMsg.setFormatType(BrowserFormat);
						fieldUpdatedMsg.setHfcObject(productUpdate);
						mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
*/

						//	Resend the current Product to the Browser, so the label data can be updated on the page.
						SessionMessage pluData(ProductDev, ProductWrite);
						pluData.setFormatType(BrowserFormat);
						pluData.setHfcObject(currentProduct);
						mySession->send(&pluData, Messenger::DEST_UI);

						if (s)
							s->requestAWeigherStatus();
					}
				}
			}

			if (Product::isLabelTypeNumField(field))
				//if (field == Product::fieldOffsetLabelTypeNum1 || field == Product::fieldOffsetLabelTypeNum2 || field == Product::fieldOffsetLabelTypeNum3)
			{
				// Todo Larry - done - How to manage the multiple labels?

				//	Update the barcode information, since the label type changed.
				BarCodeNumber tempBarCode = currentProduct->getBarCodeNum();
				tempBarCode.getFromDatabase(currentProduct->getMyDbRecord(), currentProduct, dbAccess);
				currentProduct->setBarCodeNum(tempBarCode);

				/* With the multiple barcodes per label project, the following is no longer needed.  The call to getFromDatabase() directly above takes care of it for you.
				 * Left it in here for now, for reference, and in case I need to reactivate it for some reason.
				if (s->isInSupervisor())
				{
					//	Validate the barcode prefix, in case the barcode type changed (Changing from EAN to UPC).
					int tempPrefix = Integer::valueOf(tempBarCode.getPrefix());		// off of what label for this printer?
					if (tempBarCode.getBarcodeType() == UPCA)
					{
						if ((currentProduct->isRandomWeight() || currentProduct->isByCount()) && (tempPrefix != 2) && (tempPrefix != 4))
							currentProduct->getBarCodeNum().setPrefix("2");
						else if (currentProduct->isFixedWeight() || currentProduct->isFluidOunce())
						{
							 if ((tempPrefix == 3) || (tempPrefix == 5) || (tempPrefix < 0) || (tempPrefix > 9))
								currentProduct->getBarCodeNum().setPrefix("0");
						}
					}
				}
				*/

				if (isInOperateMode)
				{
					//	Resend the current Product to the Browser, so the label data can be updated on the page.
					SessionMessage pluData(ProductDev, ProductWrite);
					pluData.setFormatType(BrowserFormat);
					pluData.setHfcObject(currentProduct);
					mySession->send(&pluData, Messenger::DEST_UI);

					updateWeightAndNotify(0, true);
				}
			}
		}

		if (isInOperateMode && isPrimarySession)
		{
			/* The following is for when either the operator pressed the print button or a stable weight was found and we're
			 * in prepack mode, and the Session sent the UI the message to send over any updates to any field for which the
			 * operator didn't press Enter to commit that field change.  When the UI sends back the WriteSingleField message,
			 * if it sends back a field with it's new data, the UI implies it is OK to also print it.  If it sends back no field,
			 * fieldCode of 0, then we use the 'shouldAlsoPrint' field of the WriteSingleField message to determine if it is OK
			 * to print; the UI is indicating we are on a page where we are allowed to print.
			 */
			if ( printKeyWasPressed )
			{
				// With Fastrack, the UI is the one that is keeping what page it is on,
				// so we are using the WriteSingleField message to tell the app to print.
				if ( field > 0 || (field == 0 && productUpdate->isShouldAlsoPrint()) )
				{
					Debug::trace("ProductManager", "printKeyWasPressed and UI responded that printing is OK, so call handleProductPrintKey2");

					handleProductPrintKey2();
				}
				else
				{
					Debug::trace("ProductManager", "printKeyWasPressed and UI responded that printing is NOT OK");

					// SOF-5499
					// clear outstanding transcations in prepack mode that did not print due to printer/product issues
					if ( OpModeProductionEntry == s->getCurrentMode() )
						if (( currentProduct ) && ( !currentProduct->isPrintingInProductionEntryMode() ) && ( !currentProduct->isDummyProduct() ))
						{
							if ( 0 < currentProduct->numOutstandingTransactions() )
							{
								Debug::trace("ProductManager", "removing most recent transaction" );
								currentProduct->removePreviousTransaction();

								// cancel previous label/transaction to print
								if ( mySession )
								{
									SessionMessage msg(ProductDev, ProductRemovePreviousTransaction);
									msg.setFormatType(InternalFormat);
									mySession->send(&msg, Messenger::DEST_RTC);
								}
							}
						}

				}
			}
			else if ( productUpdate->isShouldAlsoPrint() )
			{
				Debug::trace("ProductManager", "handleProductWriteSingleFieldMessage - should also print");
                if( field != 0 ) //HA-69: was not sent from the UI
				    handleProductPrintKey1();
			}
			else
			{
				// The following will check if we should print in "label" mode (semi-weigh mode) if we already have a stable weight on the weigher,
				// but we needed some input from the user that prevented us from printing when we originally received the stable weight, such as
				// if we had forced fields for which the user had to enter the data.
				if (isPrimarySession && isInOperateMode && haveOutstandingForcedFields && currentProduct->areForcedFieldsEntered())
				{
					Debug::trace("ProductManager.HandleWriteSingleField", "handleProductWriteSingleFieldMessage - check if we should print, due to previous stable weight");
					if (s)
						s->requestAWeigherStatus();
				}
			}
		}
	}
	else
	{
		// The new value was NOT successfully changed in the current product.
		Debug::trace("ProductManager", "update field failure");

		//currentProduct->getError(productUpdate->getFieldCode());

		if (Product::isLabelTypeNumField(field))
		{
			//	If a labelType was invalid, we need to send a message. Get the label from the database because, if there was
			//	a problem, the original labelType would be in the product.
			// TODO - Larry - done - how to manage multiple labels on one printer?
			HobRecordNum labelTypeNum = (HobRecordNum)Long::valueOf(productUpdate->getFieldNewValue());
			LabelType *pLabelType = LabelType::findRecord(labelTypeNum, currentProduct->getDeptNum(), dbAccess);
			LabelType *pCorrectedLabelType = 0;

			LabelMediaReturn lmr = labelTypeErrorTest(pLabelType, &pCorrectedLabelType, getPrinterNumFromFieldOffset(field), currentProduct->getDeptNum());		//	Printer 0, 1, or 2
			if (pLabelType)
				delete pLabelType;
			if (lmr == LabelCorrected && pCorrectedLabelType)
				delete pCorrectedLabelType;
		}
		else if (field == Product::fieldOffsetProductNum)
		{
			if (currentProduct->getProductNum() != RECORD_NOT_FOUND)
			{
				if (Debug::isTracing("ProductManager.renumber"))
					Debug::trace("ProductManager.renumber", HobString("Duplicate Product number ") + HobString(productUpdate->getFieldNewValue()));
				//	We need this delay here, otherwise, this error window will go away as if something was entered.
				//	Something to do with back to back modal prompts requiring input.
//				Sleep( 60);	//	Tried these already: 50, 45, 30, 60, 300, 3000
				mySession->addDialogMessage(ErrProductDev, ProductErrRenumberDuplicateRecord, HfcResponse::RESPONSESTYLE_OK);
			}
		}

		// Return an error message to the Browser, setting the value back to the old value.
		SessionMessage fieldNotUpdatedMsg(ProductDev, ProductWriteSingleField);
		fieldNotUpdatedMsg.setFormatType(BrowserFormat);
		productUpdate->setRecordNum(0);
		productUpdate->setFieldNewValue(origValueString);	//	Return the original value.
		fieldNotUpdatedMsg.setHfcObject(productUpdate);
		mySession->send(&fieldNotUpdatedMsg, Messenger::DEST_UI);

		ServiceSpeaker::badBeep(1);
	}
}


void ProductManager::handleWeight(HfcWeigherStatus *weight)
{
	//Debug::trace("ProductManager.SelfService", "Session::handleWeight() START");

	// There is no need to process the weight if we're not in Operate mode.  i.e. - don't update
	// product total values (used for printing the label) if we're editing the product in Supervisor mode.

	Session *s = dynamic_cast<Session *> (mySession);
	bool isInOperateMode = (s && s->isInOperateMode()) ? true : false;

	if (!s || !isInOperateMode)
		return;

	if (!weight)
	{
		Debug::trace("ProductManager.HandleWeight", "HfcWeigherStatus is NULL; it will be ignored");
		return;
	}

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	bool isTracing = Debug::isTracing("ProductManager.HandleWeight");

	if (!currentProductCanAcceptWeight)
	{
		if (isTracing)
			Debug::trace("ProductManager.HandleWeight", "current product is not accepting weight");

		if (weight && weight->isPeripheralComm() && weight->isZeroStableWeight())
			currentProductCanAcceptWeight = true;
		else
			return;
	}

	if (!currentProduct)
	{
		if (isTracing)
			Debug::trace("ProductManager.HandleWeight", HobString("There is no current product to update with the new weight!!"));
	}
	else
	{
		if ( processingKeyboardNetWt && !currentProduct->isCancelledKbdNetWeight() )	//	HA-180: Prevents the Ghost package error.
			return;

		if (currentProduct->isPrintingInProductionEntryMode())
			return;

		// Don't want to process a weight when we're waiting for the customer to take the label
		// in self-serve mode with weight required to select an item (Walmart Chile)
		if ( s->getCurrentMode() == OpModeSelfService )
		{	
			if ( config->toBoolCfg(fieldOffsetCfgSelfServeWeightRequiredToSelectItem ) )
				Debug::trace("ProductManager.HandleWeight", " in selfservice mode and IS isConfiguredForSelfServeWeightRequired");
			else
				Debug::trace("ProductManager.HandleWeight", " in selfservice mode and NOT isConfiguredForSelfServeWeightRequired");

			if ( config->toBoolCfg(fieldOffsetCfgSelfServeEnablePlaylists ) )
				Debug::trace("ProductManager.HandleWeight", " in selfservice mode and IS isConfiguredForSelfServeEnablePlaylists");
			else
				Debug::trace("ProductManager.HandleWeight", " in selfservice mode and NOT isConfiguredForSelfServeEnablePlaylists");

			if ( ( hasPrintingProduct() || currentProduct->hasOutstandingTransactions() ) && isTracing )
				Debug::trace("ProductManager.HandleWeight", "in selfservice mode and hasPrintingProduct()");
			
			if (currentProduct->hasOutstandingTransactions() && isTracing)
				Debug::trace("ProductManager.HandleWeight", "in selfservice mode and hasOutstandingTransactions()");

			if (config->toBoolCfg(fieldOffsetCfgSelfServePrintOnStableWeight) && (hasPrintingProduct() || currentProduct->hasOutstandingTransactions()))
			{
				if (isTracing)
					Debug::trace("ProductManager.HandleWeight", HobString("Printing label when we received a weight - skipping the weight."));
				return;
			}
		}

		if (isTracing)
			Debug::trace("ProductManager.HandleWeight", HobString("Updating current product with new weight"));

	// have a new weight, update the current product.
		ProductTransaction *currentTransaction = currentProduct->getCurrentTransaction();
		if (!currentTransaction)
		{
			if (isTracing)
				Debug::trace("ProductManager.HandleWeight", HobString("The product does not have a transaction object to update with the new weight!!"));
		}
		else
		{
			updateWeightAndNotify(weight, false);


			if (config->isSelfServiceScale() && (weight->isStableWeight() || weight->isSmallMotion()) && s->getResponseMessageManager())
				s->getResponseMessageManager()->manageSelfServeMessages();

			/* Check if we should also print the label now.
			 * If we're in Manual Weigh mode, don't print the label since the user hasn't pressed the print key.
			 * If we're in Auto- or Semi-Auto mode, the stable weight is the indicator to go ahead and
			 * print the label.
			 *
			 * Also, if we are waiting for this final weigher status before printing the label,
			 * now that we have it, we can print.
			 */
			const HfcWeigherStatus *transactionWeight = currentTransaction->getWeigherStatus();
			OpModeEvent currMode = s->getCurrentMode();


			if ( config->toBoolCfg(fieldOffsetCfgRequireFixedWeightVerification) &&
			        currentProduct->isFixedWeight() &&
			        currentProduct->getFixedWeight() > 0 )
			{
				if ( transactionWeight && transactionWeight->isPeripheralComm() && !transactionWeight->isManagerDeleted() &&
				     !transactionWeight->isWeigherInError() && !transactionWeight->isScaleWarmup() &&
				     ( transactionWeight->isPositiveNonZeroStableWeight() || transactionWeight->isZeroStableWeight() ) )
				{
					bool removedWeightNeededDialog = s->getResponseMessageManager()->removeMessage(ErrProductDev, ProductErrFixedWeightVerificationNeedWeight, HfcResponse::RESPONSESTYLE_OK);
					bool removedWeightInsufficientDialog = s->getResponseMessageManager()->removeMessage(ErrProductDev, ProductErrFixedWeightVerificationInsufficientWeight, HfcResponse::RESPONSESTYLE_OK);

					HobWeight weight = transactionWeight->getWeight();
					Debug::trace( "ProductManager.HandleWeight", string("fixedWeightVerify - current weight on platter is: ") + Long::toString(weight) );

					HobWeight fweight = MetSig::fixedWeightToWeightCounts(currentProduct->getFixedWeight());
					Debug::trace( "ProductManager.HandleWeight", string("fixedWeightVerify - fixed weight of product is: ") + Long::toString(fweight) );

					if ( weight <= 0 )
					{
						if ( currMode == OpModeManualWeigh && (removedWeightNeededDialog || removedWeightInsufficientDialog) )
							s->getResponseMessageManager()->addMessage(ErrProductDev, ProductErrFixedWeightVerificationNeedWeight, HfcResponse::RESPONSESTYLE_OK);
					}
					else if ( weight > 0 && weight < fweight &&
					          ( currMode == OpModeAutoWeigh || currMode == OpModeSemiWeigh || currMode == OpModeAutoManualApply ||
					            waitingForStableWeightToPrint || removedWeightNeededDialog || removedWeightInsufficientDialog ) )
					{
						HobList msgData;
						HobString data;

						HobWeight fweightCurr = MetSig::weightCountsToFixedWeight(weight);
						if (currentProduct->isFluidOunce())
							config->formatVolume(fweightCurr, data, FixedWeightText, true);
						else
							config->formatWeight(fweightCurr, data, true, FixedWeightText);

						msgData.push_back(data);
						s->getResponseMessageManager()->addMessage(ErrProductDev, ProductErrFixedWeightVerificationInsufficientWeight, HfcResponse::RESPONSESTYLE_OK, &msgData);
						return;
					}
					else
					{
						bool restartPrint = ( currMode == OpModeManualWeigh && (removedWeightNeededDialog || removedWeightInsufficientDialog) );
						bool restartAddToAccum = ( currMode == OpModeAccumulateItems && (removedWeightNeededDialog || removedWeightInsufficientDialog) );

						s->setCurrentModeOp(Session::OP_UNKNOWN);
						s->getResponseMessageManager()->removeMessage(ErrProductDev, ProductErrFixedWeightVerificationNeedWeight, HfcResponse::RESPONSESTYLE_OK);
						s->getResponseMessageManager()->removeMessage(ErrProductDev, ProductErrFixedWeightVerificationInsufficientWeight, HfcResponse::RESPONSESTYLE_OK);

						if ( restartPrint )
						{
							handleProductPrintKey1();
							return;
						}
						else if ( restartAddToAccum )
						{
							SessionMessage msg(ProductDev, ProductAddItemToAccumulatedItems);
							msg.setFormatType(BrowserFormat);
							handleMessage(&msg);
							return;
						}
					}
				}
			}

			/* Walmart Chile requirement */
			bool mode_good = false;
			if( waitingForStableWeightToPrint || ( OpModeAutoWeigh == currMode ) || ( OpModeSemiWeigh == currMode ) || (OpModeAutoManualApply == currMode ) )
				mode_good = true;

			bool config_good = false;
			if (config->toBoolCfg(fieldOffsetCfgSelfServePrintOnStableWeight))
			{
				config_good = true;
				Debug::trace("ProductManager.SelfService", " Weight Required so config_good: true");
            }

			if ((mode_good) || (config_good && (currMode == OpModeSelfService) && (!currentProduct->isByCount())))	// Walmart Chile
			{
				bool okToPrint = true;
				if (labelMediaCheck() != LabelOk)
				{
					okToPrint = false;
				}
				else if (!currentProduct->isReadyToPrint())
				{
					okToPrint = false;

					// FT-??? fix crash
//METSIG??  Do we want to consider all of the weigher statuses and weight > minWeightToPrint to be MetSig
					if (( transactionWeight ) && ( transactionWeight->isStableWeight() ))
						if (( currentTransaction ) && ( 0 < currentTransaction->getTotalNetWeight() ) && ( currentTransaction->getTotalNetWeight() < Configuration::getInstance()->getMinWeightToPrint() ))
						{
							Debug::trace("ProductManager.HandleWeight", HobString("total weight is > 0 and less than min to print!! - total net weight: ") + HobString(Long::toString(currentTransaction->getTotalNetWeight())));							
							// Dispatch this event to notify any listener that we won't be printing due to total net weight
							// being less than the minimum weight to print. (specifically, a wrapper)
							ProductStatusMsg psm(mySession, currentProduct, currentTransaction, ProductStatusMsg::WeightBelowMinimumWeightToPrint);
							mySession->send(&psm, Messenger::DEST_RTC, false);

							// display error dialog to operator if in prepack mode
							if(currMode == OpModeAutoManualApply)
							{
								ProductDevError errCode = currentProduct->getNotReadyToPrintErrorCode();
								mySession->addDialogMessage(ErrProductDev, errCode, HfcResponse::RESPONSESTYLE_OK);
							}
						}
				}

				 // even if the product doesn't need a weight on the scale (by-count) normally,
				 // in auto- or semi-auto- weigh modes, there does need to be a stable weight on the weigher.
				else if (!transactionWeight)
				{
					okToPrint = false;
				}
				else if (transactionWeight->isWeigherInError())
				{
					okToPrint = false;
				}
				else if (!transactionWeight->isStableWeight())
				{
					okToPrint = false;
				}
				 // enabled the following line so that you have to have a weight on the weigher,
				 // even for by count or fixed weight items, when in auto or semi-weigh modes.
				// Random weight and bycount-with-weight items have to be over the minimum weight to print limits.
				// All other (fixed weight, fluid ounce, and bycount (non-weight-on-a-bycount) items can have any weight, but it has to be > 0.
				// Also for these other items, since the transaction is not updated with the weight on the platter, we have to look at the
				// weigher status object directly.
//METSIG??  Do we want to consider all of the weigher statuses and weight > minWeightToPrint to be MetSig
				else if ( ((currentProduct->isRandomWeight() || currentProduct->isByCountWithWeight()) && currentTransaction->getTotalNetWeight() < config->getMinWeightToPrint()) ||
							 transactionWeight->isZeroStableWeight() )
				{
					Debug::trace("ProductManager.HandleWeight", HobString("ztotal weight is less than min to print, but is ready to print!!"));
					Debug::trace("ProductManager.HandleWeight", HobString("ztotal net weight: ") + HobString(Long::toString(currentTransaction->getTotalNetWeight())));
					okToPrint = false;
					if ( (currentProduct->isRandomWeight() || currentProduct->isByCountWithWeight()) && transactionWeight->isStableWeight() && currentTransaction->getTotalNetWeight() > 0)
					{
						// Dispatch this event to notify any listener that we won't be printing due to total net weight
						// being less than the minimum weight to print. (specifically, a wrapper)
						ProductStatusMsg psm(mySession, currentProduct, currentTransaction, ProductStatusMsg::WeightBelowMinimumWeightToPrint);
						mySession->send(&psm, Messenger::DEST_RTC, false);
					}
				}
				else if ((SessionManager::getInstance()->isWrapperAttached() /* ||
							(SessionManager::getInstance()->isCLAAttached() && s->getCurrentMode() != OpModeAutoManualApply) */ )
						   && weight->getEventType() != WgStatusAndPrint)
				{
					Debug::trace("ProductManager.HandleWeight", HobString("Have a wrapper or a CLA, but weigher event is not WgStatusAndPrint, so not printing label!!"));
					okToPrint = false;
				}
				else if ((config->toBoolCfg(fieldOffsetCfgOperateLoginRequired) ||
						  (config->toBoolCfg(fieldOffsetCfgTrackingOperatorChanges) && currentProduct->getCurrentTransaction()->hasOperatorChangedData())) &&
						 s && s->getOperatorManager()->countAllOperators() > 0 && s->getOperatorManager()->getLoggedInOperator() == 0 &&
						 currentProduct->getCurrentTransaction())
				{
					// Note: For Fastrack, the check Configuration::getInstance()->isRFIDHWPresent() was removed because logging in when printing
					// is now the standard, we no longer have the operator login at the beginning of entering Operate Mode.
					//
					// Operator login is required when they press print if tracking transactions is enabled, regardless of whether they changed
					// any field or not, or if the operator has changed something in the current transaction and "tracking operator changes" is set.

					waitingForLoginBeforePrinting = true;

					SessionMessage msg(OperatorDev, OpPromptForIDOrRFID);
					msg.setFormatType(BrowserFormat);
					mySession->send(&msg, Messenger::DEST_UI);

					okToPrint = false;
				}
				else if (currentProduct->hasKbdNetWeightLabel())
				{
					okToPrint = false;
				}
				/* Walmart Chile requirement */
				else if ( ( currMode == OpModeSelfService ) && ( config_good ) && ( currentProduct->isByCount() ) )
				{
					// don't auto print yet, let the user finish entering all digits in the by count, then accept when they press print
					okToPrint = false;
				}

				if (okToPrint)
				{
					//Debug::trace("ProductManager.SelfService", " since okToPrint call prepareToPrint() to PRINT" );
					currentProduct->setLabelBeingPrinted(Product::PRINTING_PRODUCTION_LABEL);
					s->prepareToPrint();
				}
			}
		}
	}
	//Debug::trace("ProductManager.SelfService", "Session::handleWeight() END");
}


void ProductManager::updateWeightAndNotify(HfcWeigherStatus *weight, bool forceNotification)
{
	// if (currentProduct)	/* replaced this line with the next 8-02-07 per keyboard net weight */
	if (currentProduct && !(currentProduct->hasKbdNetWeightLabel() && currentProduct->isRandomWeight() && (currentProduct->getKeyboardNetWeight() == -1)))
	{
		// have a new stable weight, update the current product.
		// todo - also send the RTC a ProductUpdate message. Will Product do this?
		bool isTracing = Debug::isTracing("ProductManager.UpdateWeightAndNotify");

		if (isTracing)
			Debug::trace("ProductManager.UpdateWeightAndNotify", "updating with new weight");

		ProductTransaction *currentTransaction = currentProduct->getCurrentTransaction();
		
		if (currentTransaction)
		{
			if (currentProduct->hasKbdNetWeightLabel() && currentProduct->isRandomWeight() && (currentProduct->getKeyboardNetWeight() == -1))
			{
				//	Just update the weigher status
				if (weight)
				{
					currentTransaction->setWeigherStatus(weight);
					if (isTracing)
						Debug::trace("ProductManager.UpdateWeightAndNotify", "Updating the weigher status only");
				}
			}
			else
			{
				HfcProductUpdateValuesMsg previousHPUVM(currentTransaction);
				previousHPUVM.serialize(BrowserFormat);
				if (isTracing)
					Debug::trace("ProductManager.UpdateWeightAndNotify", HobString("previous serialized hpuvm: ") + previousHPUVM.getSerializedData());

				if (weight)
					currentTransaction->setWeigherStatus(weight);

				if (isTracing)
					Debug::trace("ProductManager.UpdateWeightAndNotify", "Before call to updateTotalValuesData");
				
				if (Configuration::getInstance()->isSelfServiceScale() && currentProduct->isByCount() && !currentProduct->isByCountEntered())
				{
					if (isTracing)
						Debug::trace("ProductManager.UpdateWeightAndNotify", "Self Serve, no update total values because by count not entered yet.");

					// but we still have to send the zero indicator, so just send the weigher status message to the browser.
					SessionMessage sm(Weigher, WgStatus);
					sm.setFormatType(BrowserFormat);
					sm.setHfcObject(weight);
					mySession->send(&sm, Messenger::DEST_UI);
					return;
				}

				if (currentProduct->updateTotalValuesData() && mySession)
				{
					// the above method returns true if the tare ended up changing due to exceeding the dualrange limit.
					// in which case, the tare needs to be sent to the browser.

					WriteSingleField productUpdate;
					productUpdate.setRecordNum(1);
					productUpdate.setFieldCode((Configuration::getInstance()->toBoolCfg( fieldOffsetCfgUseSecondTare )) ? Product::fieldOffsetTareBValue : Product::fieldOffsetTareAValue);

					HobString newTare = "";
					currentProduct->getFieldAsString((Configuration::getInstance()->toBoolCfg( fieldOffsetCfgUseSecondTare )) ? Product::fieldOffsetTareBValue : Product::fieldOffsetTareAValue, newTare, BrowserFormatPLUOperateMode);
					productUpdate.setFieldNewValue(newTare);

					SessionMessage fieldUpdatedMsg(ProductDev, ProductWriteSingleField);
					fieldUpdatedMsg.setFormatType(BrowserFormat);
					fieldUpdatedMsg.setHfcObject(&productUpdate);

					mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
				}

				if (mySession)
				{
					HfcProductUpdateValuesMsg newHPUVM(currentTransaction);
					newHPUVM.serialize(BrowserFormat);
					if (isTracing)
						Debug::trace("ProductManager.UpdateWeightAndNotify", HobString("New serialized hpuvm: ") + newHPUVM.getSerializedData());

					HobString prev = previousHPUVM.getSerializedData();
					if (forceNotification ||
						 (currentProduct->isUnitPriceForced() && !currentProduct->isUnitPriceEntered()) ||
						 prev != newHPUVM.getSerializedData())
						updateBrowserAndCustomerDisplay(newHPUVM);
					else if (isTracing)
						Debug::trace("ProductManager.UpdateWeightAndNotify", "uhpuvm data is the same, no need to send it to the browser.");

					// PJR-383
					// Use this block if we only want to tell the UI to display a barcode image when the weight is stable (no small motion)
					if (mySession->isPrimarySession())
						imageSelfServeBarcodeLabel(newHPUVM);

				}
			}
		}
	}
}


void ProductManager::updateBrowserAndCustomerDisplay(HfcProductUpdateValuesMsg &hpuvm)
{
	if (!mySession)
		return;
	
	Session *s = dynamic_cast<Session *> (mySession);

	// Don't want the scale to timeout and return to the start screen while the customer is putting product on the weigher.
	if (s && Configuration::getInstance()->isSelfServiceScale())
		s->manageAutoLogoutTimer();

	// Send the product values back to the Browser.
	SessionMessage pluValues(ProductDev, ProductWriteCurrentTotalValues);
	pluValues.setFormatType(BrowserFormat);
	pluValues.setHfcObject(&hpuvm);
	mySession->send(&pluValues, Messenger::DEST_UI);

	// Note:  You don't need to call determineDisplayValues() on the HfcProductUpdateValuesMsg passed in
	// to get the values to display on the customer display because it was called when the
	// HfcProductUpdateValuesMsg was serialized when sending the message directly above this comment.
	
	bool isInOperateMode = (s && s->isInOperateMode()) ? true : false;

	if (mySession->isPrimarySession() && isInOperateMode)
	{
		// PJR-383
		// Use this block if we want to tell the UI to display a barcode image when the weight is stable or when there is small motion
		//imageSelfServeBarcodeLabel(hpuvm);

		// Update the customer display.
		HobString value = hpuvm.gettotalTareDisplay();
		NumericField custDisplay(NmTare, value.c_str());
		mySession->send(&custDisplay, Messenger::DEST_RTC);

		value = hpuvm.getNetWeightDisplay();
		custDisplay.setNmFieldType(NmWeight);
		custDisplay.setFieldData(value.c_str());
		mySession->send(&custDisplay, Messenger::DEST_RTC);

		value = hpuvm.getUnitPriceDisplay();
		custDisplay.setNmFieldType(NmUnitPrice);
		custDisplay.setFieldData(value.c_str());
		mySession->send(&custDisplay, Messenger::DEST_RTC);

		value = hpuvm.getTotalPriceDisplay();
		custDisplay.setNmFieldType(NmTotalPrice);
		custDisplay.setFieldData(value.c_str());
		mySession->send(&custDisplay, Messenger::DEST_RTC);

		// PTR 643 - Update the 'Pounds for' value
		value = hpuvm.getPoundsForDisplay();
		custDisplay.setNmFieldType(NmPoundsPerPrice);
		custDisplay.setFieldData(value.c_str());
		mySession->send(&custDisplay, Messenger::DEST_RTC);

		const ProductTransaction *currentTransaction = hpuvm.getProductTransaction();
		if (currentTransaction)
		{
			// Update the 'net' annunciator (if tare > zero, the the weight displayed will be a net weight, gross - tare)
			NmState state = (currentTransaction->getTotalTare() == (HobWeight)0) ? NmOff : NmOn;
			NumericAnnunciator custAnnunciator(NmCode0, state);
			mySession->send(&custAnnunciator, Messenger::DEST_RTC);
		}

		// Update the 'zero' annunciator
		NmState state = (hpuvm.isZeroIndicator()) ? NmOn : NmOff;
		NumericAnnunciator custAnnunciator(NmCode4, state);
		mySession->send(&custAnnunciator, Messenger::DEST_RTC);

		// PTR 643 - Update the 'Pounds for' annunciator
		state = (hpuvm.getPoundsForDisplay() != "") ? NmOn : NmOff;
		NumericAnnunciator custAnnunciator2(NmCode9, state);
		mySession->send(&custAnnunciator2, Messenger::DEST_RTC);

		//PTR #644 - Update the 'prepack' annunicator
		NmState state1 = NmOff;		

		if (s)
		{
			OpModeEvent currMode = s->getCurrentMode();
		
			if ( (currMode == OpModeAutoWeigh) || 
				(currMode == OpModeSemiWeigh) || 
				(currMode == OpModeWrapOnly) || (currMode == OpModeAutoManualApply) )
				state1 = NmOn;	
		}
		
		// jhc GCD to WINDUX merge
		// As of now, the prepack indicator is not required to be turned on.
		NumericAnnunciator prepackAnnunciator(NmCode14, state1);
		mySession->send(&prepackAnnunciator, Messenger::DEST_RTC);

		//GCD -> pass the event to the GCD session so it can obtain the values to send
		GCDSession* gcdSession = SessionManager::getInstance()->GetGCDSession();
		if (gcdSession)
		{
			// As of now, the prepack indicator is not required to be turned on.
			// FT-503 prepack indicator is required now...
			gcdSession->updatePrepackAnnunciator(state1);
			gcdSession->UpdateProductWeightData(hpuvm);
		}
	}
}

// PJR-383
void ProductManager::imageSelfServeBarcodeLabel(const HfcProductUpdateValuesMsg &hpuvm)
{
	if ( !currentProduct )
		return;
	
	if ( Configuration::getInstance()->isSelfServiceScale() && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServeDisplayBarcodeLabel) )
	{
		ProductTransaction *pTrans = currentProduct->getCurrentTransaction();
		if (!pTrans)
			return;

		bool isTracing = Debug::isTracing("SelfServeBarcodeLabel");

		const HfcWeigherStatus *ws = pTrans->getWeigherStatus();

		// Use this block if we only want to tell the UI to display a barcode image when the weight is stable (no small motion)
		if ((ws && !ws->isPositiveNonZeroStableWeight() && !currentProduct->needsNoWeigherStatus()) || hpuvm.getTotalPriceDisplay() == HfcProductUpdateValuesMsg::_invalidTotalPriceDisplay_ || hpuvm.getTotalPriceDisplay() == "")
		{
			if (isTracing)
				Debug::trace("SelfServeBarcodeLabel", "imageSelfServeBarcodeLabel():  calling deleteSelfServeBarcodeLabel, isPositiveNonZeroStableWeight: " + booltostring(ws->isPositiveNonZeroStableWeight()) + "  totalprice: " + hpuvm.getTotalPriceDisplay());
			deleteSelfServeBarcodeLabel();
			return;
		}

		// Use this block if we want to tell the UI to display a barcode image when the weight is stable or has small motion
		/*
		if ((ws && !ws->isWeightValidForDisplaying() && !currentProduct->needsNoWeigherStatus()) || hpuvm.getTotalPriceDisplay() == HfcProductUpdateValuesMsg::_invalidTotalPriceDisplay_ || hpuvm.getTotalPriceDisplay() == "")
		{
			if (isTracing)
				Debug::trace("SelfServeBarcodeLabel", "imageSelfServeBarcodeLabel():  calling deleteSelfServeBarcodeLabel, isWeightValidForDisplaying: " + booltostring(ws->isWeightValidForDisplaying()) + "  totalprice: " + hpuvm.getTotalPriceDisplay());
			deleteSelfServeBarcodeLabel();
			return;
		}
		*/

		if (isTracing)
			Debug::trace("SelfServeBarcodeLabel", "imageSelfServeBarcodeLabel():  calling imageSelfServeDisplayBarcodeLabel, totalprice: " + hpuvm.getTotalPriceDisplay());

		if (currentProduct->getLabelType(ExtraPrinter_UsedForSelfServeScreenBarcodeLabel))
		{
			pTrans->setBarCodeNum(currentProduct->getBarCodeNum());
			pTrans->buildBarcodeStrings();

			// Have my Session start listening for when the Rendering is finished
			mySession->registerEvent(Printer, PrDonePostImagingSelfServeBarcodeLabelImage);

			if (isTracing)
				Debug::trace("SelfServeBarcodeLabel", "imageSelfServeBarcodeLabel():  Sending post-image message, barcodestring: " + currentProduct->getCurrentTransaction()->getBarCodeNum().getBarcodeString(UPCA));

			// Send the PostImage message
			PrinterPostImage postImage(0, FirstPrinter /*ExtraPrinter_UsedForSelfServeScreenBarcodeLabel*/, pTrans, 0, &currentProductSmartPtr);
			postImage.setEventType(PrPostImageBarcode);
			mySession->dispatchEvent(&postImage);
			//PAL_absclass::PAL_sleep(500);
		}
	}
}

// PJR-383
void ProductManager::donePostImagingSelfServeBarcodeLabelImage(int filenameCounter)
{
	if (mySession)
	{
		bool isTracing = Debug::isTracing("SelfServeBarcodeLabel");

		//mySession->deregisterEvent(Printer, PrDonePostImagingSelfServeBarcodeLabelImage);
		//PAL_absclass::PAL_sleep(500);

		// Done post-imaging/rendering SelfServe barcode label image, tell the browser to display the barcode label.
		if (isTracing)
			Debug::trace("SelfServeBarcodeLabel", "donePostImagingSelfServeBarcodeLabelImage():  Tell the browser to display the barcode label");

		SessionMessage msg(ProductDev, ProductPrint);
		msg.setFormatType(BrowserFormat);
		string data("2");
		std::string filenameString = Configuration::getInstance()->buildPrintToScreenBarcodeFilename(filenameCounter);
		data.append(string(",\"")).append(filenameString).append(string("\""));
		
		GenericHfcObject gho(data);
		msg.setHfcObject(&gho);
		mySession->send(&msg, Messenger::DEST_UI);
	}
}

// PJR-383
void ProductManager::deleteSelfServeBarcodeLabel()
{
	Configuration *config = Configuration::getInstance();
	if ( config && config->toBoolCfg(fieldOffsetCfgSelfServeDisplayBarcodeLabel) && config->isSelfServiceScale() )
	{
		bool isTracing = Debug::isTracing("SelfServeBarcodeLabel");

		//string filename = config->toStringCfgI(fieldOffsetCfgIPrintToScreenBarcodeFilename);
		//File barcodeFile(filename);
		//if (barcodeFile.exists())
		{
			//barcodeFile.remove();
			//if (isTracing)
			//	Debug::trace("SelfServeBarcodeLabel", "deleteSelfServeBarcodeLabel():  Deleted barcode label file: " + filename);

			SessionMessage msg(ProductDev, ProductPrint);
			msg.setFormatType(BrowserFormat);
			string data("2,\"\"");
			GenericHfcObject gho(data);
			msg.setHfcObject(&gho);
			mySession->send(&msg, Messenger::DEST_UI);
		}
		//else if (isTracing)
		//	Debug::trace("SelfServeBarcodeLabel", "deleteSelfServeBarcodeLabel():  No barcode label file to delete: " + filename);
	}
}


void ProductManager::handleProductReadCurrentTotalValuesMessage()
{
	Debug::trace("ProductManager", "entering handleProductReadCurrentTotalValuesMessage()");

	if (currentProduct)
	{
		ProductTransaction *currentTransaction = currentProduct->getCurrentTransaction();
		if (currentTransaction)
		{
			Debug::trace("ProductManager", "have a product and a transaction");
			if (mySession)
			{
				// Send the product total data back to the Browser.
				SessionMessage pluValues(ProductDev, ProductWriteCurrentTotalValues);
				pluValues.setFormatType(BrowserFormat);

				HfcProductUpdateValuesMsg hpuvm(currentTransaction);
				pluValues.setHfcObject(&hpuvm);

				// Send the product to the UI (Browser)
				mySession->send(&pluValues, Messenger::DEST_UI);
			}
		}
	}
}


void ProductManager::handleProductWriteCurrentTotalValuesMessage(SessionMessage *msg)
{
	bool isTracing = Debug::isTracing("ProductManager");
	if (isTracing)
		Debug::trace("ProductManager", "entering handleProductWriteCurrentTotalValuesMessage()");
	HfcProductUpdateValuesMsg *updateValuesMsg = dynamic_cast<HfcProductUpdateValuesMsg *> (msg->getHfcObject());

	if (!updateValuesMsg)
	{
		if (isTracing)
			Debug::trace("ProductManager", "handleProductWriteCurrentTotalValuesMessage() - updateValuesMsg is not the hfcObject in the message");
		return;
	}

	if (currentProduct)
	{
		ProductTransaction *currentTransaction = currentProduct->getCurrentTransaction();

//		Debug::trace("ProductManager", HobString("handleWeight - updating with new stable weight"));

		if (currentTransaction)
		{
			if (isTracing)
			{
				Debug::trace("ProductManager", "have a product and a transaction");

				Debug::trace("ProductManager", "updateValuesMsg->getTotalPrice() " + util_string::doubletostring(updateValuesMsg->getTotalPrice()));
				Debug::trace("ProductManager", "currentTransaction->getTotalPrice() " + util_string::doubletostring(currentTransaction->getTotalPrice()));
			}

			if (currentTransaction->getTotalPrice() != updateValuesMsg->getTotalPrice())
			{
				HobPrice origPrice = currentTransaction->getTotalPrice();
				if (isTracing)
					Debug::trace("ProductManager", "origPrice " + util_string::doubletostring(origPrice));
				currentTransaction->setTotalPrice(updateValuesMsg->getTotalPrice());
				if (isTracing)
					Debug::trace("ProductManager", "currentTransaction->getTotalPrice() " + util_string::doubletostring(currentTransaction->getTotalPrice()));

				if (mySession)
				{
					// Send the product total data back to the Browser.
					SessionMessage pluValues(ProductDev, ProductWriteCurrentTotalValues);
					pluValues.setFormatType(BrowserFormat);

					HfcProductUpdateValuesMsg hpuvm(currentTransaction);
					pluValues.setHfcObject(&hpuvm);

					// Send the product to the UI (Browser)
					mySession->send(&pluValues, Messenger::DEST_UI);

					Session *s = dynamic_cast<Session *> (mySession);
					if (s && s->isInOperateMode() && s->isPrimarySession() && !Configuration::getInstance()->isSelfServiceScale() && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgTrackingOperatorChanges))
					{
						//	Record the operator change.
						if ( currentTransaction && !currentProduct->isUnitPriceForced() && (currentProduct->isByCount() || currentProduct->isFixedWtOrFluidOz()) )
						{
							currentTransaction->addOperatorChangeTotalPrice(origPrice, currentTransaction->getTotalPrice());
						}
					}
				}
			}
		}
	}
}

/* Check if any printer status needs to be displayed, and tell the Session to display it.
 * Returns true if a status message gets displayed, else return false
 */
bool ProductManager::checkPrinterStatuses()
{
	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return false;

	for (int i = (int)FirstPrinter; i < (int)MaxPrinterTypes; i++)
	{
		HfcPrinterStatus *status = s->getCurrentPrinterStatus(i);

		if ( status && status->notificationReasonExists() && s->getResponseMessageManager() )
		{
			Debug::trace("ProductManager", "checkPrinterStatuses - printer status, notificationReasonExists for printer: " + inttostring(i));
			s->getResponseMessageManager()->manageStatus(status);
			return true;
		}
	}

	return false;
}

void ProductManager::handleProductPrintKey1(bool fppTestPrint)
{
	Debug::trace("ProductManager", "entering handleProductPrintKey1()");
	bool wasWaitingForLoginBeforePrinting = waitingForLoginBeforePrinting;
	waitingForLoginBeforePrinting = false;

	/* Can't print the label if the current product is not accepting weights;
	 * i.e. when the weigher was not zeroed when the user entered the plu number.
	 * No current need to display an error message.  There will already be an error message displayed
	 * indicating the user needs to remove the weight or press the rezero button.
	 */
	if (!currentProductCanAcceptWeight)
	{
		Debug::trace("ProductManager", "handlePrintKey1 - current product is not accepting weight updates, so you can't print it");
		return;
	}

	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return;

	Configuration * config = Configuration::getInstance();
	if (!config)
		return;

	OpModeEvent currMode = s->getCurrentMode();

	if ( currMode == OpModeBuildAMeal )
	{
		if ( bamSidesLeft != 0 && !bamUnlimitedSides )
		{
			s->getResponseMessageManager()->addMessage(ErrProductDev, ProductErrNeedMoreSidesAddedToMeal, HfcResponse::RESPONSESTYLE_OK);
			return;
		}

		// Check to see if we need a weight for the meal.  Do we have it?  If not, prompt for it.
		if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgWeightRequiredForMeal) && SessionManager::getInstance()->isWeigherAttached() )
		{
			HfcWeigherStatus *currentWeight = s->getCurrentWeight();
			if ( !currentWeight ||
			     ( currentWeight && currentWeight->isPeripheralComm() && !currentWeight->isManagerDeleted() &&
			       ( currentWeight->isZeroStableWeight() || (currentWeight->isSmallMotion() && currentWeight->isWithinZeroQuarterGrad() && !currentWeight->isScaleWarmup()) ) ) )
			{
				s->getResponseMessageManager()->addMessage(ErrProductDev, ProductErrWeightNeededForMeal, HfcResponse::RESPONSESTYLE_OK);
				return;
			}
			else
			{
				// Assign the current weight to the meal.
				HfcWeigherStatus copy = *currentWeight;
				copy.setDeviceType(ProductDev);
				copy.setEventType(ProductSetMealWeight);
				s->dispatchEvent(&copy);
			}

		}
	}

	// If there are any outstanding printer statuses, for any of the printers (and we are internally configured to
	// not keep displaying them, which we started doing with the HTi),
	// then redisplay those statuses, and leave; since we can't print with any printer being in some "error" state.
	// ORIGINAL Test - if (!(config->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && config->isSelfServiceScale()))
	// New options have been added to supress printing and printing labels to the screen to scan them so this test had to expand and get more limited
	if( config->isSelfServiceScale() )
	{
		Debug::trace("ProductManager", HobString("Determine if we need to supress errors: "));

		bool skipStatus = false;
		// break out each SPECIFIC reason one by one, why we want to supress error dialogs
		if (config->toIntCfg(fieldOffsetCfgSelfServeSearchKeypadFlashkeys) && (config->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors) > SelfServOption_PrintToLabel))
		{
			// If the combination Search, Keypad, and Flashkeys, AND the print screen combo level beside it is either the 2nd or 3rd option.  NOT the first which will display errors still 
			Debug::trace("ProductManager", HobString("SUPRESS dialogs: Big Y Combo turned on, and the first Print Label option is not selected"));
			skipStatus = true;
		}
		else if ( config->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen)  && !config->toIntCfg(fieldOffsetCfgSelfServeSearchKeypadFlashkeys)  )
		{
			Debug::trace("ProductManager", HobString("SUPRESS dialogs: Print to screen is turned on but  Big Y Combo turned off. This is Amazon Go Support"));
			skipStatus = true;
		}
			
		if (skipStatus && config->toBoolCfgI(CfgInternal::PRINTERSTATUSNONREPEATINGMSGSTOUI) )
		{
			if ( checkPrinterStatuses() )
				return;
		}
	}

	/* If I'm not in Manual-Weigh mode,
	 * or if they haven't selected a product,
	 * or if the net weight <= 0,
	 * or if the net weight < minimum weight to print,
	 * or if they haven't taken the last label,
	 * and the user tries to press the print key,
	 * don't let them.  Just send back an error message.
	 */
	if (Debug::isTracing("ProductManager"))
	{
		Debug::trace("ProductManager", HobString("handlePrintKey1 - currentMode: ") + HobString(Integer::toString(currMode)));
		if (!currentProduct)
			Debug::trace("ProductManager", "handlePrintKey1 - currentProduct is NULL!");
		else
			Debug::trace("ProductManager", HobString("handlePrintKey1 - currentProduct: ") + HobString(Long::toString(currentProduct->getProductNum())));
	}
	
        // tell the user your may not print their blank nutrifact label
        Debug::trace("ProductManager.nutri", HobString("hasNutrifactsLabelDataMissing() - Get ready to call it"));
        if( config->toBoolCfg(fieldOffsetDoNotPrintNutrifacts) )
        {
            Debug::trace("ProductManager.nutri", HobString("hasNutrifactsLabelDataMissing() fieldOffsetDoNotPrintNutrifacts IS TRUE"));
            if( hasNutrifactsLabelDataMissing() )
                return;						// if nutrifacts missing exit and dont print
        }
        else
            Debug::trace("ProductManager.nutri", HobString("hasNutrifactsLabelDataMissing() fieldOffsetDoNotPrintNutrifacts IS FALSE so skip testing"));

	if (!config->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
	{
		//	GCDR-158:	Don't allow the operator to print if this a Service counter scale with NO Customer display
		//				and the scale is NOT set to operate as a Prepack.
		if (config->isServiceCounterScale() && !SessionManager::getInstance()->isCustomerDisplayAttached() && !config->toBoolCfg(fieldOffsetCfgGCDPrintWithoutCustomerDisplay))
		{
			//mySession->addDialogMessage(ErrProductDev, ProductErrCannotPrintWhenGCDNotConnected, HfcResponse::RESPONSESTYLE_OK);
			Debug::trace("ProductManager", HobString("handleProductPrintKey1:: Service scale wo/Customer Display, not set to Prepack"));
			mySession->addDialogMessage(ErrProductDev, ProductErrCannotPrintWhenCustomerDisplayNotConnected, HfcResponse::RESPONSESTYLE_OK);
			return;
		}
		// We now know that it is a service scale and it has a customer display attached and it is not set to operate as a prepack. Now
		// we need to make sure if the customer display is a GCD that it is operational. GCDR-183
		if (SessionManager::getInstance()->isGCDAttached() && !SessionManager::getInstance()->isGCDOperational())
		{
			Debug::trace("ProductManager", HobString("handleProductPrintKey1:: GCD Attached but not Operational"));
			mySession->addDialogMessage(ErrProductDev, ProductErrCannotPrintWhenCustomerDisplayNotConnected, HfcResponse::RESPONSESTYLE_OK);
			return;
		}

		//Debug::trace("ProductManagerGCD", HobString("Passed the test for Cusomer Display connected"));
	}

	if ( ( s->isInAnAccumulatingMode() && !currentProduct ) ||
		 ( currMode == OpModeProductionEntry && !currentProduct ) ) // SOF-2827: Need to allow Production Entry print
	{ // Jira GCDR-199 We will fall thru this test when printing a Accumulated Items totals label from the 'Start of Run' screen, since current_product = NULL there.
		Debug::trace("ProductManager", HobString("isInAnAccumulatingMode() OR OpModeProductionEntry()"));
		handleProductPrintKey3();
		return;
	}

	//	Prepack: Costco doesn't want to have to toggle the print button for a Fixed weight or By count PLU.
	//	So, we are allowing the print key to be accepted regardless of the currentMode on non Random weight PLUs.
	bool enforceManualWeigh = (currentProduct && (currentProduct->isRandomWeight() || currentProduct->isByCountWithWeight())) ? true : false;

	// Send error message to browser indicating print key invalid right now.
	if (!currentProduct)
	{
		if ((currMode == OpModeSemiWeigh) && Configuration::getInstance()->isPrepackScale())	//	HA-65
		{
			mySession->addDialogMessage(ErrOperationMode,
									    ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::ProductRequiredToWrap,
									    HfcResponse::RESPONSESTYLE_OK);
		}
		else
		{
			mySession->addDialogMessage(ErrProductDev, ProductErrPrintNoProduct, HfcResponse::RESPONSESTYLE_OK);
		}
		return;
	}
	// FT-503 moved kbd net (turkey weigher) processing below
	else if ((!currentProduct->hasKbdNetWeightLabel()) &&
		(!config->isPrepackScale() || enforceManualWeigh) &&
		(currMode != OpModeManualWeigh) &&
		(currMode != OpModeSelfService) &&
	    (currMode != OpModeProductionEntry) &&
	    (currMode != OpModePrintMultipleLabels) &&
	    (currMode != OpModeAccumulateItems) &&
	    (currMode != OpModeBuildAMeal) &&
	    (!wasWaitingForLoginBeforePrinting) &&
		(!pick5ValueOutOfRange))
	{
		if (!Configuration::getInstance()->isPrepackScale())
		{
			mySession->addDialogMessage(ErrProductDev, ProductErrPrintWrongOpMode, HfcResponse::RESPONSESTYLE_OK);
		}
		else
		{
			mySession->addDialogMessage(ErrProductDev, ProductErrPrepackPrintWrongOpMode, HfcResponse::RESPONSESTYLE_OK);
		}
		return;
	}
#if 0
//	Reworked this section in the else if statement below for HA-180 - and because it was confusing.
//	Essentially, we are checking to see if we can print a TurkeyWeigher label in a particular mode.
//	If we can, we fall through. If not, we send an error message and exit. [CCD FEB 2015]

	// FT-503 moved kbd net (turkey weigher) processing here
	else if ((currentProduct->hasKbdNetWeightLabel()) &&
				(!config->isPrepackScale() || enforceManualWeigh) &&
				(Configuration::getInstance()->isPrepackScale() && (currMode != OpModeManualWeigh)) &&		//	HA-120
				(currMode != OpModeAutoWeigh) &&
				(currMode != OpModeAutoManualApply) &&
				(!wasWaitingForLoginBeforePrinting))
	{
		if (config->isPrepackScale())
			switchModeForKeyboardNetWt();
		else
		{
			//	This error only applies to a non-Prepack scale in Prepack mode.
			mySession->addDialogMessage(ErrProductDev, ProductErrOnlyForPrepackMode, HfcResponse::RESPONSESTYLE_OK);
		}
		return;
	}
#else
	else if (currentProduct->hasKbdNetWeightLabel())	// HA-180
	{
//		Debug::trace("ProductManager.KeyboardNetWeight", "handleProductPrintKey1():  hasKbdNetWeightLabel");
		if (Configuration::getInstance()->isPrepackScale())
		{
//			Debug::trace("ProductManager.KeyboardNetWeight", "handleProductPrintKey1():  isPrepackScale");
			if ((currMode != OpModeManualWeigh) && (!wasWaitingForLoginBeforePrinting))
			{
//				Debug::trace("ProductManager.KeyboardNetWeight", "handleProductPrintKey1():  currMode = " + inttostring(currMode) + ",  Prepack scale is in the wrong Op mode!");
				mySession->addDialogMessage(ErrProductDev, ProductErrPrepackPrintWrongOpMode, HfcResponse::RESPONSESTYLE_OK);
				return;
			}
		}
		else
		{
//			Debug::trace("ProductManager.KeyboardNetWeight", "handleProductPrintKey1():  isServiceCounterScale");
			if ((currMode != OpModeAutoManualApply) && (!wasWaitingForLoginBeforePrinting))
			{
                // HT-317 Keyboard Entered Weight will not print a label
//				Debug::trace("ProductManager.KeyboardNetWeight", "handleProductPrintKey1():  currMode = " + inttostring(currMode) + ",  ServiceCounter scale is in the wrong Op mode!");
				//	This error only applies to a ServiceCounter scale in Prepack mode.
				mySession->addDialogMessage(ErrProductDev, ProductErrOnlyForPrepackMode, HfcResponse::RESPONSESTYLE_OK);
				return;
			}
			else
			{
                // HT-317 Keyboard Entered Weight will not print a label
                // This is the main point of switching to Turkey Weigher mode
				// if we are not in the right mode to press the print key, then put us in that mode. At this point we know:
				// 1. We are a counter scale and not a prepack scale
				// 2. We are a random weight item
				// 3. We have a keyboard weight label ( turkey weigher feature )
				// 4. We have already entered a weight from the dialog
				// 5. We pressed print, so let it happen and help it get there if it's not there.
				switchModeForKeyboardNetWt();
		    }
		}
//		Debug::trace("ProductManager.KeyboardNetWeight", "handleProductPrintKey1():  Moving on...");
	}
#endif
	else if (currentProduct->getLabelBeingPrinted() != Product::PRINTING_NO_LABEL && currentProduct->getLabelBeingPrinted() != Product::PRINTING_PRODUCTION_LABEL)
		return;
	else if (!currentProduct->areForcedFieldsEntered())
	{
		// Don't display the error message that the quantity must be entered before pressing the print key
		// when we're in self-serve mode, and self-serve enable help prompts is on (fieldOffsetCfgEnableHelpPrompts).
		// This was a request from Walmart Chile.
		// I intentionally didn't handle what language the scale is using, so this same functionality will apply to Walmart Canada as well.
		if ( !( currMode == OpModeSelfService && config->toBoolCfg(fieldOffsetCfgEnableHelpPrompts) ) )
			mySession->addDialogMessage(ErrProductDev, ProductErrForcedFieldNotEntered, HfcResponse::RESPONSESTYLE_OK);
		return;
	}

	// Fixed weight verification
	else if ( config->toBoolCfg(fieldOffsetCfgRequireFixedWeightVerification) &&
	     currentProduct->isFixedWeight() &&
		 currentProduct->getFixedWeight() > 0 &&
		 currentProduct->getCurrentTransaction() &&
		 currMode != OpModeBuildAMeal && currMode != OpModeAccumulateItems )
	{
		HobWeight weight = 0;
		if ( currentProduct->getCurrentTransaction()->getWeigherStatus() )
		{
			weight = currentProduct->getCurrentTransaction()->getWeigherStatus()->getWeight();
			Debug::trace( "ProductManager", string("handlePrintKey1 - fixedWeightVerify - current weight on platter is: ") + Long::toString(weight) );
		}

		if ( weight == 0 )
		{
			s->getResponseMessageManager()->addMessage(ErrProductDev, ProductErrFixedWeightVerificationNeedWeight, HfcResponse::RESPONSESTYLE_OK);
			return;
		}
		else 
		{
			HobWeight fweight = MetSig::fixedWeightToWeightCounts(currentProduct->getFixedWeight());
			Debug::trace( "ProductManager", string("handlePrintKey1 - fixedWeightVerify - fixed weight of product is: ") + Long::toString(fweight) );
			if ( weight < fweight )
			{
				HobList msgData;
				HobString data;

				HobWeight fweightCurr = MetSig::weightCountsToFixedWeight(weight);
				if (currentProduct->isFluidOunce())
					config->formatVolume(fweightCurr, data, FixedWeightText, true);
				else
					config->formatWeight(fweightCurr, data, true, FixedWeightText);

				msgData.push_back(data);
				s->getResponseMessageManager()->addMessage(ErrProductDev, ProductErrFixedWeightVerificationInsufficientWeight, HfcResponse::RESPONSESTYLE_OK, &msgData);
				return;
			}
		}
	}

	if ((config->toBoolCfg(fieldOffsetCfgOperateLoginRequired) ||
			  (config->toBoolCfg(fieldOffsetCfgTrackingOperatorChanges) && currentProduct->getCurrentTransaction()->hasOperatorChangedData())) &&
			 currMode != OpModeSelfService && s->getOperatorManager()->countAllOperators() > 0 && s->getOperatorManager()->getLoggedInOperator() == 0 &&
			 currentProduct->getCurrentTransaction())
	{
		waitingForLoginBeforePrinting = true;

		SessionMessage msg(OperatorDev, OpPromptForIDOrRFID);
		msg.setFormatType(BrowserFormat);
		mySession->send(&msg, Messenger::DEST_UI);
		return;
	}	

	if (( OpModeProductionEntry == currMode ) && ( currentProduct ))
		if (( !currentProduct->isPrintingInProductionEntryMode() ) && ( !currentProduct->isDummyProduct() )) // FT-815
			if ( Session::OP_PRINTKEYPRESSED != s->getCurrentModeOp() )
			{
				s->setFPPTestPrint(fppTestPrint); // This may be unnecessary

				//	PTR 334: Make sure that the last Product entered is added to the printingProducts list, otherwise,
				//			 when handleProductComplete() is called for the first Product printed, currentProductSmartPtr
				//			 will be cleared, removing all instances of that last Product and it will be deleted. That's
				//			 why the last Product was not getting printed.
				
				Util::CSmartPtr<Product> printingProduct;
				if (!findPrintingProduct(currentProduct, printingProduct))
				{
					printingProducts.push_back(currentProductSmartPtr);  // Do we want to move this currentProduct pointer into here, or the smart ptr????
					Debug::trace("ProductManager", string("printingProducts.push_back2: new list len = ") + inttostring((int)printingProducts.size()));
				}

				printLabel();
			}

	if (currMode == OpModeProductionEntry ||
		 s->isInAnAccumulatingMode() ||
		 config->toBoolCfgI(CfgInternal::SIMULATEPRINTING) ||
		 labelMediaCheck() == LabelOk)
	{
		ProductTransaction* trans = currentProduct->getCurrentTransaction();
		HobOperator* oper = s->getOperatorManager()->getLoggedInOperator();
		if ( trans && oper )
			trans->associateOperator(oper);

		currentProduct->setLabelBeingPrinted(Product::PRINTING_PRODUCTION_LABEL);
		s->prepareToPrint(fppTestPrint);
	}
}

void ProductManager::handleProductPrintKey2()
{
	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return;

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	OpModeEvent currMode = s->getCurrentMode();
	bool isInOperateMode = (s->isInOperateMode()) ? true : false;
	bool printThisLabel = true;
	ProductTransaction *currentTrans = 0;

	/* If I'm not in Manual-Weigh mode,
	 * or if they haven't selected a product,
	 * or if the net weight <= 0,
	 * or if the net weight < minimum weight to print,
	 * or if they haven't taken the last label,
	 * and the user tries to press the print key,
	 * don't let them.  Just send back an error message.
	 */
	if (Debug::isTracing("ProductManager"))
	{
		Debug::trace("ProductManager", HobString("handlePrintKey2 - currentMode: ") + HobString(Integer::toString(currMode)));
		if (!currentProduct)
			Debug::trace("ProductManager", "handlePrintKey2 - currentProduct is NULL!");
		else
		{
			Debug::trace("ProductManager", HobString("handlePrintKey2 - currentProduct: ") + HobString(Long::toString(currentProduct->getProductNum())));
			if (!currentProduct->getCurrentTransaction())
				Debug::trace("ProductManager", "handlePrintKey2 - currentProductTransaction is NULL!");
			else
			{
				Debug::trace("ProductManager", HobString("handlePrintKey2 - currentProductTrans: ") + HobString(Long::toString(currentProduct->getCurrentTransaction()->getTransactionNum())));
				Debug::trace("ProductManager", HobString("handlePrintKey2 - totalnetweight: ") + HobString(Long::toString(currentProduct->getCurrentTransaction()->getTotalNetWeight())));
				Debug::trace("ProductManager", HobString("handlePrintKey2 - minweighttoprint: ") + HobString(Long::toString(config->getMinWeightToPrint())));
			}
		}
	}

	if (s->getCurrentModeOp() == Session::OP_PRINTKEYPRESSED)
		s->setCurrentModeOp(Session::OP_UNKNOWN);

/* For Fastrack, chobart_d no longer is responsible for tracking which page the operator is on, so it doesn't determine if the
 * page can print the label or not (based on W&M requiring the net weight be shown on the display to be able to print).
 * When I ask the UI if there are any other field changes that the user hasn't committed (with the Enter key), it will also
 * indicate, via the 'shouldAlsoPrint' attribute of the WriteSingleField message it sends back, that it is OK to print on this page.
 * So I'm commenting this code out.
	if (!s || !currentProduct || !s->currentPageCanPrintLabel())
		printThisLabel = false;
	else
	{
*/
		currentTrans = currentProduct->getCurrentTransaction();
		if (!currentTrans)
			printThisLabel = false;
//	}

	// Added test for OpMode so we can print a Items list if the current PLU is Random Weight with 0 weight on scale
	// Reference Jira GCDR-152
	if ( printThisLabel && !s->isInAnAccumulatingMode() && !currentProduct->isReadyToPrint() )
//	if (currentProduct->getProductType() == Product::PRODUCTTYPE_RANDOMWEIGHT &&
//		 (!currentProduct->getCurrentTransaction()->getWeigherStatus()->isStableWeight() ||
//		  currentProduct->getCurrentTransaction()->getTotalNetWeight() <= (HobWeight)0 ||
//		  currentProduct->getCurrentTransaction()->getTotalNetWeight() < config->getMinWeightToPrint())
///*			 || currentProduct->hasOutstandingTransactions()*/)
	{
		printThisLabel = false;

		if ( currMode == OpModeManualWeigh || currMode == OpModeSelfService )
		{
			const HfcWeigherStatus *currentWeigherStatus = currentTrans->getWeigherStatus();
			if (currentWeigherStatus && !currentWeigherStatus->isWeigherInError() && !currentWeigherStatus->isStableWeight())
				waitingForStableWeightToPrint = true;
			else
			{
				ProductDevError errCode = currentProduct->getNotReadyToPrintErrorCode();
				mySession->addDialogMessage(ErrProductDev, errCode, HfcResponse::RESPONSESTYLE_OK);
			}
		}
	}

	if (printThisLabel)
	{
		if (Debug::isTracing("Product"))
		{
			HobDateTime hdt;
			HobDateTime hdtsl = currentTrans->getShelfLife();
			HobDateTime hdtpl = currentTrans->getProductLife();
			HobString hsdt;
			HobString hsdtsl;
			HobString hsdtpl;
			config->formatDateTime(hdt, hsdt);
			config->formatDateTime(hdtsl, hsdtsl);
			config->formatDateTime(hdtpl, hsdtpl);
			Debug::trace("Product", HobString("print dates - current:") + hsdt + HobString("  ") +
											HobString("  shelfLife:") + hsdtsl + HobString("  ") +
											HobString("  productLife:") + hsdtpl);
		}

		waitingForStableWeightToPrint = false;
		if (isInOperateMode)
		{
			if (currentProduct->isByCount())
			{
				if (currentProduct->isByCountForced() && (currentProduct->getByCount() <= 0))
				{
					printThisLabel = false;
					//	Send some sort of error message
					mySession->addDialogMessage(ErrProductDev, ProductErrPrintBadByCount, HfcResponse::RESPONSESTYLE_OK);
				}
			}

		}


		pick5ValueOutOfRange = false;
		if ( currentProduct->isPick5Active() )
		{
			if ( printThisLabel )
			{
				if ( currentProduct->isRandomWeight() || currentProduct->isByCountWithWeight() )
				{
					HobWeight weight = currentTrans->getTotalNetWeight();
					Debug::trace("ProductManager", string("handlePrintKey1 - weight-range-verify - current weight: ") + Long::toString(weight));

					if ( weight > 0 )
					{
						int errorCode = 0;
						if (currentProduct->getWeightMax() > 0 && currentProduct->getWeightMin() > 0 && weight < currentProduct->getWeightMin() )
							errorCode = ProductErrBelowMinWeight;
						if ( currentProduct->getWeightMax() > 0 && weight > currentProduct->getWeightMax() )
								errorCode = ProductErrAboveMaxWeight;
						if ( errorCode != 0 )
						{
							pick5ValueOutOfRange = true;

							// If the error message is already displayed, remove the previous one first.
							//s->getResponseMessageManager()->removeMessage(ErrProductDev, ProductErrBelowMinWeight, HfcResponse::RESPONSESTYLE_OK);
							//s->getResponseMessageManager()->removeMessage(ErrProductDev, ProductErrAboveMaxWeight, HfcResponse::RESPONSESTYLE_OK);

							HobList msgData;
							HobString result;
							config->formatWeight(weight, result);
							msgData.push_back(result);
							result = "";
							config->formatWeight(currentProduct->getWeightMin(), result);
							msgData.push_back(result);
							result = "";
							config->formatWeight(currentProduct->getWeightMax(), result);
							msgData.push_back(result);

							s->getResponseMessageManager()->addMessage(ErrProductDev, errorCode, HfcResponse::RESPONSESTYLE_OK, &msgData);
							printThisLabel = false;
						}
					}
				}
			}

			if (printThisLabel)
			{
				HobPrice totalPrice = currentProduct->getCurrentTransaction()->getTotalPrice();
				if (totalPrice > 0)
				{
					Debug::trace("ProductManager", string("handlePrintKey1 - price-range-verify - totalPrice: ") + Long::toString(totalPrice));
					int errorCode = 0;
					if (currentProduct->getTotalPriceMax() > 0 && currentProduct->getTotalPriceMin() > 0 && totalPrice < currentProduct->getTotalPriceMin())
						errorCode = ProductErrBelowMinTotalPrice;
					if ( currentProduct->getTotalPriceMax() > 0 && totalPrice > currentProduct->getTotalPriceMax() )
							errorCode = ProductErrAboveMaxTotalPrice;
					if (errorCode != 0)
					{
						pick5ValueOutOfRange = true;

						// If the error message is already displayed, remove the previous one first.
						//s->getResponseMessageManager()->removeMessage(ErrProductDev, ProductErrBelowMinTotalPrice, HfcResponse::RESPONSESTYLE_OK);
						//s->getResponseMessageManager()->removeMessage(ErrProductDev, ProductErrAboveMaxTotalPrice, HfcResponse::RESPONSESTYLE_OK);

						HobList msgData;
						HobString result;
						config->formatPrice(totalPrice, result);
						msgData.push_back(result);
						result = "";
						config->formatPrice(currentProduct->getTotalPriceMin(), result);
						msgData.push_back(result);
						result = "";
						config->formatPrice(currentProduct->getTotalPriceMax(), result);
						msgData.push_back(result);

						s->getResponseMessageManager()->addMessage(ErrProductDev, errorCode, HfcResponse::RESPONSESTYLE_OK, &msgData);
						printThisLabel = false;
					}
				}
			}
		}


		if (printThisLabel)
		{
			/* If we are printing a label in connection with production planning, we may need
			to put up a dialog to ask the operator whether they want to update their production
			planning system. In that case we will need to wait until they respond to that dialog
			before we would continue with print procedure. */
			if ( config->toBoolCfg(fieldOffsetCfgEnableProductionPlanning) &&
				 config->toStringCfg(fieldOffsetCfgProductionPlanningUpdate).size() &&
				 currMode != OpModeBuildAMeal )
			{
				if ( !s->isFPPTestPrint() )
				{
					if (currMode == OpModeProductionEntry)
					{
						if (productionEntryContainsFPPItems)
						{
							mySession->addDialogMessage(ErrProductDev, ProductErrFPPMultiplePLUUpdate, HfcResponse::RESPONSESTYLE_FPP_PRODUCT);
							return;
						}
					}
					else
					{
						ProductTransaction* transaction = currentProduct->getCurrentTransaction();
						// We want to do a regular print if FPPType == 4.  No dialog box for type 4
						if ( transaction  && transaction->isProductionPlanningTransaction() && currentProduct->getProductionPlanningType() != Product::PRODUCTIONPLANNING_RECIPES_SUGG_SELL)
						{
							if ( config->toBoolCfgI(CfgInternal::USEPRODUCTIONPLANNINGDIALOG) )
							{
								int quantity = 1;
								HobList parameters;

								if ( transaction->getByCount() )
									quantity = transaction->getByCount();

								parameters.push_back(HobString(Integer::toString(currentProduct->getProductNum())));
								HobString asdf = currentProduct->getDescription();
								asdf.stripFontSequences();
					//			if ( currentProduct->getDescription().size() )
									parameters.push_back(asdf.substr(0, 20));
								parameters.push_back(Integer::toString(quantity));

								int code = (currMode == OpModePrintMultipleLabels) ? ProductErrFPPSinglePLUUpdateMultipleLabels : ProductErrFPPSinglePLUUpdate;
								switch ( currentProduct->getProductionPlanningType() )
								{
									case Product::PRODUCTIONPLANNING_ITEM:
									{
										mySession->addDialogMessage(ErrProductDev, code,
												HfcResponse::RESPONSESTYLE_FPP_PRODUCT, &parameters);

										return;
									}
									case Product::PRODUCTIONPLANNING_INGREDIENT:
									{
										mySession->addDialogMessage(ErrProductDev, code,
												HfcResponse::RESPONSESTYLE_FPP_INGREDIENT, &parameters);

										return;
									}
									case Product::PRODUCTIONPLANNING_MIXED:
									{
										mySession->addDialogMessage(ErrProductDev, code,
												HfcResponse::RESPONSESTYLE_FPP_DUAL, &parameters);

										return;
									}
									default:
									{
										/* If this is a non-production-planning product then continue with the
										usual print procedure. */
										break;
									}
								}
							}
						}
					}
				}
				else
				{
					if ( currMode == OpModeProductionEntry )
					{
						if ( productionEntryContainsFPPItems )
						{
							//currentProduct->getCurrentTransaction()->setUpdateProductionPlanning(false);
							SessionMessage noUpdateMsg(ProductDev, ProductDontUpdateProductionPlanning);
							noUpdateMsg.setFormatType(InternalFormat);
							mySession->send(&noUpdateMsg, Messenger::DEST_RTC);
						}
					}
					else
					{
						ProductTransaction* transaction = currentProduct->getCurrentTransaction();
						if ( transaction  && transaction->isProductionPlanningTransaction() )
						{
							switch ( currentProduct->getProductionPlanningType() )
							{
								case Product::PRODUCTIONPLANNING_ITEM:
								{
									currentProduct->getCurrentTransaction()->setUpdateProductionPlanning(false);

									break;
								}
								case Product::PRODUCTIONPLANNING_MIXED:
								{
									currentProduct->getCurrentTransaction()->setPrintLabels(true);
									currentProduct->getCurrentTransaction()->setUpdateProductionPlanning(false);

									break;
								}
								default:
									// Nothing (handle the other cases)
									break;
							}
						}
					}
				}
			}
			handleProductPrintKey3();
		}
	}

	if (!printThisLabel && currentProduct)
		currentProduct->setLabelBeingPrinted(Product::PRINTING_NO_LABEL);
}

void ProductManager::handleProductPrintKey3()
{
	Session *s( dynamic_cast<Session*>( mySession ) );
	if ( !s )
		return;

	Configuration *config( Configuration::getInstance() );
	if ( !config )
		return;

	const OpModeEvent currMode( s->getCurrentMode() );

	if (( currMode != OpModeProductionEntry ) && ( !s->isInAnAccumulatingMode() ))
	{
		Debug::trace("ProductManager", "invoke printLabel()");
		printLabel();
	}

	// Jira GCDR2-199 broke Accumulated Items out seperately
	if (( currMode == OpModeProductionEntry ) || ( currMode == OpModePrintMultipleLabels ))
	{
		Debug::trace("ProductManager", "invoke startPrintingAllLabels()");
		startPrintingAllLabels();
	}

	// Added this because this is where we end up when printing an Accumulated Items totals label from the 'Start of Run' screen
	// We end up here because current_product equals NULL in this screen. See anotated if test in function handleProductPrintKey3
	if ( s->isInAnAccumulatingMode() )
	{
        // SOF-4694 Operator not tracked when using Accumulate Items
		if (( config->toBoolCfg(fieldOffsetCfgOperateLoginRequired) ) &&
		     ( !config->toBoolCfg(fieldOffsetCfgOperateLoginRequired) && !config->toBoolCfg(fieldOffsetCfgCollectingTransactions) ) && 
		     ( s->getOperatorManager()->countAllOperators() > 0 && s->getOperatorManager()->getLoggedInOperator() == 0 ) )
		{
			waitingForLoginBeforePrinting = true;

			SessionMessage msg(OperatorDev, OpPromptForIDOrRFID);
			msg.setFormatType(BrowserFormat);
			mySession->send(&msg, Messenger::DEST_UI);
			return;
		}	

		startPrintingAllLabels();
	}

	//comment to stop continuous printing
	if ( Debug::isTracing("isContinuousPrint") )
	{
		WeigherStatus ws(mySession);
		ws.setLargeMotion();
		mySession->send(&ws, Messenger::DEST_RTC);

		PAL_absclass::PAL_sleep(1000);

		Debug::trace("ProductManager", "handlePrintKey - Sending a request weigher status");
		s->requestAWeigherStatus();
	}

	if ( s->getCurrentMode() == OpModeManualWeigh )
	{
		// Send out a request for the current weight so we can update the new current transaction.
		Debug::trace("ProductManager", "handlePrintKey - Sending a request weigher status");
		s->requestAWeigherStatus();
	}
}

/* Print the label for the current Product Transaction. */
void ProductManager::printLabel()
{
	Session *s( dynamic_cast<Session*>( mySession ) );

	if (( !s ) || ( !currentProduct ))
		return;

	// FT-809
	waitingForLoginBeforeAddToAccum = false;

	OpModeEvent currMode = s->getCurrentMode();

	if ((currMode != OpModeProductionEntry && currMode != OpModePrintMultipleLabels) ||
		(currentProduct->isNumOfLabelsEntered() && currentProduct->getCurrentTransaction() && currentProduct->getCurrentTransaction()->getNumOfLabelsToPrint() > 0))
	{
		if (currMode != OpModeProductionEntry && currMode != OpModePrintMultipleLabels && !s->isInAnAccumulatingMode())
			s->sendDialogToMonitoringSessions(true, ErrProductDev, ProductErrPrintingLabel, HfcResponse::RESPONSESTYLE_OK);
		else if (currentProduct->getProductionPlanningType() != Product::PRODUCTIONPLANNING_NO)
		{
			productionEntryContainsFPPItems = true;
		}

		CutTestManager *cutTestMgr = mgrContainer_->getCutTestManager();
		HobRecordNum cutTestNum = 0;
		if (cutTestMgr && cutTestMgr->isCuttingTestOn())
			cutTestNum = cutTestMgr->getCurrentCutTestNumber();

		ProductTransaction *transaction( currentProduct->printCurrentTransaction() );

#if defined( __linux__ )
		if (( Configuration::getInstance()->getCustomerHooksEnabled() ) && ( !currentProduct->hookBeforePrint() ))
		{
			Debug::trace("ProductManager", "printLabel() - invoking hook...");

			const long productNum( currentProduct->getProductNum() );
			const long deptNum( currentProduct->getDeptNum() );

			const std::string productNumStr( std::to_string( productNum ) );
			const std::string deptNumStr( std::to_string( deptNum ) );

			bool remotePrinting = false;

			if (( currentProduct->isPrintingRemoteLabel() ) || ( currentProduct->getRemotePrintTemplateRequest() ))
				remotePrinting = true;

			bool allowPrinting(
					remotePrinting ?
						hobart_cust_hook_before_remote_print( productNumStr.c_str(), deptNumStr.c_str() ) :
						hobart_cust_hook_before_print( productNumStr.c_str(), deptNumStr.c_str() ) );

			if ( !allowPrinting )
				return;

			currentProduct->setHookBeforePrint( true );
		}
#endif

		if (transaction)
		{
			transaction->setCutTestNumber(cutTestNum);

			if (s->getOperatorManager())
				transaction->associateOperator(s->getOperatorManager()->getLoggedInOperator());

			// SOF-3851
			// Update the transaction type in the current transaction, if any
			hfc::TransactionTypeManager *transTypeMgr( mgrContainer_->getTransactionTypeManager() );

			if ( !transTypeMgr )
				Debug::trace("ProductManager.printLabel", "no transaction type manager");
			else
			{
				Util::CSmartPtr<hfc::TransactionType> newType( transTypeMgr->getOperatorModeTransactionType() );

				if ( newType.null() )
					Debug::trace("ProductManager.printLabel", "new transaction type is NULL");
				else
				{
					Debug::trace("ProductManager.printLabel", "associate new transaction type " + util_string::inttostring((int) newType->getNumber()));
					transaction->associateTransactionType(newType);
				}
			}

			if (Debug::isTracing("ProductManager.printLabel"))
			{
				Debug::trace("ProductManager.printLabel", "**********	Committing the Transaction.	**********");
				transaction->show();
				Debug::trace("ProductManager.printLabel", "***************************************************");
			}

			mgrContainer_->getTransactionManager()->commitTransaction(transaction);

			// SOF-3209
			// track transactions and weigher state for valuemax
			if ( !Configuration::getInstance()->getValueMaxEnabled() )
				Debug::trace("ProductManager.valueMax", "Value Max DISABLED");
			else
			{
				Debug::trace("ProductManager.valueMax", "Value Max Enabled");

				// track only random weight items
				if (( Product::PRODUCTTYPE_RANDOMWEIGHT != transaction->getProductType() ) ||
					( !transaction->getWeigherStatus() ))
				{
					Debug::trace("ProductManager.valueMax", "Item is not random weight");
				}
				else
				{
					DiagnosticsValueMax valueMax( mySession );

					// compute monetary divisor
					float monetaryDivisor( 1.0 );

					int monetaryDigits( Configuration::getInstance()->getPrimaryMonetaryDecimalDigits() );

					while ( monetaryDigits-- )
						monetaryDivisor *= 10.0;

					// track counts and tilt
					const HfcWeigherStatus *weigherStatus( transaction->getWeigherStatus() );

					weigherStatus->show();

					valueMax.counts_ = weigherStatus->getCounts();

					switch ( weigherStatus->getEffectiveScaleFactor() )
					{
					case 5:
						valueMax.scaleFactor_ = 50;
						break;
					case 2:
						valueMax.scaleFactor_ = 20;
						break;
					case 1:
						valueMax.scaleFactor_ = 100;
						break;
					default:
						valueMax.scaleFactor_ = 0;
						break;
					}

					valueMax.tiltX_ = weigherStatus->getTiltX();
					valueMax.tiltX_ /= 100.0;

					valueMax.tiltY_ = weigherStatus->getTiltY();
					valueMax.tiltY_ /= 100.0;

					// track transaction info
					valueMax.transactionNum_ = transaction->getTransactionNum();

					// track item info
					valueMax.itemProductNum_ = transaction->getProductNum();

					valueMax.itemUnitPrice_ = transaction->getUnitPrice();
					valueMax.itemUnitPrice_ /= monetaryDivisor;

					valueMax.itemQuantity_ = 1;

					valueMax.itemWeight_ = transaction->getTotalNetWeight();
					valueMax.itemWeight_ /= 1000.0;

					valueMax.itemTare_ = transaction->getTotalTare();
					valueMax.itemTare_ /= 1000.0;

					valueMax.itemTotalPrice_ = transaction->getTotalPrice();
					valueMax.itemTotalPrice_ /= monetaryDivisor;

					// dispatch event
					mySession->send( &valueMax, Messenger::DEST_RTC );

				} // random weight item
			} // value max

			if (s->getOperatorManager() && s->getOperatorManager()->getLoggedInOperator())
				transaction->associateOperator(s->getOperatorManager()->getLoggedInOperator());

			if (transaction->isPrintLabels())
			{
				// Send the transaction record to the Real-time Controller for printing
				Debug::trace("ProductManager.printLabel", "Sending Product-Update Message to RTC to print a label ...");

				//	PTR 706: Let the Operating mode know if we do or don't need the weigherStatus/largeMotion
				//			 to print a label.
				bool ignoreWeigherStatus = (currentProduct && !(currentProduct->isRandomWeight() || currentProduct->isByCountWithWeight())) ? true : false;

				if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && Configuration::getInstance()->isSelfServiceScale())
				{
					Debug::trace("ProductManager.TestScreenLabel", "PrintToScreen - printLabel() print label to file");
					File printToScreenFile( Configuration::getInstance()->toStringCfgI(CfgInternal::SELFSERVEPRINTTOSCREENFILENAME) );
					if (printToScreenFile.exists())
						printToScreenFile.remove();
				}
				else
					Debug::trace("ProductManager.TestScreenLabel", "PrintToScreen - printLabel() do not print label to file");

				HfcProductUpdateMsg hpum(mySession, transaction, ignoreWeigherStatus);
				mySession->send(&hpum, Messenger::DEST_RTC);

				// Just sent the ProductUpdate message to the mode to print the label, so set up to record se
				Debug::trace("sweetheartWeight", "*** ProductManager - printLabel sent ProductUpdate - calling startRecordingSweetheartWeight() ***");
				startRecordingSweetheartWeight(transaction);

				if (Configuration::getInstance()->isSelfServiceScale())
				{
					if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen))
					{
						Debug::trace("ProductManager.TestScreenLabel", "printLabel() if ss and if printtoscreen and file does not exist");
						File printToScreenFile( Configuration::getInstance()->toStringCfgI(CfgInternal::SELFSERVEPRINTTOSCREENFILENAME) );
						if ( !printToScreenFile.exists() )
						{
							HobDateTime start;
							start.incrementSeconds(2);
							while (!printToScreenFile.exists())
							{
								Debug::trace("ProductManager.printLabel", "SELFSERVEPRINTTOSCREENFILENAME file didn't exist, so waiting 100 ms");
								PAL_absclass::PAL_sleep(100);
								HobDateTime now;
								if (now > start)
								{
									Debug::trace("ProductManager.printLabel", "Whatever! SELFSERVEPRINTTOSCREENFILENAME file didn't exist, notifying UI anyway");
									break;
								}
							}
						}

						// SOF-4432 - In self-serve mode, when displaying the label on the screen, the consumer has already finished the transaction by pressing
						// "Scan or Print". The label was just rendered above, so set the flag to ignore any weigher status messages while that plu is still on the screen.
						// If they remove the product from the platter before pressing Print, to additionally print the label, you will have to reboot the scale.
						// The new weight(zero) was processed for the current plu, but it should have been ignored, since the transaction was already over.
						s->setSelfServeIgnoreWeightChangeForPrintToScreen(true);
					}
					else
					{
						// Don't want the scale to timeout and return to the start screen while we are waiting for
						// the customer to take the label, so turn off the 'logout' timer.
						s->removeAutoLogoutTimer();
					}

					// If self service scale, tell the browser to display the 'printing' animated graphic.
					SessionMessage msg(ProductDev, ProductPrint);
					msg.setFormatType(BrowserFormat);
					string data("1");
					if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) || Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServeSearchKeypadFlashkeys) )
						data.append(string(",\"")).append(Configuration::getInstance()->toStringCfgI(CfgInternal::SELFSERVEPRINTTOSCREENFILENAME)).append(string("\""));
					GenericHfcObject gho(data);
					msg.setHfcObject(&gho);
					mySession->send(&msg, Messenger::DEST_UI);
				}
			}
			else
			{
				// If we don't print labels, then pretend the label was printed.

				Debug::trace("ProductManager.printLabel", "Transaction asks to not print labels (" +
					util_string::inttostring(transaction->getNumOfLabelsToPrint()) + "). Simulate Product Complete ...");

				transaction->setNumOfLabelsPrinted(transaction->getNumOfLabelsToPrint());

				for (int i = 1; i < transaction->getNumOfLabelsToPrint(); ++i)
				{
					ProductPackageCompleteMsg message(0, transaction->getTransactionNum(), transaction);
					Hobart::CMessageCenter::instance() << &message;
				}

				ProductCompleteMsg message(0, transaction->getTransactionNum(), transaction, true);
				Hobart::CMessageCenter::instance() << &message;
			}
		}
	}
}


int ProductManager::returnRemotePrintError(int errorNum)
{
	clearRemotePrintProduct();
	return errorNum;
}

int ProductManager::handleRemotePrintStartPrinting()
{
	bool isTracing = Debug::isTracing("ProductManager.remotePrint");
	if (isTracing)
		Debug::trace("ProductManager.remotePrint", "START handleRemotePrintStartPrinting()");

	// This mimics, mostly, the behavior of pressing the print multiple labels flashkey in Operate Mode
	// Sets the previousMode, updates the current mode to print multiple labels, tells the UI

	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return returnRemotePrintError(ProductErrPrintCantFindMainSession);

	if (!currentProduct)
		return returnRemotePrintError(ProductErrPrintNoProduct);

	ProductTransaction *transaction = currentProduct->printCurrentTransaction();
	if (!transaction)
		return returnRemotePrintError(ProductErrPrintTransactionMissingToPrint);

	currentProduct->setPrintingRemoteLabel(true);

	OpModeEvent currMode = s->getCurrentMode();
	opModePriorToPrintMultipleLabelsMode = currMode;

	if (isTracing)
		Debug::trace("ProductManager.remotePrint", "handleRemotePrintStartPrinting() - BEFORE changeMode to OpModePrintMultipleLabels");

	// Tell the Session to change to Print Multiple Labels mode.
	SessionMessage msg(OperationMode, OpModePrintMultipleLabels);
	msg.setFormatType(BrowserFormat);
	s->handleMessage(&msg);

	// during a remote print, just up above we set up a pluChg(ProductDev, ProductChange) msg so we can preimage labels
	// This creates a processEvent() in Operating Mode which calls changeProduct() and ManualWeighMode.
	// THe problem lies in just a short while we change the mode from Manual to Print Multiple Labels, and in the process
	// of doing that, it deletes the currentMode first.  If the processEvent() is NOT finished (and when it's not the 
	// scale crashes immediately), and we delete the currentMode right out of under it, instant crash. So lets give it time to finish
	// REMOTE PRINT ALLOW TIME TO CHANGE MODE
	PAL_absclass::PAL_sleep(250);

	if (isTracing)
		Debug::trace("ProductManager.remotePrint", "handleRemotePrintStartPrinting() - AFTER changeMode to OpModePrintMultipleLabels");

	//***********************************************************************************************************
	//***********************************************************************************************************
	//*****                                                                                                 *****
	//*****  Most of the next block of code is based on handleProductPrintKey3(), and printLabel()          *****
	//*****                                                                                                 *****
	//***********************************************************************************************************
	//***********************************************************************************************************

	currMode = s->getCurrentMode();
	if (currMode != OpModePrintMultipleLabels)		// just changed from if == opModeProductionEntry
	{
		if (isTracing)
			Debug::trace("ProductManager.remotePrint", "handleRemotePrintStartPrinting() - changed to OpModePrintMultipleLabels mode, but current mode is not OpModePrintMultipleLabels");
		return returnRemotePrintError(ProductErrPrintWrongOpMode);
	}

	bool ignoreWeigherStatus = (currentProduct && !(currentProduct->isRandomWeight() || currentProduct->isByCountWithWeight())) ? true : false;

	HfcProductUpdateMsg hpum(mySession, transaction, ignoreWeigherStatus);
	mySession->send(&hpum, Messenger::DEST_RTC);

	if (isTracing)
		Debug::trace("ProductManager.remotePrint", "handleRemotePrintStartPrinting() - BEFORE calling startPrintingAllLabels()");

	startPrintingAllLabels();

	if (isTracing)
		Debug::trace("ProductManager.remotePrint", "FINISHED handleRemotePrintStartPrinting() - returning RemotePrintSuccessfulPrint");
	return RemotePrintSuccessfulPrint;
}


int ProductManager::handleRemotePrintProduct(RemotePrintRequest* remotePrintRequest)
{
	bool isTracing = Debug::isTracing("ProductManager.remotePrint");
	if (isTracing)
		Debug::trace("ProductManager.remotePrint", "START handleRemotePrintProduct()");

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return ProductErrPrintCantFindMainSession;

	if ( !remotePrintRequest->isReceived(fieldOffsetRemotePrintProductNum) )
		return ProductErrPrintNoPluPassedIn;					// invalid plu

	HobRecordNum remotePrintPluNum = remotePrintRequest->getProductNum();
	if ( remotePrintPluNum < 1 || remotePrintPluNum > Product::MAX_PRODUCT_NUM )
		return ProductErrPrintInvalidPLU;					// invalid plu

	// Need to look up a product and get its deets
	// Read the product from the database
	if (isTracing)
		Debug::trace("ProductManager.remotePrint", "calling handleRemoteReadProductMessage()");

	Product prod;
	prod.setProductNum(remotePrintPluNum);

	int status_code = handleRemoteReadProductMessage(&prod);
	if (status_code != 0)
	{
		if (isTracing)
			Debug::trace("ProductManager.remotePrint", string("handleRemoteReadProductMessage() returned: ") + Integer::toString(status_code));
		return returnRemotePrintError(status_code);
	}

	// Debug::trace("ProductManager.remotePrint", "DONE handleProductReadMessage()");

	// Dont want to have to toggle the print button for a Fixed weight or By count PLU.
	// So, we are allowing the print key to be accepted irregardless of the currentMode on non Random weight PLUs.
	// would walmart want this condition enforced? They have it right now.

	if ( ( currentProduct->isRandomWeight() || currentProduct->isByCountWithWeight() ) && s->getCurrentMode() != OpModeManualWeigh)
		// because we can't use print key in Prepack Mode and must have a weight on it,
		// plus can't choose random weight in production entry mode at the scale,
		// plus if in Accumulate Items mode, we can bring up the item that needs to be weighed on the scale,
		//    but if someone walks up and places an item on the scale and presses Print, it will tell them there are no items in the list to print, which will be confusing
		return returnRemotePrintError(ProductErrPrintWrongOpMode);



	//***********************************************************************************************************
	//***********************************************************************************************************
	//*****                                                                                                 *****
	//*****  Most of the next block of code is based on handleProductPrintKey2()                            *****
	//*****                                                                                                 *****
	//***********************************************************************************************************
	//***********************************************************************************************************

	/* If I'm not in Manual-Weigh mode,
	* or if they haven't selected a product,
	* or if the net weight <= 0,
	* or if the net weight < minimum weight to print,
	* or if they haven't taken the last label,
	* and the user tries to press the print key,
	* don't let them.  Just send back an error message.
	*/

	waitingForStableWeightToPrint = false;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (REMOTE_PRINT_SUPPORT_BYCOUNT)
	{
		if (remotePrintRequest->isReceived(fieldOffsetRemotePrintByCount))
		{
			// so a value was passed in as the by count. See if it's valid
			int newByCount = remotePrintRequest->getByCount();

			if (newByCount > 99)
				return returnRemotePrintError(ProductErrPrintByCountAboveNineNine);
			if (newByCount <= 0)
				return returnRemotePrintError(ProductErrPrintInvalidByCount);

			if (currentProduct->isByCountForced())
				currentProduct->setByCountForced(false);

			handleRemoteByCountUpdate(newByCount);		// update the by count
		}
		else if (currentProduct->isByCountForced())
			return returnRemotePrintError(ProductErrPrintNoForcedByCounts);		// NOT TESTED
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (REMOTE_PRINT_SUPPORT_UNITPRICE)
	{
		if (remotePrintRequest->isReceived(fieldOffsetRemotePrintUnitPrice))
		{
			// then we must have a value and we may be updating if valid
			HobPrice newPrice = remotePrintRequest->getUnitPrice();

			if (newPrice > Configuration::getInstance()->getMaxUnitPrice())
				return returnRemotePrintError(ProductErrPrintExceedsMaxPrice);

			if (newPrice < 0)
				return returnRemotePrintError(ProductErrPrintInvalidUnitPrice);

			if (currentProduct->isUnitPriceForced())
				currentProduct->setUnitPriceForced(false);

			handleRemoteUnitPriceUpdate(newPrice);		// update the unit price
		}
		else if (currentProduct->isUnitPriceForced())
			return returnRemotePrintError(ProductErrPrintBadUnitPrice);			// NOT TESTED
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (REMOTE_PRINT_SUPPORT_CHANGE_LABEL)
	{
		if (remotePrintRequest->isReceived(fieldOffsetRemotePrintLabelTypeNum))
		{
			HobRecordNum newLabelTypeNum = remotePrintRequest->getLabelTypeNum();

			if (newLabelTypeNum <= 0)
				return returnRemotePrintError(ProductErrPriLabelNotFound);

			int status_code = handleRemoteLabelUpdate(newLabelTypeNum);
			if (status_code > RemotePrintSuccessfulPrint)
			{
				clearRemotePrintProduct();
				return status_code;
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (REMOTE_PRINT_SUPPORT_EXPANDED_TEXT)
	{
		if (remotePrintRequest->isReceived(fieldOffsetRemotePrintExpandedTextNum))
		{
			// TODO - update the expanded text - nothing yet
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (remotePrintRequest->isReceived(fieldOffsetRemotePrintNumOfLabels))
	{
		// support the number of labels sent in to be printed

		int numOfLabels = remotePrintRequest->getNumOfLabels();

		if (numOfLabels <= 0)
		{
			if (isTracing)
				Debug::trace("ProductManager.remotePrint", string("handleRemotePrintProduct() - num of labels invalid: ") + Integer::toString(numOfLabels));
			return returnRemotePrintError(ProductErrNumberOfStreamingLabelisZeroOrInvalid);
		}

		// Ignore what user passed in if we need to weigh the item, just set it to 1.
		if (currentProduct->isRandomWeight() || currentProduct->isByCountWithWeight())
			numOfLabels = 1;

		// Set the number of labels to be printed
		bool fieldUpdated = currentProduct->updateField(Product::fieldOffsetNumOfLabels, Integer::toString(numOfLabels));   // 87,4,true
	}
	else
	{
		if (isTracing)
			Debug::trace("ProductManager.remotePrint", string("handleRemotePrintProduct() - num of labels not passed in, returning: ") + Integer::toString(ProductErrNumberOfStreamingLabelisZeroOrInvalid));
		return returnRemotePrintError(ProductErrNumberOfStreamingLabelisZeroOrInvalid);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if ( remotePrintRequest->isReceived(fieldOffsetRemotePrintSellByDate) )
	{
		// support the sell by days sent in to be printed - the number of days need to be converted to a date

		int numOfDays = remotePrintRequest->getSellByDays();

		if ( numOfDays < 0 )
		{
			if ( isTracing )
				Debug::trace("ProductManager.remotePrint", string("handleRemotePrintProduct() - num of sell by days not passed in or invalid, returning: ") + Integer::toString(numOfDays));
			
			// we dont want to make it if we dont pass in sell by days then we have an error. We just want to go ahead and print.
			//return returnRemotePrintError(ProductErrNumberOfSellByDaysisZeroOrInvalid);
		}
		else
		{
			// Set the number of labels to be printed in the currentProduct
			bool fieldUpdated = currentProduct->updateField(Product::fieldOffsetShelfLifeDays, Integer::toString(numOfDays));

			// update the date on the label (hopefully)
			currentProduct->adjustShelfLifeDays(numOfDays);
		}
	}
	// we dont want to make it if we dont pass in sell by days then we have an error. We just want to go ahead and print.
	// else
	// {
	// 	 if (isTracing)
	//		 Debug::trace("ProductManager.remotePrint", string("handleRemotePrintProduct() - num of sell by days not passed in or invalid, returning: ") + Integer::toString(ProductErrNumberOfSellByDaysisZeroOrInvalid));
	//	 return returnRemotePrintError(ProductErrNumberOfSellByDaysisZeroOrInvalid);
	// }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	if ( remotePrintRequest->isPrintedLocally() )		// true means print cmd came from external host location, not from same scale
	{
		// cant lock out screen with RW or WtOnBC items bc the user has to push the print key
		//if ( currentProduct->isFixedWeight() || ( currentProduct->isByCount() && !currentProduct->isByCountWithWeight() ) )
			//mySession->addDialogMessage(ErrSessionDev, SessionDevRemotePrintWarning, HfcResponse::RESPONSESTYLE_OK);	// no RW or BCRW on print multiple labels
	}

	// remote print
	if ( currentProduct->isRandomWeight() || currentProduct->isByCountWithWeight() )
	{
		// Then we are trying to print a random weight or by count with weight item and we need to display it to 
		// the screen on the scale so the user can add a weight and then press the print key themselves. 
		// If a remotePrint command contains a RW PLU, send the product to the UI to be displayed. Dont display FW or BC
		SessionMessage pluData(ProductDev, ProductRemoteRequestRWItem);
		pluData.setFormatType(BrowserFormat);
		HobString remotePLU = HobString(Long::toString(currentProduct->getProductNum()));
		GenericHfcObject gho(remotePLU);
		pluData.setHfcObject(&gho);
		mySession->send(&pluData, Messenger::DEST_UI);

		s->requestAWeigherStatus();
		if (isTracing)
			Debug::trace("ProductManager.remotePrint", string("handleRemotePrintProduct() - using random weight or by count with weight item, returning: ") + Integer::toString(status_code));
		return ProductErrPrintRandomWeightPluReturned;		// tell user to put a weight on the platter and press print
	}


	status_code = handleRemotePrintStartPrinting();
	if (isTracing)
		Debug::trace("ProductManager.remotePrint", string("handleRemotePrintStartPrinting() returned: ") + Integer::toString(status_code));
	return status_code;
}


//  It's a party!  Print that cake label!
int ProductManager::handleRemotePrintTemplate(RemotePrintTemplateRequest *rptr)
{
	bool isTracing = Debug::isTracing("ProductManager.remotePrint");
	if (isTracing)
		Debug::trace("ProductManager.remotePrint", "START handleRemotePrintTemplate()");

	// NOTE: We are taking ownership of the RemotePrintTemplateRequest * passed in!

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	HobRecordNum ltnum = rptr->getLabelTypeNum();
	LabelType *lt = LabelType::findRecord(ltnum, 0, mySession);
	if ( !lt || lt->getErrorCode() == RECORD_NOT_FOUND )
	{
		if (isTracing)
			Debug::trace("ProductManager.remotePrint", string("No label type for template label: ") + Long::toString(ltnum) + "  returning: " + Integer::toString(ProductErrPriLabelNotFound));
		// Just return an error, we're not going to look in the family for an alternative label type
		if (lt)
		{
			delete lt;
			lt = 0;
		}
		if (rptr)
		{
			delete rptr;
			rptr = 0;
		}
		return ProductErrPrintInvalidTemplateNumber;
	}

	int status_code = checkForRemoteSizingStatus(lt, LabelTypeManager::EXACT_LABELFIT);	// ordinarily this would be responsemsgmgr->manage-Status()
	
	if (isTracing)
		Debug::trace("ProductManager.remotePrint", string("checkForRemoteSizingStatus() for template label returned: ") + Integer::toString(status_code));

	if (status_code > 0)
	{
		if (lt)
		{
			delete lt;
			lt = 0;
		}
		if (rptr)
		{
			delete rptr;
			rptr = 0;
		}
		return status_code;
	}

	short type = lt->determineBarcodeType();
	HobString barcodeString = rptr->getBarcodeString();

	if (isTracing)
	{
		Debug::trace("ProductManager.remotePrint", string("checkForRemoteSizingStatus() type: ") + Integer::toString(type) );
		Debug::trace("ProductManager.remotePrint", string("checkForRemoteSizingStatus() barcodeString: ") +  barcodeString );
	}

	if ( type != RECORD_NOT_FOUND && ( barcodeString.empty() || !barcodeString.isNumeric() || (type == UPCA && barcodeString.length() != 12) || (type == EAN13 && barcodeString.length() != 13) ) )
	{
		Debug::trace("ProductManager.remotePrint", string("checkForRemoteSizingStatus() begin processing 101162: ProductErrPrintInvalidBarcodeForTemplate") );
		Debug::trace("ProductManager.remotePrint", string("checkForRemoteSizingStatus() begin processing 101162: barcodeString.length(): ") + inttostring(barcodeString.length()) );

		Debug::trace("ProductManager.remotePrint", string("checkForRemoteSizingStatus() begin processing 101162: if type == UPCA, then type is 0"));
		Debug::trace("ProductManager.remotePrint", string("checkForRemoteSizingStatus() begin processing 101162: if type == UPCA, then barcodeString.lenght MUST = 12"));
		Debug::trace("ProductManager.remotePrint", string("checkForRemoteSizingStatus() begin processing 101162: if type == EAN13, then type is 3"));
		Debug::trace("ProductManager.remotePrint", string("checkForRemoteSizingStatus() begin processing 101162: if type == EAN13, then barcodeString.lenght MUST = 13"));

		if (lt)
		{
			delete lt;
			lt = 0;
		}
		if (rptr)
		{
			delete rptr;
			rptr = 0;
		}
		return ProductErrPrintInvalidBarcodeForTemplate;
	}

	Product *myProd = new Product();
	if (!myProd)
	{
		if (lt)
		{
			delete lt;
			lt = 0;
		}
		if (rptr)
		{
			delete rptr;
			rptr = 0;
		}
		return ProductErrPrintNoProduct;
	}

	if (isTracing)
		Debug::trace("ProductManager.remotePrint", string("handleRemotePrintTemplate - created a dummy Product*: ") + Hex::toString((long long)myProd));

	myProd->setDBAccess(mySession);
	myProd->setProductNum42(REMOTEPRINTTEMPLATE_PRODUCTNUM);
	myProd->setProductType(Product::PRODUCTTYPE_FIXEDWEIGHT);
	myProd->setDescription("It was a dark and stormy night.");
	myProd->setByCount(1);
        myProd->setLabelTypeNum(Product::fieldOffsetLabelTypeNum1, ltnum);
	myProd->setLabelTypeNum(Product::fieldOffsetDefaultLabelTypeNum1, ltnum);
	myProd->setLabelTypeNum(Product::fieldOffsetManualLabelTypeNum1, ltnum);
	myProd->setLabelType(FirstPrinter, lt);
	// Now the product owns the RemotePrintTemplateRequest pointer
	myProd->setRemotePrintTemplateRequest(rptr);

	myProd->getBarCodeNum().setBarcodeType(USER_SUPPLIED);
	myProd->getBarCodeNum().setBarcodeString(USER_SUPPLIED, barcodeString);

	// Default some of the other 'remote print' variables.
	waitingForStableWeightToPrint = false;

	// The product now has everything it needs to print the cake label.
	setCurrentProduct(myProd);


	status_code = handleRemotePrintStartPrinting();
	if (isTracing)
		Debug::trace("ProductManager.remotePrint", string("handleRemotePrintStartPrinting() returned: ") + Integer::toString(status_code));
	return status_code;
}


void ProductManager::handleRemoteByCountUpdate(int newByCount)
{
	if (!currentProduct || !mySession)
		return;

	int origValue = currentProduct->getByCount();						    // productUpdate->getFieldOrigValue();
	if ( newByCount == origValue )
		return;				// no need to udpate

	HobString productValue = "";

	bool fieldUpdated = currentProduct->updateField(Product::fieldOffsetByCount, Integer::toString(newByCount));		// 10,3,true
	if ( fieldUpdated )
	{
		ProductTransaction *currentTransaction = currentProduct->getCurrentTransaction();
		if (currentTransaction)
		{
			currentProduct->setOpModeByCount();
			// ***** this is the updater for unit price when BC changes *****
			currentTransaction->calculateFSDiscounts(currentProduct);

		}

		if ( currentProduct->isFixedWtOrFluidOz() )
		{
			//if ( !currentProduct->isByCountForced() || (currentProduct->isByCountForced() && currentProduct->getByCount() > 0) )
			//	currentProduct->setOpModeByCount();

			if (currentProduct->isFixedWtOrFluidOz() && !currentProduct->isByCountForced())
				currentProduct->setOpModeFixedWt();

			// keep around in case of weight on a bycount comes back
			//if (currentProduct->isByCountWithWeight())
			//{
			//	Session *s = dynamic_cast<Session *>(mySession);
			//	if (s)
			//		s->requestAWeigherStatus();
			//}
			//else
			//{
			//	ProductTransaction *currentTransaction = currentProduct->getCurrentTransaction();
			//	if (currentTransaction)
			//	{
			//		//	ByCount has changed, so update the FS discounts.
			//		currentTransaction->calculateFSDiscounts(currentProduct);
			//
			//		// Send the product total data back to the Browser.
			//		HfcProductUpdateValuesMsg hpuvm(currentTransaction);
			//		updateBrowserAndCustomerDisplay(hpuvm);
			//	}
			//}
		}
	}
}

void ProductManager::handleRemoteUnitPriceUpdate(HobPrice newPrice)
{
	if (!currentProduct)
		return;

	HobPrice origValue = currentProduct->getUnitPrice();	// productUpdate->getFieldOrigValue();


	// bc we update the unit price for RW in the current product, not in 101,15 like this
	//if (currentTransaction && !currentProduct->isRandomWeight() )
	ProductTransaction *currentTransaction = currentProduct->getCurrentTransaction();
	if ( currentTransaction )
		currentTransaction->setTotalPrice(newPrice);

	bool fieldUpdated = currentProduct->updateField(Product::fieldOffsetUnitPrice, Long::toString(newPrice));	// 12,2.98
}

int ProductManager::handleRemoteLabelUpdate(HobRecordNum remoteLabelNumber)
{
	WriteSingleField *productUpdate = new WriteSingleField(currentProduct->getProductNum(), Product::fieldOffsetLabelTypeNum1, Long::toString(remoteLabelNumber));

	LabelType *labelType = currentProduct->getLabelType(0);
	productUpdate->setFieldOrigValue( Long::toString(labelType->getLabelTypeNum()) );

	SessionMessage msg(ProductDev, ProductWriteSingleField);
	msg.setHfcObject(productUpdate);

	int returnVal = handleRemoteLabelSingleFieldMessage(&msg);  // Just created my own 101,16 handleProductWriteSingleFieldMessage(&msg)

	return returnVal;
}

int ProductManager::handleRemoteLabelSingleFieldMessage( SessionMessage *msg )
{
	// This method is like handleProductWriteSingleFieldMessage() but with just the label parts included
	int returnVal = RemotePrintSuccessfulPrint;

	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return ProductErrPrintCantFindMainSession;

	Configuration *config = Configuration::getInstance();
	if (!config)
		return ProductErrPrintCantFindConfiguration;

	WriteSingleField *productUpdate = dynamic_cast<WriteSingleField *> (msg->getHfcObject());
	if (!productUpdate || ( productUpdate->getRecordNum() != currentProduct->getProductNum() && s->getCurrentMode() != OpModeBuildAMeal ) )
		return ProductErrPrintCantCreateProductUpdate;

	HobString origValueString = productUpdate->getFieldOrigValue();
	HobString newValueString = productUpdate->getFieldNewValue();
	LabelType *pCorrectedLabelType = 0;
	short field = productUpdate->getFieldCode();
	HobString productValue = "";
	bool tryToUseOriginalLabel = false;
	int fieldUpdated = 0;

	if (field > 0)				// this will be 29
	{
		// The updateField() method will return non-0 if an error occurred updating the field.
		fieldUpdated = currentProduct->updateField(field, newValueString, true); 
		currentProduct->getFieldAsString(field, productValue, BrowserFormatPLUOperateMode );
	}

    // Used just for labelType testing when changing the label
	LabelType *joeLabelType2 = currentProduct->getLabelType(0);
	HobString joeLabel2 = (joeLabelType2) ? Long::toString(joeLabelType2->getLabelTypeNum()) : "0";
	
	Debug::trace("ProductManager.remotePrint", HobString("getLabel() 1: ") + joeLabel2 );

	if ( productValue == newValueString || fieldUpdated != 0 )		// hopefully we can use the new label entered
	{
		short printer = 0;
		LabelType *pLabelType = currentProduct->getLabelType(printer);
		LabelType *pCorrectedLabelType = 0;

		// Will we need to remote-ize this method? Yes. The error can be displayed in 
		// the labelTypeErrorTest but we will return it instead and display the result in else of next if text
		LabelMediaReturn result = labelTypeErrorTest(pLabelType, &pCorrectedLabelType, printer, currentProduct->getDeptNum());

		if (result == LabelOk || result == LabelCorrected)
		{
			if (result == LabelCorrected)
			{
				//	Set the corrected label type in the product record before we check the fields on the label.
				Debug::trace("ProductManager.remotePrint", string("handleRemoteLabelUpdateMessage():  LabelCorrected  - Found a label in the family to fit stock: PrinterNum = 0   OrigNum = ") + Long::toString(pLabelType->getLabelTypeNum()) + string("	NewNum = ") + Long::toString(pCorrectedLabelType->getLabelTypeNum()));
				
				currentProduct->setLabelType(printer, pCorrectedLabelType);
				return ProductErrPrintPrintOnCorrectedLabel;
			}

			pLabelType = currentProduct->getLabelType(0);

            // Used just for labelType testing when changing the label
			LabelType *joeLabelType2 = currentProduct->getLabelType(0);
			HobString joeLabel2 = (joeLabelType2) ? Long::toString(joeLabelType2->getLabelTypeNum()) : "0";
			Debug::trace("ProductManager.remotePrint", HobString("getLabel() 1: ") + joeLabel2 );
	
			if (pLabelType)
				pLabelType->generateEmbeddedLabels();
			
			// Send a Product Change message to the RTC so it knows the Imager needs to image the new label.
			SessionMessage pluChg(ProductDev, ProductChangeSmart);
			pluChg.setFormatType(InternalFormat);
			HfcProductChangeMsg hpcm(mySession, this);
			pluChg.setHfcObject(&hpcm);
			mySession->send(&pluChg, Messenger::DEST_RTC);

			//	Update the barcode information, since the label type changed.
			BarCodeNumber tempBarCode = currentProduct->getBarCodeNum();
			tempBarCode.getFromDatabase(currentProduct->getMyDbRecord(), currentProduct, dbAccess);
			currentProduct->setBarCodeNum(tempBarCode);
		}
		else
		{
			// display the result code from LabelMediaReturn result = labelTypeErrorTest(pLabelType, &pCorrectedLabelType, printer)
			tryToUseOriginalLabel = true;
		}
	}
	else
	{
		tryToUseOriginalLabel = true;
	}

	if ( tryToUseOriginalLabel )
	{
		// override label failed, can we use the original one still?
		short printer = 0;
		LabelType *pLabelType = currentProduct->getLabelType(printer);

		// see if we can correct to the original
		currentProduct->setLabelType(printer, 0);
		productValue = origValueString;
		productUpdate->setFieldNewValue(productValue);

		fieldUpdated = currentProduct->updateField(field, origValueString, true); 
		currentProduct->getFieldAsString(field, productValue, BrowserFormatPLUOperateMode );

		// Will we need to remote-ize this method? Yes. The error can be displayed in 
		// the labelTypeErrorTest but we will return it instead and display the result in else of next if text
		LabelMediaReturn result = labelTypeErrorTest(pLabelType, &pCorrectedLabelType, printer, currentProduct->getDeptNum());

		if ( result == LabelOk )
			return ProductErrPrintPrintOnDefaultLabel;
		else if ( result == LabelCorrected )
			return ProductErrPrintPrintOnCorrectedLabel;
		else if ( result == LabelInvalid )
			return ProductErrPrintInvalidDefaultAndOverride;
		else if ( result == LabelWontFit )
			return ProductErrPrintLabelDoesNotFit;
		else
			return ProductErrPrintUnknownLabelError;
	}

	return returnVal;
}

void ProductManager::clearRemotePrintProduct()
{
	bool returnToStartScreen = true;

	setCurrentProduct((Product *)NULL);

	// Send a Product Change message to the RTC so it knows it is dealing with a new plu.
	Debug::trace("ProductManager.remotePrint", "clearRemotePrintProduct - sending new plu message to RTC");
	SessionMessage noProductMsg(ProductDev, ProductChange);
	noProductMsg.setFormatType(InternalFormat);
	HfcProductChangeMsg hpcm(mySession, (Product *)NULL);
	noProductMsg.setHfcObject(&hpcm);
	mySession->send(&noProductMsg, Messenger::DEST_RTC);

	SessionMessage noProductMsg2(ProductDev, ProductChangeSmart);
	noProductMsg2.setFormatType(InternalFormat);
	HfcProductChangeMsg hpcm2(mySession, this);
	noProductMsg2.setHfcObject(&hpcm2);
	mySession->send(&noProductMsg2, Messenger::DEST_RTC);

	SessionMessage clearProductMsg(ProductDev, ProductClearUI);
	clearProductMsg.setFormatType(BrowserFormat);  // passing in alternate format when done printing in Production Entry mode, but clear plu after printing is false.
	GenericHfcObject returnToStartScreenFlag(Bool::toString(returnToStartScreen));
	clearProductMsg.setHfcObject(&returnToStartScreenFlag);
	mySession->send(&clearProductMsg, Messenger::DEST_UI);
}

int ProductManager::checkForRemoteSizingStatus(LabelType *labelType, bool requiresExactFit)
{
	// Use ProductManager::labelTypeErrorTest() when we start changing the label

	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return ProductErrPrintCantFindMainSession;

	//	Compare the label type against the label stock
	HfcPrinterStockMetrics *printerStockMetrics = s->getPrinterStockMetrics(0);
	if (!printerStockMetrics)
	{
		Debug::trace("ProductManager.remotePrint", "checkForRemoteSizingStatus: Label type record IS NULL - no family label found");
		return ProductErrPriLabelWontFit;
	}

	if ( !labelType || labelType->getErrorCode() != RECORD_OK )
	{
		Debug::trace("ProductManager.remotePrint", "checkForRemoteSizingStatus: Label type record IS NULL or NOT OK - not searching the family" );
		return ProductErrPriLabelNotFound;
	}

	if (printerStockMetrics->getLabelLengthInLines() <= 0)
	{
		Debug::trace("ProductManager.remotePrint", "checkForRemoteSizingStatus: printerStockMetrics->getLabelLengthInLines() <= 0");
		return ProductErrPriLabelWontFit;
	}

	LabelMediaReturn result = LabelInvalid;

	Debug::trace("ProductManager.remotePrint", "checkForRemoteSizingStatus: else printerStockMetrics->getLabelLengthInLines() > 0" );
				
	HobRecordNum labelTypeNum = labelType->getLabelTypeNum();
	HobRecordNum deptNum = labelType->getDeptNum();
	//HobRecordNum prodDeptNum = (currentProduct) ? currentProduct->getDeptNum() : 0;
	result = LabelTypeManager::validateLabelTypeWithStock(labelType->getLabelStockDescriptor(), printerStockMetrics, requiresExactFit, &labelTypeNum, &deptNum, deptNum /*prodDeptNum*/, dbAccess);
				
	if (result == LabelWontFit || result == LabelInvalid)
	{
		Debug::trace("ProductManager.remotePrint", "checkForRemoteSizingStatus: validateLabelTypeWithStock result == LabelWontFit" );
		return ProductErrPriLabelWontFit;
	}

	Debug::trace("ProductManager.remotePrint", "FINISHED checkForRemoteSizingStatus()" );

	return 0;
}


void ProductManager::handleCancelPrinting()
{
	Session *s = dynamic_cast<Session*>( mySession );

	// Prevent the session from timing out waiting for the Browser to respond to the
	// request for last minute changes before printing.
	if (( s ) && ( s->getCurrentModeOp() == Session::OP_PRINTKEYPRESSED ))
		s->setCurrentModeOp( Session::OP_UNKNOWN );

	if ( currentProduct )
		currentProduct->setLabelBeingPrinted( Product::PRINTING_NO_LABEL );

	// Now, if we need to go back to the start screen we will do that.
	bool printedShelfLifeExpirationLabel = false;
	bool printedRunTotalsLabel = false;

	if ( ( s->isInOperateMode() ) && ( s->isPrimarySession() ) )
	{
		// This is unnecessary at this point though it may be useful in the future
		if (( currentProduct->getLabelBeingPrinted() == Product::PRINTING_NEW_EXPIRATION_LABEL ) ||
			( currentProduct->getLabelBeingPrinted() == Product::PRINTING_EXISTING_EXPIRATION_LABEL ))
			printedShelfLifeExpirationLabel = true;

		if ( currentProduct->getLabelBeingPrinted() == Product::PRINTING_RUN_TOTALS_LABEL )
			printedRunTotalsLabel = true;
	}

	if (( Configuration::getInstance()->isServiceCounterScale() &&
		  Configuration::getInstance()->isClearPluAfterPrinting() &&
		  s->getCurrentMode() == OpModeManualWeigh &&
		  !printedShelfLifeExpirationLabel &&
		  !printedRunTotalsLabel )
		|| ( Configuration::getInstance()->isSelfServiceScale() ))
	{
		clearCurrentProduct( true );
	}
}


/*  This method sends the message to the Production Entry mode object to tell it to start printing the labels.
 *  Also used for Accumulate Items mode.
 */
void ProductManager::startPrintingAllLabels()
{
	if ( !mySession )
		return;

#if defined( __linux__ )
	if (( Configuration::getInstance()->getCustomerHooksEnabled() ) && ( currentProduct ) && ( !currentProduct->hookBeforePrint() ))
	{
		Debug::trace("ProductManager", "startPrintingAllLabels() - invoking hook...");

		const long productNum( currentProduct->getProductNum() );
		const long deptNum( currentProduct->getDeptNum() );

		const std::string productNumStr( std::to_string( productNum ) );
		const std::string deptNumStr( std::to_string( deptNum ) );

		bool remotePrinting( false );

		if (( currentProduct->isPrintingRemoteLabel() ) || ( currentProduct->getRemotePrintTemplateRequest() ))
			remotePrinting = true;

		bool allowPrinting(
			remotePrinting ?
			hobart_cust_hook_before_remote_print(productNumStr.c_str(), deptNumStr.c_str()) :
			hobart_cust_hook_before_print(productNumStr.c_str(), deptNumStr.c_str()) );

		if ( !allowPrinting )
			return;

		currentProduct->setHookBeforePrint( true );
	}
#endif

	Session* s = dynamic_cast<Session*> (mySession);

	// When the Production Entry mode prints labels, it will start dispatching messages indicating how many labels have printed, and the count of how many are to be printed.
	// We start listening for these messages, to pass them along to the UI, for displaying a progress bar.
	if (s && s->getCurrentMode() == OpModeProductionEntry)
		s->registerEvent(OperationMode, OpModeLabelsPrintedCounts);

	// Send a message to the Production Entry Mode to tell it to start printing the labels.
	SessionMessage msg(ProductDev, ProductStartPrintAllLabels);
	msg.setFormatType(InternalFormat);
	mySession->send(&msg, Messenger::DEST_RTC);

	// Need to also send a message to the Browser to indicate that the labels are about to start printing
	// (so it can change the menu, for one thing)
	if ( s && !s->isInAnAccumulatingMode() )
	{
		msg.setFormatType(BrowserFormat);
		mySession->send(&msg, Messenger::DEST_UI);
	}
	else
	{
		// OpModeAccumulateItems needs to clear the current product, to get it into the list of printing products, and to make the UI look cleaner.
		//clearCurrentProduct(true, false);
		Util::CSmartPtr<Product> printingProduct;
		if (currentProduct && currentProduct->hasOutstandingTransactions() && !findPrintingProduct(currentProduct, printingProduct))
			printingProducts.push_back(currentProductSmartPtr);
	}
}


void ProductManager::handlePackageComplete(HfcProductPackageCompleteMsg * packageCompleteMsg)
{
	/* Since only the primary console session can initiate printing a label, only the primary
	 * console's products will we be receiving Product complete messages.
	 * Thus, if my session is not the primary console session, ignore the Product Complete message.
	 */
	bool isTracing = Debug::isTracing("ProductManager.handlePackageComplete");

	if ( isTracing )
		Debug::trace("ProductManager.handlePackageComplete", "started");

	Session *s = dynamic_cast<Session *> (mySession);
	if ( !s || !packageCompleteMsg || !dbAccess )
		return;

	ProductTransaction *trans = const_cast<ProductTransaction*>(packageCompleteMsg->getProductTransaction());
	if ( !trans || !trans->getProduct() )
		return;

	if ( isTracing )
		Debug::trace("ProductManager.handlePackageComplete", "The message has a transaction (" +
			util_string::longtostring(trans->getTransactionNum()) + ") which has a product *");

	const Product *prod = trans->getProduct();
	Util::CSmartPtr<Product> printingProduct;

	if ( prod != currentProduct && !findPrintingProduct(prod, printingProduct) )
	{
		if (isTracing)
			Debug::trace("ProductManager.handlePackageComplete", "I have no outstanding transactions for that product *; it is being ignored!!!");

		return;
	}

	if ( s->isInOperateMode() && s->isPrimarySession() )
	{
		trans->updateTransactionTotalValues();
		if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgCollectingTransactions) &&
				(s->getCurrentMode() == OpModeProductionEntry || prod->isPrintingInProductionEntryMode()) )
		{
			hfc::TransactionManager* transManager = mgrContainer_->getTransactionManager();

			if ( transManager )
				transManager->handleSaveTransaction(transManager->createProductionEntryTransaction(trans).get());
		}
	}

	if ( prod->isPrintingRemoteLabel() )
		Debug::trace("ProductManager.remotePrint", string("handlePackageComplete() - labels printed ... ") + inttostring(trans->getNumOfLabelsPrinted()) + " of " + inttostring(trans->getNumOfLabelsToPrint()));

	if (isTracing)
		Debug::trace("ProductManager.handlePackageComplete", "finished");
}


void ProductManager::handleProductComplete(HfcProductCompleteMsg *productCompleteMsg)
{
//	*****************************************************************************************************************
//	CLARE: Check the boolean in the HfcProductCompleteMsg. If it didn't print, don't update totals and such.
//	Send an error message to the browser.
//	*****************************************************************************************************************

	/* todo
	 * Look for the associated item in the trackingProducts List.  If not found, LOG ERROR MESSAGE.
	 * If found, check it for the transaction number.  If not found, LOG ERROR MESSAGE.
	 * Process the transaction that the Real-Time Controller is finished with:
	 * save totals, free up memory, etc. ...
	 */
	/*
	if (! found Product)
	{
		Debug::trace("ProductManager", "Error - I received a Product Complete message, but I couldn't find the corresponding Product object.");
	}
	if (! found ProductTransaction)
	{
		Debug::trace("ProductManager", "Error - I received a Product Complete message, but I couldn't find the corresponding ProductTransaction object.");
	}


	if (product * from incoming message != &currentProduct &&
			!product->hasOutstandingTransactions())
		delete product;
	*/

	/* Since only the primary console session can initiate printing a label, only the primary
	 * console's products will we be receiving Product complete messages.
	 * Thus, if my session is not the primary console session, ignore the Product Complete message.
	 */

	bool isTracing = Debug::isTracing("ProductManager.HandleProductComplete");

	if ( !productCompleteMsg )
		Debug::trace("ProductManager.printAccumulate", "We dont have a transaction so this is likely going to fail");
        
	if (isTracing)
		Debug::trace("ProductManager.HandleProductComplete", ((productCompleteMsg->getEventType() == ProductComplete) ? "HandleProductComplete" : "HandleProductIncomplete"));

	Session *s = dynamic_cast<Session *> (mySession);

	if (!s || !productCompleteMsg || !dbAccess)
		return;

	bool isInOperateMode = (s->isInOperateMode()) ? true : false;

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	ProductTransaction *trans = (ProductTransaction *)NULL;
	trans = productCompleteMsg->getProductTransaction();

	if (!trans)
		return;

	if (isTracing)
		Debug::trace("ProductManager.HandleProductComplete", HobString("The message has a transaction ") + HobString(Long::toString(trans->getTransactionNum())));

	Product *prod = trans->getProduct();

	if (!prod)
		return;

	if (isTracing)
		Debug::trace("ProductManager.HandleProductComplete", "The transaction has a product *");

	// SOF-4432 - When we're completely done with the plu, Clear the flag to ignore weigher statuses when in self-serve mode, displaying the label on the screen, 
	// and the consumer has already finished the transaction by pressing "Scan or Print".
	s->setSelfServeIgnoreWeightChangeForPrintToScreen(false);

	// FT-873: do *NOT* dereference prod (or dummyProduct for that matter) as it may have been deleted already
	if (( currentProductSmartPtrSave ) && ( dummyProduct ) && ( prod == dummyProduct ))
	{
		bool tracing = Debug::isTracing("ProductManager.supportPrint");
		if (tracing)
		{
			Debug::trace("ProductManager.supportPrint", HobString("handleProductComplete - dummy Product*=") + HobString(Hex::toString((long long) prod)));
			Debug::trace("ProductManager.supportPrint", HobString("handleProductComplete - current Product*=") + HobString(Hex::toString((long long) currentProduct)));
		}

		currentProductSmartPtr = currentProductSmartPtrSave;
		currentProduct = currentProductSmartPtr.get();
		currentProductSmartPtrSave = 0;

		dummyProduct = NULL; // FT-873

		Debug::trace("ProductManager.HandleProductComplete", HobString("handleProductComplete() set current Product") + HobString(Hex::toString((long long) currentProduct)));

		SessionMessage supportProductMsg(ProductDev, ProductChange);
		supportProductMsg.setFormatType(InternalFormat);
		HfcProductChangeMsg hpcm(mySession, currentProduct);
		supportProductMsg.setHfcObject(&hpcm);
		mySession->send(&supportProductMsg, Messenger::DEST_RTC);

		SessionMessage supportProductMsg2(ProductDev, ProductChangeSmart);
		supportProductMsg2.setFormatType(InternalFormat);
		HfcProductChangeMsg hpcm2(mySession, this);
		supportProductMsg2.setHfcObject(&hpcm2);
		mySession->send(&supportProductMsg2, Messenger::DEST_RTC);

		Debug::trace("ProductManager.HandleProductComplete", "handleProductComplete() finished setting up for current plu");

		return;
	}

	Product *foundProduct = (Product *)NULL;
	Util::CSmartPtr<Product> printingProduct;

	bool foundPrintingProduct = false;
	if (prod != currentProduct)
	{
		foundPrintingProduct = findPrintingProduct(prod, printingProduct);
		if (!foundPrintingProduct)
		{
			if (isTracing)
				Debug::trace("ProductManager.HandleProductComplete", "I have no outstanding transactions for that product *; it is being ignored!!!");

			return;
		}
		else
			foundProduct = printingProduct.get();
	}
    
	bool printedShelfLifeExpirationLabel = false;
	bool printedRunTotalsLabel = false;
	bool saveForTwoPrinterSystemEnabled = false;

	if (( isInOperateMode ) && ( mySession->isPrimarySession() ))
	{
		// Rules: saveForTwoPrinterSystemEnabled: Listed below when tested for the rest of them
		if (FpmManager::getInstance()->isWrapperTwoPrinter())
		{
			if (!currentProduct)
			{
				Debug::trace("ProductManager.HandleProductComplete", "currentProduct is NULL so exit");
				SessionMessage msg(ProductDev, ProductComplete);
				msg.setFormatType(BrowserFormat);
				mySession->send(&msg, Messenger::DEST_UI);
				return;
			}

			if (currentProduct->getProductNum() == 0)
			{
				//Debug::trace("ProductManager.HandleProductComplete", "currentProduct PLU Number equals 0 so exit");
				SessionMessage msg(ProductDev, ProductComplete);
				msg.setFormatType(BrowserFormat);
				mySession->send(&msg, Messenger::DEST_UI);
				return;
			}

		    LabelType* lt1 = currentProduct->getLabelType(FirstPrinter);						//See if and when we save transaction and run totals when we are a 2P system
		    LabelType* lt2 = currentProduct->getLabelType(SecondPrinter);

		    if (lt1 && lt2 && ((lt1->getLabelTypeNum() > 0) && (lt2->getLabelTypeNum() > 0)))
			    saveForTwoPrinterSystemEnabled = true;											// we are  a two printer system and can ONLY save when conditions met
		}
		
		if (( prod->getLabelBeingPrinted() == Product::PRINTING_NEW_EXPIRATION_LABEL ) ||
		    ( prod->getLabelBeingPrinted() == Product::PRINTING_EXISTING_EXPIRATION_LABEL ))
		{
			// Finished printing the shelf life expiration label.
			if (productCompleteMsg->isPrintOk())
				mgrContainer_->getExpirationLabelManager()->handleProductCompleteMessage(prod->getLabelBeingPrinted(), prod->getProductNum(), prod->getDeptNum(), prod->getDescription(), trans->getExpirationLabelDateTime());
			else
				Debug::trace("ProductManager.HandleProductComplete", "Printed Shelf Life Expiration Label failed");

			printedShelfLifeExpirationLabel = true;
		}
		else if ( prod->getLabelBeingPrinted() == Product::PRINTING_RUN_TOTALS_LABEL )
		{
			// Finished printing the run totals label.
			if ( !productCompleteMsg->isPrintOk() )
				Debug::trace( "ProductManager.HandleProductComplete", "Printed Run Totals Label failed" );
			else
			{
				// existing functionality is NOT to clear totals after we print run totals... if
				// this needs to change you can modify this variable
				bool clearTotals = false;

				if (clearTotals)
					mgrContainer_->getProductTotalManager()->clearAndDisplayProductRunTotal();
			}

			printedRunTotalsLabel = true;
			trans->setPrintedRunTotalsTransLabel(true);
		}
		else
		{
			// Finished printing the regular label type.
			if (config->isSelfServiceScale())
			{
				// If self service scale, tell the browser to remove the 'printing' animated graphic.
				SessionMessage msg(ProductDev, ProductPrint);
				msg.setFormatType(BrowserFormat);
				GenericHfcObject gho("0");
				msg.setHfcObject(&gho);
				mySession->send(&msg, Messenger::DEST_UI);
			}
// HT-317 Keyboard Entered Weight will not print a label TESTING OF VALUES			
//			bool isItOk       = productCompleteMsg->isPrintOk();
//			int labelsToPrint = trans->getNumOfLabelsToPrint();
//			int labelsPrinted = trans->getNumOfLabelsPrinted();
//
//			Debug::trace("ProductManager.HandleProductComplete", " isItOkToPrint: " + HobString(Bool::toString(isItOk)) );
//			Debug::trace("ProductManager.HandleProductComplete", " labelsToPrint: " + HobString(Integer::toString(labelsToPrint)) );
//			Debug::trace("ProductManager.HandleProductComplete", " labelsPrinted: " + HobString(Integer::toString(labelsPrinted)) );
//
			if ( productCompleteMsg->isPrintOk() || (trans->getNumOfLabelsToPrint() > 0 && trans->getNumOfLabelsPrinted() > 0) )
			{
				//	PTR 1662:	For a prepack scale, this is 1 package. Two or more printers is still 1 package.
				//				The printer manager increments the # of pkgs. for each label taken (HLX feature).

				// save 2P transactions - or not
				bool saveTwoPrinterTotalsNow = false;
				if (saveForTwoPrinterSystemEnabled )
				{
					Debug::trace("ProductManager.HandleProductComplete", "Two Printer system and  saveForTwoPrinterSystemEnabled IS TRUE - lets see if number of label printed is 2 yet");

					// RULES:  since handleProductComplete is called once for each label printed we only want to save the totals only once both labels are printed and that is it
					// RULES:  now if the LabelsToPrint value is fixed, and it indicates 2 instead of 1 it is reporting right now, this test could be simplified and test if 
					// RULES:  LabelsPrinted == 2 AND LabelsPrinted == 2 THEN SAVE
					// 1: Two Printer System
					// 2: LabelsPrinted = 2
					// 3: LabelsToPrint = 2
					// 4: currentPLU has 2 valid labels to print
					
					// We are changing this to save the transaction when at least one label is printed, instead of 2, because Costco had a situation where
					// they could be turning off the second printer (for the USDA label), which would run the package through the wrapper as if it was only 
					// supposed to print one label.  But then the transaction wouldn't be saved since 2 labels weren't printed, and so when the operator 
					// ends a cut test, there wouldn't be any transactions for the cut test, and so we would delete the cut test.
					//if (trans->getNumOfLabelsPrinted() == 2)
					if (trans->getNumOfLabelsPrinted() >= 1)
						saveTwoPrinterTotalsNow = true;
				}

				if (Configuration::getInstance()->isPrepackScale())
					trans->setNumberPackages(1);

				//	Update Transaction Values
				if (FpmManager::getInstance()->isWrapperTwoPrinter() && saveForTwoPrinterSystemEnabled )
				{
					if (!trans->getPrintedRunTotalsTransLabel() && saveTwoPrinterTotalsNow)
						trans->updateTransactionTotalValues();
				}
				else
	            {
					if (!trans->getPrintedRunTotalsTransLabel())
						trans->updateTransactionTotalValues();							// was previously done automtically
				}

                if (FpmManager::getInstance()->isWrapperTwoPrinter() && saveTwoPrinterTotalsNow )
				{
					// Send ProductComplete
					SessionMessage msg(ProductDev, ProductComplete);
					msg.setFormatType(BrowserFormat);
					mySession->send(&msg, Messenger::DEST_UI);

				}

				OnlineOrderProduct *ooProd = dynamic_cast<OnlineOrderProduct *> (prod);
				if (ooProd)
				{
					// Need to send this product's total price, total weight, total by count, etc. back to the browser so we can inform the blackbox/server of the totals
					Debug::trace("OnlineOrderProduct", " Handling Online Order Product: " + ptrtostring(ooProd));

					GenericHfcObject data( longtostring(ooProd->getOrderNum()) + "," +
						                   longtostring(ooProd->getOrderProductId()) + "," +
						                   longtostring(ooProd->getProductNum()) + "," +
						                   ( (ooProd->isRandomWeight() || ooProd->isByCountWithWeight()) ? longtostring(trans->getTotalNetWeight()) : "0") + "," +
						                   inttostring(trans->getByCount()) + "," +
						                   longtostring(trans->getTotalPrice()) );
					Debug::trace("OnlineOrderProduct", " Sending product totals to UI, for forwarding on to blackbox: " + data.getData());

					SessionMessage msg(ProductDev, ProductWriteOnlineOrderingProductTotals);
					msg.setFormatType(BrowserFormat);
					msg.setHfcObject(&data);
					mySession->send(&msg, Messenger::DEST_UI);

				}


				CutTestManager *cutTestMgr = mgrContainer_->getCutTestManager();	//	HA-56: Save transaction if Cutting test is on.
				if ((config->toBoolCfg(fieldOffsetCfgCollectingTransactions) || (cutTestMgr && cutTestMgr->isCuttingTestOn())))
				{
					if ((trans->getNumOfLabelsPrinted() > 1) && (!prod->isDummyProduct()) &&
						((OpModeProductionEntry == s->getCurrentMode()) || (prod->isPrintingInProductionEntryMode())))
					{
						hfc::TransactionManager* transManager = mgrContainer_->getTransactionManager();

						if (transManager)
						{
							//	Save Transaction - only if this is not a Printed Run Totals Label. Dont update if we are just PRINTING a label
							if (FpmManager::getInstance()->isWrapperTwoPrinter() && saveForTwoPrinterSystemEnabled)
							{
								if (!trans->getPrintedRunTotalsTransLabel() && saveTwoPrinterTotalsNow)
									transManager->handleSaveTransaction(transManager->createProductionEntryTransaction(trans));
							}
							else
							{
								if (!trans->getPrintedRunTotalsTransLabel())
									transManager->handleSaveTransaction(transManager->createProductionEntryTransaction(trans));
							}
						}
					}
					else
					{
						//	Save Transaction - only if this is not a Printed Run Totals Label. Dont update if we are just PRINTING a label
						if (FpmManager::getInstance()->isWrapperTwoPrinter() && saveForTwoPrinterSystemEnabled)
						{
							if (!trans->getPrintedRunTotalsTransLabel() && saveTwoPrinterTotalsNow)
								mgrContainer_->getTransactionManager()->handleSaveTransaction(trans);
						}
						else
						{
							if (!trans->getPrintedRunTotalsTransLabel())
								mgrContainer_->getTransactionManager()->handleSaveTransaction(trans);
						}
					}
				}

				//	Update the current ProductRunTotal here.
				if (mgrContainer_->getProductTotalManager())
				{
					//	Save RunTotals - only if this is not a Printed Run Totals Label. Dont update if we are just PRINTING a label
					if (FpmManager::getInstance()->isWrapperTwoPrinter() && saveForTwoPrinterSystemEnabled )
					{
						if (!trans->getPrintedRunTotalsTransLabel() && saveTwoPrinterTotalsNow)
							mgrContainer_->getProductTotalManager()->updateAndDisplayProductRunTotal(trans);
					}
					else
					{
						if (!trans->getPrintedRunTotalsTransLabel())
							mgrContainer_->getProductTotalManager()->updateAndDisplayProductRunTotal(trans);
					}
				}

				if ( trans->isUpdateProductionPlanning() && !s->isFPPTestPrint() && prod->getProductionPlanningType() != Product::PRODUCTIONPLANNING_RECIPES_SUGG_SELL )
					updateProductionPlanningApplication(trans);


				DBProductStatisticsRM *dbProductStatisticsRM = dbAccess->getDBProductStatisticsRM();
				if (dbProductStatisticsRM)
				{
					std::string hour;
					std::string lang;

					if (config->isSelfServiceScale())
					{
						// determine the hour for the statistics record to be updated
						hour = Integer::toString(config->determineHourForQuery());
						LanguageSelection langNum = (s && !config->toBoolCfgI(CfgInternal::ONLYPRILANGFORPRODSTATS)) ? s->getCurrentLanguageSelection() : PrimaryLanguage;
						lang = (langNum == PrimaryLanguage) ? config->toStringCfg(fieldOffsetCfgPrimaryLanguageCode) : config->toStringCfg(fieldOffsetCfgSecondaryLanguageCode);
					}
					else
					{
						//starting with Fastrack, these statistics aren't just for self-serve anymore

						// For Fastrack, we're not using a productStatistics hour method, use 0, which means to just accumulate all product stats into one bucket (for the plu)
						hour = "0";
						// For Fastrack, (non-selfServe), we're not separating the statistics for the language, so just use the primary language.
						lang = config->toStringCfg(fieldOffsetCfgPrimaryLanguageCode);
					}

					if (isTracing)
						Debug::trace("ProductManager.HandleProductComplete", "looking for prodStats record  hour:" + hour + "  lang:" + lang);


					if (hour.size() && lang.size() && prod->getMyDbRecord())
					{
						edb::DBProductStatistic *myProductStatistic = dbProductStatisticsRM->find(prod->getMyDbRecord(), lang, hour);
						if (!myProductStatistic)
						{
							if (isTracing)
								Debug::trace("ProductManager.HandleProductComplete", "prodStats record not found - creating it");
							myProductStatistic = dbProductStatisticsRM->createRecord(prod->getMyDbRecord());
							myProductStatistic->setLanguage(lang);
							myProductStatistic->setHourNumber(hour);
							myProductStatistic->setTotalLabelsPrinted(0);
						}

						if (myProductStatistic)
						{
							// Have a statistics record.  Update it to indicate the number of times the record was printed.
							int labelsPrinted = (myProductStatistic->isTotalLabelsPrintedNull()) ? 1 : myProductStatistic->getTotalLabelsPrinted() + 1;
							if (isTracing)
								Debug::trace("ProductManager.HandleProductComplete", "prodStats updating  labelsPrinted:" + inttostring(labelsPrinted));
							myProductStatistic->setTotalLabelsPrinted(labelsPrinted);
							HobDateTime dt;
							myProductStatistic->setRecordedDateTime(dt.convertToDBFormat());
							dbProductStatisticsRM->saveRecord(myProductStatistic);

							delete myProductStatistic;
						}
					}
				}

			}
			else
			{
				if (isTracing)
					Debug::trace("ProductManager.HandleProductComplete", "There was a problem with the print!");

				// If the label didn't print right, then we don't need to keep track of sweethearting
				Debug::trace("sweetheartWeight", "*** ProductManager - handleProductComplete label print problem - calling stopRecordingSweetheartWeight() ***");
				stopRecordingSweetheartWeight(false);
			}

			// This logout has to come after saving the operator changes to the database above.
			// This used to check 'config->isRFIDHWPresent()' as a condition for logging out, but starting with Fastrack, non-RFID scales behave like
			// HLX RFID scales, users only login when needed for a particular plu (operator login required, or they changed a field that has an
			// access level set for it, or they are tracking Operator changes and a field was changed), and we logout when we're done with that item.
			//	HA-185: We don't auto log out for a Prepack scale.
			if ( s->getOperatorManager() && Configuration::getInstance()->isServiceCounterScale() )
			{
				HobOperator *oper = s->getOperatorManager()->getLoggedInOperator();
				if (oper && oper->getLoginMethod() == HobOperator::LOGINMETHOD_OPNUM)
				{
					bool cutTestOn = false;
					CutTestManager *cutTestMgr = mgrContainer_->getCutTestManager();
					if (cutTestMgr)
						cutTestOn = cutTestMgr->isCuttingTestOn();

					// FT-548: do not auto logout operator when in pre-pack mode, or when a cutting test is being run
					if ( ( OpModeAutoWeigh != s->getCurrentMode() ) && ( OpModeAutoManualApply != s->getCurrentMode() ) && !cutTestOn )
					{
						SessionMessage logout(OperatorDev, OpLogout);
						s->getOperatorManager()->handleMessage(&logout);
					}
				}
			}
		}
	}

	Debug::trace("ProductManager.HandleProductComplete", "Hey lets check printedRunTotalsLabel AT THE END: " + HobString(Bool::toString(printedRunTotalsLabel)));

	if ( printedRunTotalsLabel && (trans->getNumOfLabelsToPrint() > 0 && trans->getNumOfLabelsPrinted() > 0))
	{
		// SOF-4977 Product totals not correct on the 2P wrapper
		// I see this as getNumOfLabels = 1 and NumOfLabelsPrinted = 1.  if printedRunTotalsLabel is true, which means we wanted a Run Totals Label, and getNumOfLabelsPrinted() = 1
		// then that means we printed out Run Totals Label. So why do we still need the NumOfLabelsToPrint > 1?  I dont think we do.  And if it is 1, then the next time thru
		// this method, we actually SAVE the transaction to the list of transactions (and run totals) because getNumOfLabels = 1 and NumOfLabelsPrinted = 1. We dont want to SAVE 
		// a trans when we just printed a run totals label, so set this to 0
		Debug::trace("ProductManager.HandleProductComplete", "IF Checked printedRunTotalsLabel AT THE END : " + HobString(Bool::toString(printedRunTotalsLabel)));
		Debug::trace("ProductManager.HandleProductComplete", "IF Checked trans->getNumOfLabelsPrinted() : " + HobString(Bool::toString(trans->getNumOfLabelsPrinted())));
		Debug::trace("ProductManager.HandleProductComplete", "IF Checked trans->getNumOfLabelsToPrint() : " + HobString(Bool::toString(trans->getNumOfLabelsToPrint())));
		trans->setNumOfLabelsToPrint(0);
	}
	else
	{
		Debug::trace("ProductManager.HandleProductComplete", "ELSE Checked printedRunTotalsLabel AT THE END : " + HobString(Bool::toString(printedRunTotalsLabel)));
		Debug::trace("ProductManager.HandleProductComplete", "ELSE Checked trans->getNumOfLabelsPrinted() : " + HobString(Bool::toString(trans->getNumOfLabelsPrinted())));
		Debug::trace("ProductManager.HandleProductComplete", "ELSE Checked trans->getNumOfLabelsToPrint() : " + HobString(Bool::toString(trans->getNumOfLabelsToPrint())));
	}

	// For Print Multiple Labels, we need to create the next transaction while the current one still exists (it will be
	// deleted in the up-coming call to completeTransaction(trans).
	if (s->getCurrentMode() == OpModePrintMultipleLabels && productCompleteMsg->getEventType() != ProductIncomplete)
	{
		if (currentProduct)
			currentProduct->createProductTransaction();
	}

	if (productCompleteMsg->getEventType() != ProductIncomplete)
	{
		if (isTracing)
			Debug::trace("ProductManager.HandleProductComplete", "calling prod->completeTransaction() because not Product Incomplete");
		prod->completeTransaction(trans);
	}

	prod->setLabelBeingPrinted(Product::PRINTING_NO_LABEL);

	if (isTracing)
		Debug::trace("ProductManager.HandleProductComplete", "The product is done handling the transaction");
	
	// The IF is for productionEntry. The else is for accumulateItems to remove the item. And the last item from accumulateItems
	// in the list is not being removed. Kind of what PTR 334 is saying for productionEntry. 
	// last check before I enter or skip the else

	//	PTR 334: Make sure that for Production Entry, the Product is removed from the printingProducts list
	//			 so its CSmartPtr can be cleared, thus deleting the Product record. Otherwise, the CSmartPtrs
	//			 will remain in this list until the ProductManager goes away.
	if (prod && (s->getCurrentMode() == OpModeProductionEntry))
	{
		//Debug::trace("ProductManager.HandleProductComplete", "Here 1");

		if (isTracing)
			Debug::trace("ProductManager.HandleProductComplete", "OpModeProductionEntry");

		if (!prod->hasOutstandingTransactions())
		{
			if (foundProduct)
				foundProduct = 0;

			if (isTracing)
				Debug::trace("ProductManager.HandleProductComplete", "OpModeProductionEntry:  This product has no outstanding transactions so it is being deleted.");
			
			removeAPrintingProduct(prod);       // production entry - allow it to do the original method if deleting one by one on handleProductComplete()

			// SOF-5498
			if ( prod == currentProduct )
			{
				Debug::trace("ProductManager.HandleProductComplete", "Complete message was for current product- clearing out current product");
				/*
				* SOF-5828 - This line below was causing a restart required when exiting production entry it also causes the GCD not to clear after a P.E run
				* I tested the original issue reported in 5498 without this line and it would not happen - someone else should review this...
				*/
				//currentProduct = NULL;
			}

			Debug::trace("ProductManager.HandleProductComplete", "Bye bye HandleProductComplete() when OpModeProductionEntry");

			return;
		}
	}
	else if ( (prod != currentProduct) || (prod && (s->getCurrentMode() == OpModeAccumulateItems)) ) 
	{
		/* If the product for which we received the ProductComplete message is not the current product,
		 * and it doesn't have any other outstanding transactions (for which we are waiting for a
		 * ProductComplete message), then delete that product.  We no longer have any use for it.
		 */

		//Debug::trace("ProductManager.HandleProductComplete", "Here 2");

		if (isTracing)
			Debug::trace("ProductManager.HandleProductComplete", "Complete message is not for the current product");

		if (!prod->hasOutstandingTransactions())
		{
			if (isTracing)
				Debug::trace("ProductManager.HandleProductComplete", "That product has no outstanding transactions, it is being deleted");
			
			if( prod && (s->getCurrentMode() == OpModeAccumulateItems) && printingProducts.size() > 0 )
				removeAPrintingProduct(prod);				// JHC - This was added to remove that last pesky item

			// Dont do any of this here, no pointer deletes, it will lock up the scale for dereferencing a dereferenced pointer.  
			// Actual code was deleted, but this comment had to be made and stressed. 
			// prod = 0;
			// foundProduct = 0;
			
			Debug::trace("ProductManager.HandleProductComplete", "Bye bye HandleProductComplete() when OpModeAccumulateItems");

			return;
		}
	}
	else
	{
		//Debug::trace("ProductManager.HandleProductComplete", "Here 4");

		if (( printedShelfLifeExpirationLabel ) || ( printedRunTotalsLabel ))
		{
			// Sending the ProductChange message will cause the PrinterManager to re-render the label, and when it does,
			// the label rendered will be the product's regular label, not the shelf life expiration label that just printed.
			Debug::trace("ProductManager", "HandleProductComplete - sending ProductChange to RTC after handling ProductComplete for an expiration or run totals label");
			ProductChangeMsg donePrintingExpProductChangeMsg(mySession, this);
			mySession->send(&donePrintingExpProductChangeMsg, Messenger::DEST_RTC, false);
		}
	}

	if (prod->isPrintingInProductionEntryMode() && productCompleteMsg->getEventType() != ProductIncomplete && s->getCurrentMode() == OpModeProductionEntry)
	{
		// Don't come in here if current mode is OpModePrintMultipleLabels; go to the else
		// Debug::trace("ProductManager.handleProductComplete", HobString("IF handleProductComplete() mode: ") + HobString(Integer::toString(s->getCurrentMode())));
		
		Debug::trace("ProductManager.HandleProductComplete", "Here 5");
		clearCurrentProduct(false);
	}
	else
	{
		//Debug::trace("ProductManager.HandleProductComplete", "Here 6");
		prod->setPrintingInProductionEntryMode(false);

		//Debug::trace("ProductManager.HandleProductComplete", "Here 8");
		if ((config->isServiceCounterScale() &&
			config->isClearPluAfterPrinting() &&
			(s->getCurrentMode() == OpModeManualWeigh || (s->getCurrentMode() == OpModePrintMultipleLabels && productCompleteMsg->getEventType() != ProductIncomplete)) &&
			!printedShelfLifeExpirationLabel)
			|| config->isSelfServiceScale())
		{
			clearCurrentProduct(true);
		}
		else
		{
			//Debug::trace("ProductManager.HandleProductComplete", HobString("ELSE handleProductComplete() mode: ") + HobString(Integer::toString(s->getCurrentMode())));

			// If we're in Print Multiple Labels mode, leave that mode.
			if (s->getCurrentMode() == OpModePrintMultipleLabels && productCompleteMsg->getEventType() != ProductIncomplete)
			{
				if (currentProduct)
				{
					currentProduct->setNumOfLabelsForced(false);

					if (currentProduct->getCurrentTransaction())
					{
						currentProduct->getCurrentTransaction()->setNumberPackages(1);
						currentProduct->getCurrentTransaction()->setNumOfLabelsToPrint(1);
						currentProduct->getCurrentTransaction()->setNumOfLabelsPrinted(0);
					}
					else
					{
						currentProduct->createProductTransaction();
						currentProduct->initProductTransaction();
					}
				}

				Debug::trace("ProductManager.HandleProductComplete", string("handleProductComplete() before handleLeavePrintMultipleLabelsMode() opModePriorToPrintMultipleLabelsMode: ") + Integer::toString(opModePriorToPrintMultipleLabelsMode));

				handleLeavePrintMultipleLabelsMode();
			}

			//FT-1001			if (currentProduct)
			//				resendProductChange();
		}
	}

	Debug::trace("ProductManager.HandleProductComplete", "Bye bye HandleProductComplete() I made it to the end");

	s->sendDialogToMonitoringSessions(false, ErrProductDev, ProductErrPrintingLabel, HfcResponse::RESPONSESTYLE_OK);
}

void ProductManager::handleRemoteProductComplete(HfcProductCompleteMsg *productCompleteMsg)
{
	bool isTracing = Debug::isTracing("ProductManager.remotePrint");

	if (!productCompleteMsg)
		return;

	if (isTracing)
		Debug::trace("ProductManager.remotePrint", string("START handleRemoteProductComplete() - ") + ((productCompleteMsg->getEventType() == ProductComplete) ? "ProductComplete" : "ProductIncomplete"));

	ProductTransaction *trans = productCompleteMsg->getProductTransaction();
	if (!trans)
		return;

	Product *prod = trans->getProduct();
	if (!prod)
		return;

	Session *s = dynamic_cast<Session *> (mySession);
	if (s && s->isInOperateMode() && mySession->isPrimarySession())
	{
		if (productCompleteMsg->isPrintOk() || (trans->getNumOfLabelsToPrint() > 0 && trans->getNumOfLabelsPrinted() > 0))
			trans->updateTransactionTotalValues();   // Update Transaction.

		if (productCompleteMsg->getEventType() != ProductIncomplete)
			prod->completeTransaction(trans);


		if (isTracing)
			Debug::trace("ProductManager.remotePrint", string("handleRemoteProductComplete() - labels printed... ") + inttostring(trans->getNumOfLabelsPrinted()) + " of " + inttostring(trans->getNumOfLabelsToPrint()));

		int labelsPrinted = trans->getNumOfLabelsPrinted();
		int status = 0;
		if (productCompleteMsg->getEventType() == ProductIncomplete)
		{
			// This will only be processed if we got a printer status error

			// TODO remotePrint - TEST TO REMOVE. Or move to a different spot if needed.
			SessionMessage msg(ProductDev, ProductStopPrintAllLabels);
			msg.setFormatType(BrowserFormat);
			mySession->send(&msg, Messenger::DEST_UI);

			status = s->checkForRemotePrinterStatus();
			if (status <= 0)
				status = ProductErrPrintUnknownPrinterError;
		}
		else
		{
			if ( productCompleteMsg->isPrintOk() )
				status = RemotePrintSuccessfulPrint;
			else
			{
				status = s->checkForRemotePrinterStatus();
				if (status <= 0)
				{
					if ( trans->getNumOfLabelsToPrint() != trans->getNumOfLabelsPrinted() )
						status = ProductErrPrintPrintJobCancelViaUI;
					else
						status = ProductErrPrintUnknownPrinterError;
				}
			}

			clearCurrentProduct(true);
		}

		Debug::trace("ProductManager.remotePrint", string("handleRemoteProductComplete() - before calling handleReturnRemotePrintMessage() - status: ") + inttostring(status) + "  labelsPrinted: " + inttostring(labelsPrinted));
		s->remotePrintReturnMessage(status, labelsPrinted);
	}

	if (isTracing)
		Debug::trace("ProductManager.remotePrint", "FINISH handleRemoteProductComplete()");
}


bool ProductManager::findPrintingProduct(const Product *prodToFind, Util::CSmartPtr<Product> &printingProduct)
{
	bool result = false;

	if (!printingProducts.empty())
	{
		list< Util::CSmartPtr<Product> >::iterator pos = printingProducts.begin();
		while (pos != printingProducts.end())
		{
			if ((*pos).get() == prodToFind)
			{
				printingProduct = *pos;
				result = true;
				break;
			}
			pos++;
		}
	}

	return result;
}

bool ProductManager::removeAPrintingProduct(const Product *prodToFind)
{
	bool result = false;
	Util::CSmartPtr<Product> prod;   
        
	if (!printingProducts.empty())
	{
		list< Util::CSmartPtr<Product> >::iterator pos = printingProducts.begin();

		while (pos != printingProducts.end())
		{
			if ( (*pos).get() == prodToFind )
			{
				printingProducts.erase(pos);
				//*pos = 0;

				result = true;
				break;
			}

			pos++;
		}

		//*pos = 0;		// Tried moving this here from up above, still fails
	}

	return result;
}

bool ProductManager::clearPrintingProductsList()
{
	Debug::trace("ProductManager.clearPrintingProductsList", "START method clearPrintingProductsList() ");
	
	bool result = false;   
        
	if (!printingProducts.empty())
	{
		list< Util::CSmartPtr<Product> >::iterator pos = printingProducts.begin();

		while ( !printingProducts.empty() )
		{
			printingProducts.pop_back();
		}

		result = true;
	}

	return result;
}

void ProductManager::setHasProductionEntryItems(bool value)
{
	productionEntryItemsAdded = value;	
	if (value == false)
		productionEntryContainsFPPItems = false;
}

bool ProductManager::hasProductionEntryItems() const
{
	return productionEntryItemsAdded;
}

void ProductManager::setHasAccumulateItems(bool value)
{
	accumulateItemsAdded = value;
}

bool ProductManager::hasAccumulateItems() const
{
	return accumulateItemsAdded;
}

bool ProductManager::isPrintingRemoteLabel() const
{
	return ((currentProduct && currentProduct->isPrintingRemoteLabel()) ? true : false);
}

bool ProductManager::hasPrintingProduct() const
{
	Debug::trace("ProductManager", string("hasPrintingProduct: list len = ") + inttostring((int)printingProducts.size()));
	return (!printingProducts.empty());
}

void ProductManager::handleProductTree()
{
	if (!dbAccess)
		return;

	ProductTree *tree = new ProductTree(dbAccess);
	if (tree)
	{
		if (mySession)
		{
			tree->buildProductTree();		//	Build the Product tree list.

			// FIRST, send the Class tree to the browser.
			SessionMessage classTreeMsg(ClassDev, ClassGetTree);
			classTreeMsg.setFormatType(BrowserFormat);
			classTreeMsg.setHfcObject(tree->getClassList());

			// Send the error message to the UI (Browser)
			mySession->send(&classTreeMsg, Messenger::DEST_UI);

			// Send the Product tree to the browser.
			SessionMessage productTreeMsg(ProductDev, ProductGetTree);
			productTreeMsg.setFormatType(BrowserFormat);
			productTreeMsg.setHfcObject(tree->getProductList());

			// Send the error message to the UI (Browser)
			mySession->send(&productTreeMsg, Messenger::DEST_UI);
		}
		delete tree;
	}
}

void ProductManager::handleProductGetByDescription(SessionMessage *msg)
{
	HfcProductDescLookup *pluLookup = dynamic_cast<HfcProductDescLookup *> (msg->getHfcObject());
	if (!pluLookup || !dbAccess)
		return;

	DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
	if (!dbProductsRM)
		return;

	if (mySession)
	{
		Session* session = dynamic_cast<Session*>(mySession);
		if ( !session )
			session = SessionManager::getInstance()->getPrimaryConsoleSession();

		pluLookup->lookupProductsByDesc(dbProductsRM, session ? session->getCurrentLanguageSelection() : PrimaryLanguage);

		// Send the list of Products and Product descriptions to the browser.
		SessionMessage productsMsg(ProductDev, ProductFindByDescription);
		productsMsg.setFormatType(BrowserFormat);
		productsMsg.setHfcObject(pluLookup);

		// Send the error message to the UI (Browser)
		mySession->send(&productsMsg, Messenger::DEST_UI);
	}
}


void ProductManager::handleProductReadLifeDataMessage()
{
	if (currentProduct)
	{
		Debug::trace("ProductManager", "This message doesn't do anything ... Sending Shelf Life and Product Life data to UI");
	}
}



/**************************************************************************************************
 **************************************************************************************************
 ***																							***
 ***						 Product Support Data											   ***
 ***																							***
 **************************************************************************************************
 **************************************************************************************************/

void ProductManager::handleProductShowTextLinks(SessionMessage *msg)
{
	if (!msg)
		return;

	HfcProductSupportDataMsg *supportMsg = dynamic_cast<HfcProductSupportDataMsg *> (msg->getHfcObject());
	if (!supportMsg || !mySession || !dbAccess)
		return;

	TextLinks *textLinks = supportMsg->getTextLinks(dbAccess, getCurrentProduct());
	if (textLinks)
	{
		SessionMessage linkData(ProductDev, ProductShowLinksToSupportData);
		linkData.setFormatType(BrowserFormat);
		linkData.setHfcObject(textLinks);

		// Send the linkData to the UI (Browser)
		mySession->send(&linkData, Messenger::DEST_UI);
		delete textLinks;
		textLinks = (TextLinks *)NULL;
	}
}


void ProductManager::handleProductReadSupportRecord(SessionMessage *msg)
{
	if (!msg)
		return;

	HfcProductSupportDataMsg *supportMsg = dynamic_cast<HfcProductSupportDataMsg *> (msg->getHfcObject());
	if (!supportMsg)
		return;

	if (currentProduct)
	{
		EvolDeviceType responseDev = UnknownDevice;
		HfcObject *responseData = (HfcObject *)NULL;

		ProdSupportTextType textType = supportMsg->getProdSupportTextType();
		switch (textType)
		{
			case psttNutrifacts:
				responseDev = NutrifactsDev;
				responseData = currentProduct->getNutrifacts();
				break;

			case psttExpandedText:
				responseDev = ExpandedTextDev;
				responseData = currentProduct->getProductText(psttExpandedText);
				break;

			case psttSpecialMessage:
				responseDev = SpecialMessageDev;
				responseData = currentProduct->getProductText(psttSpecialMessage);
				break;

			case psttProductNote:
				responseDev = ProductNotesDev;
				responseData = currentProduct->getProductText(psttProductNote);
				break;

			case psttMarqueeText:
				responseDev = MarqueeTextDev;
				responseData = currentProduct->getProductText(psttMarqueeText);
				break;

			case psttProductText5:
				responseDev = ProductText5Dev;
				responseData = currentProduct->getProductText(psttProductText5);
				break;

			case psttProductText6:
				responseDev = ProductText6Dev;
				responseData = currentProduct->getProductText(psttProductText6);
				break;

			case psttProductText7:
				responseDev = ProductText7Dev;
				responseData = currentProduct->getProductText(psttProductText7);
				break;

			case psttProductText8:
				responseDev = ProductText8Dev;
				responseData = currentProduct->getProductText(psttProductText8);
				break;

			case psttProductText9:
				responseDev = ProductText9Dev;
				responseData = currentProduct->getProductText(psttProductText9);
				break;

			default:
				break;
		}

		if (mySession && responseData)
		{
			Debug::trace("ProductManager", HobString("Sending ProductReadSupportRecord type:") + HobString(Integer::toString((int)responseDev)) + HobString(" to UI"));

			SessionMessage writeSupportMsg(responseDev, DtWrite);
			writeSupportMsg.setFormatType(BrowserFormat);
			writeSupportMsg.setHfcObject(responseData);

			// Send the product support message to the UI (Browser)
			mySession->send(&writeSupportMsg, Messenger::DEST_UI);
		}
	}
}


void ProductManager::handleProductReadSupportData(SessionMessage *msg)
{
	if (!msg)
		return;

	HfcProductSupportDataMsg *supportMsg = dynamic_cast<HfcProductSupportDataMsg *> (msg->getHfcObject());
	if (!supportMsg)
		return;

	if (currentProduct)
	{
		HobString textIs;
		ProdSupportTextType textType = supportMsg->getProdSupportTextType();
		supportMsg->setText("");
		HobString currentText("");

		switch (textType)
		{
			case psttNutrifacts:
				if (currentProduct->getNutrifacts())
				{
					currentText = currentProduct->getNutrifacts()->getColumn1Data();
					supportMsg->setText(currentProduct->getNutrifacts()->getColumn1Data());
					currentText = currentProduct->getNutrifacts()->getColumn2Data();
					supportMsg->setTextBlock2(currentProduct->getNutrifacts()->getColumn2Data());
					currentText = currentProduct->getNutrifacts()->getColumn3Data();
					supportMsg->setTextBlock3(currentProduct->getNutrifacts()->getColumn3Data());
					currentText = currentProduct->getNutrifacts()->getColumn4Data();
					supportMsg->setTextBlock4(currentProduct->getNutrifacts()->getColumn4Data());

					//	Check to see if more than 1 Product uses this Nutrifacts record
					supportMsg->setUsedByMany(currentProduct->getNutrifacts()->isUsedByMany());
				}
				textIs = "Nutrifacts";
				break;

			case psttExpandedText:
			case psttSpecialMessage:
			case psttProductNote:
			case psttMarqueeText:
			case psttProductText5:
			case psttProductText6:
			case psttProductText7:
			case psttProductText8:
			case psttProductText9:
				if (currentProduct->getProductText(textType))
				{
					supportMsg->setText(currentProduct->getProductText(textType)->getText());

					//	Check to see if more than one Product uses this Product text
					supportMsg->setUsedByMany(currentProduct->getProductText(textType)->isUsedByMany());
				}
				textIs = "ProductText type:" + inttostring(textType);
				break;

			default:
				textIs = "UNKNOWN";
				break;
		}
		if (Debug::isTracing("ProductManager"))
		{
			HobString myDebug("Sending ");
			myDebug.append(textIs).append(" to UI");
			Debug::trace("ProductManager", myDebug);
		}

		if (mySession)
		{
			SessionMessage writeSupportMsg(ProductDev, ProductWriteSupportData);
			writeSupportMsg.setFormatType(BrowserFormat);
			writeSupportMsg.setHfcObject((HfcObject *)supportMsg);

			// Send the product support message to the UI (Browser)
			mySession->send(&writeSupportMsg, Messenger::DEST_UI);
		}
	}
}


void ProductManager::handleProductWriteSupportData(SessionMessage *msg)
{
	if (!msg)
		return;

	HfcProductSupportDataMsg *supportMsg = dynamic_cast<HfcProductSupportDataMsg *> (msg->getHfcObject());
	if (!supportMsg)
		return;

	if (currentProduct)
	{
		HobString textIs("");
		HobString currentText("");
		ProdSupportTextType textType = supportMsg->getProdSupportTextType();

		switch (textType)
		{
			case psttNutrifacts:
				currentText = supportMsg->getText();
				currentProduct->setNutrifactsTextCol1(supportMsg->getText());

				currentText = supportMsg->getTextBlock2();
				currentProduct->setNutrifactsTextCol2(supportMsg->getTextBlock2());

				currentText = supportMsg->getTextBlock3();
				currentProduct->setNutrifactsTextCol3(supportMsg->getTextBlock3());

				currentText = supportMsg->getTextBlock4();
				currentProduct->setNutrifactsTextCol4(supportMsg->getTextBlock4());

				textIs = "Nutrifacts";
				break;

			case psttExpandedText:
			case psttSpecialMessage:
			case psttProductNote:
			case psttMarqueeText:
			case psttProductText5:
			case psttProductText6:
			case psttProductText7:
			case psttProductText8:
			case psttProductText9:
				currentText = supportMsg->getText();
				currentProduct->setProductTextText(textType, currentText);
				textIs = "ProductText type:" + inttostring(textType);
				break;

			default:
				textIs = "UNKNOWN";
				break;
		}
		HobString showText(currentText.c_str(), currentText.length());

		if(!textIs.empty())
			Debug::trace("ProductManager", HobString("The ") + textIs + HobString(" string ***** ") + showText + HobString(" ***** string has been written to the product. "));
	}
}


void ProductManager::handleProductChangeSupportData(SessionMessage *msg)
{
	if (!msg)
		return;

	bool returnMessage = false;
	bool foundText = true;

	HfcProductSupportDataMsg *supportMsg = dynamic_cast<HfcProductSupportDataMsg *> (msg->getHfcObject());
	if (!supportMsg || !currentProduct || !dbAccess)
		return;

	supportMsg->setMgrContainer(mgrContainer_);

	ProdSupportFunction editFunction = supportMsg->getProdSupportFunction();
	switch(editFunction)
	{
		case psfNew:
		{
			Debug::trace("ProductManager", HobString("handleProductChangeSupportData():   editFunction = psfNew"));
			supportMsg->newText(dbAccess, currentProduct);
			handleSaveRecord(currentProduct);						//	Save the change.
			// The Nutrition Editor Applet needs to receive the new nutrition record, even
			// if it is empty (since it was created new)
			if ( supportMsg->getProdSupportTextType() == psttNutrifacts )
				returnMessage = true;
			break;
		}

		case psfCopyAndEdit:
		{
			Debug::trace("ProductManager", HobString("handleProductChangeSupportData():   editFunction = psfCopyAndEdit"));
			foundText = supportMsg->copyText(dbAccess, currentProduct);
			if (foundText)
			{
				handleSaveRecord(currentProduct);					//	Save the change.
				returnMessage = true;
			}
			break;
		}

		case psfDelete:
			Debug::trace("ProductManager", HobString("handleProductChangeSupportData():   editFunction = psfDelete"));
			supportMsg->deleteText(dbAccess, currentProduct);
			handleSaveRecord(currentProduct);						//	Save the change.
			break;

		case psfDeleteForAll:
			Debug::trace("ProductManager", HobString("handleProductChangeSupportData():   editFunction = psfDeleteForAll"));
			supportMsg->deleteText(dbAccess, currentProduct);
			handleSaveRecord(currentProduct);						//	Save the change.
			break;

		case psfUse:
			Debug::trace("ProductManager", HobString("handleProductChangeSupportData():   editFunction = psfUse"));
			foundText = supportMsg->useText(dbAccess, currentProduct);
			if(foundText)
			{
				handleSaveRecord(currentProduct);					//	Save the change.
				returnMessage = true;
			}
			break;

		default:
			Debug::trace("ProductManager", HobString("handleProductChangeSupportData():   editFunction = ") + HobString(Integer::toString(editFunction)));
			break;
	}
	if (!foundText && mySession)
	{
		// Send error message to browser indicating a problem with the Label Type.
		int errorNumber;
		switch (supportMsg->getProdSupportTextType())
		{
			case psttExpandedText:
				errorNumber = ProductErrExpandedTextNotFound;
				break;
			case psttSpecialMessage:
				errorNumber = ProductErrSpecialMessageNotFound;
				break;
			case psttProductNote:
				errorNumber = ProductErrProductNoteNotFound;
				break;
			case psttNutrifacts:
				errorNumber = ProductErrNutrifactsNotFound;
				break;
			case psttMarqueeText:
				errorNumber = ProductErrMarqueeTextNotFound;
				break;
			default:
				errorNumber = ProductErrUnknownError;
				break;
		}
		mySession->addDialogMessage(ErrProductDev, errorNumber, HfcResponse::RESPONSESTYLE_OK);
	}
	else if (returnMessage && mySession)
	{
		SessionMessage writeSupportMsg(ProductDev, ProductWriteSupportData);
		writeSupportMsg.setFormatType(BrowserFormat);
		writeSupportMsg.setHfcObject((HfcObject *)supportMsg);

		// Send the SupportMsg to the UI (Browser)
		mySession->send(&writeSupportMsg, Messenger::DEST_UI);
	}
}


void ProductManager::handleProductReadNutriLabelMessage()
{
	if (currentProduct && mySession)
	{
		bool createdTempLabel = false;
		LabelType *label = currentProduct->getLabelType(FirstPrinter);
		Debug::trace("ProductManager.nutri", HobString("handleProductReadNutriLabelMessage() - checking if first label is nutri label"));
		if (!label || !label->labelHasNutrifacts())
		{
			Debug::trace("ProductManager.nutri", HobString("handleProductReadNutriLabelMessage() - first label is not nutri label, checking second"));
			label = currentProduct->getLabelType(SecondPrinter);
			if (!label || !label->labelHasNutrifacts())
			{
				Debug::trace("ProductManager.nutri", HobString("handleProductReadNutriLabelMessage() - second label is not nutri label, checking third"));
				label = currentProduct->getLabelType(ThirdPrinter);
				if (!label || !label->labelHasNutrifacts())
				{
					Debug::trace("ProductManager.nutri", HobString("handleProductReadNutriLabelMessage() - no label is nutri label"));
					
					// For Self-serve, we have a default label type to display so that the supervisor can enter nutritional data
					// without setting one of the label types to be a nutri-label.  In Self-serve, we can display the nutri-data,
					// but they really don't want to print a label for it, nor do they want to assign one of the label types to be
					// the nutri-label for every plu, just so they can enter the nutri-data for any product.
					if (Configuration::getInstance()->isSelfServiceScale())
					{
						// multidepttodo - the following file is not on the HT Series SD card, so this code will never execute.  I believe we want to use the nutrifact label used for viewing on the UI, which is supposed to be in the rlabels db table (#10080).
						// multidepttodo - So if we are currently displaying a nutri-data label in self serve mode without the plu having one of its labels being a nutri-data label, then the UI must be defaulting to us label #10080???
						CMFile *fp = new CMFile(Configuration::getInstance()->toStringCfgI(CfgInternal::DEFAULTNUTRILABELTEMPLATE).c_str(), CommMechanism::MODE_INPUT);
						if (fp)
						{
							fp->setMyReaderWriter(new CMReaderWriterEvolTag());
							HobString data = "";
							int len = fp->cmReceive(data);
							if (len > 0)
							{
								label = new LabelType(data.c_str(), len, EvolutionTaggedFormat);
								if (label)
								{
									label->setupLabel(label->getOrigBinaryData());
									if (!label->labelHasNutrifacts())
									{
										delete label;
										label = 0;
									}
									else
									{
										createdTempLabel = true;
									}
								}
							}

							delete fp;
						}
					}
				}
			}
		}

		// The label either is a nutritional label, or it contains one as an embedded label.
		// So get the nutritional label type pointer from the label type.
		LabelType *labelToUse = 0;

		if (!label)
		{
			Debug::trace("ProductManager.nutri", HobString("handleProductReadNutriLabelMessage() - no label is nutri label, creating blank label to send to browser"));
			label = new LabelType((HobRecordNum)0);
			createdTempLabel = true;
			labelToUse = label;
		}
		else
		{
			Debug::trace("ProductManager.nutri", "handleProductReadNutriLabelMessage() - nutri label found : " + ptrtostring(label));
			labelToUse = const_cast <LabelType *> (label->getEmbeddedNutritionLabelType());
			Debug::trace("ProductManager.nutri", "handleProductReadNutriLabelMessage() - nutri label pointer is : " + ptrtostring(labelToUse));
		}

		// Send the LabelType to the UI (Browser)
		SessionMessage ltMsg(LabelTypeDev, DtWrite);
		ltMsg.setFormatType(BrowserFormat);
		labelToUse->setFormatType(ViewLabelFormat);
		ltMsg.setHfcObject(labelToUse);
		mySession->send(&ltMsg, Messenger::DEST_UI);

		if (createdTempLabel)
			delete label;
	}
}

bool ProductManager::hasNutrifactsLabelDataMissing()
{
    bool hasNutriMissing = false;

    if (currentProduct && mySession)
    {

        // find out if we have nutri_data first. If we have it then we are wasting our time.
        if ( currentProduct->getNutrifacts() )
        {
            Debug::trace("ProductManager.nutri", HobString("hasNutrifactsLabelDataMissing() - Product has nutri so no issues"));
            return false;
        }

        // could nest these ifs in one another
        Debug::trace("ProductManager.nutri", HobString("hasNutrifactsLabelDataMissing() - checking if first label is nutri label"));
        LabelType *label = currentProduct->getLabelType( FirstPrinter );
        if ( label && label->labelHasNutrifacts() )
        {
            Debug::trace("ProductManager.nutri", HobString("hasNutrifactsLabelDataMissing() - first label is nutri label"));
            hasNutriMissing = true;
        }

        label = currentProduct->getLabelType( SecondPrinter );
        if ( label && label->labelHasNutrifacts() )
        {
            Debug::trace("ProductManager.nutri", HobString("hasNutrifactsLabelDataMissing() - second label is nutri label"));
            hasNutriMissing = true;
        }

        label = currentProduct->getLabelType( ThirdPrinter );
        if ( label && label->labelHasNutrifacts() )
        {
            Debug::trace("ProductManager.nutri", HobString("hasNutrifactsLabelDataMissing() - third label is nutri label"));
            hasNutriMissing = true;          
        }

        if ( hasNutriMissing )
        {
            if ( mySession )
                mySession->addDialogMessage(ErrProductDev, ProductErrDoNotPrintEmptyNutrifactsLabel, HfcResponse::RESPONSESTYLE_OK);
        }
    }

    return hasNutriMissing;
}


void ProductManager::checkForReportingClassError()
{
	if (!currentProduct || !mySession)
		return;

	Class *checkClass = currentProduct->getReportingClass();
	if (checkClass && (checkClass->getErrorCode() != RECORD_OK || checkClass->getClassLinkStatus() != CLASS_LINK_OK))
		// Send error message to browser indicating a problem with the class.
		mySession->addDialogMessage(ErrProductDev, ProductErrReportingClass, HfcResponse::RESPONSESTYLE_OK);
}

// Precondition: session != NULL
// Precondition: mySession->isInOperateMode == true
bool ProductManager::hasLabelTypeChanged(short printerNum)
{
	LabelMediaReturn oldValue = LabelInvalid;
	LabelMediaReturn newValue = LabelInvalid;

	if (currentProduct /*&& !currentProduct->isPrintingInProductionEntryMode()*/)
	{
		LabelType *pLabelType = currentProduct->getLabelType(printerNum);
		if (pLabelType != NULL)
		{
			oldValue = pLabelType->getLabelMediaError();

			LabelType *pCorrectedLabelType = 0;
			newValue = labelTypeErrorTest(pLabelType, &pCorrectedLabelType, printerNum, currentProduct->getDeptNum());
			if (newValue == LabelCorrected)
			{
				//	Set the corrected label type in the product record.
				Debug::trace("ProductManager", HobString("hasLabelTypeChanged() - Label type - Found a label in the family to fit stock: PrinterNum = ") + HobString(Integer::toString(printerNum)) + HobString("	OrigNum = ") + HobString(Long::toString(pLabelType->getLabelTypeNum())) + HobString("	NewNum = ") + HobString(pCorrectedLabelType ? Long::toString(pCorrectedLabelType->getLabelTypeNum()) : "?!?"));
				/* For 'label types by mode' project,
				 * Commenting out the next line because the label type doesn't need to be set here.  This method is only called when a PrinterStockMetrics message is received by the Session,
				 * see SessionPrinter - handlePrinterMessage - case PrStockMetrics, to determine if a different length label has been loaded in the printer.  If so, and a new label type that
				 * fits that label has been found, or now no label that fits was found, a call will be made to setUpLabelTypes(), which is directly below, which will set up the Label Type object
				 * in the Product.  And it is there that the correct label type num attribute will also be updated.  For now, we just need to know that the previous label doesn't fit on the new
				 * label stock.
				currentProduct->setLabelType(printerNum, pCorrectedLabelType);
				*/
			}
		}
	}

	return (oldValue != newValue);
}


// returns true if an error is found for any printer
bool ProductManager::setUpLabelTypes()
{
	bool rv = false;
	bool allLabelTypesAreNull = true;

	if (currentProduct != NULL)
	{
		Session *s = dynamic_cast<Session *> (mySession);
		for (short printerNum = FirstPrinter; printerNum < MaxPrinterTypes; printerNum++)
		{
			LabelType *labelType = currentProduct->getLabelType(printerNum);
			Debug::trace("ProductManager", "checkForLabelTypeError() labelType: " + ptrtostring(labelType));

			if (labelType)
			{
				allLabelTypesAreNull = false;

				// side effect of this routine
				// labelType may be invalid after function call!
				// Ergo, get the label Pointer again!
				LabelType *pCorrectedLabelType = 0;
				LabelMediaReturn result = labelTypeErrorTest(labelType, &pCorrectedLabelType, printerNum, currentProduct->getDeptNum());
				if (result == LabelCorrected)
				{
					if (!pCorrectedLabelType)
						Debug::trace("ProductManager", "setUpLabelTypes() - result from labelTypeErrorTest() is LabelCorrected but the corrected labelType is null!!");
					else
					{
						Debug::trace("ProductManager", HobString("setUpLabelTypes() - Label type - Found a label in the family to fit stock: PrinterNum = ") + HobString(Integer::toString(printerNum)) + HobString("	OrigNum = ") + HobString(Long::toString(labelType->getLabelTypeNum())) + HobString("	NewNum = ") + HobString(Long::toString(pCorrectedLabelType->getLabelTypeNum())));
						currentProduct->setLabelType(printerNum, pCorrectedLabelType);
						labelType = currentProduct->getLabelType(printerNum);

						if (labelType)
						{
							bool isInOperateMode = (s && s->isInOperateMode()) ? true : false;
							if (isInOperateMode)
							{
								// Update the label type num for the active set of label types being used, based on the mode.
								// The label type num of the label type being set in this method could be different if a better match was found in the family (Operate mode).
								currentProduct->updateLabelTypeNumForMode(printerNum, labelType->getLabelTypeNum());
							}
						}
					}
				}

				if (labelType->isLabelMediaError() == true)
				{
					Debug::trace("ProductManager", HobString("checkForLabelTypeError() labelType->isLabelMediaError() == true"));
					rv = true;
					//printerNum = MaxPrinterTypes;
				}
			}
			else
			{
				// IF the label is NULL, should we check if status and printhead is up?  What about other issues?  Any status?  Some?
				//Debug::trace("ProductManager", "IN setUpLabelTypes() - where labelType is NULL for printer[" + HobString(Integer::toString(printerNum)) + "]");
				// Label is NULL here, but if the printhead is up, we won't print - so see if we can see if we have a disengaged status
				Session* s = dynamic_cast<Session*>(mySession);

				if (!s)
					break;

				HfcPrinterStatus* status = s->getCurrentPrinterStatus(printerNum);

				bool isHeadDisengaged = false;

				if (status)
				{
					//if ( status->isHeadDisengaged() )
					//	Debug::trace("ProductManager.Joe", "IN setUpLabelTypes() - where labelType is NULL for printer[" + HobString(Integer::toString(printerNum)) + "] AND Printhead is disengaged");
                    //
					//if ( isHeadDisengaged |= status->isHeadDisengaged() )
					//	Debug::trace("ProductManager.Joe", "IN setUpLabelTypes() - where labelType is NULL for printer[" + HobString(Integer::toString(printerNum)) + "] AND Printhead is bitshifted disengaged");
				
					status->notificationReasonExists();
				}
				else
				{
					// no currentStatus - maybe request it instead
					Debug::trace("ProductManager.Joe", "IN setUpLabelTypes() - where labelType is NULL && !status for printer[" + HobString(Integer::toString(printerNum)) + "]. Request the status???");

					// Just in case - lets see if there is something wrong with this printer.  Grab the status
					// Might have to check or set "IgnorePrinter" so it can run anyway, or perform some additional checks
					if (printerNum > FirstPrinter)
					{
						PrinterRequestStatus prs(mySession, (PrinterType)printerNum);
						mySession->send(&prs, Messenger::DEST_RTC);
					}
				}
			}
		}

		if ( allLabelTypesAreNull )
		{
			//	There needs to be a valid label for at least one printer - not all can be null.
			rv = true;

			if (mySession)
			{
				if ( (s->isInOperateMode() && !Configuration::getInstance()->isSelfServiceScale() ) || !( ( Configuration::getInstance()->isSelfServiceScale() ) &&  (SelfServOption_PrintToScreen == Configuration::getInstance()->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors) ) ) )
					mySession->addDialogMessage(ErrProductDev, ProductErrLabelType, HfcResponse::RESPONSESTYLE_OK);
			}
			if (Debug::isTracing("ProductManager"))
				Debug::trace("ProductManager", HobString("Label type: All label types are null!"));
		}
	}

	return rv;
}

// After setting correctedLabelType and returning LabelCorrected, if the caller wants the corrected label type to be used
// for the product (which used to be set in this method), they must call currentProduct->setLabelType() for the affected
// printer and the corrected label type.  I (Larry) changed this method to not automatically set the corrected label into the
// current product when coding the shelf life expiration label printing for the HLX; I wanted the correct-sized label, but didn't
// want it set into the product as the normal label type.
LabelMediaReturn ProductManager::labelTypeErrorTest(LabelType *labelType, LabelType **correctedLabelType, short printerNum, HobRecordNum deptNumOfProduct, bool forceErrorTestInAnyMode)
{
	LabelMediaReturn result = LabelInvalid;
	Debug::trace("ProductManager", HobString("labelTypeErrorTest() entered"));

	Session *s = dynamic_cast<Session *> (mySession);

	// Put error check here for isSelfServ, isPrintLabelToScreen, and IgnoreLabelErrors
	if ( Configuration::getInstance()->isSelfServiceScale() && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && ( SelfServOption_PrintToScreen == Configuration::getInstance()->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors) ) )
	{
		Debug::trace("ProductManager.ToggleLabel", "Self Serv ignore all errors - labelType->setLabelMediaError(" + inttostring(result) + ")");
		labelType->setLabelMediaError(LabelOk);
		return LabelOk;
	}
	// v v v  PJR-444 - SOF-5063 - Print wider labels on Zebra printer  v v v
	else
	{
		Device* printerDevice = Device::findDevice(Printer, printerNum, s);
		if ( printerDevice && printerDevice->isANonHobartLabelPrinter() )
		{
			// For a Zebra printer, don't have the label dimensions, so just assume the label type will fit.
			Debug::trace("Session", "checking labelTypeErrorTest() for a non-Hobart label printer (Zebra printer),set that the label type is OK");
			labelType->setLabelMediaError(LabelOk);
			return LabelOk;
		}
	}
	// ^ ^ ^  PJR-444 - SOF-5063 - Print wider labels on Zebra printer  ^ ^ ^

	if (labelType == NULL)
	{
		if ( printerNum == FirstPrinter && Configuration::getInstance()->isSelfServiceScale() )
		{
			//	I'm not sure what error message we need to send here.
			//	There needs to be a valid label for the Primary printer.
			if (mySession &&
				( (s->isInOperateMode() ) &&
				( Configuration::getInstance()->isSelfServiceScale() && !Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && ( SelfServOption_PrintToScreen != Configuration::getInstance()->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors) ) ) )
				)
				mySession->addDialogMessage(ErrProductDev, ProductErrLabelType, HfcResponse::RESPONSESTYLE_OK);

			Debug::trace("ProductManager.ToggleLabel", HobString("Label type: No Primary label type selected!"));
		}
		else
			result = LabelOk;
	}
	else if (s)
	{
		bool isInOperateMode = (s->isInOperateMode()) ? true : false;

		//	Only compare the label type against the label stock if we are in Operate mode
		HfcPrinterStockMetrics *printerStockMetrics = s->getPrinterStockMetrics(printerNum);

		if (printerStockMetrics)
		{
			labelType->setLabelMediaError(LabelInvalid);

			if (isInOperateMode || forceErrorTestInAnyMode)
			{
				// if printhead is disengaged(pri), the IF is skipped
				if (printerStockMetrics->getLabelLengthInLines() > 0)
				{
					HobRecordNum labelTypeNum = labelType->getLabelTypeNum();
					HobRecordNum deptNum = labelType->getDeptNum();
					HobRecordNum prodDeptNum = deptNumOfProduct;

					result = LabelTypeManager::validateLabelTypeWithStock(labelType->getLabelStockDescriptor(), printerStockMetrics, LabelTypeManager::BEST_LABELFIT, &labelTypeNum, &deptNum, prodDeptNum, dbAccess);

					if (result == LabelWontFit)
					{
						// Send error message to browser indicating a problem with the Label Type.
						// if we are not Accumulate Items with Item strip mode ON
						if( !( s->getCurrentMode() == OpModeBuildAMeal || Configuration::getInstance()->toBoolCfg(fieldOffsetCfgEnableItemStripMode) ) )
						{
							mySession->addDialogMessage(ErrProductDev, (ProductErrPriLabelWontFit + printerNum), HfcResponse::RESPONSESTYLE_OK);

							Debug::trace("ProductManager.ToggleLabel", HobString("Label type won't fit - no family label found: PrinterNum = ") + HobString(Integer::toString(printerNum)) + HobString("	LabelTypeNum = ") + HobString(Integer::toString(labelType->getLabelTypeNum())));
							labelType->setLabelMediaError(LabelWontFit);
						}
					}
					else if (result == LabelCorrected)
					{
						// We are looking through the family using the normal/traditional lookup scheme that looks for the label type number in the plu's dept,
						// then default dept, then ROM, and returns it wherever it exists in the hierarchy.

						Debug::trace("ProductManager.ToggleLabel", HobString("Label type won't fit. Family label found: PrinterNum = ") + inttostring(printerNum) + HobString(",  LabelTypeNum = ") + longtostring(labelTypeNum));

						LabelType *newLabelType = LabelType::findRecord(labelTypeNum, deptNumOfProduct, dbAccess);
						/*
						
						// This commented out code would have been used for the label family lookup method where we could first look for the best-fit label in the current dept first.  If a best-fit label isn't found in the
						// current dept, then we'll look for one in the family in the default dept (dept 0).  If one still isn't found, then we'll look in the
						// ROM labels db table.  Even if a better-fitting label type exists in the default dept, that is the same label type number as the one
						// we are starting with, or another in the family is a better fit in the default dept than the one found in the current dept, we will
						// use the best-fit label type found in the current dept.
						// James Meyer chose not to use this algorithm,	instead wanting to go through the family using the normal lookup scheme that looks for
						// the label type number in the plu's dept,	then default dept, then ROM, and returns it wherever it exists in the hierarchy.

						// I'm leaving this code here in case we ever want this alternative method, since it worked.

						// If a better-fit label type was found, by the call to LabelTypeManager::validateLabelTypeWithStock() above, then both labelTypeNum and deptNum could have changed.
						// So use those to lookup the new label type that is the better fit.
						if (deptNum == RECORD_NOT_FOUND)
							// The corrected label type is coming from the ROM labels.
							newLabelType = LabelType::findROMRecord(labelTypeNum, dbAccess);
						else
							// The corrected label type is coming from the custom labels table.
							newLabelType = LabelType::findCustomRecord(labelTypeNum, deptNum, dbAccess);
						*/
						if (newLabelType)
						{
							if (newLabelType->getErrorCode() == RECORD_OK)
							{
								//	Do not send a message to browser indicating the Label Type was changed.
								//	It will be done from the method that called this one.

								Debug::trace("ProductManager.ToggleLabel", "Label is GOOD! Sending LabelCorrected.");
								newLabelType->setLabelMediaError(LabelCorrected);
								*correctedLabelType = newLabelType;
							}
							else
							{
								Debug::trace("ProductManager.ToggleLabel", "Label is bad! Sending LabelInvalid.");
								result = LabelInvalid;
								delete newLabelType;
							}
						}
						else
						{
							Debug::trace("ProductManager.ToggleLabel", "Label does not exist! Sending LabelInvalid.");
							result = LabelInvalid;
						}
					}
					else
					{
						Debug::trace("ProductManager.ToggleLabel", "labelType->setLabelMediaError(" + inttostring(result) + ")");
						labelType->setLabelMediaError(result);
					}
				}
				else
				{
					if( !Configuration::getInstance()->isSelfServiceScale() )
					{
						Debug::trace("ProductManager.ToggleLabel", "Sending LabelWontFit!");
						labelType->setLabelMediaError(LabelWontFit);
						result = LabelWontFit;
						mySession->addDialogMessage(ErrProductDev, ProductErrPriLabelNotSized + printerNum, HfcResponse::RESPONSESTYLE_OK);
					}
				}
			}
			else if ( labelType && labelType->getErrorCode() == RECORD_OK )
			{
				labelType->setLabelMediaError(LabelOk);
				result = LabelOk;
			}

			if ( result == LabelInvalid || (labelType && labelType->getErrorCode() != RECORD_OK) )
			{
				// Send error message to browser indicating a problem with the Label Type.
				mySession->addDialogMessage(ErrProductDev, (ProductErrPriLabelNotFound + printerNum), HfcResponse::RESPONSESTYLE_OK);

				Debug::trace("ProductManager.ToggleLabel", HobString("Label type: PrinterNum = ") + HobString(Integer::toString(printerNum)) + HobString("	ErrorCode = ") + HobString(Integer::toString(labelType->getErrorCode())) + HobString("	LabelTypeNum = ") + HobString(Integer::toString(labelType->getLabelTypeNum())));
			}

		}
		else
		{
			labelType->setLabelMediaError(LabelOk);
		}
	}
	return result;
}

LabelMediaReturn ProductManager::labelMediaCheck()		//	Called before printing the label
{
	if ( !currentProduct ||
	     currentProduct->isPrintingInProductionEntryMode() ||
	     ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && Configuration::getInstance()->isSelfServiceScale() )
	   )
		return LabelOk;

	LabelMediaReturn result = LabelInvalid;
	bool anyLabelTypeInvalid = false;
	bool allLabelTypesAreNull = true;

	bool noLabelTypesForLoadedPrinter = true;

	bool printerNotSized = false;		//	PTR 679: Check if we need to redisplay a printer error.

	Session *s = dynamic_cast<Session *> (mySession);

	for (short printerNum = FirstPrinter; printerNum < MaxPrinterTypes; printerNum++)
	{
		LabelType *labelType = getCurrentProduct()->getLabelType(printerNum);
		if (labelType)
		{
			allLabelTypesAreNull = false;

			HfcPrinterStockMetrics *printerStockMetrics = (s) ? s->getPrinterStockMetrics(printerNum) : 0;
			if (printerStockMetrics && printerStockMetrics->getLabelLengthInLines() > 0)
			{
				noLabelTypesForLoadedPrinter = false;

				if (labelType->getErrorCode() != RECORD_OK)
				{
					result = LabelInvalid;
					anyLabelTypeInvalid = true;

					// Send error message to browser indicating a problem with the Label Type.
					if (mySession)
						mySession->addDialogMessage(ErrProductDev, (ProductErrPriLabelNotFound + printerNum), HfcResponse::RESPONSESTYLE_OK);

					if (Debug::isTracing("ProductManager.labelMediaCheck"))
						Debug::trace("ProductManager.labelMediaCheck", HobString("Label type: PrinterNum = ") + HobString(Integer::toString(printerNum)) + HobString("	ErrorCode = ") + HobString(Integer::toString(labelType->getErrorCode())) + HobString("	LabelTypeNum = ") + HobString(Integer::toString(labelType->getLabelTypeNum())));
				}
				else if (labelType->isLabelMediaError() == true)
				{
					result = LabelInvalid;
					anyLabelTypeInvalid = true;

					// Send error message to browser indicating a problem with the Label Type.
					if (mySession)
						mySession->addDialogMessage(ErrProductDev, (ProductErrPriLabelWontFit + printerNum), HfcResponse::RESPONSESTYLE_OK);

					if (Debug::isTracing("ProductManager.labelMediaCheck"))
						Debug::trace("ProductManager.labelMediaCheck", HobString("Label type won't fit - no family label found: PrinterNum = ") + HobString(Integer::toString(printerNum)) + HobString("	LabelTypeNum = ") + HobString(Integer::toString(labelType->getLabelTypeNum())));
				}
				else
					result = LabelOk;
			}
			else
			{
				printerNotSized = true;		//	PTR 679: Why is the label stock not sized?
			}
		}
	}

	if ( allLabelTypesAreNull )
	{
		//	There needs to be a valid label for at least one printer - not all can be null.
		noLabelTypesForLoadedPrinter = false;
		anyLabelTypeInvalid = true;

		if (mySession &&
			((s->isInOperateMode()) &&
			(Configuration::getInstance()->isSelfServiceScale() && !Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && (SelfServOption_PrintToScreen != Configuration::getInstance()->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors))))
			)
			mySession->addDialogMessage(ErrProductDev, ProductErrLabelType, HfcResponse::RESPONSESTYLE_OK);

		if (Debug::isTracing("ProductManager"))
			Debug::trace("ProductManager", HobString("Label type: All label types are null!"));
	}

	if ( noLabelTypesForLoadedPrinter )
	{
		// Send error message to browser indicating a problem with the Label Type.
		if ( mySession )
		{
			bool isHeadDisengaged = false;

			// SOF-3481 dont send if head disengaged
			for ( short i = FirstPrinter; i < MaxPrinterTypes; i++ )
			{
				Session *s = dynamic_cast<Session*>( mySession );

				if ( !s )
					break;

				HfcPrinterStatus *status = s->getCurrentPrinterStatus( i );

				if (( status ) && ( (isHeadDisengaged |= status->isHeadDisengaged()) ))
				{
					// SOF-3505
					ResponseMessageManager *manager = s->getResponseMessageManager();

					if ( manager )
					{
						if ( Configuration::getInstance()->isPrepackScale() )
							manager->addPrinterMessage( PrinterErrHeadUpForEpp, HfcResponse::RESPONSESTYLE_OK, (PrinterType) i );
						else
							manager->addPrinterMessage( PrinterErrHeadUp, HfcResponse::RESPONSESTYLE_OK, (PrinterType) i );
					}

					break;
				}
			}

			if ( !isHeadDisengaged )
				mySession->addDialogMessage( ErrProductDev, ProductErrNoLabelTypesSelected, HfcResponse::RESPONSESTYLE_OK );
		}

		bool debugOn = Debug::isTracing("ProductManager.labelMediaCheck");
		if (debugOn)
			Debug::trace("ProductManager.labelMediaCheck", "No label types selected for any printer with label stock loaded!");

		//	PTR 679: Request a PrinterStatus so we can redisplay any errors.
		if (printerNotSized)
		{
			if (debugOn)
				Debug::trace("ProductManager.labelMediaCheck", "A printer is not sized. Requesting PrinterStatus from all printers.");
			for (short printerNum = FirstPrinter; printerNum < MaxPrinterTypes; printerNum++)
			{
				PrinterRequestStatus prs(mySession, (PrinterType)printerNum);
				mySession->send(&prs, Messenger::DEST_RTC);
			}
		}
	}

	if (Configuration::getInstance()->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
		return LabelOk;
	else
		return (anyLabelTypeInvalid) ? LabelInvalid : result;
}


void ProductManager::checkForPLUGraphicsError()
{
	short index;
	for (index = 0; index < Configuration::getInstance()->toIntCfgI(CfgInternal::MAXGRAPHICSPERPRODUCT); index++)
	{
		checkForGraphicError(index);
	}
}


void ProductManager::checkForGraphicError(short index)
{
	Graphic *graphic = getCurrentProduct()->getGraphic(index);
	if (graphic && mySession)
	{
		// Send error message to browser indicating a problem with the Graphic.
		if ((graphic->getErrorCode() != RECORD_OK) && (!graphic->getGraphicName().empty()))
		{
			// WEH-45
			// ignore this error when graphic intended for GCD
			if (( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgGCDUseTransactionalLayout) ) &&
				( Configuration::getInstance()->toIntCfg(fieldOffsetCfgGCDProductAdvertType) == (index+1)))
			{
				; // do nothing
			}
			else
			{
				mySession->addDialogMessage(ErrProductDev, ProductErrGraphic, HfcResponse::RESPONSESTYLE_OK);
			}
		}
	}
}

short ProductManager::getPrinterNumFromFieldOffset(short fieldCode)
{
	// TODO - Larry - done - how are we going to manage the current 1:1 label:printer deal? 

	short printerNum = FirstPrinter;

	switch (fieldCode)
	{
		case Product::fieldOffsetLabelTypeNum2:
		case Product::fieldOffsetDefaultLabelTypeNum2:
		case Product::fieldOffsetManualLabelTypeNum2:
		case Product::fieldOffsetPrepackLabelTypeNum2:
		case Product::fieldOffsetProdEntryLabelTypeNum2:
		case Product::fieldOffsetSelfServLabelTypeNum2:
		case Product::fieldOffsetPick5LabelTypeNum2:
		case Product::fieldOffsetOnlineOrderingLabelTypeNum2:
			printerNum = SecondPrinter;
			break;
		case Product::fieldOffsetLabelTypeNum3:
		case Product::fieldOffsetDefaultLabelTypeNum3:
		case Product::fieldOffsetManualLabelTypeNum3:
		case Product::fieldOffsetPrepackLabelTypeNum3:
		case Product::fieldOffsetProdEntryLabelTypeNum3:
		case Product::fieldOffsetSelfServLabelTypeNum3:
		case Product::fieldOffsetPick5LabelTypeNum3:
		case Product::fieldOffsetOnlineOrderingLabelTypeNum3:
			printerNum = ThirdPrinter;
			break;
		default:
			break;
	}
	return printerNum;
}


Util::CSmartPtr<std::string> ProductManager::buildProductionPlanningURI(const std::string& uriString, hfc::Transaction * trans, bool increment)
{
	if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgEnableProductionPlanning))
	{
		int factor = increment ? 1 : -1;
		if ( trans )
			factor *= trans->getNumberPackages();
		int quantity = factor;
		HobWeight weight = 0;

		if ( trans )
		{
			if ( trans->getProductType() == Product::PRODUCTTYPE_RANDOMWEIGHT )
				weight = trans->getFinalNetWeight() * factor;
			else
				quantity = trans->getByCount() * factor;

			return buildProductionPlanningURI(uriString, trans->getProductNumber(), weight, quantity);
		}
	}
	return Util::CSmartPtr<std::string>(0);
}


Util::CSmartPtr<std::string> ProductManager::buildProductionPlanningURI(const std::string& uriString, HobRecordNum plu, HobWeight weight, int quantity)
{
	const bool traceOn( Debug::isTracing("ProductManager.ProductionPlanningURI") );

	Configuration *config( Configuration::getInstance() );

	Session *s( dynamic_cast<Session*>( mySession ) );

	if ( !s )
		return Util::CSmartPtr<std::string>( 0 );

	// retrieve http parameter names
	std::string queryStore( config->toStringCfgI(CfgInternal::HTTPQUERYSTORE) );
	std::string queryDept( config->toStringCfgI(CfgInternal::HTTPQUERYDEPT) );
	std::string queryModel( config->toStringCfgI(CfgInternal::HTTPQUERYMODEL) );
	std::string queryPLU( config->toStringCfgI(CfgInternal::HTTPQUERYPLU) );
	std::string queryWeight( config->toStringCfgI(CfgInternal::HTTPQUERYWEIGHT) );
	std::string queryQuantity( config->toStringCfgI(CfgInternal::HTTPQUERYQUANTITY) );
	std::string queryLanguage( config->toStringCfgI(CfgInternal::HTTPQUERYLANGUAGE) );

	// retrieve language
	std::string language;

	if ( SecondaryLanguage == s->getCurrentLanguageSelection() )
	{
		language.assign( config->getSecondaryLanguageCode() );
		language.append( 1, '.' ).append( config->getSecondaryCountryCode() );
	}
	else
	{
		language.assign( config->getPrimaryLanguageCode() );
		language.append( 1, '.' ).append( config->getPrimaryCountryCode() );
	}

	util_string::toupper( language );

	// format uri
	comm::HTTPURI uri( uriString );
	comm::HTTPURI::parameterNames_iterator iter( uri.queryBegin() );

	if (traceOn)
		Debug::trace("ProductManager.ProductionPlanningURI", "processing params...");

	while ( iter != uri.queryEnd() )
	{
		if (traceOn)
			Debug::trace("ProductManager.ProductionPlanningURI", "param " + (*iter) );

		if ( *iter == queryStore )
			uri.setQueryParameter(*iter, config->toStringCfg(fieldOffsetCfgStoreId));
		else if ( *iter == queryDept )
			uri.setQueryParameter(*iter, config->toStringCfg(fieldOffsetCfgDeptName_NOT_USED));
		else if ( *iter == queryModel )
			uri.setQueryParameter(*iter, config->toStringCfgI(CfgInternal::SCALENAME));
		else if ( *iter == queryPLU )
			uri.setQueryParameter(*iter, util_string::longtostring(plu));
		else if ( *iter == queryWeight )
		{
			HobString result;
			Configuration::getInstance()->formatWeight(weight, result);
			uri.setQueryParameter(*iter, result);
		}
		else if ( *iter == queryQuantity )
			uri.setQueryParameter(*iter, util_string::inttostring(quantity));
		else if ( *iter == queryLanguage )
			uri.setQueryParameter(*iter, language);

		++iter;
	}

	if (traceOn)
		Debug::trace("ProductManager.ProductionPlanningURI", "formatting...");

	Util::CSmartPtr<std::string> result( uri.formatURI() );

	if (traceOn)
	{
		if (result.null())
			Debug::trace("ProductManager.ProductionPlanningURI", "done: <invalid uri>");
		else
			Debug::trace("ProductManager.ProductionPlanningURI", "done: " + (*result) );
	}

	return result;
}

std::string ProductManager::buildProductionPlanningJSONDocument( ProductTransaction *trans, bool increment, ProductTransaction *masterTrans )
{
	Session *s( dynamic_cast<Session*>( mySession ) );
	std::string doc;

	if ( s )
	{
		Configuration *config( Configuration::getInstance() );

		// set master trans
		if ( !masterTrans )
			masterTrans = trans;

		// retrieve weight and quantity
		int factor( increment ? 1 : -1 );

		if ( masterTrans )
			factor *= masterTrans->getNumberPackages();

		HobString weight;
		int quantity( factor );

		if ( masterTrans )
		{
			HobWeight rawWeight( 0 );

			if ( Product::PRODUCTTYPE_RANDOMWEIGHT == masterTrans->getProductType() )
				rawWeight = masterTrans->getFinalNetWeight() * factor;
			else
				quantity = masterTrans->getByCount() * factor;

			// WEH-15
			// record quantity and weight for meal item
			if ( trans != masterTrans )
			{
				quantity = factor;

				if ( config->toBoolCfg(fieldOffsetCfgWeightRequiredForMeal) )
					rawWeight = masterTrans->getFinalNetWeight() * factor;
			}

			config->formatWeight( rawWeight, weight );
		}

		// retrieve store and dept
		std::string storeId(config->toStringCfg(fieldOffsetCfgStoreId));
		// No longer used as of 9-19-17- claim back later - std::string deptName(config->toStringCfg(fieldOffsetCfgDeptName));

		// retrieve ip and gateway address
		std::string ipAddress;
		std::string gatewayAddress;

		config->getIPAndGatewayAddresses(ipAddress, gatewayAddress);

		// retrieve hostname
		std::string hostname( bootParameters::getDNSHostName() );

		// generate JSON document
		doc.append( "{\r\n" );

		if ( hostname.length() )
		{
			doc.append( "  \"hostname\": " );
			doc.append( "\"" ).append( hostname ).append( "\",\r\n" );
		}

		if ( ipAddress.length() )
		{
			doc.append( "  \"ipAddress\": " );
			doc.append( "\"" ).append( ipAddress ).append( "\",\r\n" );
		}

		doc.append( "  \"utcNumber\": " );
		doc.append( util_string::longtostring(time( NULL )) ).append( ",\r\n" );

		if ( storeId.length() )
		{
			doc.append( "  \"storeNumber\": " );
			doc.append( storeId ).append( ",\r\n" );
		}

		// No longer used as of 9-19-17- claim back later - 
		//if ( deptName.length() )
		//{
		//	doc.append( "  \"deptNumber\": " );
		//	doc.append( deptName ).append( ",\r\n" );
		//}

		doc.append( "  \"scaleModel\": " );
		doc.append( "\"" ).append( config->toStringCfgI(CfgInternal::SCALENAME) ).append( "\",\r\n" );

		if ( trans )
		{
			BarCodeNumber bcn ( masterTrans->getBarCodeNum() );

			HobString totalPrice;
			config->formatPrice( masterTrans->getFinalPrice(), totalPrice );

			// LAP-BARCODE
			//doc.append( "  \"barcode\": " );
			//doc.append( "\"" ).append( bcn.getBarcodeString() ).append( "\",\r\n" );

			doc.append( "  \"totalPrice\": " );
			doc.append( totalPrice ).append( ",\r\n" );

			doc.append( "  \"pluNumber\": " );
			doc.append( util_string::longtostring(trans->getProductNumber()) ).append( ",\r\n" );

			doc.append( "  \"sideServed\": " );
			doc.append( "[],\r\n" );
		}

		doc.append( "  \"totalWeight\": " );
		doc.append( weight ).append( ",\r\n" );

		doc.append( "  \"prodQty\": " );
		doc.append( util_string::inttostring(quantity) ).append( "\r\n" );

		doc.append( "}\r\n" );
	}

	return doc;
}

std::string ProductManager::buildProductionPlanningJSONDocument( ProductTransactionMaster *master, bool increment )
{
	ProductTransaction *meal( master->getFirstTransaction() );
	std::string doc;

	if ( meal )
	{
		// build JSON doc of meal item
		doc = buildProductionPlanningJSONDocument( meal, increment, master->getMasterProductTransaction() );

		// build list of side items
		std::string sides;
		ProductTransaction *side;

		while ( (side = master->getNextTransaction()) )
		{
			int quantity( 1 );

			// how many sides?
			if ( Product::PRODUCTTYPE_RANDOMWEIGHT != side->getProductType() )
				quantity *= side->getByCount();

			// make sure positive number
			if ( quantity < 0 )
				quantity *= -1;

			// add side to list
			while ( quantity-- )
			{
				if ( sides.length() )
					sides.append( ", " );

				sides.append( util_string::longtostring( side->getProductNumber() ) );
			}
		}

		// insert sides into document
		if ( sides.length() )
		{
			// retrieve http parameter names
			static const std::string sideServedString( "\"sideServed\": [" );

			// locate plu parameter
			std::string::size_type loc( doc.find( sideServedString ) );

			// insert side items
			if ( std::string::npos != loc )
				doc.insert( loc + sideServedString.length(), sides );
		}
	}

	return doc;
}

void ProductManager::updateProductionPlanningApplication( ProductTransaction *prodTransaction )
{
	// check production planning enabled
	if ( !Configuration::getInstance()->toBoolCfg( fieldOffsetCfgEnableProductionPlanning ) )
		return;
	// verify this transaction is FPP and affects production
	else if (( !prodTransaction ) || ( !prodTransaction->isProductionPlanningTransaction() ) || ( hfc::TransactionType::AffectsProductionNone == prodTransaction->getAffectsProductionFlag() ))
		return;
	// verify session
	else if ( !mySession )
		return;

	const bool traceOn( Debug::isTracing( "ProductManager.ProductionPlanning" ) );

	if ( traceOn )
	{
		Debug::trace( "ProductManager.ProductionPlanning", "updateProductionPlanningApplication(): prodTransaction = " + util_string::ptrtostring(prodTransaction) );
		Debug::trace( "ProductManager.ProductionPlanning", "updateProductionPlanningApplication(): master = " + util_string::ptrtostring(prodTransaction->getProductTransactionMaster()) );
	}

	// build uri for update
	Util::CSmartPtr<std::string> uri(
		buildProductionPlanningURI(
				Configuration::getInstance()->toStringCfg( fieldOffsetCfgProductionPlanningUpdate ),
				prodTransaction,
				hfc::TransactionType::AffectsProductionPositively == prodTransaction->getAffectsProductionFlag() ) );

	if ( uri.null() )
	{
		if ( traceOn )
			Debug::trace( "ProductManager.ProductionPlanning", "Problems building URI for Production Planning CGI call. Not performed." );

		comm::CGICallPerformedMessage message( false );
		Hobart::CMessageCenter::instance() << &message;

		return;
	}
	else if ( uri->empty() )
	{
		if ( traceOn )
			Debug::trace( "ProductManager.ProductionPlanning", "URI for Production Planning CGI call is empty. Not performed." );

		comm::CGICallPerformedMessage message( false );
		Hobart::CMessageCenter::instance() << &message;

		return;
	}

	// build payload for FPP update
	std::string payload;

	if ( traceOn )
		Debug::trace( "ProductManager.ProductionPlanning", "Going to request Production Planning CGI call with URI: " + *uri );

	// FT-1694: use POST request method for FPP
	if ( 4 == Configuration::getInstance()->toIntCfgI( CfgInternal::HTTPREQUESTMETHOD ) )
	{
		std::string contentType( Configuration::getInstance()->toStringCfgI( CfgInternal::HTTPREQUESTCONTENTTYPE ) );

		if ( traceOn )
			Debug::trace( "ProductManager.ProductionPlanning", "Handling content type: " + contentType );

		if ( "application/json" == contentType )
		{
			ProductTransactionMaster *master( prodTransaction->getProductTransactionMaster() );
			ProductTransaction *meal( NULL );

			// check for meal item
			if (( master ) && ( 1 < master->getTransactionCount() ))
				if ( (meal = master->getFirstTransaction()) )
				{
					Product *mealProd( meal->getProduct() );

					// bad pointer
					if ( !mealProd )
					{
						if ( traceOn )
							Debug::trace( "ProductManager.ProductionPlanning", "Transaction missing product record" );
					}

					// not a meal item
					else if ( !mealProd->isMealBuilderItem() )
					{
						if ( traceOn )
							Debug::trace( "ProductManager.ProductionPlanning", "Not a meal item" );

						meal = NULL;
					}

					// only update FPP for meal item, ignore the sides (they are included in the meal FPP tx)
					else if ( prodTransaction != meal )
					{
						if ( traceOn )
							Debug::trace( "ProductManager.ProductionPlanning", "Ignoring FPP Update for side item" );

						return;
					}
				}

			if ( meal )
				payload = buildProductionPlanningJSONDocument( master, hfc::TransactionType::AffectsProductionPositively == prodTransaction->getAffectsProductionFlag() );
			else
				payload = buildProductionPlanningJSONDocument( prodTransaction, hfc::TransactionType::AffectsProductionPositively == prodTransaction->getAffectsProductionFlag() );
		}
	}

	// perform update
	comm::PerformCGICallMessage message( uri, payload );
	mySession->send( &message, Messenger::DEST_RTC );
}


void ProductManager::handleProductTransactionTypeChanged()
{
	Debug::trace("ProductManager", "handleProductTransactionTypeChanged() ");

	Session *s( dynamic_cast<Session*>( mySession ) );

	if ( !s )
		Debug::trace("ProductManager", "bad session pointer");
	else if ( !s->isInOperateMode() )
		Debug::trace("ProductManager", "not in operate mode");
	else if ( !currentProduct )
		Debug::trace("ProductManager", "no current product");
	else
	{
		// Update the transaction type in the current transaction, if any
		hfc::TransactionTypeManager *transTypeMgr( mgrContainer_->getTransactionTypeManager() );
		ProductTransaction *currentTrans( currentProduct->getCurrentTransaction() );

		if ( !currentTrans )
			Debug::trace("ProductManager", "no current transaction");
		else if ( !transTypeMgr )
			Debug::trace("ProductManager", "no transaction type manager");
		else
		{
			Util::CSmartPtr<hfc::TransactionType> newType( transTypeMgr->getOperatorModeTransactionType() );

			if ( newType.null() )
				Debug::trace("ProductManager", "new transaction type is NULL");
			else
			{
				Debug::trace("ProductManager", "associate new transaction type " + util_string::inttostring((int) newType->getNumber()));
				currentTrans->associateTransactionType(newType);
			}
		}

		// Check the settings of the new TransactionType to see if it changes the UPC number.
		const HobString upcVendorNum( currentProduct->getBarCodeNum().getVendorNumber() );
		const HobString upcProductNum( currentProduct->getBarCodeNum().getProductNumber() );

		int fieldOffset = 0;
		HobString upcString("");

		applyTransactionTypeToUpcNumber();		//	Potentially adjust the UPC number for Auto change UPC digit.

		if ( upcVendorNum != currentProduct->getBarCodeNum().getVendorNumber() )
		{
			fieldOffset = Product::fieldOffsetBarcodeVendorNum;
			upcString = currentProduct->getBarCodeNum().getVendorNumber();
		}
		if (( fieldOffset == 0)  && ( upcProductNum != currentProduct->getBarCodeNum().getProductNumber() ))
		{
			fieldOffset = Product::fieldOffsetBarcodeProductNum;
			upcString = currentProduct->getBarCodeNum().getProductNumber();
		}

		Debug::trace("ProductManager", "field offset " + util_string::inttostring(fieldOffset));
		Debug::trace("ProductManager", "upc string " + upcString);

		if ( fieldOffset > 0 )
		{
			Debug::trace("ProductManager", "update product due to new upc");

			//	The UPC number has changed.
			//	Send the modified UPC 'part' to the browser in a WriteSingleField message.
			WriteSingleField productUpdate;
			productUpdate.setRecordNum(1);
			productUpdate.setFieldCode(fieldOffset);
			productUpdate.setFieldNewValue(upcString);

			SessionMessage fieldUpdatedMsg(ProductDev, ProductWriteSingleField);
			fieldUpdatedMsg.setFormatType(BrowserFormat);
			fieldUpdatedMsg.setHfcObject(&productUpdate);

			mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
		}
	}

	Debug::trace("ProductManager", "handleProductTransactionTypeChanged() done");
}


void ProductManager::handleProductLabelFormSize(HfcProductLabelFormSizeMsg *labelFormSizeMsg)
{
	bool recognizedType = true;
	if ( currentProduct && currentProduct->getProductNum() == labelFormSizeMsg->getProductNumber() )
	{
		int width = 0, height = 0;
		std::string fontName;
		int pointSize = 0;

		int textType = labelFormSizeMsg->getLabelFormType();
		switch ( textType )
		{
			case psttExpandedText:   // EXPANDED_TEXT
			case psttSpecialMessage: // SPECIAL_MESSAGE
			case psttProductNote:
			case psttMarqueeText:
			case psttProductText5:
			case psttProductText6:
			case psttProductText7:
			case psttProductText8:
			case psttProductText9:
				currentProduct->getProductTextSize(textType, width, height, fontName, pointSize);
				break;

			case psttProductDescription: // DESCRIPTION
				currentProduct->getDescriptionSize(width, height, fontName, pointSize);
				break;

			case psttCoolTrackingText: // Cool TRACKING_TEXT
				currentProduct->getCoolTrackingTextSize(width, height, fontName, pointSize);
				break;

			default:
				recognizedType = false;
				break;
		}

		if ( mySession )
		{
			Debug::trace("ProductManager", "Sending ProductLabelFormSize to UI");

			HfcProductLabelFormSizeMsg newMessage(*labelFormSizeMsg);
			newMessage.setLabelFormWidth(width);
			newMessage.setLabelFormHeight(height);
			newMessage.setDefaultFontName(fontName);
			newMessage.setDefaultPointSize(pointSize);

			SessionMessage writeLabelFormSizeMsg(ProductDev, ProductWriteLabelFormSize);
			writeLabelFormSizeMsg.setFormatType(BrowserFormat);
			writeLabelFormSizeMsg.setHfcObject(&newMessage);

			// Send the product support message to the UI (Browser)
			mySession->send(&writeLabelFormSizeMsg, Messenger::DEST_UI);
		}
	}
}


/**************************************************************************************************
 **************************************************************************************************
 ***																							***
 ***						 Self-Service Related Methods									   ***
 ***																							***
 **************************************************************************************************
 **************************************************************************************************/

#include "ULong.h"

#if 0
void ProductManager::addDataToSoftKeyList(SoftKeyLists *currentGridData, DBQuery<DBProduct> *productsQuery, DBQuery<DBCategory> *categoriesQuery)
{
	if (!mySession || !currentGridData)
		return;

bool isTiming = Debug::isTracing("Session.MessageTiming");

if (isTiming)
{
	struct timespec tp1, tp2;
	clock_gettime(CLOCK_REALTIME, &tp1);

	if (productsQuery)
	{
		DBQuery<DBProduct>::const_iterator tempIter = productsQuery->begin();
		while (tempIter != productsQuery->end())
			++tempIter;
	}

	clock_gettime(CLOCK_REALTIME, &tp2);

	HobString tp1s(ULong::toString(tp1.tv_sec));			// seconds
	HobString tp1n(Long::toString(tp1.tv_nsec));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("looping through products query() - start time (sec, msec): ") + tp1s + HobString(" ") + tp1n);

	HobString tp2s(ULong::toString(tp2.tv_sec));			// seconds
	HobString tp2n(Long::toString(tp2.tv_nsec));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("looping through products query() -   end time (sec, msec): ") + tp2s + HobString(" ") + tp2n);

	long tp1m = tp1.tv_nsec / 1000000;
	long tp2m = tp2.tv_nsec / 1000000;
	time_t tp2sec = tp2.tv_sec;
	if (tp2m < tp1m)
	{
		tp2sec--;
		tp2m+=1000;
	}
	time_t elapsedSec = tp2sec - tp1.tv_sec;
	long elapsedMilli = tp2m - tp1m;
	HobString els(ULong::toString(elapsedSec));			// seconds
	HobString eln(Long::toString(elapsedMilli));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("looping through products query() - elapsed time (sec, msec): ") + els + HobString(" ") + eln);
}


if (isTiming)
{
	struct timespec tp1, tp2;
	clock_gettime(CLOCK_REALTIME, &tp1);

	if (categoriesQuery)
	{
		DBQuery<DBCategory>::const_iterator tempIter = categoriesQuery->begin();
		while (tempIter != categoriesQuery->end())
			++tempIter;
	}

	clock_gettime(CLOCK_REALTIME, &tp2);

	HobString tp1s(ULong::toString(tp1.tv_sec));			// seconds
	HobString tp1n(Long::toString(tp1.tv_nsec));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("looping through categories query() - start time (sec, msec): ") + tp1s + HobString(" ") + tp1n);

	HobString tp2s(ULong::toString(tp2.tv_sec));			// seconds
	HobString tp2n(Long::toString(tp2.tv_nsec));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("looping through categories query() -   end time (sec, msec): ") + tp2s + HobString(" ") + tp2n);

	long tp1m = tp1.tv_nsec / 1000000;
	long tp2m = tp2.tv_nsec / 1000000;
	time_t tp2sec = tp2.tv_sec;
	if (tp2m < tp1m)
	{
		tp2sec--;
		tp2m+=1000;
	}
	time_t elapsedSec = tp2sec - tp1.tv_sec;
	long elapsedMilli = tp2m - tp1m;
	HobString els(ULong::toString(elapsedSec));			// seconds
	HobString eln(Long::toString(elapsedMilli));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("looping through categories query() - elapsed time (sec, msec): ") + els + HobString(" ") + eln);
}



	// It is assumed that both the products and categories queries are sorted alphabetically before calling this method.

	DBQuery<DBProduct>::const_iterator *productsIter = (DBQuery<DBProduct>::const_iterator *)NULL;
	DBQuery<DBCategory>::const_iterator *categoriesIter = (DBQuery<DBCategory>::const_iterator *)NULL;

	if (productsQuery)
	{
		//Debug::trace("ProductManager", "addDataToSoftKeyList() - products query passed in is not null");
		DBQuery<DBProduct>::const_iterator tempIter = productsQuery->begin();
		productsIter = new DBQuery<DBProduct>::const_iterator(tempIter);
	}
	if (categoriesQuery)
	{
		//Debug::trace("ProductManager", "addDataToSoftKeyList() - categories query passed in is not null");
		DBQuery<DBCategory>::const_iterator tempIter = categoriesQuery->begin();
		categoriesIter = new DBQuery<DBCategory>::const_iterator(tempIter);
	}

	DBProduct *p = (DBProduct *)NULL;
	Product *cellProduct = (Product *)NULL;
	DBCategory *c = (DBCategory *)NULL;
	Category *cellCategory = (Category *)NULL;

	HobString prodName("");
	HobString catName("");

	int ip = 0, ic = 0;
	LanguageSelection langNum = mySession->getCurrentLanguageSelection();
	short whichIsNext = 0;  // 0 = initial state (none have been processed), 1 = product is next, 2 = category is next


struct timespec tp1, tp2;
clock_gettime(CLOCK_REALTIME, &tp1);


	while ((productsIter && *productsIter != productsQuery->end()) || (categoriesIter && *categoriesIter != categoriesQuery->end()))
	{
		if (productsIter && (!categoriesIter || *categoriesIter == categoriesQuery->end()))
		{
			//Debug::trace("ProductManager", "addDataToSoftKeyList() - only have a products query, only adding products");
			p = new DBProduct(*(*productsIter));
			whichIsNext = 1;
		}
		else if (categoriesIter && (!productsIter || *productsIter == productsQuery->end()))
		{
			//Debug::trace("ProductManager", "addDataToSoftKeyList() - only have a categories query, only adding categories");
			c = new DBCategory(*(*categoriesIter));
			whichIsNext = 2;
		}
		else
		{
			/* If the last time through this loop, we selected a product to add to the grid, then we processed the product and incremented
			 * the products iterator; so we need to get the new product that the iterator points to.  The category c remains the same as
			 * it was before, so just use it.  If the last time through this loop, we selected a category, then the opposite is true; we
			 * need to get the new category that the categories iterator now points to and the product remains the same as it was the last
			 * time through the loop.
			 */
			//Debug::trace("ProductManager", "addDataToSoftKeyList() - have both a products query and a categories query");
			if (whichIsNext == 1 || whichIsNext == 0)
			{
				//Debug::trace("ProductManager", "addDataToSoftKeyList() - getting next product from the query");
				p = new DBProduct(*(*productsIter));
				prodName = "";
				if (p)
				{
					if (langNum == PrimaryLanguage && !p->isName1Null())
						prodName = p->getName1();
					else if (langNum == SecondaryLanguage && !p->isName2Null())
						prodName = p->getName2();
					else if (!p->isDescriptionNull())
					{
						prodName = p->getDescription();
						prodName.stripFontSequences();
						if (Configuration::getInstance()->getNumOfDescCharsToUse() > 0)
							prodName = prodName.substr(0, Configuration::getInstance()->getNumOfDescCharsToUse());
					}
					prodName.toUpper();
					//Debug::trace("ProductManager", HobString("addDataToSoftKeyList() - found next dbproduct  prodName:") + prodName);
				}
			}
			if (whichIsNext == 2 || whichIsNext == 0)
			{
				//Debug::trace("ProductManager", "addDataToSoftKeyList() - getting next category from the query");
				c = new DBCategory(*(*categoriesIter));
				catName = "";
				if (c)
				{
					if (langNum == PrimaryLanguage && !c->isName1Null())
						catName = c->getName1();
					else if (langNum == SecondaryLanguage && !c->isName2Null())
						catName = c->getName2();
					catName.toUpper();
					//Debug::trace("ProductManager", HobString("addDataToSoftKeyList() - found next dbcategory  catName:") + catName);
				}
			}

			//Debug::trace("ProductManager", HobString("addDataToSoftKeyList() - which is next  pname:") + prodName + HobString("  cname:") + catName);
			if (prodName < catName)
			{
				//Debug::trace("ProductManager", "addDataToSoftKeyList() - product is next");
				whichIsNext = 1;
			}
			else if (prodName > catName)
			{
				//Debug::trace("ProductManager", "addDataToSoftKeyList() - category is next");
				whichIsNext = 2;
			}
			else // the names are the same, so put the product before the category.
			{
				//Debug::trace("ProductManager", "addDataToSoftKeyList() - names are the same, so product is next");
				whichIsNext = 1;
			}
		}

		if (whichIsNext == 1)
		{
			if (p)
			{
				Debug::trace("ProductManager", HobString("addDataToSoftKeyList() - have a product for a cell:") + HobString(p->getPLU()));

				cellProduct = new Product(p, dbAccess, false);
				if (cellProduct)
				{
					currentGridData->add(*cellProduct, langNum);
					ip++;
					delete cellProduct;
					cellProduct = (Product *)NULL;
					// Don't need to delete p; it was deleted when the cellProduct was deleted.
					p = (DBProduct *)NULL;
				}
				else
				{
					delete p;
					p = (DBProduct *)NULL;
				}
			}
			++(*productsIter);
		}
		else if (whichIsNext == 2)
		{
			if (c)
			{
				Debug::trace("ProductManager", HobString("addDataToSoftKeyList() - have a category for a cell:") + HobString(c->getNumber()));

				cellCategory = new Category(c, dbAccess);
				if (cellCategory)
				{
					currentGridData->add(*cellCategory, langNum);
					ic++;
					delete cellCategory;
					cellCategory = (Category *)NULL;
					// Don't need to delete c; it was deleted when the cellCategory was deleted.
					c = (DBCategory *)NULL;
				}
				else
				{
					delete c;
					c = (DBCategory *)NULL;
				}
			}
			++(*categoriesIter);
		}

	}


if (isTiming)
{
	clock_gettime(CLOCK_REALTIME, &tp2);

	HobString tp1s(ULong::toString(tp1.tv_sec));			// seconds
	HobString tp1n(Long::toString(tp1.tv_nsec));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("building grid cell lists - start time (sec, msec): ") + tp1s + HobString(" ") + tp1n);

	HobString tp2s(ULong::toString(tp2.tv_sec));			// seconds
	HobString tp2n(Long::toString(tp2.tv_nsec));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("building grid cell lists -   end time (sec, msec): ") + tp2s + HobString(" ") + tp2n);

	long tp1m = tp1.tv_nsec / 1000000;
	long tp2m = tp2.tv_nsec / 1000000;
	time_t tp2sec = tp2.tv_sec;
	if (tp2m < tp1m)
	{
		tp2sec--;
		tp2m+=1000;
	}
	time_t elapsedSec = tp2sec - tp1.tv_sec;
	long elapsedMilli = tp2m - tp1m;
	HobString els(ULong::toString(elapsedSec));			// seconds
	HobString eln(Long::toString(elapsedMilli));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("building grid cell lists - elapsed time (sec, msec): ") + els + HobString(" ") + eln);
}


	if (productsIter)
		delete productsIter;
	if (categoriesIter)
		delete categoriesIter;

	//Debug::trace("ProductManager", HobString("addDataToSoftKeyList() - read this number of products:") + HobString(Integer::toString(ip)));
	//Debug::trace("ProductManager", HobString("addDataToSoftKeyList() - read this number of categories:") + HobString(Integer::toString(ic)));
}
#endif


#if 0
void ProductManager::handleProductFindSoftKeysBySearchString(SessionMessage *msg)
{
	Debug::trace("ProductManager", "handleProductFindSoftKeysBySearchString() starting ...");
	if (!msg || !dbAccess || !mySession)
		return;

	DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
	if (!dbProductsRM)
		return;

	GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
	if (!gho)
		return;

	HobString lookupString(gho->getData());


HobDateTime t1, t2, t3, t4;
bool isTiming = Debug::isTracing("Session.MessageTiming");

if (isTiming)
{
	t1.setToCurrent();
	HobString t1s = t1.formatDateTime();
	Debug::trace("ProductManager", HobString("handleProductFindSoftKeysBySearchString() - before retrieveKeywordInName query - time: ") + t1s);
}


	//DBQuery<DBProduct> *currentProductsQuery = dbProductsRM->retrieveKeywordInDescription(lookupString);
	DBQuery<DBProduct> *currentProductsQuery = dbProductsRM->retrieveKeywordInName(lookupString);
	if (!currentProductsQuery)
		return;


if (isTiming)
{
	t2.setToCurrent();
	HobString t2s = t2.formatDateTime();
	HobString diff1 = HobString(Long::toString(t2.timeDiff(t1)));
	Debug::trace("ProductManager", HobString("handleProductFindSoftKeysBySearchString() - after retrieveKeywordInName query - time: ") + t2s);
	Debug::trace("ProductManager", HobString("handleProductFindSoftKeysBySearchString() - elapsed time for retrieveKeywordInName query (sec): ") + diff1);


	struct timespec tp1, tp2;
	clock_gettime(CLOCK_REALTIME, &tp1);

	if (currentProductsQuery)
	{
		DBQuery<DBProduct>::const_iterator tempIter = currentProductsQuery->begin();
		while (tempIter != currentProductsQuery->end())
			++tempIter;
	}

	clock_gettime(CLOCK_REALTIME, &tp2);

	HobString tp1s(ULong::toString(tp1.tv_sec));			// seconds
	HobString tp1n(Long::toString(tp1.tv_nsec));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("looping through products query() - start time (sec, msec): ") + tp1s + HobString(" ") + tp1n);

	HobString tp2s(ULong::toString(tp2.tv_sec));			// seconds
	HobString tp2n(Long::toString(tp2.tv_nsec));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("looping through products query() -   end time (sec, msec): ") + tp2s + HobString(" ") + tp2n);

	long tp1m = tp1.tv_nsec / 1000000;
	long tp2m = tp2.tv_nsec / 1000000;
	time_t tp2sec = tp2.tv_sec;
	if (tp2m < tp1m)
	{
		tp2sec--;
		tp2m+=1000;
	}
	time_t elapsedSec = tp2sec - tp1.tv_sec;
	long elapsedMilli = tp2m - tp1m;
	HobString els(ULong::toString(elapsedSec));			// seconds
	HobString eln(Long::toString(elapsedMilli));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("looping through products query() - elapsed msg time (sec, msec): ") + els + HobString(" ") + eln);
}


	if (currentGridData)
	{
		delete currentGridData;
		currentGridData = (SoftKeyLists *)NULL;
	}
	currentGridData = new SoftKeyLists();
	addDataToSoftKeyList(currentGridData, currentProductsQuery);

	currentGridData->insertNextandPreviousCells(mySession->getCurrentLanguageSelection());


if (isTiming)
{
	t3.setToCurrent();
	HobString t3s = t3.formatDateTime();
	HobString diff2 = HobString(Long::toString(t3.timeDiff(t2)));
	Debug::trace("ProductManager", HobString("handleProductFindSoftKeysBySearchString() - after building grid - time: ") + t3s);
	Debug::trace("ProductManager", HobString("handleProductFindSoftKeysBySearchString() - elapsed time for building grid (sec): ") + diff2);
}


	// Send the softkey lists to the UI (Browser)
	SessionMessage sklMsg(ProductDev, ProductWriteSoftKeyList);
	sklMsg.setFormatType(BrowserFormat);
	sklMsg.setHfcObject(currentGridData);
	mySession->send(&sklMsg, Messenger::DEST_UI);

	if (currentProductsQuery)
		delete currentProductsQuery;
}
#endif


#if 0
void ProductManager::handleProductReadMostCommonSoftKeyList()
{
	int cellCount = Configuration::getInstance()->getGridXElements() * Configuration::getInstance()->getGridYElements();
	Debug::trace("ProductManager", HobString("handleProductReadMostCommonSoftKeyList() - cellCount:") + HobString(Integer::toString(cellCount)));
	if (!mySession || cellCount <= 0)
		return;

	/*******************************************************************************************
	 *  Get the most common products for the current language and 'hour'.
	 ******************************************************************************************/
	if (!dbAccess)
		return;

	DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
	if (!dbProductsRM)
		return;

	DBProductStatisticsRM *psRM = dbAccess->getDBProductStatisticsRM();
	if (!psRM)
		return;

	Debug::trace("ProductManager", HobString("handleProductReadMostCommonSoftKeyList() - using MostCommon presentation method"));
	Debug::trace("ProductManager", HobString("handleProductReadMostCommonSoftKeyList() - using language:") + HobString(Integer::toString(mySession->getCurrentLanguageSelection())));

	currentPresentationMethodDataType = PresentationMethod::Type_MostCommon;
	int hour = determineHourForQuery();

	Debug::trace("ProductManager", HobString("handleProductReadMostCommonSoftKeyList() - using hour:") + HobString(Integer::toString(hour)));

	LanguageSelection langNum = mySession->getCurrentLanguageSelection();
	std::string lang = (langNum == PrimaryLanguage) ? Configuration::getInstance()->getPrimaryLanguageCode() : Configuration::getInstance()->getSecondaryLanguageCode();
	DBQuery<DBProductStatistic> *prodStatsQuery = psRM->retrieveMostPrinted(lang, hour);
	//DBQuery<DBProductStatistic> *prodStatsQuery = psRM->retrieveMostPrinted(pm->getMyDbRecord(), hour);
	//DBQuery<DBProduct> *prodStatsQuery = dbProductsRM->retrieveAll();

	if (!prodStatsQuery)
	{
		Debug::trace("ProductManager", "handleProductReadMostCommonSoftKeyList() - prodStatsQuery is null or empty");
		// Don't have any items to send.
		// Should I send an 'error' message to indicate this?
		return;
	}
	else
		Debug::trace("ProductManager", "handleProductReadMostCommonSoftKeyList() - prodStatsQuery is not null");


	if (currentGridData)
	{
		delete currentGridData;
		currentGridData = (SoftKeyLists *)NULL;
	}
	currentGridData = new SoftKeyLists();

	Product *cellProduct = (Product *)NULL;
	int i = 0;
	Debug::trace("ProductManager", "handleProductReadMostCommonSoftKeyList() - before getting iterator");
	DBQuery<DBProductStatistic>::const_iterator prodStatsQueryIter = prodStatsQuery->begin();
	//DBQuery<DBProduct>::const_iterator prodStatsQueryIter = prodStatsQuery->begin();

	while (i < cellCount && (prodStatsQueryIter != prodStatsQuery->end()))
	{
		Debug::trace("ProductManager", HobString("handleProductReadMostCommonSoftKeyList() - next loop through the query"));
		DBProduct* p = dbProductsRM->findLinkedRecord(&*prodStatsQueryIter);
		Debug::trace("ProductManager", HobString("handleProductReadMostCommonSoftKeyList() - after finding dbProduct record from the statistics record"));
		//DBProduct *p = new DBProduct(*prodStatsQueryIter);

		++prodStatsQueryIter;

		if (p)
		{
/*
			Debug::trace("ProductManager", HobString("handleProductReadMostCommonSoftKeyList() - have a product for a cell:") + HobString(p->getPLU()));
			if (i == cellCount-1 && (prodStatsQueryIter != prodStatsQuery->end()))
			{
				// There are too many products than will fit on the screen.  Add the 'next' cell so the
				// user can get to the remaining items.  Since this is the first page for this query, we
				// don't need a 'previous' cell yet.
				// todo

				currentGridData->add(Softkey::DataType_Next, cellProduct->getProductNum());
				i++;
				delete p;
			}
			else
			{
*/
				if (cellProduct)
					delete cellProduct;
				cellProduct = new Product(p, dbAccess, false);
				if (cellProduct)
				{
					i++;
					currentGridData->add(*cellProduct, langNum);
				}
				else
					delete p;
/*
			}
*/
		}
		else
			Debug::trace("ProductManager", "handleProductReadMostCommonSoftKeyList() - didn't get the dbproduct record");

	}

	Debug::trace("ProductManager", HobString("handleProductReadMostCommonSoftKeyList() - done processing query - i:") + HobString(Integer::toString(i)));

	if (cellProduct)
		delete cellProduct;

	//currentGridData->blankFillLists(cellCount);

	// Send the softkey lists to the UI (Browser)
	SessionMessage sklMsg(ProductDev, ProductWriteSoftKeyList);
	sklMsg.setFormatType(BrowserFormat);
	sklMsg.setHfcObject(currentGridData);
	mySession->send(&sklMsg, Messenger::DEST_UI);

	if (prodStatsQuery)
		delete prodStatsQuery;
}
#endif


#if 0
void ProductManager::handleProductReadSoftKeyList(SessionMessage *msg)
{
	Debug::trace("ProductManager", "handleProductReadSoftKeyList() starting ...");
	if (!msg || !mySession || !dbAccess)
		return;

	DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
	if (!dbProductsRM)
		return;

HobDateTime t1, t2, t3, t4, t5, t6, t7;
bool isTiming = Debug::isTracing("Session.MessageTiming");


	ReadSoftKeyParams *rskp = dynamic_cast<ReadSoftKeyParams *> (msg->getHfcObject());
	if (!rskp)
		return;

	if (rskp->getDataType() == PresentationMethod::Type_MostCommon)
	{
		handleProductReadMostCommonSoftKeyList();
		return;
	}

	DBQuery<DBProduct> *currentProductsQuery = (DBQuery<DBProduct> *)NULL;
	DBQuery<DBCategory> *currentCategoriesQuery = (DBQuery<DBCategory> *)NULL;

	if (rskp->getDataType() == PresentationMethod::Type_Previous)
	{
		if (!currentGridData)
			return;

		currentGridData->gotoPreviousPage();
	}
	else if (rskp->getDataType() == PresentationMethod::Type_Next)
	{
		if (!currentGridData)
			return;

		currentGridData->gotoNextPage();
	}
	else
	{
		switch (rskp->getDataType())
		{
			/*******************************************************************************************
			 *  Get the products and subcategories for a particular category.
			 ******************************************************************************************/
			case PresentationMethod::Type_Category:
			{
				if (!dbProductsRM)
					return;

				HobRecordNum categoryNum = rskp->getRecordNum();

				DBCategoriesRM *dbCategoriesRM = dbAccess->getDBCategoriesRM();
				if (!dbCategoriesRM)
					return;

				DBCategory *dbCategory = dbCategoriesRM->findNumber(Long::toString(categoryNum));
				if (!dbCategory)
					return;
				else
					Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - found categoryNum:") + HobString(Long::toString(categoryNum)));


if (isTiming)
{
	t1.setToCurrent();
	HobString t1s = t1.formatTime(FormatTime_HHMMSS);
	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - before child categories query - time: ") + t1s);
}


				currentCategoriesQuery = dbCategoriesRM->retrieveChildCategories(dbCategory);
				if (!currentCategoriesQuery)
					Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - sub-categories of category query is NULL - catNum:") + HobString(Long::toString(categoryNum)));
/*
				else
				{
					DBQuery<DBCategory>::const_iterator tempIter = currentCategoriesQuery->begin();
					if (tempIter == currentCategoriesQuery->end())
						Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - categories query is empty for categoryNum:") + HobString(Long::toString(categoryNum)));
					else
						Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - categories query is NOT empty for categoryNum:") + HobString(Long::toString(categoryNum)));
				}
*/

if (isTiming)
{
	t2.setToCurrent();
	HobString t2s = t2.formatTime(FormatTime_HHMMSS);

	HobString diff1 = HobString(Long::toString(t2.timeDiff(t1)));
	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - elapsed time for categories query (sec): ") + diff1);

	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - before products query - time: ") + t2s);
}


	struct timespec ttp1, ttp2;
	clock_gettime(CLOCK_REALTIME, &ttp1);

				currentProductsQuery = dbProductsRM->retrieveLinkedtoCategory(dbCategory);
				if (!currentProductsQuery)
					Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - products linked to category is NULL - catNum:") + HobString(Long::toString(categoryNum)));
/*
				else
				{
					DBQuery<DBProduct>::const_iterator tempIter = currentProductsQuery->begin();
					if (tempIter == currentProductsQuery->end())
						Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - products query is empty for categoryNum:") + HobString(Long::toString(categoryNum)));
					else
						Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - products query is NOT empty for categoryNum:") + HobString(Long::toString(categoryNum)));
				}
*/

if (isTiming)
{
	clock_gettime(CLOCK_REALTIME, &ttp2);

	HobString tp1s(ULong::toString(ttp1.tv_sec));			// seconds
	HobString tp1n(Long::toString(ttp1.tv_nsec));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("initial determine products query() - start time (sec, msec): ") + tp1s + HobString(" ") + tp1n);

	HobString tp2s(ULong::toString(ttp2.tv_sec));			// seconds
	HobString tp2n(Long::toString(ttp2.tv_nsec));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("initial determine products query() -   end time (sec, msec): ") + tp2s + HobString(" ") + tp2n);

	long ttp1m = ttp1.tv_nsec / 1000000;
	long ttp2m = ttp2.tv_nsec / 1000000;
	time_t ttp2sec = ttp2.tv_sec;
	if (ttp2m < ttp1m)
	{
		ttp2sec--;
		ttp2m+=1000;
	}
	time_t elapsedSec = ttp2sec - ttp1.tv_sec;
	long elapsedMilli = ttp2m - ttp1m;
	HobString els(ULong::toString(elapsedSec));			// seconds
	HobString eln(Long::toString(elapsedMilli));			// nanoseconds (0 - 1,000,000,000)
	Debug::trace("Session.MessageTiming", HobString("initial determine products query() - elapsed time (sec, msec): ") + els + HobString(" ") + eln);
}

if (isTiming)
{
	t3.setToCurrent();
	HobString t3s = t3.formatTime(FormatTime_HHMMSS);

	HobString diff2 = HobString(Long::toString(t3.timeDiff(t2)));
	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - elapsed time for products query (sec): ") + diff2);

	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - after products query - time: ") + t3s);
}

				currentPresentationMethodDataType = PresentationMethod::Type_Category;

				delete dbCategory;
				break;
			}

			/*******************************************************************************************
			 *  Get the products marked as 'specials'.
			 ******************************************************************************************/
			case PresentationMethod::Type_Specials:
			{
				if (!dbProductsRM)
					return;

				HobRecordNum specialsNum = rskp->getRecordNum();
				if (specialsNum > 0)
				{
					DBSpecialProgramsRM *dbSpecialProgramsRM = dbAccess->getDBSpecialsRM();
					if (!dbSpecialProgramsRM)
						return;

if (isTiming)
{
	t1.setToCurrent();
	HobString t1s = t1.formatTime(FormatTime_HHMMSS);
	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - before dbSpecialProgramsRM->findNumber() - time: ") + t1s);
}
					DBSpecialProgram *dbSpecialProgram = dbSpecialProgramsRM->findNumber(Long::toString(specialsNum));
					if (!dbSpecialProgram)
					{
						Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - using the retrieveSpecials query for specials - COULDN'T FIND SPECIAL TYPE - type:") + HobString(Integer::toString(specialsNum)));
						return;
					}
if (isTiming)
{
	t2.setToCurrent();
	HobString t2s = t2.formatTime(FormatTime_HHMMSS);

	HobString diff1 = HobString(Long::toString(t2.timeDiff(t1)));
	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - elapsed time for reading the special (sec): ") + diff1);
}

					Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - using the retrieveSpecials query for specials - type:") + HobString(Integer::toString(specialsNum)));
if (isTiming)
{
	t1.setToCurrent();
	HobString t1s = t1.formatTime(FormatTime_HHMMSS);
	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - before dbProductsRM->retrieveSpecials() - time: ") + t1s);
}
					currentProductsQuery = dbProductsRM->retrieveSpecials(dbSpecialProgram);
if (isTiming)
{
	t2.setToCurrent();
	HobString t2s = t2.formatTime(FormatTime_HHMMSS);

	HobString diff1 = HobString(Long::toString(t2.timeDiff(t1)));
	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - elapsed time for retrieveSpecials (sec): ") + diff1);
}
					delete dbSpecialProgram;
				}
				else
				{
					Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - using the retrieveAllSpecials query for specials"));
if (isTiming)
{
	t1.setToCurrent();
	HobString t1s = t1.formatTime(FormatTime_HHMMSS);
	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - before dbProductsRM->retrieveAllSpecials() - time: ") + t1s);
}
					currentProductsQuery = dbProductsRM->retrieveAllSpecials();
if (isTiming)
{
	t2.setToCurrent();
	HobString t2s = t2.formatTime(FormatTime_HHMMSS);

	HobString diff1 = HobString(Long::toString(t2.timeDiff(t1)));
	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - elapsed time for retrieveAllSpecials (sec): ") + diff1);
}
				}

				if (!currentProductsQuery)
				{
					Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - couldn't get the products query for specials"));
					return;
				}

				currentPresentationMethodDataType = PresentationMethod::Type_Specials;
				break;
			}

			default:
				return;
				break;
		}

		if (currentGridData)
		{
			delete currentGridData;
			currentGridData = (SoftKeyLists *)NULL;
		}
		currentGridData = new SoftKeyLists();


if (isTiming)
{
	t4.setToCurrent();
	HobString t4s = t4.formatDateTime();

	HobString diff4 = HobString(Long::toString(t4.timeDiff(t3)));
	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - elapsed time from end of products query to before adding data to softKeyLists (sec): ") + diff4);
}


		addDataToSoftKeyList(currentGridData, currentProductsQuery, currentCategoriesQuery);


if (isTiming)
{
	t5.setToCurrent();
	HobString diff5 = HobString(Long::toString(t5.timeDiff(t4)));
	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - elapsed time for adding products and categories to softKeyLists (sec): ") + diff5);
}


		currentGridData->insertNextandPreviousCells(mySession->getCurrentLanguageSelection());
		currentGridData->gotoFirstPage();
	}

	// Send the softkey lists to the UI (Browser)
	SessionMessage sklMsg(ProductDev, ProductWriteSoftKeyList);
	sklMsg.setFormatType(BrowserFormat);
	sklMsg.setHfcObject(currentGridData);


if (isTiming)
{
	t6.setToCurrent();
	HobString diff6 = HobString(Long::toString(t6.timeDiff(t5)));
	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - elapsed time for adding previous and next cells to grid, and getting ready to send to the browser (sec): ") + diff6);
}


	mySession->send(&sklMsg, Messenger::DEST_UI);


if (isTiming)
{
	t7.setToCurrent();
	HobString diff7 = HobString(Long::toString(t7.timeDiff(t6)));
	Debug::trace("ProductManager", HobString("handleReadSoftKeyList() - elapsed time for sending the grid to the browser (sec): ") + diff7);
}


	if (currentProductsQuery)
		delete currentProductsQuery;

	if (currentCategoriesQuery)
		delete currentCategoriesQuery;
}
#endif

void ProductManager::handleProductReadAllProductGraphics()
{
	if (!mySession)
		return;

	GraphicFileNames *gfn = new GraphicFileNames(Graphic::TYPE_PRODUCTGRAPHIC);
	if (gfn)
	{
#if 0
		if (gfn->getListCount() > 0)
		{
#endif
			// Send the Graphic file names found back to the Browser.
			SessionMessage nameData(ProductDev, ProductWriteAllSoftKeyGraphicNames);
			nameData.setFormatType(BrowserFormat);
			nameData.setHfcObject(gfn);

			mySession->send(&nameData, Messenger::DEST_UI);
#if 0
		}
		else
		{
			//	Send a message indicating no files were found in this directory.
		}
#endif
		delete gfn;
	}
}



/**************************************************************************************************
 **************************************************************************************************
 ***																							***
 ***						 Product Self-Serve Statistics									  ***
 ***																							***
 **************************************************************************************************
 **************************************************************************************************/

// If the first record was not read, this method will return NULL.
ProductStatistic *ProductManager::firstProductStatistic()
{
	if (!dbAccess)
		return (ProductStatistic *)NULL;

	DBProductStatisticsRM *dbProductStatisticsRM = dbAccess->getDBProductStatisticsRM();
	if (!dbProductStatisticsRM)
		return (ProductStatistic *)NULL;

	// If the user calls firstProductStatistic() after they have already called it,
	// then delete the old query and create a new one.
	destroyProductStatisticsQueryAndIter();

	allProductStatisticsQuery = dbProductStatisticsRM->retrieveAll();

	// Couldn't get the query from which to access the ProductStatistics, just return false.
	if (!allProductStatisticsQuery)
		return (ProductStatistic *)NULL;

	DBQuery<DBProductStatistic>::const_iterator tempIter = allProductStatisticsQuery->begin();
	if (tempIter == allProductStatisticsQuery->end())
		return (ProductStatistic *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextProductStatistic()
	 * method call below).
	 */
	allProductStatisticsIter = new DBQuery<DBProductStatistic>::const_iterator(tempIter);
	DBProductStatistic *ps = new DBProductStatistic(*(*allProductStatisticsIter));
	ProductStatistic *newProductStatistic = new ProductStatistic(ps, dbAccess);
	return newProductStatistic;
}


/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstProductStatistic() method, the first call to this method will
 * result in the first record being returned.
 */
ProductStatistic *ProductManager::nextProductStatistic()
{
	if (!dbAccess)
		return (ProductStatistic *)NULL;

	if (allProductStatisticsQuery && !allProductStatisticsQuery->isEnabled())
		destroyProductStatisticsQueryAndIter();

	if (!allProductStatisticsQuery || !allProductStatisticsIter)
		return firstProductStatistic();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allProductStatisticsIter);
	if (*allProductStatisticsIter == allProductStatisticsQuery->end())
	{
		// reclaim the memory from the query and the iterator used to traverse all ProductStatistics in the db.
		destroyProductStatisticsQueryAndIter();
		return (ProductStatistic *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBProductStatistic copy constructor in order for the Hfc ProductStatistic to
	 * control the destruction of the DBProductStatistic record that the iterator contains; otherwise, the DBProductStatistic will
	 * be deleted when the iterator goes out of scope.  The DBProductStatistic copy will be deleted by the ProductStatistic record
	 * created below.
	 */
	DBProductStatistic *ps = new DBProductStatistic(*(*allProductStatisticsIter));
	ProductStatistic *newProductStatistic = new ProductStatistic(ps, dbAccess);
	return newProductStatistic;
}


// Provide a countAll method so the caller doesn't need to know about a dbProductStatisticsRM.
int ProductManager::countAllProductStatistics()
{
	if (!dbAccess)
		return -1;

	DBProductStatisticsRM *dbProductStatisticsRM = dbAccess->getDBProductStatisticsRM();
	if (!dbProductStatisticsRM)
		return -1;

	return dbProductStatisticsRM->countAll();
}


void ProductManager::handleDeleteAllProductStatistic()
{
	if (!dbAccess)
		return;

	DBProductStatisticsRM *dbProductStatisticsRM = dbAccess->getDBProductStatisticsRM();
	if (!dbProductStatisticsRM)
		return;

	dbProductStatisticsRM->deleteAll();

	destroyProductStatisticsQueryAndIter();
}

/* This method is only used when restoring product statistic records into an empty
 * database after a firmware upgrade that indicates a new version of the database
 * is being installed.  There shouldn't be any product statistic records in the database
 * with which to update the data being restored.  Thus, we don't need to look for an
 * existing record as is done for other datatypes.  Just go ahead and save the product statistic
 * record that is received.
 */
void ProductManager::handleRestoreProductStatistic(ProductStatistic *restoreStatistic)
{
	if (!restoreStatistic || !dbAccess)
		return;

	Debug::trace("ProductManager", HobString("handleProductStatisticRestore - Product number:") + HobString(Long::toString(restoreStatistic->getProductNum())) +
												HobString("  language:") + restoreStatistic->getLanguage() +
												HobString("  hour:") + HobString(Integer::toString(restoreStatistic->getHourNum())));
	restoreStatistic->setDBAccess(dbAccess);
	restoreStatistic->saveRecord();
}


/**************************************************************************************************
 **************************************************************************************************
 ***																							***
 ***						 other															  ***
 ***																							***
 **************************************************************************************************
 **************************************************************************************************/

void ProductManager::assignCoolListToProduct(DBCoolList *dbCoolList)
{
	if (!currentProduct || !dbCoolList)
		return;

	//	This will write the dbCoolList to the dbProduct rec and will save the dbProduct rec to the db.
	currentProduct->saveCoolList(dbCoolList);
	handleSaveRecord(currentProduct);
}

void ProductManager::destroyQueriesAndIters()
{
	destroyProductsQueryAndIter();
	destroyProductStatisticsQueryAndIter();
}

void ProductManager::destroyProductsQueryAndIter()
{
	if (allProductsIter)
	{
		delete allProductsIter;
		allProductsIter = (DBQuery<DBProduct>::const_iterator *)NULL;
	}

	if (allProductsQuery)
	{
		delete allProductsQuery;
		allProductsQuery = (DBQuery<DBProduct> *)NULL;
	}
}

void ProductManager::destroyProductStatisticsQueryAndIter()
{
	if (allProductStatisticsIter)
	{
		delete allProductStatisticsIter;
		allProductStatisticsIter = (DBQuery<DBProductStatistic>::const_iterator *)NULL;
	}

	if (allProductStatisticsQuery)
	{
		delete allProductStatisticsQuery;
		allProductStatisticsQuery = (DBQuery<DBProductStatistic> *)NULL;
	}
}


void ProductManager::purgeOldProductStatistics(int daysToKeepStatistics)
{
	// Check for product statistics that have been around longer than the configured number of days to keep them around.

	MessageLogger::logMessage(MessageLogger::ScheduledEventsLog, "Time to purge old Product statistics.");

	if (daysToKeepStatistics <= 0 || !dbAccess)
		return;

	DBProductStatisticsRM *dbProductStatisticsRM = dbAccess->getDBProductStatisticsRM();
	if (!dbProductStatisticsRM)
		return;

	bool isTracing = Debug::isTracing("PurgeOldRecords");
	util_time::Time t0 = util_time::Time();


	util_time::DateTime cutOffDate;
	cutOffDate -= daysToKeepStatistics;
	util_time::DateTime cutOffDateTime(cutOffDate), statDT;

	if (isTracing)
		Debug::trace("PurgeOldRecords", t0.operator std::string() + " - begin of purgeOldProductStatistics() older than " + inttostring(daysToKeepStatistics) + " days - cut-off date is " + cutOffDateTime.operator std::string());

	DBQuery<DBProductStatistic>* allProdStatsQuery = dbProductStatisticsRM->retrieveDateTimeOrder();
	if (allProdStatsQuery)
	{
		DBProductStatistic *dbProductStatistic = 0;

		bool invalidDate = false;
		bool loop = true;
		DBQuery<DBProductStatistic>::const_iterator tempIter = allProdStatsQuery->begin();

		while (( loop ) && ( tempIter != allProdStatsQuery->end() ))
		{
			dbProductStatistic = new DBProductStatistic(*tempIter);
			if (!dbProductStatistic)
				loop = false;
			else
			{
				if (dbProductStatistic->isRecordedDateTimeNull())
				{
					if (isTracing)
						Debug::trace("PurgeOldRecords", "recordedDateTime is null, deleting dbProductStatistic record  plu#:" + dbProductStatistic->getKey3());

					dbProductStatisticsRM->deleteRecord(dbProductStatistic);   //delete it;
					++tempIter;
				}
				else
				{
					// Since I partially based this method from Santos' equivalent method in TransactionManager, I'm bringing along this part:

					// I've seen a couple of times where the transaction stored in the db has empty date as in
					// '  /  /    ,14:21:41'. This becomes an invalid date as we process it and it causes an
					// exception to be thrown. Until we find the problem the following ugly code will be here.
					invalidDate = false;
					try { statDT = util_string::stringdbtodatetime(dbProductStatistic->getRecordedDateTime()); }
					catch ( const util_time::Date::InvalidDate& )
					{
						if (isTracing)
							Debug::trace("PurgeOldRecords", "purgeOldProductStatistics() Got an invalid date exception");

						// Delete this record
						invalidDate = true;
					}

					if ( invalidDate || statDT < cutOffDateTime )
					{
						if (isTracing)
							Debug::trace("PurgeOldRecords", "expiration time is past limit, deleting dbProductStatistic record  plu#:" + dbProductStatistic->getKey3() + " statDate:" + dbProductStatistic->getRecordedDateTime());

						dbProductStatisticsRM->deleteRecord(dbProductStatistic);   //delete it;
						++tempIter;
					}
					else
						// There won't be anymore records to delete once we decide to keep the current record, since they are retrieved in date/time order.
						loop = false;
				}

				delete dbProductStatistic;
				dbProductStatistic = 0;
			}
		}

		delete allProdStatsQuery;
		allProdStatsQuery = 0;
	}

	if (isTracing)
	{
		util_time::Time t1 = util_time::Time();
		Debug::trace("PurgeOldRecords", " - end of purgeOldProductStatistics() - elapsed time: " + util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
	}
	MessageLogger::logMessage(MessageLogger::ScheduledEventsLog, "Finished purging old Product statistics.");
}


void ProductManager::handleApplyPriceDiscount(Product *msgProduct, int UNUSED(event))
{
	Session *s = dynamic_cast<Session *> (mySession);
	if (!s || !s->isInOperateMode() || !currentProduct)
		return;

	bool traceOn = Debug::isTracing("ProductManager");
	HobString msgDebug("handleApplyPriceDiscount():  ");
	HobString msgDebug2;
	HobPrice origPrice = currentProduct->getUnitPrice();
	HobPrice newPrice = 0;
	HobPrice savings = 0;
	short discType = msgProduct->getDiscountType();
	long discountValue = msgProduct->getDiscountValue1();
	switch (discType)
	{
		case Cents_off_unit_price:
			newPrice = origPrice - discountValue;
			savings = discountValue;
			msgDebug2 = "Cents_off";
			break;
		case Percent_off_net_value:
		{
			if (!currentProduct->isRandomWeight())
				return;
			msgDebug2 = "Percent_off";
			HobValue tempVal = origPrice * discountValue;
			savings = Rounding::roundPriceWeight(tempVal, Configuration::getInstance()->getPricePrecision(), Configuration::getInstance()->getPercentagePrecision() + 2, Configuration::getInstance()->getPricePrecision(), (short)Configuration::getInstance()->toIntCfg(fieldOffsetCfgFSRoundingMethod));
			break;
		}
	}
	newPrice = origPrice - savings;
	if (newPrice < 0)
	{
		newPrice = 0;
		if (traceOn)
		{
			HobString msgDebug3 = msgDebug;
			msgDebug3.append("Setting newPrice to 0 as we cannot have a negative price!");
			Debug::trace("ProductManager", msgDebug3);
		}
	}
	currentProduct->setUnitPrice(newPrice);

	if (traceOn)
	{
		msgDebug2.append("\n origPrice = ").append(Long::toString(origPrice));
		msgDebug2.append("\n   savings = ").append(Long::toString(savings));
		msgDebug2.append("\n  newPrice = ").append(Long::toString(newPrice));
		Debug::trace("ProductManager", msgDebug + msgDebug2);
	}
//	HobString newPriceStr;
//	 if (newPrice == 0)
//	 	newPriceStr = " ";
//	else
//		newPriceStr = Configuration::getInstance()->formatHobPrice(newPrice);
	WriteSingleField *productUpdate = new WriteSingleField(1, Product::fieldOffsetUnitPrice,
			newPrice ? Configuration::getInstance()->formatHobPrice(newPrice) : " ");

	// Send the FIELD-UPDATED-SUCCESSFULLY message back to the browser.
	SessionMessage fieldUpdatedMsg(ProductDev, ProductWriteSingleField);
	fieldUpdatedMsg.setFormatType(BrowserFormat);
	fieldUpdatedMsg.setHfcObject(productUpdate);
	mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);

	updateWeightAndNotify(0, true);

/*
	if (mySession->isPrimarySession() && mySession->isInOperateMode())
	{
		if (Debug::isTracing("ProductManager"))
		{
			HobString msgDebug3 = msgDebug;
			msgDebug3.append("Sending message to re-preimage. mode= ").append(Integer::toString(mySession->getCurrentMode()));
			Debug::trace("ProductManager", msgDebug3);
		}

		// Send a Product Change message to the RTC so it knows the Imager needs to be updated.
		SessionMessage pluChg(ProductDev, ProductChange);
		pluChg.setFormatType(InternalFormat);
		HfcProductChangeMsg hpcm(mySession, currentProduct);
		pluChg.setHfcObject(&hpcm);
		mySession->send(&pluChg, Messenger::DEST_RTC);
	}
*/
	ServiceSpeaker::badBeep(1);
	//	todo:	What impact will this have on Frequent Shopper?
	//			Also, how will we handle this for a Package discount? Will we handle a Package discount?
}


void ProductManager::handlePrintMultipleLabels()
{
	Session *s = dynamic_cast<Session *> (mySession);
	if (!s || !s->isInOperateMode())
		return;

	if ( s->getCurrentMode() == OpModeProductionEntry || s->isInAnAccumulatingMode() )
	{
		mySession->addDialogMessage(ErrProductDev, ProductErrCannotPrintMultLabelInProdEntry, HfcResponse::RESPONSESTYLE_OK);
		return;
	}

	if (!currentProduct)
	{
		mySession->addDialogMessage(ErrProductDev, ProductErrNoProductForPrintMultLabel, HfcResponse::RESPONSESTYLE_OK);
		return;
	}
	else if (currentProduct->isRandomWeight() || currentProduct->isByCountWithWeight())
	{
		mySession->addDialogMessage(ErrProductDev, ProductErrCannotPrintMultLabelsForRandomWeight, HfcResponse::RESPONSESTYLE_OK);
		return;
	}
	else if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgRequireFixedWeightVerification) && currentProduct->isFixedWeight() )
	{
		// In Print Multiple Labels mode, if fixed weight verification is required don't allow looking up a fixed weight item.
		mySession->addDialogMessage(ErrProductDev, ProductErrFixedWeightVerificationNoPrintMultiple, HfcResponse::RESPONSESTYLE_OK);
		return;
	}
	else if (currentProduct->getProductionPlanningType() == Product::PRODUCTIONPLANNING_INGREDIENT)
	{
		mySession->addDialogMessage(ErrProductDev, ProductErrCannotPrintMultLabelsForFPPType2, HfcResponse::RESPONSESTYLE_OK);
		return;
	}
	else if ( Configuration::getInstance()->isPrepackScale() && !Configuration::getInstance()->toBoolCfgI(fieldOffsetCfgIAllowProductionEntryModeOnWrapper) )
	{
		mySession->addDialogMessage(ErrSessionDev, SessionDevProductionEntryNotEnabled, HfcResponse::RESPONSESTYLE_OK);
		return;
	}

	if (s->getCurrentMode() == OpModePrintMultipleLabels)
	{
		//Debug::trace("ProductManager", "CALL handleLeavePrintMultipleLabelsMode() via handlePrintMultipleLabels()");
		handleLeavePrintMultipleLabelsMode();
	}

	// Save the current operating mode so we can return to it when we're done with Print Multiple Labels mode.
	opModePriorToPrintMultipleLabelsMode = s->getCurrentMode();

	// Tell the Session to change to Print Multiple Labels mode.
	SessionMessage msg(OperationMode, OpModePrintMultipleLabels);
	msg.setFormatType(BrowserFormat);
	s->handleMessage(&msg);

	//	Send the Weigh Mode / Total Type message back to the Browser, so it can update the screen for Print Multiple Labels mode.
	SessionMessage updateWeighModeMsg(SessionDev, SnReadWeighModeTransactionType);
	s->handleMessage(&updateWeighModeMsg);

	// Set the number of labels to print to 0 and make it a forced field.
	// When we send the ProductWrite message below, the browser will prompt the user for the number of labels to print.
	currentProduct->updateField(Product::fieldOffsetNumOfLabels, HobString("0"));

	currentProduct->setNumOfLabelsEntered(false);
	currentProduct->setNumOfLabelsForced(true);

	SessionMessage pluData(ProductDev, ProductWrite);
	currentProduct->setFormatType(BrowserFormatPLUOperateMode);
	pluData.setFormatType(BrowserFormat);
	pluData.setHfcObject(currentProduct);
	mySession->send(&pluData, Messenger::DEST_UI);
}

void ProductManager::handleLeavePrintMultipleLabelsMode()
{
	//Debug::trace("ProductManager", "START handleLeavePrintMultipleLabelsMode()" );
	
	Session *s = dynamic_cast<Session *> (mySession);

	if (!s || s->getCurrentMode() != OpModePrintMultipleLabels)
		return;

	if ( opModePriorToPrintMultipleLabelsMode < OpModeIdle || opModePriorToPrintMultipleLabelsMode >= OpModeMax )
		opModePriorToPrintMultipleLabelsMode = OpModeManualWeigh;

	Debug::trace("ProductManager", "handleLeavePrintMultipleLabelsMode() going back to opModePriorToPrintMultipleLabelsMode: " + HobString(Integer::toString(opModePriorToPrintMultipleLabelsMode)));

	// go back to previous operating mode.
	SessionMessage msg(OperationMode, opModePriorToPrintMultipleLabelsMode);
	//msg.setFormatType(BrowserFormat);
	//s->handleMessage(&msg);
	msg.setFormatType(InternalFormat);
	mySession->dispatchEvent(&msg, mySession);

	//	Send the Weigh Mode / Total Type message back to the Browser, so it can update the screen.
	SessionMessage updateWeighModeMsg(SessionDev, SnReadWeighModeTransactionType);
	updateWeighModeMsg.setFormatType(InternalFormat);
	mySession->dispatchEvent(&updateWeighModeMsg, mySession);

	//Debug::trace("ProductManager", "FINISH handleLeavePrintMultipleLabelsMode()" );
}

void ProductManager::handleResetPrice()
{
	Session *s = dynamic_cast<Session *> (mySession);
	if (!s || !s->isInOperateMode() || !currentProduct)
		return;

	Product *lookedUpProduct = Product::findRecord(currentProduct->getProductNum(), currentProduct->getDeptNum(), dbAccess);

	if (!lookedUpProduct)
		return;

	currentProduct->setUnitPrice(lookedUpProduct->getUnitPrice());
//	HobString newPriceStr;
//	 if (currentProduct->getUnitPrice() == 0)
//	 	newPriceStr = " ";
//	else
//		newPriceStr = Configuration::getInstance()->formatHobPrice(currentProduct->getUnitPrice());
	WriteSingleField *productUpdate = new WriteSingleField(1, Product::fieldOffsetUnitPrice,
			currentProduct->getUnitPrice() ? Configuration::getInstance()->formatHobPrice(currentProduct->getUnitPrice()) : " ");

	// Send the FIELD-UPDATED-SUCCESSFULLY message back to the browser.
	SessionMessage fieldUpdatedMsg(ProductDev, ProductWriteSingleField);
	fieldUpdatedMsg.setFormatType(BrowserFormat);
	fieldUpdatedMsg.setHfcObject(productUpdate);
	mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
/*
	if (mySession->isPrimarySession() && mySession->isInOperateMode())
	{
		if (Debug::isTracing("ProductManager"))
		{
			HobString myDebug2 = msgDebug;
			myDebug2.append("Sending message to re-preimage. mode= ").append(Integer::toString(mySession->getCurrentMode()));
			Debug::trace("ProductManager", myDebug2);
		}

		// Send a Product Change message to the RTC so it knows the Imager needs to be updated.
		SessionMessage pluChg(ProductDev, ProductChange);
		pluChg.setFormatType(InternalFormat);
		HfcProductChangeMsg hpcm(mySession, currentProduct);
		pluChg.setHfcObject(&hpcm);
		mySession->send(&pluChg, Messenger::DEST_RTC);
	}
*/
	ServiceSpeaker::badBeep(1);
	//	todo:	What impact will this have on Frequent Shopper?

	delete lookedUpProduct;
}


void ProductManager::saveCOOLInfoToProduct(short fieldCode)
{
	Session *s = dynamic_cast<Session *> (mySession);

	//	This should only be called from handleProductWriteSingleFieldMessage()

	bool traceOn = Debug::isTracing("ProductManager.saveCOOL");
	HobString myDebug = "";
	if (traceOn)
	{
		myDebug = "saveCOOLInfoToProduct():   ";
		Debug::trace("ProductManager.saveCOOL", myDebug);
	}

	if (!currentProduct || !dbAccess || !s->isInOperateMode())
	{
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("EXITING !!! currentProduct = ").append(ptrtostring(currentProduct));
			myDebug2.append("   dbAccess = ").append(ptrtostring(dbAccess));
			myDebug2.append("   isInOperateMode = ").append(Bool::toString(s->isInOperateMode()));
			Debug::trace("ProductManager.saveCOOL", myDebug2);
		}
		return;
	}

	if ((fieldCode != Product::fieldOffsetProdCoolTextNum) && (fieldCode != Product::fieldOffsetProdCOOLTrackingText) &&
		(fieldCode != Product::fieldOffsetProdCoolRequired))
	{
		if (traceOn)
			Debug::trace("ProductManager.saveCOOL", myDebug + "EXITING !!! NOT CoolTextNum, COOLTrackingText or CoolRequired fields.");
		return;
	}

	//	This method will get the updated field from the product, based on the fieldCode.
	//	This field will be saved in the dbProduct record of the current product and saved to the database.
	//	Obviously, the product must exist in the database.
	//	This happens in Operate mode for COOL only.
	//	This could happen in Supervisor mode - but we will not allow it.
	//	Typically we don't save product changes in Operate mode - the only exception is COOL data.


	Product *foundProductAgain = Product::findRecord(currentProduct->getProductNum(), currentProduct->getDeptNum(), dbAccess);
	if (!foundProductAgain || foundProductAgain->getErrorCode() != RECORD_OK)
	{
		if (traceOn)
			Debug::trace("ProductManager.saveCOOL", myDebug + "EXITING !!! Couldn't refind the product.");
		return;
	}

	bool saveField = true;

	switch (fieldCode)
	{
		case Product::fieldOffsetProdCoolTextNum:
		{
			if (currentProduct->getCoolTextNum() == RECORD_NOT_FOUND)
				foundProductAgain->setCoolTextNum(RECORD_NOT_FOUND);		//	No COOL Text number
			else
			{
				foundProductAgain->setCoolTextNum(currentProduct->getCoolTextNum());
				//	Since they have selected COOL text, make sure the coolRequired flag is set.
				currentProduct->setCoolRequired(true);
				foundProductAgain->setCoolRequired(currentProduct->isCoolRequired());
			}
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("fieldOffsetProdCoolTextNum:  coolTextNum = ").append(Long::toString(currentProduct->getCoolTextNum()));
				myDebug2.append("   coolRequired = ").append(Bool::toString(currentProduct->isCoolRequired()));
				Debug::trace("ProductManager.saveCOOL", myDebug2);
			}
			break;
		}

		case Product::fieldOffsetProdCOOLTrackingText:
		{
			foundProductAgain->setCoolTrackingText(currentProduct->getCoolTrackingText());
			
			// PTR #248
			// Since they have entered a COOL tracking number, make sure the coolRequired flag is set.
			currentProduct->setCoolRequired(true);
			foundProductAgain->setCoolRequired(currentProduct->isCoolRequired());
			
			if (traceOn)
				Debug::trace("ProductManager.saveCOOL", myDebug + "fieldOffsetProdCOOLTrackingText.");
			break;
		}

		case Product::fieldOffsetProdCoolRequired:
		{
			// Whether COOL is required is not something the operator can change permanently in the db.
//			foundProductAgain->setCoolRequired(currentProduct->isCoolRequired());
			if (!currentProduct->isCoolRequired())
			{
				currentProduct->setCoolTextNum(RECORD_NOT_FOUND);		//	No COOL Text number if COOL is not required.
// Fix PTR#56 (haberrc)				foundProductAgain->setCoolTextNum(RECORD_NOT_FOUND);		//	No COOL Text number if COOL is not required.
			}
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("fieldOffsetProdCoolRequired:  coolRequired = ").append(Bool::toString(currentProduct->isCoolRequired()));
				myDebug2.append("   coolTextNum = ").append(Long::toString(currentProduct->getCoolTextNum()));
				Debug::trace("ProductManager.saveCOOL", myDebug2);
			}
			break;
		}

		default:
			saveField = false;
			break;
	}

	if (saveField)
	{
		if (traceOn)
			Debug::trace("ProductManager.saveCOOL", myDebug + "Saving the COOL data to the product.");

		handleSaveRecord(foundProductAgain);
	}

	delete foundProductAgain;
}

void ProductManager::applyTransactionTypeToUpcNumber()
{
	Session *s = dynamic_cast<Session *> (mySession);

	if (!s || !currentProduct)
		return;

	bool traceOn = Debug::isTracing("ProductManager");
	HobString myDebug("applyTransactionTypeToUpcNumber():   ");

	Util::CSmartPtr<hfc::TransactionType> currentTransactionType = mgrContainer_->getTransactionTypeManager()->getOperatorModeTransactionType();
	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("currentTransactionType = ").append(ptrtostring(currentTransactionType.get()));
		Debug::trace("ProductManager", myDebug2);
	}
	if ( currentTransactionType.notNull() )
	{
		//	Test for Automatic change of the UPC digit.
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("currentTransactionType->isAutomaticChangeUPCDigit() = ").append(Bool::toString(currentTransactionType->isAutomaticChangeUPCDigit()));
			Debug::trace("ProductManager", myDebug2);
		}
		if ( currentTransactionType->isAutomaticChangeUPCDigit() )
		{
			Debug::trace("ProductManager", "Automatically changing the most significant UPC digit!");
			currentProduct->getBarCodeNum().replaceMostSignificantUPCDigit(currentTransactionType->getUPCChangeDigit());
		}
	}
}


bool ProductManager::validateProductForPlatterTare()
{
	Session *s = dynamic_cast<Session *> (mySession);
	if (s && s->isInOperateMode() && currentProduct && currentProduct->isRandomWeight())
		return true;
	else
		return false;
}

void ProductManager::handlePlatterTare(HfcWeigherStatus *weigherStatus)
{
	// There is no need to process the weight if we're not in Operate mode.
	Session *s = dynamic_cast<Session *> (mySession);
	if (!s || !s->isInOperateMode())
		return;

	bool traceOn = Debug::isTracing("ProductManager.PlatterTare");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "handlePlatterTare():  ";

	if (!weigherStatus)
	{
		if (traceOn)
			Debug::trace("ProductManager.PlatterTare", msgDebug + "HfcWeigherStatus is NULL - it will be ignored.");
		return;
	}

	if (!currentProductCanAcceptWeight)
	{
		if (traceOn)
			Debug::trace("ProductManager.PlatterTare", msgDebug + "current product is not accepting weight");

		if (weigherStatus && weigherStatus->isPeripheralComm() && weigherStatus->isZeroStableWeight())
			currentProductCanAcceptWeight = true;
		else
			return;
	}

	if ( !currentProduct )
	{
		if (traceOn)
			Debug::trace("ProductManager.PlatterTare", msgDebug + "there is no current product to update with the new platter tare!!");
	}
	else if (( currentProduct->isRandomWeight() ) || ( currentProduct->isByCountWithWeight() ))
	{
		if (( weigherStatus ) && ( !weigherStatus->isWeigherInError() ))
		{
			if ( weigherStatus->isStableWeight() ) //|| ( weigherStatus->isSmallMotion() ))
			{
				if (traceOn)
					Debug::trace("ProductManager.PlatterTare", msgDebug + "updating current product with new platter tare");

				bool tareUpdated = true;

				//	We are going to send the new platter tare value back to the browser as a WriteSingleField message.
				HobString tareStr;
				int fieldCode = (Configuration::getInstance()->toBoolCfg( fieldOffsetCfgUseSecondTare )) ? Product::fieldOffsetTareBValue : Product::fieldOffsetTareAValue;

				HobWeight newTare = weigherStatus->getWeight();
				if (newTare > Configuration::getInstance()->getMaxTare())
				{
					tareUpdated = false;
					if (mySession)
						mySession->addDialogMessage(ErrProductDev, ProductErrTareGreaterThanMaxTare, HfcResponse::RESPONSESTYLE_OK);

					currentProduct->getFieldAsString(fieldCode, tareStr, BrowserFormat);
				}
				else
				{
					(Configuration::getInstance()->toBoolCfg( fieldOffsetCfgUseSecondTare )) ? currentProduct->setTareB(newTare) : currentProduct->setTareA(newTare);

					currentProduct->getFieldAsString(fieldCode, tareStr, BrowserFormat);

					//	Record the operator change first.
					ProductTransaction *currentTransaction = currentProduct->getCurrentTransaction();
					if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgTrackingOperatorChanges) && currentTransaction)
						currentTransaction->addOperatorChange((Configuration::getInstance()->toBoolCfg( fieldOffsetCfgUseSecondTare )) ? Product::fieldOffsetTareBValue : Product::fieldOffsetTareAValue, "", tareStr);
				}

				// The record number (plu number), in the WriteSingleField message sent back to the browser,
				// is being used to indicate whether the change was successful(1) or not(0).
				WriteSingleField newTareField(((tareUpdated) ? 1 : 0), fieldCode, tareStr);

				// Send the message back to the browser.
				SessionMessage platterTareMsg(ProductDev, ProductWriteSingleField);
				platterTareMsg.setFormatType(BrowserFormat);
				platterTareMsg.setHfcObject(&newTareField);
				mySession->send(&platterTareMsg, Messenger::DEST_UI);
			}

			// FT-1387: do not allow platter tare when motion
			else
			{
		        SessionMessage ptMsg(SessionDev, SnPlatterTareUnstable);
		        ptMsg.setFormatType(BrowserFormat);
		        mySession->send(&ptMsg, Messenger::DEST_UI);
			}
		}
	}
}

// FT-809
bool ProductManager::isWaitingForLoginBeforeAddToAccum() const
{
	return waitingForLoginBeforeAddToAccum;
}
bool ProductManager::isWaitingForLoginBeforeAddToProductionEntry() const
{
	return waitingForLoginBeforeAddToProductionEntry;
}
bool ProductManager::isWaitingForLoginBeforePrinting() const
{
	return waitingForLoginBeforePrinting;
}
bool ProductManager::isWaitingForLoginBeforeWriteSingleField() const
{
	return waitingForLoginBeforeWriteSingleField;
}
void ProductManager::cancelWaitingForLogin()
{
	waitingForLoginBeforeAddToAccum = false; // FT-809
	waitingForLoginBeforeAddToProductionEntry = false;
	if (productWaitingForProductionEntryLogin)
	{
		delete productWaitingForProductionEntryLogin;
		productWaitingForProductionEntryLogin = 0;
	}
	waitingForLoginBeforePrinting = false;
	waitingForLoginBeforeWriteSingleField = false;
	productUpdateWaitingForLogin = 0;
}

void ProductManager::switchModeForKeyboardNetWt()
{
	Session *s = dynamic_cast<Session *> (mySession);

	// HA-180:  We can't rely on processingKeyboardNetWt to determine if we can switch modes.
	//			We need to look at the current mode and compare it with the mode we want to switch to.
	OpModeEvent currentMode = s ? s->getCurrentMode() : OpModeIdle;
	OpModeEvent modeNeeded = Configuration::getInstance()->isServiceCounterScale() ? OpModeAutoManualApply : OpModeManualWeigh;
	Debug::trace("ProductManager.KeyboardNetWeight", "switchModeForKeyboardNetWt():  processingKeyboardNetWt = " + booltostring(processingKeyboardNetWt) + ",  currentMode = " + inttostring(currentMode) + ",  modeNeeded = " + inttostring(modeNeeded));

	if (currentMode != modeNeeded)
	{
		SessionMessage mode(SessionDev, SnProductSwitchToManualWeighMode);		//	HA-120
		if (modeNeeded == OpModeAutoManualApply)
		{
			// FT-503 correct mode is auto manual apply (pre-pack)
			Debug::trace("ProductManager.KeyboardNetWeight", "switchModeForKeyboardNetWt():  Sending message (SessionDev, SnProductSwitchToAutoManualApplyMode)");
			mode.setEventType(SnProductSwitchToAutoManualApplyMode);
		}
		else
		{
			Debug::trace("ProductManager.KeyboardNetWeight", "switchModeForKeyboardNetWt():  Sending message (SessionDev, SnProductSwitchToManualWeighMode)");
		}
		s->handleMessage(&mode);

		//	Request the new mode to be sent to the UI.
		SessionMessage newMode(SessionDev, SnReadWeighModeTransactionType);
		s->handleMessage(&newMode);
	}
	processingKeyboardNetWt = true;
}

void ProductManager::switchModeBackAfterKeyboardNetWt()
{
	Session *s = dynamic_cast<Session *> (mySession);
	Debug::trace("ProductManager.KeyboardNetWeight", "switchModeBackAfterKeyboardNetWt():  processingKeyboardNetWt = " + booltostring(processingKeyboardNetWt) + ",  currentMode = " + inttostring(s->getCurrentMode()));

	if (processingKeyboardNetWt && s)
	{
		Debug::trace("ProductManager.KeyboardNetWeight", "switchModeBackAfterKeyboardNetWt():  Sending message (OperationMode, SnProductReturnToPriorOpMode)");
		SessionMessage mode(SessionDev, SnProductReturnToPriorOpMode);
		s->handleMessage(&mode);

		//	Request the new mode to be sent to the UI.
		SessionMessage newMode(SessionDev, SnReadWeighModeTransactionType);
		s->handleMessage(&newMode);
	}
	processingKeyboardNetWt = false;
}


void ProductManager::adjustLabelRotations(Product *newProduct)
{
	if (!newProduct)
		return;

	Configuration *tmpConfig = Configuration::getInstance();
	if (!tmpConfig)
		return;

	RotationOverrideSettings rotOverrideSetting = ROSNone;
	short rotOverrideValue = 0;

	rotOverrideSetting = (RotationOverrideSettings)tmpConfig->toIntCfg(fieldOffsetCfgRotationOverrideSetting1);
	rotOverrideValue = (short)tmpConfig->toIntCfg(fieldOffsetCfgRotationOverrideValue1);
	Debug::trace("Product.LabelApplier", "adjustLabelRotations():  RotationOverrideSetting1 = " + inttostring(rotOverrideSetting) + ",  RotationOverrideValue1 = " + inttostring(rotOverrideValue));
	if (rotOverrideSetting == ROSSetRotationTo)
		newProduct->setLabelRotation(FirstPrinter, rotOverrideValue);
	else if (rotOverrideSetting == ROSAdjustRotationBy)
		newProduct->setLabelRotation(FirstPrinter, offsetLabelRotation(newProduct->getLabelRotation(FirstPrinter), rotOverrideValue));

	rotOverrideSetting = (RotationOverrideSettings)tmpConfig->toIntCfg(fieldOffsetCfgRotationOverrideSetting2);
	rotOverrideValue = (short)tmpConfig->toIntCfg(fieldOffsetCfgRotationOverrideValue2);
	Debug::trace("Product.LabelApplier", "adjustLabelRotations():  RotationOverrideSetting2 = " + inttostring(rotOverrideSetting) + ",  RotationOverrideValue2 = " + inttostring(rotOverrideValue));
	if (rotOverrideSetting == ROSSetRotationTo)
		newProduct->setLabelRotation(SecondPrinter, rotOverrideValue);
	else if (rotOverrideSetting == ROSAdjustRotationBy)
		newProduct->setLabelRotation(SecondPrinter, offsetLabelRotation(newProduct->getLabelRotation(SecondPrinter), rotOverrideValue));

	rotOverrideSetting = (RotationOverrideSettings)tmpConfig->toIntCfg(fieldOffsetCfgRotationOverrideSetting3);
	rotOverrideValue = (short)tmpConfig->toIntCfg(fieldOffsetCfgRotationOverrideValue3);
	Debug::trace("Product.LabelApplier", "adjustLabelRotations():  RotationOverrideSetting3 = " + inttostring(rotOverrideSetting) + ",  RotationOverrideValue3 = " + inttostring(rotOverrideValue));
	if (rotOverrideSetting == ROSSetRotationTo)
		newProduct->setLabelRotation(ThirdPrinter, rotOverrideValue);
	else if (rotOverrideSetting == ROSAdjustRotationBy)
		newProduct->setLabelRotation(ThirdPrinter, offsetLabelRotation(newProduct->getLabelRotation(ThirdPrinter), rotOverrideValue));
}

short ProductManager::offsetLabelRotation(short curRotation, short offsetRotation)
{
	Debug::trace("Product.LabelApplier", "offsetLabelRotation(" + inttostring(curRotation) + ", " + inttostring(offsetRotation) + ")");
	return (curRotation + offsetRotation) % 360;
}


void ProductManager::handleChangeRotationPlacement(GenericHfcObject *gho)
{
	Session *s = dynamic_cast<Session *> (mySession);
	if (!gho || !currentProduct || !s || !s->isInOperateMode())
		return;

	int codedValue = Integer::valueOf(gho->getData().c_str());

	if (codedValue < 0x1000000)
	{
		Debug::trace("ProductManager.Placement", "handleChangeLabelPlacementAndPosition() use legacy method:   codedValue = " + Integer::toString(codedValue) );


		int codedValue = Integer::valueOf(gho->getData().c_str());
	    int printer = codedValue / 27;
	    Debug::trace("ProductManager.Placement", "handleChangeLabelPlacementAndPosition():   codedValue = " + Integer::toString(codedValue) + ",   printer = " + Integer::toString(printer));
	    int fieldOffset = 0;

	    int placement = -1;
	    int temp = codedValue % 3;
	    if (temp == 1)
		    placement = 1;		//	LocLeft
	    else if (temp == 2)
		    placement = 0;		//	LocRight
		    
	    Debug::trace("ProductManager.Placement", "handleChangeLabelPlacementAndPosition():   placement = " + Integer::toString(placement));
	    
	    if (placement != -1)
	    {
		    switch (printer)
		    {
			    case 2:
				    fieldOffset = Product::fieldOffsetLabelPlacement3;
			    break;
			    case 1:
				    fieldOffset = Product::fieldOffsetLabelPlacement2;
			    break;
			    default:
				    fieldOffset = Product::fieldOffsetLabelPlacement1;
			    break;
		    }
		    
			WriteSingleField wsf(currentProduct->getProductNum(), fieldOffset, Integer::toString(placement));
			SessionMessage msg(ProductDev, ProductWriteSingleField);
			msg.setFormatType(BrowserFormat);
			msg.setHfcObject(&wsf);
			handleMessage(&msg);
			Debug::trace("ProductManager.Placement", "handleChangeLabelPlacementAndPosition():   Changed label placement.");
		}

	    int rotation = ((codedValue - 3) / 3) * 45;
	    Debug::trace("ProductManager.Placement", "handleChangeLabelPlacementAndPosition():   rotation = " + Integer::toString(rotation));

	    if (rotation >= 0)
	    {
			switch (printer)
			{
				case 2:
					fieldOffset = Product::fieldOffsetLabelRotation3;
				break;
				case 1:
					fieldOffset = Product::fieldOffsetLabelRotation2;
				break;
				default:
					fieldOffset = Product::fieldOffsetLabelRotation1;
				break;
			}
			
			WriteSingleField wsf(currentProduct->getProductNum(), fieldOffset, Integer::toString(rotation));
			SessionMessage msg(ProductDev, ProductWriteSingleField);
			msg.setFormatType(BrowserFormat);
			msg.setHfcObject(&wsf);
			handleMessage(&msg);
			Debug::trace("ProductManager.Placement", "handleChangeLabelPlacementAndPosition():   Changed label rotation.");
	
			//	Now we need to send some sort of message to the UI!
			s->addDialogMessage(ErrProductDev, ProductErrRotationPlacementChanged, HfcResponse::RESPONSESTYLE_OK);
	
			// Create a timer to clear the message.
		    TimerManager *timerManager = TimerManager::getInstance();
		    if (timerManager)
			    timerManager->addTimer(3, Timer::RepeatNO, timedFuncChangeRotationPlacement);
	    }
    }
	else
	{
		Debug::trace("ProductManager.Placement", "handleChangeLabelPlacementAndPosition() use NEW method:   codedValue = " + Integer::toString(codedValue));

		int p1Rotation = codedValue & 0xf;
		int p1Placement = (codedValue & 0xf0) >> 4;

		int p2Rotation = (codedValue & 0xf00) >> 8;
		int p2Placement = (codedValue & 0xf000) >> 12;

		// turns out to actually rotate the labels on the package, they dont want the enum they want the actual degrees of rotation
		int sentRotation1 = (p1Rotation - 1) * 45;
		int sentRotation2 = (p2Rotation - 1) * 45;

		// turns out to actually place the labels on the package, the actual location they want is the current location - 1.
		int sentPlacement1 = p1Placement;
		int sentPlacement2 = p2Placement;

		// future 3rd printer
		//int p3Rotation = (codedValue & 0xf0000) >> 16;
		//int p3Platement = (codedValue & 0xf00000) >> 20;

		//if (sentRotation1 > 0)		// 1 is 0 degrees
		if(p1Rotation > 0) // 0 is no change, 1 is 0 degrees for p1Rotation. The sentRotation sends the 0
		{
			WriteSingleField wsf1(currentProduct->getProductNum(), Product::fieldOffsetLabelRotation1, Integer::toString(sentRotation1));
			SessionMessage msg1(ProductDev, ProductWriteSingleField);
			msg1.setFormatType(BrowserFormat);
			msg1.setHfcObject(&wsf1);
			handleMessage(&msg1);
		}

		if (p1Placement != 2) // 2 is no change
		{
			WriteSingleField wsf2(currentProduct->getProductNum(), Product::fieldOffsetLabelPlacement1, Integer::toString(p1Placement));
			SessionMessage msg2(ProductDev, ProductWriteSingleField);
			msg2.setFormatType(BrowserFormat);
			msg2.setHfcObject(&wsf2);
			handleMessage(&msg2);
		}

		Debug::trace("ProductManager.Placement", "handleChangeLabelPlacementAndPosition():   Changed primary label rotations and placement.");
		
		if (p2Rotation > 0)		// 1 is 0 degrees
		{
			WriteSingleField wsf3(currentProduct->getProductNum(), Product::fieldOffsetLabelRotation2, Integer::toString(sentRotation2));
			SessionMessage msg3(ProductDev, ProductWriteSingleField);
			msg3.setFormatType(BrowserFormat);
			msg3.setHfcObject(&wsf3);
			handleMessage(&msg3);
		}

		if (sentPlacement2 != 2) // 2 is no change
		{
			WriteSingleField wsf4(currentProduct->getProductNum(), Product::fieldOffsetLabelPlacement2, Integer::toString(sentPlacement2));
			SessionMessage msg4(ProductDev, ProductWriteSingleField);
			msg4.setFormatType(BrowserFormat);
			msg4.setHfcObject(&wsf4);
			handleMessage(&msg4);
		}

		Debug::trace("ProductManager.Placement", "handleChangeLabelPlacementAndPosition():   Changed secondary label rotation and placement.");

		//	Now we need to send some sort of message to the UI!
		s->addDialogMessage(ErrProductDev, ProductErrRotationPlacementChanged, HfcResponse::RESPONSESTYLE_OK);

		// Create a timer to clear the message.
		TimerManager *timerManager = TimerManager::getInstance();
		if (timerManager)
			timerManager->addTimer(3, Timer::RepeatNO, timedFuncChangeRotationPlacement);
	}
}

void ProductManager::assignCOOLTextToCurrentProduct(HobRecordNum currentTextNum, HobRecordNum newTextNum)
{
	if (currentProduct)
	{
		bool hasChanged = false;
		if (currentProduct->getCoolDefaultTextNum() == currentTextNum)
		{
			currentProduct->setCoolDefaultTextNum(newTextNum);
			hasChanged = true;

			SessionMessage fieldUpdatedMsg(ProductDev, ProductWriteSingleField);
			fieldUpdatedMsg.setFormatType(BrowserFormat);
			WriteSingleField fieldUpdate(1, Product::fieldOffsetProdCoolDefaultTextNum, Long::toString(newTextNum));
			fieldUpdatedMsg.setHfcObject(&fieldUpdate);
			mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
		}
		if (currentProduct->getCoolTextNum() == currentTextNum)
		{
			currentProduct->setCoolTextNum(RECORD_NOT_FOUND);
			hasChanged = true;

			SessionMessage fieldUpdatedMsg(ProductDev, ProductWriteSingleField);
			fieldUpdatedMsg.setFormatType(BrowserFormat);
			WriteSingleField fieldUpdate(1, Product::fieldOffsetProdCoolTextNum, Long::toString(newTextNum));
			fieldUpdatedMsg.setHfcObject(&fieldUpdate);
			mySession->send(&fieldUpdatedMsg, Messenger::DEST_UI);
		}
		if (hasChanged)
			handleSaveRecord(currentProduct);
	}
}

void ProductManager::handleProductToggleLabelTypeMessage(SessionMessage *msg)
{
	Session *s = dynamic_cast<Session *> (mySession);
	if ( !currentProduct || !s || !s->isInOperateMode() || !msg )
		return;

	GenericHfcObject *msgdata = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
	if (!msgdata)
		return;

	int printerNum = 0;
	HobRecordNum labelTypeNumToggle = 0;
	HobRecordNum newLabelTypeNum = 0;
	HobString labelTypeStr;

	HobString toggleString(msgdata->getData());
	toggleString.unQuoteForCSV();
	Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  toggleString = [" + toggleString + "]");

	HobTokenizerCSV tok(&toggleString, BrowserFormatSeparator);
	HobString token;

	int i = 0;
	while ( tok.next(&token) != -1 )
	{
		Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  i = " + inttostring(i) + ",  token = [" + token + "]");
		if (i == 0)
		{
			printerNum = util_string::stringtoint(token);
		}
		else if (i == 1)
		{
			labelTypeNumToggle = util_string::stringtolong(token);
		}
		++i;
	}
	Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  printerNum = " + inttostring(printerNum) + ",  labelTypeNumToggle = " + longtostring(labelTypeNumToggle));

	if ((labelTypeNumToggle > 0) && (printerNum >= 0) && (printerNum < MaxPrinterTypes)) 
	{
		Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  1)       altLabelTypeNum[" + inttostring(printerNum) + "] = " + inttostring(altLabelTypeNum[printerNum]));
//		Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  1) altFamilyLabelTypeNum[" + inttostring(printerNum) + "] = " + inttostring(altFamilyLabelTypeNum[printerNum]));
		Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  1)      mainLabelTypeNum[" + inttostring(printerNum) + "] = " + inttostring(mainLabelTypeNum[printerNum]));
//		Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  1)    activeLabelTypeNum[" + inttostring(printerNum) + "] = " + inttostring(activeLabelTypeNum[printerNum]));

		HobRecordNum resultLTNum = -1;
		HobRecordNum altLTNum = altLabelTypeNum[printerNum];

		if ( labelTypeNumToggle == altLTNum )
		{
			altLabelTypeNum[printerNum] = 0;
			newLabelTypeNum = mainLabelTypeNum[printerNum];
		}
		else
		{
			altLabelTypeNum[printerNum] = labelTypeNumToggle;
			newLabelTypeNum = labelTypeNumToggle;
		}
//		altFamilyLabelTypeNum[printerNum] = altLabelTypeNum[printerNum];
		Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():        altLabelTypeNum[" + inttostring(printerNum) + "] = " + inttostring(altLabelTypeNum[printerNum]));
//		Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  altFamilyLabelTypeNum[" + inttostring(printerNum) + "] = " + inttostring(altFamilyLabelTypeNum[printerNum]));

		labelTypeStr = longtostring(newLabelTypeNum);
		Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  newLabelTypeNum = " + labelTypeStr);

		// TODO - Larry - done - how to manage the multiple labels on a single printer
		int field = currentProduct->getActiveLabelTypeNumOffsetForMode(printerNum);

		// int fieldUpdated = currentProduct->updateField(field, labelTypeStr, true);
		Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  field = " + inttostring(field));

		WriteSingleField *productUpdate = new WriteSingleField(currentProduct->getProductNum(), field, labelTypeStr);
		if (productUpdate)
		{
			productUpdate->setFieldOrigValue(longtostring(mainLabelTypeNum[printerNum]));

			SessionMessage msg(ProductDev, ProductWriteSingleField);
			msg.setHfcObject(productUpdate);
			handleProductWriteSingleFieldMessage(&msg);

			LabelType *lblType = currentProduct->getLabelType(printerNum);
			if (lblType)
			{
				resultLTNum = lblType->getLabelTypeNum();
	Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  resultLTNum = " + longtostring(resultLTNum) + ",  newLabelTypeNum = " + longtostring(newLabelTypeNum));
//				if ( newLabelTypeNum != resultLTNum )
//				{
//					altFamilyLabelTypeNum[printerNum] = altLabelTypeNum[printerNum];
//	Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  Set altFamilyLabelTypeNum[" + inttostring(printerNum) + "] = " + inttostring(altFamilyLabelTypeNum[printerNum]));
//				}
			}
		}
	}

	Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  2)       altLabelTypeNum[" + inttostring(printerNum) + "] = " + inttostring(altLabelTypeNum[printerNum]));
//	Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  2) altFamilyLabelTypeNum[" + inttostring(printerNum) + "] = " + inttostring(altFamilyLabelTypeNum[printerNum]));
	Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  2)      mainLabelTypeNum[" + inttostring(printerNum) + "] = " + inttostring(mainLabelTypeNum[printerNum]));
//	Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  2)    activeLabelTypeNum[" + inttostring(printerNum) + "] = " + inttostring(activeLabelTypeNum[printerNum]));
}

void ProductManager::handleProductToggleSecLabelTypeMessage(SessionMessage *msg)
{
	HobRecordNum labelTypeNumToggle = 0;
	HobRecordNum newLabelTypeNum = 0;
	HobString labelTypeStr;
	HobRecordNum resultLTNum = -1;
	HobRecordNum altLTNum = altLabelTypeNum[SecondPrinter];

	Session *s = dynamic_cast<Session *> (mySession);
	if ( !currentProduct || !s || !s->isInOperateMode() || !msg )
		return;

	GenericHfcObject *msgdata = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
	if (!msgdata)
		return;

	
	//is secondary label print turned ON
	if(secLabelTypeEnable == true)
	{
		if(origSecLabelTypeNum)
		{
			labelTypeNumToggle = origSecLabelTypeNum;
			
		}

	}

	Debug::trace("ProductManager.ToggleLabel", "labelTypeNumToggle = " + Long::toString(labelTypeNumToggle));
	Debug::trace("ProductManager.ToggleLabel", "Saved origSecLabelTypeNum = " + Long::toString(origSecLabelTypeNum));

	altLabelTypeNum[SecondPrinter] = labelTypeNumToggle;
	newLabelTypeNum = labelTypeNumToggle;

	labelTypeStr = longtostring(newLabelTypeNum);
	Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  newLabelTypeNum = " + labelTypeStr);

	// TODO - Larry - done - how to manage the multiple labels on a single printer
	int field = currentProduct->getActiveLabelTypeNumOffsetForMode(SecondPrinter);

	// int fieldUpdated = currentProduct->updateField(field, labelTypeStr, true);
	Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  field = " + inttostring(field));

	WriteSingleField *productUpdate = new WriteSingleField(currentProduct->getProductNum(), field, labelTypeStr);
	if (productUpdate)
	{
		productUpdate->setFieldOrigValue(longtostring(mainLabelTypeNum[SecondPrinter]));

		SessionMessage msg(ProductDev, ProductWriteSingleField);
		msg.setHfcObject(productUpdate);
		handleProductWriteSingleFieldMessage(&msg);

		LabelType *lblType = currentProduct->getLabelType(SecondPrinter);
		if (lblType)
		{
			resultLTNum = lblType->getLabelTypeNum();
			Debug::trace("ProductManager.ToggleLabel", "handleProductToggleLabelTypeMessage():  resultLTNum = " + longtostring(resultLTNum) + ",  newLabelTypeNum = " + longtostring(newLabelTypeNum));

		}
	}

}

void ProductManager::resetUpLabelTypes(OpModeEvent newMode)
{
	if (!currentProduct)
		return;

	// When Walmart Cake Label (branch-walmart-cake-label) was merged into trunk "isRemotePrintMsgReceived" no longer existed,
	// so the last parameter was removed.
	//bool selectedNewLabelTypes = currentProduct->selectLabelTypesForMode(newMode, isRemotePrintMsgReceived);
	const bool selectedNewLabelTypes( currentProduct->selectLabelTypesForMode(newMode) );
	Debug::trace("ProductManager", "resetUpLabelTypes(): have new labels " + longtostring(selectedNewLabelTypes));

	if (selectedNewLabelTypes)
	{
		// We're about to set the label type objects in the Product based on which mode's set of label types that was determined directly above.
		currentProduct->setupLabelTypesForMode();

		const bool labelTypeError( setUpLabelTypes() );
		Debug::trace("ProductManager", "resetUpLabelTypes(): setup label types error is " + longtostring(labelTypeError));

		setupForToggleLabelTypeMessage();

		// Generate the embedded labels
		Debug::trace("ProductManager.embeddedLabels", "Generating Embedded labels for current product ...");
		currentProduct->generateEmbeddedLabels();

		// SOF-3361 need to image label old one is bad

		// Send a Product Change message to the RTC so it knows the Imager needs to image the new label.
		SessionMessage pluChg(ProductDev, ProductChangeSmart);
		pluChg.setFormatType(InternalFormat);
		HfcProductChangeMsg hpcm(mySession, this);
		pluChg.setHfcObject(&hpcm);
		mySession->send(&pluChg, Messenger::DEST_RTC);

		//	Update the barcode information, since the label type changed.
		BarCodeNumber tempBarCode = currentProduct->getBarCodeNum();
		tempBarCode.getFromDatabase(currentProduct->getMyDbRecord(), currentProduct, dbAccess);
		currentProduct->setBarCodeNum(tempBarCode);

		// This is a standard lookup made by the scale itself.  Send the results back to the scale to display PLU on screen
		Debug::trace("ProductManager", "resetUpLabelTypes(): sending ProductWrite to UI");
		SessionMessage pluData(ProductDev, ProductWrite);
		currentProduct->setFormatType(BrowserFormatPLUOperateMode);
		pluData.setFormatType(BrowserFormat);
		pluData.setHfcObject(currentProduct);
		mySession->send(&pluData, Messenger::DEST_UI);

		// SOF-2742
		// A result of the ProductWrite message being sent to the UI directly above is that the UI clears all Product information before redisplaying it.
		// This includes the unit pricing for a by count item.  But the unit pricing that the UI displays comes from the 101, 15 (ProductWriteCurrentTotalValues)
		// message, not the Product Write message.  So let's send the ProductWriteCurrentTotalValues message to the UI.
		handleProductReadCurrentTotalValuesMessage();
	}
}


void ProductManager::setupForToggleLabelTypeMessage()
{
	if (!currentProduct)
		return;

	// Clear out the label type info to toggle labels and set up for the new PLU.
	for (short printerNum = FirstPrinter; printerNum < MaxPrinterTypes; printerNum++)
	{
		LabelType *labelType = currentProduct->getLabelType(printerNum);
		if (!labelType)
			mainLabelTypeNum[printerNum] = 0;
		else
			mainLabelTypeNum[printerNum] = labelType->getLabelTypeNum();

		//			activeLabelTypeNum[printerNum] = mainLabelTypeNum[printerNum];
		Debug::trace("ProductManager.ToggleLabel", "handleProductReadMessage():       mainLabelTypeNum[" + inttostring(printerNum) + "] = " + inttostring(mainLabelTypeNum[printerNum]));
		//			Debug::trace("ProductManager.ToggleLabel", "handleProductReadMessage():     activeLabelTypeNum[" + inttostring(printerNum) + "] = " + inttostring(activeLabelTypeNum[printerNum]));
	}
}



void ProductManager::startRecordingSweetheartWeight(ProductTransaction *trans)
{
	// We are checking for sweethearting extra weight if:
	//   1. We printed a label (detected by the PrLabelReady printer status, and
	//   2. Configuration::fieldOffsetCfgMinSweetheartWeight: "minSweetheartWeight" > 0, and
	//   3. Configuration::fieldOffsetCfgCollectingTransactions: "collectTransactions" == true, and
	//   4. In Manual Weigh Mode (this mode), but not self-serve mode, and
	//   5. Product is random weight, or bycount with weight

	Configuration *cfg = Configuration::getInstance();
	if (!cfg)
		return;

	bool isTracing = Debug::isTracing("sweetheartWeight");
	string traceHdr("startRecordingSweetheartWeight():  ");

	if (cfg->toIntCfg(fieldOffsetCfgMinSweetheartWeight) == 0 || !cfg->toBoolCfg(fieldOffsetCfgCollectingTransactions))
	{
		if (isTracing)
			Debug::trace("sweetheartWeight", traceHdr + "NO - min weight: " + cfg->toStringCfg(fieldOffsetCfgMinSweetheartWeight) + "  tracking transactions: " + cfg->toStringCfg(fieldOffsetCfgCollectingTransactions));
		sweetheartWeightTransaction = 0;
		sweetheartWeightProductSmartPtr = 0;
		return;
	}

	// TODO_SWEETY:  Do we need to consider:  currentProduct->hasKbdNetWeightLabel()
	if (!currentProduct || (!currentProduct->isRandomWeight() && !currentProduct->isByCountWithWeight()))
	{
		if (isTracing)
			if (!currentProduct)
				Debug::trace("sweetheartWeight", traceHdr + "NO - currentProduct*: " + ptrtostring(currentProduct));
			else
				Debug::trace("sweetheartWeight", traceHdr + "NO - isRandomWeight: " + booltostring(currentProduct->isRandomWeight()) + "  isByCountWithWeight: " + booltostring(currentProduct->isByCountWithWeight()));
		sweetheartWeightTransaction = 0;
		sweetheartWeightProductSmartPtr = 0;
		return;
	}

	Session *s = dynamic_cast<Session *>(mySession);
	if (s)
	{
		OpModeEvent currMode = s->getCurrentMode();
		if ( currMode == OpModeManualWeigh || (currMode == OpModeSelfService && !cfg->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen)) )
		{
			if (isTracing)
				Debug::trace("sweetheartWeight", traceHdr + "YES - before setting sweetheartWeightTransaction*: " + ptrtostring(sweetheartWeightTransaction));

			if (trans)
				trans->setSweetheartWeight(0);
			sweetheartWeightTransaction = trans;

			if (isTracing)
				Debug::trace("sweetheartWeight", traceHdr + "YES - after setting sweetheartWeightTransaction*: " + ptrtostring(sweetheartWeightTransaction));

			if (isTracing)
			{
				Debug::trace("sweetheartWeight", traceHdr + "YES - currentProduct*: " + ptrtostring(currentProduct) + "  currentTransaction*: " + ptrtostring(trans));

				if (sweetheartWeightProductSmartPtr.notNull())
				{
					Debug::trace("sweetheartWeight", traceHdr + "YES - before setting sweetheartWeightProductSmartPtr: prod*: " + ptrtostring(sweetheartWeightProductSmartPtr.get()));
					Debug::trace("sweetheartWeight", traceHdr + "YES - before setting sweetheartWeightProductSmartPtr: prod's trans*: " + ( sweetheartWeightProductSmartPtr.get() ? ptrtostring(sweetheartWeightProductSmartPtr.get()->getCurrentTransaction()) : "0" ));
				}
				else
					Debug::trace("sweetheartWeight", traceHdr + "YES - before setting sweetheartWeightProductSmartPtr: NULL");
			}

			sweetheartWeightProductSmartPtr = currentProductSmartPtr;

			if (sweetheartWeightProductSmartPtr.notNull())
			{
				Debug::trace("sweetheartWeight", traceHdr + "YES - after setting sweetheartWeightProductSmartPtr: prod*: " + ptrtostring(sweetheartWeightProductSmartPtr.get()));
				Debug::trace("sweetheartWeight", traceHdr + "YES - after setting sweetheartWeightProductSmartPtr: prod's trans*: " + (sweetheartWeightProductSmartPtr.get() ? ptrtostring(sweetheartWeightProductSmartPtr.get()->getCurrentTransaction()) : "0"));
			}
			else
				Debug::trace("sweetheartWeight", traceHdr + "YES - after setting sweetheartWeightProductSmartPtr: NULL");
		}
		else
		{
			if (isTracing)
				Debug::trace("sweetheartWeight", traceHdr + "NO - OpModeManualWeigh: " + booltostring(currMode == OpModeManualWeigh) + "  OpModeSelfService: " + booltostring(currMode == OpModeSelfService) + "  CfgSelfServePrintToScreen): " + cfg->toStringCfg(fieldOffsetCfgSelfServePrintToScreen));
			sweetheartWeightTransaction = 0;
			sweetheartWeightProductSmartPtr = 0;
		}
	}
}

void ProductManager::stopRecordingSweetheartWeight(bool writeSweetheartWeightToDbIfItExists)
{
	// We are checking for sweethearting extra weight if:
	//   1. We printed a label (detected by the PrLabelReady printer status, and
	//   2. Configuration::fieldOffsetCfgMinSweetheartWeight: "minSweetheartWeight" > 0, and
	//   3. Configuration::fieldOffsetCfgCollectingTransactions: "collectTransactions" == true, and
	//   4. In Manual Weigh Mode (this mode), but not self-serve mode, and
	//   5. Product is random weight, or bycount with weight

	bool isTracing = Debug::isTracing("sweetheartWeight");
	string traceHdr("stopRecordingSweetheartWeight():  ");

	if (isTracing)
	{
		if (sweetheartWeightProductSmartPtr.notNull())
		{
			Debug::trace("sweetheartWeight", traceHdr + "- at start of method, sweetheartWeightProductSmartPtr: prod*: " + ptrtostring(sweetheartWeightProductSmartPtr.get()));
			Debug::trace("sweetheartWeight", traceHdr + "- at start of method, sweetheartWeightProductSmartPtr: prod's trans*: " + (sweetheartWeightProductSmartPtr.get() ? ptrtostring(sweetheartWeightProductSmartPtr.get()->getCurrentTransaction()) : "0"));
		}
		else
			Debug::trace("sweetheartWeight", traceHdr + "- at start of method, sweetheartWeightProductSmartPtr: NULL");

		Debug::trace("sweetheartWeight", traceHdr + "- sweetheartWeightTransaction*: " + ptrtostring(sweetheartWeightTransaction));
		if (sweetheartWeightTransaction)
			Debug::trace("sweetheartWeight", traceHdr + "- at start of method, sweetheartWeight: " + longtostring(sweetheartWeightTransaction->getSweetheartWeight()));
	}

	if ( writeSweetheartWeightToDbIfItExists && sweetheartWeightTransaction && sweetheartWeightTransaction->getSweetheartWeight() > 0 )
	{
		if (isTracing)
		{
			Debug::trace("sweetheartWeight", traceHdr + "- **************************************************************************************************");
			Debug::trace("sweetheartWeight", traceHdr + "- stopping looking for sweetheart weight, but we currently have one, so write it out to the database");
			Debug::trace("sweetheartWeight", traceHdr + "- current sweetheart transId: " + longtostring(sweetheartWeightTransaction->getTransactionId()));
			Debug::trace("sweetheartWeight", traceHdr + "- current sweetheart weight: " + longtostring(sweetheartWeightTransaction->getSweetheartWeight()));
			Debug::trace("sweetheartWeight", traceHdr + "- **************************************************************************************************");
		}
		mgrContainer_->getTransactionManager()->handleSaveTransaction(sweetheartWeightTransaction);
	}

	if (sweetheartWeightProductSmartPtr.notNull())
	{
		//  Don't delete this pointer.  It'll get deleted when its associated product gets deleted.
		if (sweetheartWeightTransaction)
			sweetheartWeightTransaction->setSweetheartWeight(0);
		sweetheartWeightTransaction = 0;

		if (isTracing)
			Debug::trace("sweetheartWeight", traceHdr + "- set sweetheartWeightTransaction: 0");

		// To delete the product we're tracking for sweetheart weight, just set it to null.
		sweetheartWeightProductSmartPtr = 0;

		if (isTracing)
		{
			if (sweetheartWeightProductSmartPtr.notNull())
			{
				Debug::trace("sweetheartWeight", traceHdr + "- after clearing sweetheartWeightProductSmartPtr: prod*: " + ptrtostring(sweetheartWeightProductSmartPtr.get()));
				Debug::trace("sweetheartWeight", traceHdr + "- after clearing sweetheartWeightProductSmartPtr: prod's trans*: " + (sweetheartWeightProductSmartPtr.get() ? ptrtostring(sweetheartWeightProductSmartPtr.get()->getCurrentTransaction()) : "0"));
			}
			else
				Debug::trace("sweetheartWeight", traceHdr + "- after clearing sweetheartWeightProductSmartPtr: NULL");
		}

	}
}


void ProductManager::handleSweetheartWeight(HfcWeigherStatus *weight)
{
	bool isTracing = Debug::isTracing("sweetheartWeight");
	string traceHdr("handleSweetheartWeight ");

	// No need to handle it if there is no product that we're tracking for sweetheart weight
	if (sweetheartWeightProductSmartPtr.null())
	{
		Debug::trace("sweetheartWeight", traceHdr + "- no sweetheart product");
		return;
	}
	if (!sweetheartWeightTransaction)
	{
		Debug::trace("sweetheartWeight", traceHdr + "- no sweetheart transaction");
		return;
	}
	if (!weight)
	{
		Debug::trace("sweetheartWeight", traceHdr + "- no weight to process");
		return;
	}
	if (weight->isWeigherInError())
	{
		Debug::trace("sweetheartWeight", traceHdr + "- weight: WeigherInError");
		return;
	}
	if (!weight->isPositiveNonZeroStableWeight())
	{
		Debug::trace("sweetheartWeight", traceHdr + "- weight: not PositiveNonZeroStableWeight");
		return;
	}


	// Originally from:   void ProductManager::updateWeightAndNotify(HfcWeigherStatus *weight, bool forceNotification)
	// I picked out the parts I needed.
	// The only thing I really needed was the call to currentProduct->updateTotalValuesData(), which calculates the total net weight (considering tare, etc.),
	// so now I'm just including the necessary parts from:   bool Product::updateTotalValuesData(), to calculate the total net sweetheart weight

		// The following comment was in Product::updateTotalValuesData(). It states to use current values from the Product object.  But since we've already printed, the values have
		// been put into the Product's ProductTransaction.  So below, when I get the tare to use in the net total weight calculations, I just use the tare set in the transaction,
		// which has already been qualified as tareA or tareB.
		/* When update the current transaction's 'total value' fields, use the current values of the
		* product fields (unit price, by count, etc.) from the Product class, not the duplicates of these
		* fields in the ProductTransaction class; they won't be set up there until the ProductTransaction
		* is to be printed.
		*/

		// The following comment was in Product::updateTotalValuesData(), but for the purposes of sweetheart total_net_weight calculation, I'm not considering a change in dualRangeMode up to down, or vice versa.
		// Just leaving it here for reference.
		// The default is that this method did not result in an adjustment to the tare as a result of changing from dualRangeMode up to down.

		HobWeight netWeight = weight->getWeight();
		HobWeight totalTare = 0;

		// For sweetheart checking, we are only considering random weight and ByCountWithWeight, so we use this block of code, and comment-out/skip the else clause (but don't ignore the Santa Clause, "you know, like the last line in a contract")
		//if (isRandomWeight() || isByCountWithWeight())
		//{
			// HT-136 Use correct Tare value
			//totalTare = (scaleConfig->toBoolCfg(fieldOffsetCfgUseSecondTare)) ? getTareB().getValue() : getTareA().getValue();
			// Just use the tare in the transaction; it's already been set when print occurred
		totalTare = sweetheartWeightTransaction->getTare();

		MetSig::applyTare(netWeight, totalTare, weight);

		// TareSetback only has to do with a wrapper, and it's in OpModeAutoWeigh mode, but that's not a mode in which we track sweetheart weight, so the next line is commented out
		//MetSig::applyTareSetback(netWeight, totalTare);
	//}
	//else
	//{
	//	totalTare = 0;
	//}

		Product *sweetheartProd = sweetheartWeightTransaction->getProduct();
		if (sweetheartProd)
		{
			if (sweetheartProd->isRandomWeight() && netWeight > 0 && (sweetheartProd->getProportionalTare()) && !Configuration::getInstance()->isPrepackScale())	//	Proportional tare only on a Random weight.
			{
				MetSig::applyProportionalTare(netWeight, totalTare, sweetheartProd->getProportionalTare(), weight);
			}
		}

		//currentProductTransaction_->setTotalNetWeight(netWeight);

		// The following comment and block of code was in Product::updateTotalValuesData(),
		// but for the purposes of sweetheart total_net_weight calculation, I'm not considering a change in dualRangeMode up to down, or vice versa.
		// And I'm doubly not including it because it recursively calls updateTotalValuesData(), which I'm not going to set that up for the purposes of calculating sweetheart weight.
		// It just doesn't have to be that accurate!  (In my opinion anyways)

		/* Here is where we can determine if we have to switch the dualRangeDirection from up to down because the weight goes over the dualRangeLimit,
		* when we are using the net weight as the weight for the limit check.  When we are using the gross weight, the Session has already determined
		* in which direction we are moving, up or down, since in this case, the tare doesn't play any part in determining if we are over the dual range limit.
		* We now know the net weight, and can use it to determine if we are past the dualRangeLimit if the configuration parameter that indicates
		* that we should use the net weight to make this determination is set.
		*/
		//if (!hasKbdNetWeightLabel())
		//{
		//	bool changedDualRangeDirection = MetSig::checkToSwitchDualRangeDirectionOnNetWeight(weigherStatus, netWeight);
		//	if (changedDualRangeDirection)
				// Now that we have changed the direction from up to down, we have to recalculate the net weight with tares that are using a
				// scale factor of 1, not 5, when in avoir weighing, or that use a scale factor of 5, not 2, when in metric weighing.
				// So to do this recursively call this updateTotalValuesData() method.
				//	Dual range is only available on the HLX and new USB weigher peripheral with dual-range load cell. (haberrc)
		//		updateTotalValuesData();
		//}


	if (isTracing)
	{
		Debug::trace("sweetheartWeight", traceHdr + "- transaction weight: " + longtostring(sweetheartWeightTransaction->getTotalNetWeight()));
		Debug::trace("sweetheartWeight", traceHdr + "- new weight: " + longtostring(weight->getWeight()));
		Debug::trace("sweetheartWeight", traceHdr + "- new net weight: " + longtostring(netWeight));
		Debug::trace("sweetheartWeight", traceHdr + "- current sweetheart weight: " + longtostring(sweetheartWeightTransaction->getSweetheartWeight()));
	}
	
	/* For now, not going to consider that if they return to the transaction weight, after adding or subtracting weight, then no sweethearting going on 
	if ( netWeight == sweetheartWeightTransaction->getTotalNetWeight() )
	{
		Debug::trace("sweetheartWeight", traceHdr + "- new weight = transaction weight, so no sweethearting");
		sweetheartWeightTransaction->setSweetheartWeight(0);
		return;
	}
	*/	
	if ( ( netWeight - sweetheartWeightTransaction->getTotalNetWeight() ) < Configuration::getInstance()->toIntCfg(fieldOffsetCfgMinSweetheartWeight) )
	{
		Debug::trace("sweetheartWeight", traceHdr + "- weight: not big enough to report sweethearting");
		return;
	}
	if ( sweetheartWeightTransaction->getSweetheartWeight() > netWeight )
	{
		Debug::trace("sweetheartWeight", traceHdr + "- weight: current sweetheart weight is already greater than new weight, ignoring this new weight");
		return;
	}

	sweetheartWeightTransaction->setSweetheartWeight(netWeight);

	Debug::trace("sweetheartWeight", traceHdr + "- sweetheart weight set to: " + longtostring(netWeight));
}

void ProductManager::setSecondayLabelTypeEnable(bool state)
{
	secLabelTypeEnable = state;
}

bool ProductManager::getSecondayLabelTypeEnable(void)
{
	return secLabelTypeEnable;
}
