
#include "OpModeEvent.h"
#include "AccumulateItemsMode.h"

#include "PrinterManager.h"
#include "PrinterStatus.h"
#include "PrinterLabelComplete.h"
#include "PrinterLabelReady.h"
#include "PrinterRequestStockMetrics.h"
#include "PrinterNomSize.h"
#include "ProductUpdateMsg.h"
#include "ProductChangeMsg.h"
#include "ProductFinishedPrintingAllLabelsMsg.h"
#include "ProductCompleteMsg.h"
#include "ProductPackageCompleteMsg.h"
#include "ProductRemoveAccumItemMsg.h"
#include "ProductReadAllAccumulatedItemsMsg.h"
#include "PythonScripter.h"

#include "ProductTransaction.h"
#include "ProductTransactionMaster.h"
#include "LabelType.h"
#include "ResourceHeader.h"
#include "LabelStockDescriptor.h"
#include "FormRecord.h"

#include "Messages/UIDialogMsg.h"

#include "Configuration.h"
#include "HfcPrinterStockMetrics.h"
#include "ProductManager.h"

using namespace std;

#include "Integer.h"
#include "Hex.h"
#include "Bool.h"
#include "HobString.h"
#include "MultiLanguageText.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AccumulateItemsMode::dataType = 0;
std::string AccumulateItemsMode::dataTypeName("AccumulateItemsMode"); 
#endif

AccumulateItemsMode::AccumulateItemsMode(Product *pCurrentProduct,
								 //WeigherManager *pWeigherManager,
								 //PrinterManager *pFirstPrinter, 
								 //PrinterManager *pSecondPrinter,
								 //PrinterManager *pThirdPrinter,
								 //LabelApplierManager *pLabelApplierMananger,
								 //WrapperManager *pWrapperManager) :	ProductionEntryMode(
								 Hobart::CEvolMsgReceiver *pWeigherManager,
								 Hobart::CEvolMsgReceiver *pFirstPrinter, 
								 Hobart::CEvolMsgReceiver *pSecondPrinter,
								 Hobart::CEvolMsgReceiver *pThirdPrinter,
								 Hobart::CEvolMsgReceiver *pLabelApplierMananger,
								 Hobart::CEvolMsgReceiver *pWrapperManager) :	ProductionEntryMode(
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
	registerEvent(ProductDev, ProductUpdate);
	registerEvent(ProductDev, ProductStartPrintAllLabels);
	registerEvent(ProductDev, ProductDontUpdateProductionPlanning);
	registerEvent(ProductDev, ProductRemoveSingleAccumulatedItem);
	registerEvent(ProductDev, ProductRequestReadAllAccumulatedItems);
	registerEvent(ProductDev, ProductRemoveAllAccumulatedItems);
    registerEvent(ProductDev, ProductChangeSmart);

	registerEvent(Printer, PrStatus);
	registerEvent(Printer, PrLabelComplete);
	registerEvent(Printer, PrLabelReady);
	registerEvent(Printer, PrStockMetrics);
	registerEvent(Printer, PrNomSize);

	stateMachine = aimInitial;
	Debug::trace("AccumulateItemsMode.StateMachine", "stateMachine = aimInitial");

	///sendWrapperAbort();

	// set the CLA mode to Manual
	///setLabelApplierMode(Manual);
	///setWeigherMode(ScaleManualMode);
	///requestWeigherStatus();

	sendPrinterResetImmediate(FirstPrinter);
	sendPrinterResetImmediate(SecondPrinter);
	sendPrinterResetImmediate(ThirdPrinter);

	takeLabelTime = 5;
	setTakeLabelTime(FirstPrinter);
	setTakeLabelTime(SecondPrinter);
	setTakeLabelTime(ThirdPrinter);

	actualInstalledLabelSizeInLines = 0;
	requestLabelSize();

	currentPackId = 0;

	startedPrintingLabels = false;
	outOfMedia = false;
	
	ptm = new ProductTransactionMaster();

	setupCfgParams();

	init();

	///if (pCurrentProduct != NULL)
	///{
	///	ProductChangeMsg pcMsg(this, (const Product *) pCurrentProduct);

	///	char *pCC = (char *) pcMsg.serialize();
	///	handleProductMessage((ProductEvent) pcMsg.getEventType(), pCC);
	///}
}

AccumulateItemsMode::~AccumulateItemsMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

///	emptyProductTransactionQ();

	while ( ptmPrev.size() )
	{
		delete ptmPrev.front();
		ptmPrev.pop_front();
	}

	if (ptm)
	{
		delete ptm;
		ptm = 0;
	}

	if (savedMasterLabelType)
	{
		delete savedMasterLabelType;
		savedMasterLabelType = 0;
	}
	///setLabelApplierMode(Conveyor);
}				

void AccumulateItemsMode::init()
{
	savedMasterLabelType = 0;
	anotherLabelNeedsPrinted = false;
}


void AccumulateItemsMode::postImagePrinters()
{
	Debug::trace("AccumulateItemsMode", "postImagePrinters() entered");

	if (printerScenario == PrimaryPrinterOnly)
	{
		postImagePrinter(FirstPrinter, pFirstPrinter, currentPackId++);

		stateMachine = aimPrintingPrimary;
		Debug::trace("AccumulateItemsMode.StateMachine", "stateMachine = aimPrintingPrimary");
	}
	else if (printerScenario == SecondPrinterOnly)
	{
		postImagePrinter(SecondPrinter, pSecondPrinter, currentPackId++);

		stateMachine = aimPrintingSecondary;
		Debug::trace("AccumulateItemsMode.StateMachine", "stateMachine = aimPrintingSecondary");
	}
	else if (printerScenario == PrimaryAndSecondPrinter)
	{
		postImagePrinter(FirstPrinter, pFirstPrinter, currentPackId++);
		postImagePrinter(SecondPrinter, pSecondPrinter, currentPackId++);

		stateMachine = aimPrintingPrimaryPrintingSecondary;
		Debug::trace("AccumulateItemsMode.StateMachine", "stateMachine = aimPrintingPrimaryPrintingSecondary");
	}
	else
	{
		Debug::trace("AccumulateItemsMode.Error", "Invalid printerScenario");
	}
}


void AccumulateItemsMode::handleProductSmartPtr(Util::CSmartPtr<Product> &pCurrentProductSmartPtr)
{
	Product *pProduct = 0;

	pProduct = pCurrentProductSmartPtr.get();

	emptyProductQ();
	if (pProduct)
		productQ.push(pCurrentProductSmartPtr);
	else
		pCurrentProduct = 0;

	if (stateMachine == aimInitial || stateMachine == aimWaitingForProductUpdate)
	{
		if (getNewProduct() == true)
		{
			changeProduct();
			preImagePrinters();	

			stateMachine = aimWaitingForProductUpdate;
			Debug::trace("AccumulateItemsMode.StateMachine", "stateMachine = aimWaitingForProductUpdate");
		}
	}
}


UINT8 AccumulateItemsMode::isThereRoomOnLabel(int numOfItems)
{
	UINT8 returnErrCode = 0; // no error

	Debug::trace("AccumulateItemsMode", "isThereRoomOnLabel() entered");
	if( numOfItems < 0 )
		numOfItems = 0;

	// FT-529: changed code from switch statement to if statement, 3.5" stock should handle 3.0"
	// label without problems

	if ( Length8_000_Lines <= actualInstalledLabelSizeInLines ) // 8.000 inch
	{
		if( numOfItems < 15 )
			returnErrCode = 0;
		else
			returnErrCode = 1;
	}
	else if ( Length6_000_Lines <= actualInstalledLabelSizeInLines ) // 6.000 inch
	{
		if( numOfItems < 11 )
			returnErrCode = 0;
		else
			returnErrCode = 1;
	}
	else if ( Length4_000_Lines <= actualInstalledLabelSizeInLines ) // 4.000 inch
	{
		if( numOfItems < 7 )
			returnErrCode = 0;
		else
			returnErrCode = 1;
	}
	else if ( Length3_000_Lines <= actualInstalledLabelSizeInLines ) // 3.000 inch
	{
		if( numOfItems < 5 )
			returnErrCode = 0;
		else
			returnErrCode = 1;
	}
	else if ( Length2_375_Lines <= actualInstalledLabelSizeInLines ) // 2.375 inch
	{
		if( numOfItems < 3 )
			returnErrCode = 0;
		else
			returnErrCode = 1;
	}
	else if ( Length1_750_Lines <= actualInstalledLabelSizeInLines ) // 1.75 inch
	{
		if( numOfItems < 2 )
			returnErrCode = 0;
		else
			returnErrCode = 1;
	}
	else
	{
		returnErrCode = 2;
	}

	return( returnErrCode );
}


void AccumulateItemsMode::processStartPrinting()
{
	bool renderedLabel = false;

	Product *masterProd = ptm->getMasterProduct();
	ProductTransaction *masterTrans = ptm->getMasterProductTransaction();
	if (!masterProd || !masterTrans)
		return;

	// Set the transaction number for the label (in the EAN barcode)
	UINT32 transNum = (UINT32)(Configuration::getInstance()->toIntCfg(fieldOffsetCfgAccumItemsTransactionNumber));
	masterTrans->setTransactionNumberForBarcode(transNum);
	masterTrans->setTransactionGroupNumber(transNum);

	Product *firstItemProd = 0;

	// Start by assuming we will use the master product to setup the barcode data.
	Product *productToUseForBarcode = masterProd;

	// ======= PJR 454 - receive bcn from the cloud =======

	bool useCloud = false; 
	Debug::trace("BarcodeCloud", std::to_string(Configuration::getInstance()->toIntCfg(fieldOffsetCfgReadBarcodeFromCloud))); // sanity check
	if (Configuration::getInstance()->toIntCfg(fieldOffsetCfgReadBarcodeFromCloud) == 1 && Configuration::getInstance()->toIntCfg(fieldOffsetCfgEnableAccumItems) == 1)
	{
		// build data string based on current transaction
		std::string argument = createData();
		useCloud = true;
		Debug::trace("BarcodeCloud", argument);

		const string command(PythonScripter::buildPythonCall("getBarcodeFromCloud.py", -1, argument));
		std::string scriptResult;
		std::string bcnFromCloud;
		std::string totalPriceFromCloud;
		Debug::trace("BarcodeCloud", "command " + command);
		int scriptValue(PAL_absclass::PAL_executeWithResults(command, scriptResult));
		Debug::trace("BarcodeCloud", "output from script " + scriptResult);

		if (scriptResult.size() > 2)
		{
			bcnFromCloud = scriptResult.substr(0, scriptResult.find(" "));

			totalPriceFromCloud = scriptResult.substr(scriptResult.find(" ") + 1, scriptResult.length());
			Debug::trace("BarcodeCloud", "totalPriceFromCloud " + totalPriceFromCloud);

			BarCodeNumber temp = productToUseForBarcode->getBarCodeNum();
			temp.setBarcodeString(BarcodeType::USER_SUPPLIED, bcnFromCloud);
			temp.setBarcodeType(BarcodeType::USER_SUPPLIED);
			masterProd->setBarCodeNum(temp);
			masterTrans->setBarCodeNum(temp);
			long long totalPriceLL = std::stoll(totalPriceFromCloud);
			masterTrans->setTotalPrice(totalPriceLL);

		}
		// error retrieving barcode - generate locally with given specifications 
		// will generate an EAN128c barcode with the format given in the config linux file
		else if (scriptResult.size()  <= 2)
		{
			Debug::trace("BarcodeCloud", "error getting barcode from the cloud");
			BarCodeNumber localBcn = productToUseForBarcode->getBarCodeNum();
			localBcn.setPrefix("470"); // prefix will always be 470 (I hope?)
			localBcn.setBarcodeType(BarcodeType::EAN128C);
			Debug::trace("BarcodeCloud", Configuration::getInstance()->toStringCfgI(CfgInternal::BARCODEFORMATTOUSEOFFLINE));
			localBcn.setFormatString(BarcodeType::EAN128C, Configuration::getInstance()->toStringCfgI(CfgInternal::BARCODEFORMATTOUSEOFFLINE));
			localBcn.buildBarcodeStrings(masterTrans, true);

			masterProd->setBarCodeNum(localBcn);
			masterTrans->setBarCodeNum(localBcn);
			//masterTrans->buildBarcodeStrings();
		}
	}

	// ======= END PJR 454 =======

	// Now need to set up the barcode string to render.  This gets done in the master transaction.
	if (cfgAccumBarcodeUseFirstItem)
	{
		// Use the first item in the list to setup the barcode data.
		ProductTransaction *firstItemTrans = ptm->getFirstTransaction();
		if (firstItemTrans)
		{
			firstItemProd = firstItemTrans->getProduct();
			if (firstItemProd)
				productToUseForBarcode = firstItemProd;
		}
	}


	BarCodeNumber bcn = productToUseForBarcode->getBarCodeNum();
	bcn.setFormatString(EAN13, Configuration::getInstance()->toStringCfg(fieldOffsetCfgAccumItemsEANFormatString));

	if (productToUseForBarcode == masterProd)
	{
		bcn.setPrefix(Configuration::getInstance()->toStringCfg(fieldOffsetCfgAccumItemsEANPrefix));
		bcn.setEANCode(Configuration::getInstance()->toStringCfg(fieldOffsetCfgAccumItemsEANCode));
		bcn.setVendorNumber("00000");

		HobString sProdNum = Configuration::getInstance()->toStringCfg(fieldOffsetCfgAccumItemsUPCProductNumber);
		bcn.setUPCNumber(sProdNum);
	}

	LabelType *masterLabelType = (savedMasterLabelType) ? savedMasterLabelType : masterProd->getLabelType(FirstPrinter);
	short masterBarcodeType = (masterLabelType) ? masterLabelType->getBarcodeType() : UPCA;  // This should never happen since the master product should always have a label type, but just in case, don't want to crash.
	// LAP-BARCODE
	bcn.setBarcodeType(masterBarcodeType);

	/* LAP-BARCODE
	// If we're using the barcode data from the first product in the list, then check if the barcode types are different between
	// the first product in the list and the dummy master product.  If they are different, then we will attempt to fill in the
	// AccumulateItem's label's barcode with substitute data from the first item.  For example, if the AccumulateItem's barcode is
	// EAN, but the first item's barcode is UPC, we will attempt to set the EAN code in the AccumulateItem's label's barcode with
	// the UPC product num from the first product in the list.
	if (productToUseForBarcode == firstItemProd)
	{
		//bcn.setEANCode(Configuration::getInstance()->toStringCfg(fieldOffsetCfgAccumItemsEANCode));

		//HobString sProdNum = Configuration::getInstance()->toStringCfg(fieldOffsetCfgAccumItemsUPCProductNumber);
		//bcn.setUPCNumber(sProdNum);

		LabelType *firstItemLabelType = firstItemProd->getLabelType(FirstPrinter);
		short firstItemBarcodeType = (firstItemLabelType) ? firstItemLabelType->getBarcodeType() : UPCA;  // This should never happen since the master product should always have a label type, but just in case, don't want to crash.

		if (firstItemBarcodeType != masterBarcodeType)
		{
			if (masterBarcodeType == UPCA || masterBarcodeType == UPCD3 || masterBarcodeType == UPCE)
			{
				// First item is EAN barcode, but AccumulateItem's label is UPC barcode, so try to put the first item's ean code in the UPC product number field
				bool canSet = bcn.setUPCNumberFromEANCode();
			}
			else
			{
				// First item is UPC barcode, but AccumulateItem's label is EAN barcode, so try to put the first item's UPC product number in the ean code field
				bool canSet = bcn.setEANCodeFromUPCNumber();
			}
		}
	}
*/
	if (useCloud == false)
	{
		masterProd->setBarCodeNum(bcn);
		masterTrans->setBarCodeNum(bcn);
		masterTrans->buildBarcodeStrings();
	}
	

	Debug::trace("AccumulateItemsMode", "handling ProductStartPrintAllLabels - before calling createFakeLabel, labelType=" + HobString(Hex::toString((long long)masterProd->getLabelType(FirstPrinter))));
	if (cfgEnableItemStripMode)  // FHA
	{
		needToPrintMultipleLabels = true; // instead of calling determineIfNeedToPrintMultipleLabels(), just assume we need to print multiple labels; the createFakeLabel() method will put the correct number of items on the label
		if (needToPrintMultipleLabels)
		{
			if (masterLabelType && !savedMasterLabelType)
			{
				HobRecordNum ltNum = masterLabelType->getLabelTypeNum();
				HobRecordNum deptNum = masterLabelType->getDeptNum();
				if (ltNum != 0)
				{
					savedMasterLabelType = LabelType::findRecord(ltNum, deptNum, masterLabelType->getDBAccess());
				}
			}
		}

		// if in item strip mode need to create the entire label, otherwise only printing total label which is in the master already
		LabelType *newLabelType = createFakeLabel(savedMasterLabelType);
		Debug::trace("AccumulateItemsMode", "handling ProductStartPrintAllLabels - after calling createFakeLabel, newLabelType=" + HobString(Hex::toString((long long)newLabelType)));
		if (newLabelType)
			masterProd->setLabelType(FirstPrinter, newLabelType);
	}


	pCurrentProduct = masterProd;
	pCurrentProductTransaction = masterTrans;
	if (pCurrentProductSmartPtr.get() == NULL)
	{
		pCurrentProductSmartPtr = Util::CSmartPtr<Product>();
		pCurrentProductSmartPtr = masterProd;		//	Need a smart pointer to image the label.
	}

	if (pCurrentProduct)
	{
		Debug::trace("AccumulateItemsMode", HobString("handling ProductStartPrintAllLabels - before calling changeProduct()"));
		changeProduct();

		if (pCurrentProductTransaction && pCurrentProduct && printerScenario != NoPrinters)
		{
			startedPrintingLabels = true;

			renderedLabel = true;

#if 0 // moved test to case: ProductUpdate fix for Jira GCDR-80
			if (ptm->getTransactionCount() >= Configuration::getInstance()->toIntCfgI(CfgInternal::ACCUMITEMSITEMCOUNTRENDERNOTIFY))
			{
				UIDialogMsg dialog(this, SessionDev, SessionDevAccumulateItemsRenderingManyProducts, HfcResponse::RESPONSESTYLE_OK, false);
				dispatchEvent(&dialog);
			}
#endif
			Debug::trace("AccumulateItemsMode", HobString("handling ProductStartPrintAllLabels - before calling preImagePrinters()"));
			//						if (!Configuration::getInstance()->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
			preImagePrinters();
			Debug::trace("AccumulateItemsMode", HobString("handling ProductStartPrintAllLabels - done with calling preImagePrinters()"));

			Debug::trace("AccumulateItemsMode", HobString("handling ProductStartPrintAllLabels - before calling postImagePrinters to print label"));
			//						if (!Configuration::getInstance()->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
			postImagePrinters();
			//						else
			//						{
			//							Debug::trace("AccumulateItemsMode", HobString("handling ProductStartPrintAllLabels - printing faked, sending success"));
			//							finishHandlingPrinting(true);
			//						}
		}
	}

	if (!renderedLabel)
	{
		// Finalize functionality as if the label has been printed.

		// Remove all other transactions, even the master trans, from the itemized list (Don't delete them, the mode doesn't OWN them).
		ptmPrev.push_back(ptm);
		ptm = new ProductTransactionMaster();

		if (8 < ptmPrev.size())
		{
			delete ptmPrev.front();
			ptmPrev.pop_front();
		}

		setupCfgParams();

		// Let the Session/ProductManager know we are done with the current Itemized List.
		ProductFinishedPrintingAllLabelsMsg finishedMessage(0, false);
		dispatchEvent(&finishedMessage);
	}
	Debug::trace("AccumulateItemsMode", "ProductStartPrintAllLabels handling finished");
}


void AccumulateItemsMode::handleProductMessage(ProductEvent productEvent, const char *recvBuffer)
{
	switch (productEvent)
	{
		// Operator has chosen a new product
		case ProductChange:
		{
			//stateMachine = aimWaitingForProductUpdate;
			Debug::trace("AccumulateItemsMode.StateMachine", HobString("ProductChange - stateMachine = ") + HobString(Integer::toString(stateMachine)));
			break;
		}

		// Operator has chosen a new product, react to it if not in Item Strip mode
		//case ProductChange:
		case ProductChangeSmart:
		{
			ProductChangeMsg prodChange(recvBuffer);
			ProductManager *productManager = (ProductManager *) prodChange.getProductMgr();

			// FT-811 printing expanded text
			if ( productManager )
			{
				Util::CSmartPtr<Product> prod = productManager->getCurrentProductSmartPtr();

				if (( prod ) && ( prod->isDummyProduct() ))
				{
					pCurrentProductSmartPtr = productManager->getCurrentProductSmartPtr();
					handleProductSmartPtr(pCurrentProductSmartPtr);
					break;
				}
			}

			// If not printing individual labels we do not need to process the smart pointers
			if ( !cfgEnableItemStripMode )  // FHA
			{
				// Item strip mode is off, need to print individual labels
				// If printing totals labels only we do need to process the smart pointers
				if ( !cfgEnableAccumItemsTotalsOnly )  // FHA
				{
					if (productManager)
					{
						pCurrentProductSmartPtr = productManager->getCurrentProductSmartPtr();
						handleProductSmartPtr(pCurrentProductSmartPtr);
					}
					if (stateMachine == aimFinishingPrintingLabels)
					{
						// Must have printed the totals label
						stateMachine = aimWaitingForProductUpdate;
						Debug::trace("AccumulateItemsMode.StateMachine", "stateMachine = aimWaitingForProductUpdate");
					}
				}
				else
				{
					if ( stateMachine == aimInitial || stateMachine == aimFinishingPrintingLabels )	//	GCDR-187
					{
						// Assumes that this is master transaction w/ print totals label only selected
						stateMachine = aimWaitingForProductUpdate;
						Debug::trace("AccumulateItemsMode.StateMachine", "stateMachine = aimWaitingForProductUpdate");
					}
				}
			}
			break;
		}
		// Operator has added the item to the Accumulated Item list
		case ProductUpdate:
		{
			bool	isWeightRequired;
			ProductUpdateMsg prodUpdate(recvBuffer);
			ProductTransaction *pNextProductTransaction = (ProductTransaction *) prodUpdate.getProductTransaction();
			Product *pNextProduct = (pNextProductTransaction ? pNextProductTransaction->getProduct() : NULL);

			if ( !pNextProductTransaction || !pNextProduct )
				break;

			// FT-811 printing expanded text
			if ( stateMachine == aimWaitingForProductUpdate && pNextProduct->isDummyProduct() )
			{
				// printing individual labels
				pCurrentProductTransaction = pNextProductTransaction;
				postImagePrinters();

				break;
			}

			// Changed the test against the min weight to print from > to <=. This alloweed the situation of min weigh to print being 0
			if ((pNextProduct->isRandomWeight() || pNextProduct->isByCountWithWeight()) && pNextProduct->getCurrentTransaction() && 
				pNextProduct->getCurrentTransaction()->getTotalNetWeight() <= Configuration::getInstance()->getMinWeightToPrint())
				isWeightRequired = true;
			else
				isWeightRequired = false;

			if ( !isWeightRequired )
			{
				HobString desc = pNextProduct->getDescription();
				desc.stripFontSequences();
				pNextProduct->setDescription(desc);
			}

			if (isMasterProductProdNum(pNextProduct->getProductNum()))
			{
				if (savedMasterLabelType)
				{
					delete savedMasterLabelType;
					savedMasterLabelType = 0;
				}
				currentItemToPrint = 1;
			}

			// If we're adding the first item to the list, check and see if it's barcode information would fit in the Accumulate Item label's barcode
			if ( !isMasterProductProdNum( pNextProduct->getProductNum() ) && ptm && ptm->getTransactionCount() == 0 )
			{
				bool doesItFit = checkIfItemBarcodeDataFits(pNextProduct);
				if (!doesItFit)
				{
					UIDialogMsg dialog(this, SessionDev, SessionDevAccumulateItemsFirstItemBarcodeFitError, HfcResponse::RESPONSESTYLE_OK, false);
					dispatchEvent(&dialog);
					break;
				}
			}

			if ( cfgEnableItemStripMode && !isWeightRequired )  // FHA
			{ // Item strip mode
				if (!startedPrintingLabels)
				{
					bool doWeAddThisToList = false;
					UINT8 IsThereRoomOnLabel = 0;

					Debug::trace("AccumulateItemsMode", HobString("ProductUpdate received:  transaction=") + HobString(Hex::toString((long long) pNextProductTransaction)));

					// If we received a transaction, add it to the list. But first ck for butt cut label stock and if item will fit on label
					if (ptm && pNextProductTransaction)
					{
						if ( actualInstalledLabelSizeInLines == 0 )
						{ // Undefined label stock
							doWeAddThisToList = false;
							UIDialogMsg dialog(this, SessionDev, SessionDevAccumulateItemsUndefinedLabelType, HfcResponse::RESPONSESTYLE_OK, false);
							dispatchEvent(&dialog);
						}
						else
						{
							// We can now print multiple labels, so the max number of items is now what it used to be just for continuous, in internal config (which happens to be 99)
							if (ptm->getTransactionCount() >= Configuration::getInstance()->toIntCfgI(CfgInternal::ACCUMITEMSITEMCOUNTRENDERNOTIFY))
							{
								UIDialogMsg dialog(this, SessionDev, SessionDevAccumulateItemsMaxItemsExceeded, HfcResponse::RESPONSESTYLE_OK, false);
								dispatchEvent(&dialog);
								doWeAddThisToList = false;
							}
							else
							{
								IsThereRoomOnLabel = isThereRoomOnLabel((ptm->getTransactionCount() + 1));
								/* We don't need to check this anymore now that we can split it across labels
								if ( IsThereRoomOnLabel == 1 )
								{ // too many items would be in the list
									UIDialogMsg dialog(this, SessionDev, SessionDevAccumulateItemsRenderingManyProducts, HfcResponse::RESPONSESTYLE_OK, false);
									dispatchEvent(&dialog);
									doWeAddThisToList = false;
								}
								else */ if (IsThereRoomOnLabel == 2)
								{ // no label type for itemized receipt on installed label stock size
									UIDialogMsg dialog(this, SessionDev, SessionDevAccumulateItemsNoValidLabelType, HfcResponse::RESPONSESTYLE_OK, false);
									dispatchEvent(&dialog);
									doWeAddThisToList = false;
								}
								else
									doWeAddThisToList = true;
							}
						}
						
						if ( doWeAddThisToList )
						{
							ptm->addProductTransaction(pNextProductTransaction);
						}
						Debug::trace("AccumulateItemsMode", "ProductUpdate handling finished");
					}
				}
			}
			else
			{
				// Must be individual labels mode
				if ( ptm && pNextProductTransaction && stateMachine == aimWaitingForProductUpdate && !isWeightRequired )
				{  
					if ( isMasterProductProdNum(pNextProduct->getProductNum()) )
						// This is the first update containing the master transaction record, create the item list
						ptm->addProductTransaction(pNextProductTransaction);
					else
					{
						// Product to add to the list, print first then add to list when print finished
						if ( cfgEnableAccumItemsTotalsOnly )  // FHA
							// Printing totals label only. Add to list now because no print complete message will be received.
							ptm->addProductTransaction(pNextProductTransaction);
						else
						{
							// printing individual labels
							pCurrentProductTransaction = pNextProductTransaction;
							postImagePrinters();
						}
					}
				}
				else
				{
					// This is an error condition!!
					Debug::trace("AccumulateItemsMode.Error", "Missed Product Update");
					if ( !isWeightRequired )
						sendProductComplete(pNextProductTransaction, false);
				}
			}
			break;
		}

		case ProductRemoveSingleAccumulatedItem:
		{
			int i;
			if (!startedPrintingLabels)
			{
				ProductRemoveAccumItemMsg removeIt(recvBuffer);
				int index = removeIt.getIndexOfItemToRemove();
				Debug::trace("AccumulateItemsMode", HobString("handling ProductRemoveSingleAccumulatedItem - removed transaction, wanting to remove trans at index: ") + HobString(Integer::toString(index)));

				// An index of 0 being passed in the ProductRemoveAccumItemMsg means to remove the last item in the list,
				// without the user actually selecting the last one in the list to remove.

				if ( index == 0 )
				{ // check for empty list
					i = ptm->getTransactionCount();
					if ( i <= 0 )
					{ // list empty
						UIDialogMsg dialog(this, SessionDev, SessionDevAccumulateItemsEmptyList, HfcResponse::RESPONSESTYLE_OK, false);
						dispatchEvent(&dialog);
						i = 0;
					}
					else
					{ // remove last entry in a list with at least 1 entry
						index = i;
					}
				}
 				ProductTransaction *removedTrans = 0;
 				if ( index > 0 && ptm)
 					removedTrans = ptm->removeItemFromList(index);
				if ( index > 0 && removedTrans)
				{
					Debug::trace("AccumulateItemsMode", HobString("handling ProductRemoveSingleAccumulatedItem - removed transaction, sending ProductComplete failed for: ") + HobString(Hex::toString((long long) removedTrans)));
					// Tell the operator they have removed a transaction
					UIDialogMsg dialog(this, SessionDev, SessionDevAccumulateItemsItemRemoved, HfcResponse::RESPONSESTYLE_OK, false);
					dispatchEvent(&dialog);
					sendProductComplete(removedTrans, false);
				}


				// If we removed the first item in the list, and there are still more items in the list, we will now check to see if the new first item
				// in the list can set up the barcode information without overflow, when fieldOffsetCfgAccumBarcodeUseFirstItem is set
				if ( 1 == index && ptm && ptm->getTransactionCount() > 0 )
				{
					ProductTransaction *newFirstItemTransaction = ptm->getTransactionPositionNum(1);
					Product *newFirstItem = (newFirstItemTransaction) ? newFirstItemTransaction->getProduct() : 0;
					if (newFirstItem)
					{
						bool doesItFit = checkIfItemBarcodeDataFits(newFirstItem);
						if (!doesItFit)
						{
							UIDialogMsg dialog(this, SessionDev, SessionDevAccumulateItemsFirstItemBarcodeFitWarning, HfcResponse::RESPONSESTYLE_OK, false);
							dispatchEvent(&dialog);
						}
					}
				}


				Debug::trace("AccumulateItemsMode", "handling ProductRemoveSingleAccumulatedItem - end");
			}
			break;
		}

		case ProductRemoveAllAccumulatedItems:
		{
			if (ptm && !startedPrintingLabels)
			{
				Debug::trace("AccumulateItemsMode", "handling ProductRemoveAllAccumulatedItems - start");
				ProductTransaction *removedTrans = 0;
				int i = ptm->getTransactionCount();
				while (i > 0)
				{
					// Keep removing the last one from the list, until they're all gone.
					removedTrans = ptm->removeItemFromList(0);
					if (removedTrans)
					{
						Debug::trace("AccumulateItemsMode", HobString("handling ProductRemoveAllAccumulatedItems - removed transaction, sending ProductComplete failed for: ") + HobString(Hex::toString((long long) removedTrans)));
						sendProductComplete(removedTrans, false);
					}
					i--;
				}
				Debug::trace("AccumulateItemsMode", "handling ProductRemoveAllAccumulatedItems - end");
			}
			break;
		}

		case ProductRequestReadAllAccumulatedItems:
		{
			// The Session is asking for all the current item transactions in the ProductTransactionMaster.
			// We'll just send back a pointer the ProductTransactionMaster; Session can then just iterate it.
			if (!startedPrintingLabels)
			{
				ProductReadAllAccumulatedItemsMsg readAllMsg(this, ptm);
				dispatchEvent(&readAllMsg);
			}
			break;
		}

		// Operator has pressed the Print button to print the Accumulated Item list or just totals label.
		case ProductStartPrintAllLabels:
		{
			if (!ptm || ptm->getTransactionCount() == 0)
			{
				UIDialogMsg dialog(this, SessionDev, SessionDevAccumulateItemsNoProductsInList, HfcResponse::RESPONSESTYLE_OK, false);
				dispatchEvent(&dialog);
			}
			else if (!startedPrintingLabels)
			{
				Debug::trace("AccumulateItemsMode", "ProductStartPrintAllLabels received");

				stateMachine = aimWaitingForProductUpdate;
				Debug::trace("AccumulateItemsMode.StateMachine", "aim - handling ProductStartPrintAllLabels - stateMachine = aimWaitingForProductUpdate");

				pCurrentProductSmartPtr = Util::CSmartPtr<Product>();
				currentItemToPrint = 1;
				if (savedMasterLabelType)
				{
					delete savedMasterLabelType;
					savedMasterLabelType = 0;
				}

				anotherLabelNeedsPrinted = false;
				processStartPrinting();
			}

			Debug::trace("AccumulateItemsMode", "ProductStartPrintAllLabels handling finished");
			break;
		}

		case ProductDontUpdateProductionPlanning:
		{
			Debug::trace("AccumulateItemsMode", "ProductDontUpdateProductionPlanning received");
/*
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
*/
			Debug::trace("AccumulateItemsMode", "ProductDontUpdateProductionPlanning handling finished");
			break;
		}

		default:
		{
			break;
		}
	}					
}

void AccumulateItemsMode::handlePrinterMessage(PrEvent prEvent, const char *recvBuffer)
{
	switch (prEvent)
	{
		case PrStatus: 
		{						
			Debug::trace("AccumulateItemsMode", "PrStatus received");

			PrinterStatus prStatus(recvBuffer);
			//prStatus.show();
			PrinterType printerType = prStatus.getPrinterType();

			// Accumulate items only works with the primary/internal printer!!  We do not print to an second EPP or Zebra printer!!
			if ( printerType == FirstPrinter )
			{
				outOfMedia = (prStatus.isOutOfMedia()) ? true : false;
				//SOF-5190 handling of out of stock condition of FSSS
				if( !outOfMedia ) {
					outOfMedia = (prStatus.isOutOfStock()) ? true : false;
				}
				if (prStatus.isHeadDisengaged() == true)
				{
					Debug::trace("AccumulateItemsMode", "PrStatus: isHeadDisengaged is true");

					updatePrinterHeadUp(printerType);
					
					if (stateMachine != aimPrinterHeadUp)
					{
						//stateMachine = aimPrinterHeadUp;
						Debug::trace("AccumulateItemsMode.StateMachine", "stateMachine = aimPrinterHeadUp");

						//if (pCurrentProductTransaction)
						//	productFailed();
					}
					Debug::trace("AccumulateItemsMode", "PrStatus: isHeadDisengaged handling finished");
				}
				else if (prStatus.isSizeCmdComplete() == true)
				{
					Debug::trace("AccumulateItemsMode", "PrStatus: isSizeCmdComplete is true");

					updateSizeCmdComplete(printerType);

					Debug::trace("AccumulateItemsMode", "PrStatus: isSizeCmdComplete handling finished");
				}
			}
			break;
		}	
		case PrStockMetrics:
		{
			Debug::trace("AccumulateItemsMode", "PrStockMetrics received");
			if (Configuration::getInstance()->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
			{
				actualInstalledLabelSizeInLines = Configuration::getInstance()->toIntCfgI("fakePrinterLabelLength");
			}
			else
			{
				PrinterStockMetrics prMetrics(recvBuffer);
				PrinterType printerType = prMetrics.getPrinterType();

				// Accumulate items only works with the primary/internal printer!!  We do not print to an second EPP or Zebra printer!!
				if ( printerType == FirstPrinter )
				{
					if ((prMetrics.getLabelLengthInLines() > 0 || prMetrics.isContinuousStock()) && !isPrinterHeadUp())
					{
						if (startedPrintingLabels)
						{
							startedPrintingLabels = false;  // reset before restarting the print, otherwise the print won't restart

							EvolMessage msg(ProductDev, ProductStartPrintAllLabels);
							dispatchEvent(&msg, this);
						}
					}
					actualInstalledLabelSizeInLines = prMetrics.getLabelLengthInLines();
					//actualInstalledLabelSize = prMetrics.getLabelLength(actualInstalledLabelSizeInLines);
				}
			}
			break;
		}
		case PrLabelReady: 
		{						
			Debug::trace("AccumulateItemsMode.StateMachine", HobString("aim - handling PrLabelReady - stateMachine upon entry is: ") + HobString(Integer::toString(stateMachine)));
			break;
		}
        case PrNomSize:
        {
			if (Configuration::getInstance()->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
			{
				actualInstalledLabelSizeInLines = Configuration::getInstance()->toIntCfgI("fakePrinterLabelLength");
			}
			else
			{
                PrinterNomSize nomSize(recvBuffer);
				PrinterType printerType = nomSize.getPrinterType();

				// Accumulate items only works with the primary/internal printer!!  We do not print to an second EPP or Zebra printer!!
				if ( printerType == FirstPrinter )
				{
					Debug::trace("AccumulateItemsModeMode.PrinterMessage", "Got PrNomSize message. PrinterType = " +
							inttostring(nomSize.getPrinterType()) + ", ActualSize = " + inttostring(nomSize.getActualSize()));

					actualInstalledLabelSizeInLines = nomSize.getActualSize();
				}
			}
            break;
        }
		case PrLabelComplete: 
		{						
 			Debug::trace("AccumulateItemsMode.StateMachine", HobString("aim - handling PrLabelComplete - stateMachine upon entry is: ") + HobString(Integer::toString(stateMachine)));

			PrinterLabelComplete prLabelComplete(recvBuffer);
			PrinterType printerType = prLabelComplete.getPrinterType();

			// Accumulate items only works with the primary/internal printer!!  We do not print to an second EPP or Zebra printer!!
			if ( printerType == FirstPrinter )
			{
	//			PrinterType printerType = prLabelComplete.getPrinterType();
				ProductTransaction *pPT = (ProductTransaction *)prLabelComplete.getProductTransaction();
				Product *prod = (pPT ? pPT->getProduct() : NULL);
				bool printOk = prLabelComplete.isPrintOk();
				Debug::trace("AccumulateItemsMode", HobString("aim - handling PrLabelComplete - printOk=") + HobString(Bool::toString(printOk)));

				// FT-811 printing expanded text
				if (( prod ) && ( prod->isDummyProduct() ))
				{
					sendProductComplete(pPT, printOk);

					stateMachine = aimWaitingForProductUpdate;
					Debug::trace("AccumulateItemsMode.StateMachine", "stateMachine = mwmWaitingForProductUpdate");

					if (getNewProduct() == true)
					{
						changeProduct();
						preImagePrinters();
					}

					break;
				}

				if (cfgEnableItemStripMode)  // FHA
				{
					// Item strip mode
					if (anotherLabelNeedsPrinted && printOk)
						processStartPrinting();
					else
						finishHandlingPrinting(printOk);
				}
				else if ( printOk )
				{
					bool wasPrinting = (stateMachine == aimPrintingPrimary || stateMachine == aimPrintingPrimaryPrintingSecondary);

					// still printing individual labels and adding them to the list
					if ( ptm && pPT && wasPrinting )
					{
						// have a master transaction record and a valid product
						ptm->addProductTransaction(pPT);

						// FT-674: update ui with item list
						ProductReadAllAccumulatedItemsMsg readAllMsg(this, ptm);
						dispatchEvent(&readAllMsg);
					}

					// If Print key was hit finish up printing this item list otherwise assume a printer issue and ignore hopeing to recover later
					if ( startedPrintingLabels )
					{
						// Print key was hit, printing the totals label finish up printing this item list
						stateMachine =  aimFinishingPrintingLabels;
						finishHandlingPrinting(printOk);
					}
					// Individual label mode, after the label is printed OK add it to the item list
					else if ( wasPrinting )
					{
						stateMachine =  aimWaitingForProductUpdate;
					}
				}
				// FT-672 handle a printer error
				else
				{
					// If Print key was hit finish up printing this item list otherwise assume a printer issue and ignore hopeing to recover later
					if ( startedPrintingLabels )
					{
						// Print key was hit, printing the totals label finish up printing this item list
						stateMachine =  aimFinishingPrintingLabels;
						finishHandlingPrinting(printOk);
					}
				}

				Debug::trace("AccumulateItemsMode.StateMachine", "aim - handling PrLabelComplete complete");
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


void AccumulateItemsMode::finishHandlingPrinting(bool printOk )
{
	if (ptm && !isPrinterHeadUp() && !outOfMedia)
	{
		ProductTransaction *masterTrans = ptm->getMasterProductTransaction();
		int transGroup = 0;
		if (masterTrans)
			transGroup = masterTrans->getTransactionGroupNumber();

		// Need to spin through all transactions in the transaction list and send product complete messages to the Session,
		// so they can be processed as "printed).

		ProductTransaction *trans = ptm->getFirstTransaction();
		while (trans)
		{
			if (printOk)
			{
				Debug::trace("AccumulateItemsMode", HobString("aim - handling PrLabelComplete - before sending product complete for this transaction: ") + HobString(Hex::toString((long long) trans)));
				trans->setNumOfLabelsPrinted(trans->getNumOfLabelsToPrint());
				trans->setTransactionGroupNumber(transGroup);
				sendProductComplete(trans, true);
			}
			else
			{
				Debug::trace("AccumulateItemsMode", HobString("aim - handling PrLabelComplete - printOK is false - before sending product incomplete for this transaction: ") + HobString(Hex::toString((long long) trans)));
				sendProductComplete(trans, false);
			}
			trans = ptm->getNextTransaction();
		}

		// Remove all other transactions, even the master trans, from the itemized list (Don't delete them, the mode doesn't OWN them).
		ptmPrev.push_back( ptm );
		ptm = new ProductTransactionMaster();

		if ( 8 < ptmPrev.size() )
		{
			delete ptmPrev.front();
			ptmPrev.pop_front();
		}

		startedPrintingLabels = false;

		setupCfgParams();

		// Let the Session/ProductManager know we are done with the current Itemized List.
		ProductFinishedPrintingAllLabelsMsg finishedMessage(0, printOk);
		dispatchEvent(&finishedMessage);
	}

	Debug::trace("AccumulateItemsMode.StateMachine", "aim - finishHandelingPrinting - stateMachine = aimWaitingForProductUpdate");
	if ( stateMachine != aimFinishingPrintingLabels && cfgEnableItemStripMode )
		stateMachine = aimWaitingForProductUpdate;  // Change state if in Item Strip mode only
}


void AccumulateItemsMode::productFailed()
{
	Debug::trace("AccumulateItemsMode", "entering productFailed()");
	sendProductComplete(pCurrentProductTransaction, false);
	pCurrentProductTransaction = NULL;

	unlockWeigher();
}


LabelType *AccumulateItemsMode::createFakeLabel(LabelType *origLabelType)
{
	Debug::trace("AccumulateItemsMode", "Creating fake label for report");

	anotherLabelNeedsPrinted = false;

	if (!ptm)
		return 0;

	// Create the label resource header
	HobartLabel *hl = const_cast<HobartLabel *>(origLabelType->getHobartLabel());
	if (!hl)
		return 0;

	ResourceHeader origHeader = hl->getResourceHeader();
	ResourceHeader newHeader(origHeader);
	Debug::trace("AccumulateItemsMode", "Created ResourceHeader");

	// Create the label stock descriptor
	LabelStockDescriptor *origLSD = hl->getLabelStockDescriptor();
	if (!origLSD)
		return 0;

	LabelStockDescriptor newLSD(*origLSD);
	Debug::trace("AccumulateItemsMode", "Created LabelStockDescriptor (width = " +
		util_string::inttostring(newLSD.getWidthDots()) + ", length = " + util_string::inttostring(newLSD.getLength()) + ")");

	// Create the forms list and the label forms. These label forms don't need to
	// be deleted because they are referenced by the forms list. This forms list
	// is copied (pointers duplicated) by the HobartLabel below, which when deleted
	// disposes of the forms pointed to by the elements of the list.
//	UINT16 labelWidth = newLSD.getWidthDots();
//	UINT16 leftMargin = 0, rightMargin = 0;
//	UINT16 topMargin = 0, bottomMargin = 0;
//	UINT16 horiSeparation = 0, vertSeparation = 0;

	HobartLabel::FormsListType list;
	Debug::trace("AccumulateItemsMode", "Created empty FormList");
	int cumulativeVerticalOffset = 0;
	bool found177 = false;
	int finalLength = 0;

	hl->startAtListTop();
	FormRecord *origForm = hl->getFormFromList();

	// Item receipt mode with continuous labels
	if ( actualInstalledLabelSizeInLines == Configuration::getInstance()->toIntCfgI(CfgInternal::DIMBLENGTHCONTINUOUS) || needToPrintMultipleLabels )
	{
		if ( cfgEnableItemStripMode || !cfgEnableAccumItemsTotalsOnly )
		{
			int origVertical177 = 0;
			int origSize177 = 90;

			while (origForm)
			{
				FormRecord *newForm = new FormRecord(*origForm);
				if (origForm->getFieldType() == INDIRECT_LABEL_REFERENCE && origForm->getVerticalJustify() == 177)
				{
					// If we've already handled embedded form 177, then we can skip any other one found in the original label type
					if (!found177)
					{
						found177 = true;
						origVertical177 = origForm->getOriginVertical();
						origSize177 = origForm->getSizeVertical();

						int labelItems = (actualInstalledLabelSizeInLines == Configuration::getInstance()->toIntCfgI(CfgInternal::DIMBLENGTHCONTINUOUS)) ? ptm->getTransactionCount() : numItemsForThisLabel(origLSD->getLength());
						int transactionItemsLeft = ptm->getTransactionCount() - currentItemToPrint + 1;
						int numOfItemsForThisLabel = (transactionItemsLeft > labelItems) ? labelItems + 1 : transactionItemsLeft;

						for (int i = 0; i < numOfItemsForThisLabel; i++)
						{
							newForm->setVerticalJustify( (newForm->getVerticalJustify() * 100) + currentItemToPrint++ );
							list.push_back(newForm);
							cumulativeVerticalOffset += 90;  // origSize177;  // +14;
							newForm = new FormRecord(*origForm);
							newForm->setOriginVertical(origVertical177 + cumulativeVerticalOffset);
						}

						if (transactionItemsLeft > labelItems)
						{
							anotherLabelNeedsPrinted = true;

							int constantTextHeight = 20;
							newForm = new FormRecord(0, origVertical177 + cumulativeVerticalOffset + 20, WideLabelWidth, constantTextHeight, Fixed | PreImage | TextEntity, CONSTANT_TEXT, CenterJustify, 0, 0, 0);
							hfc::MultiLanguageText internationalize;
							std::string cont = internationalize("continued");
							newForm->setData((UINT32*)cont.c_str());
							newForm->setFamilyName((const UINT8*)"Hobart");
							newForm->setPointSize(6);
							list.push_back(newForm);
							cumulativeVerticalOffset += constantTextHeight + 1;
							finalLength = newForm->getOriginVertical() + newForm->getSizeVertical() + 21;
							newLSD.setLength(finalLength);

							break;  // break out of the 'while origForm' loop - We don't want to add embedded label 178 to this label, since we need another label to get everything printed.
						}
					}
				}
				else
				{
					if (found177)
					{
						newForm->setOriginVertical(origVertical177 + cumulativeVerticalOffset);
						cumulativeVerticalOffset += newForm->getSizeVertical() + 1;
						finalLength = newForm->getOriginVertical() + newForm->getSizeVertical() + 1;
					}
					list.push_back(newForm);
				}

				origForm = hl->getFormFromList();
			}
			if (found177)
				newLSD.setLength(finalLength);
		}

		// FT-673 no item list when totals only label
		else
		{
			while (origForm)
			{
				bool addToList = true;

				// found item detail, skip it...
				if (origForm->getFieldType() == INDIRECT_LABEL_REFERENCE && origForm->getVerticalJustify() == 177 )
					addToList = false;

				if ( addToList )
				{
					FormRecord *newForm = new FormRecord(*origForm);
					list.push_back(newForm);
				}

				origForm = hl->getFormFromList();
			}
		}
	}

	// Item receipt mode with Butt Cut lables installed, no longer have to adjust for the Imbedded 177 label forms
	else
	{
		while (origForm)
		{
			FormRecord *newForm = new FormRecord(*origForm);
			list.push_back(newForm);
			origForm = hl->getFormFromList();
		}
	}

	// Create the Hobart Label based on the header, label stock descriptor, 
	// and list of forms. There's no need to delete it since it is retained
	// and disposed of by the LabelType instance below.
	HobartLabel* hLabel = new HobartLabel(newHeader, newLSD, list);
	Debug::trace("AccumulateItemsMode", "Created HobartLabel (width = " +
		util_string::inttostring(hLabel->getLabelStockDescriptor()->getWidthDots()) +
		", length = " + util_string::inttostring(hLabel->getLabelStockDescriptor()->getLength()) + ")");

	LabelType *fakeLabel = new LabelType((HobRecordNum)0);        // Assign it a zero number to not have a conflict
	fakeLabel->setHobartLabel(hLabel);
	fakeLabel->setDeptNum(origLabelType->getDeptNum());
	fakeLabel->setLabelTypeNum(origLabelType->getLabelTypeNum());
	fakeLabel->setErrorCode(RECORD_OK);
	fakeLabel->setLabelMediaError(LabelOk);
	fakeLabel->setDBAccess(origLabelType->getDBAccess());
	fakeLabel->generateEmbeddedLabels();
	Debug::trace("AccumulateItemsMode", "Created fake label (width = " +
		util_string::inttostring(fakeLabel->getWidthDots()) +
		", length = " + util_string::inttostring(fakeLabel->getLength()) + ")");

	return fakeLabel;
}

void AccumulateItemsMode::operator=(const AccumulateItemsMode &thisMode)
{
	//	This function eliminates the C4512 warning.
	thisMode;
}


bool AccumulateItemsMode::isMasterProductProdNum(HobRecordNum prodNum) const
{
	return (prodNum == 999999);
}


bool AccumulateItemsMode::checkIfItemBarcodeDataFits(Product *productToCheck) const
{
	bool result = true;

	Product *masterProd = ptm->getMasterProduct();
	if ( !masterProd )
		return false;

	// Now need to set up the barcode string to render.  This gets done in the master transaction.
	if ( cfgAccumBarcodeUseFirstItem )
	{
		BarCodeNumber bcn = productToCheck->getBarCodeNum();
		bcn.allowswappingOfEanCodeAndProductNumber(true);
		masterProd->setBarCodeNum(bcn);
		productToCheck->setBarCodeNum(bcn);

		/*   Code prior to PJR 285 - allow multiple barcodes per label
		BarCodeNumber bcn = productToCheck->getBarCodeNum();
		bcn.setFormatString( EAN13, Configuration::getInstance()->toStringCfg(fieldOffsetCfgAccumItemsEANFormatString) );

		LabelType *masterLabelType = masterProd->getLabelType(FirstPrinter);
		short masterBarcodeType = (masterLabelType) ? masterLabelType->getBarcodeType() : UPCA;  // This should never happen since the master product should always have a label type, but just in case, don't want to crash.
		bcn.setBarcodeType(masterBarcodeType);

		// If we're using the barcode data from the first product in the list, then check if the barcode types are different between
		// the first product in the list and the dummy master product.  If they are different, then we will attempt to fill in the
		// AccumulateItem's label's barcode with substitute data from the first item.  For example, if the AccumulateItem's barcode is
		// EAN, but the first item's barcode is UPC, we will attempt to set the EAN code in the AccumulateItem's label's barcode with
		// the UPC product num from the first product in the list.

		LabelType *firstItemLabelType = productToCheck->getLabelType(FirstPrinter);
		short firstItemBarcodeType = (firstItemLabelType) ? firstItemLabelType->getBarcodeType() : UPCA;  // This should never happen since the master product should always have a label type, but just in case, don't want to crash.

		if (firstItemBarcodeType != masterBarcodeType)
		{
			if (masterBarcodeType == UPCA || masterBarcodeType == UPCD3 || masterBarcodeType == UPCE)
			{
				// First item is EAN barcode, but AccumulateItem's label is UPC barcode, so try to put the first item's ean code in the UPC product number field
				result = bcn.setUPCNumberFromEANCode();
			}
			else
			{
				// First item is UPC barcode, but AccumulateItem's label is EAN barcode, so try to put the first item's UPC product number in the ean code field
				result = bcn.setEANCodeFromUPCNumber();
			}
		}
		*/
	}

	return result;
}


void AccumulateItemsMode::setupCfgParams()
{
	Configuration * cfg = Configuration::getInstance();
	if (cfg)
	{
		cfgEnableItemStripMode = cfg->toBoolCfg(fieldOffsetCfgEnableItemStripMode);
		cfgEnableAccumItemsTotalsOnly = cfg->toBoolCfg(fieldOffsetCfgEnableAccumItemsTotalsOnly);
		cfgAccumBarcodeUseFirstItem = cfg->toBoolCfg(fieldOffsetCfgAccumBarcodeUseFirstItem);
	}
	else
	{
		cfgEnableItemStripMode = true;
		cfgEnableAccumItemsTotalsOnly = false;
		cfgAccumBarcodeUseFirstItem = false;
	}
}


bool AccumulateItemsMode::determineIfNeedToPrintMultipleLabels()
{
	if (!ptm)
		return false;

	if ( actualInstalledLabelSizeInLines == Configuration::getInstance()->toIntCfgI(CfgInternal::DIMBLENGTHCONTINUOUS) )
		return false;

	Debug::trace("AccumulateItemsMode", "determineIfNeedToPrintMultipleLabels start");

	Product *masterProd = ptm->getMasterProduct();
	if (!masterProd)
		return false;

	LabelType *masterLabelType = masterProd->getLabelType(FirstPrinter);
	if (!masterLabelType)
		return false;

	// Get the label resource header
	HobartLabel *hl = const_cast<HobartLabel *>(masterLabelType->getHobartLabel());
	if (!hl)
		return false;

	// Create the label stock descriptor
	LabelStockDescriptor *lsd = hl->getLabelStockDescriptor();
	if (!lsd)
		return false;

	if (ptm->getTransactionCount() > numItemsForThisLabel(lsd->getLength()))
		return true;

	return false;
}

int AccumulateItemsMode::numItemsForThisLabel(int labelLength)
{
	switch (labelLength)
	{
		case Length2_375_Lines:
			return 2; break;
		case Length3_000_Lines:
			return 4; break;
		case Length4_000_Lines:
			return 6; break;
		case Length6_000_Lines:
			return 10; break;
		case Length8_000_Lines:
			return 14; break;
		default:
			break;
	}
	return 0;
}

// collects and formats all the information about the transaction for the getBarcodeFromCloud.py script
// serializes as "countryCode,storeNbr,checkDigit,serverAddresss,transactionDate, description(empty), [partialPrice, pieces, productNbr, ....], totalItems, totalPrice"
std::string AccumulateItemsMode::createData()
{

	std::string data;
	std::string productData = "[";
	HobDateTime* hdt = new HobDateTime();
	Product* itemProd = 0;
	int productSum = 0;

	data += "\"" + Configuration::getInstance()->toStringCfgI(CfgInternal::BARCODEREQUESTTIMEOUT) + ",";
	data += Configuration::getInstance()->getPrimaryCountryCode() + ",";
	data += Configuration::getInstance()->toStringCfg(fieldOffsetCfgStoreId); //storenbr
	data += ",";
	data += "0"; // check digit
	data += ",";
	data += Configuration::getInstance()->toStringCfg(fieldOffsetCfgBarcodeServerAddress); // server address

	// get date and convert to required format
	time_t now;
	time(&now);
	hdt->setCalendarTime(now);
	std::string date = hdt->convertToTransactionDateFormat();	
	int pos = 0;
	while ((pos = date.find("/", pos)) != std::string::npos) {
		date.replace(pos, 1, "-");
		pos += 1;
	}
	date.replace(date.find(","), 1, "T");
	date += "Z";

	data += "," + date + ",";  // date
	// end date

	ProductTransaction* itemTrans = ptm->getFirstTransaction();
	
	data += " "; 
	data += ","; // description - just send empty string (for now)

	if (itemTrans)
	{
		for (int i = 0; i < ptm->getTransactionCount(); i++)
		{
			itemProd = itemTrans->getProduct();

			Debug::trace("BarcodeCloud.createData", std::to_string(itemProd->getUnitPrice()));
			Debug::trace("BarcodeCloud.createData", std::to_string(itemProd->getProductNum()));
			Debug::trace("BarcodeCloud.createData", std::to_string(itemProd->getByCount()));

			float partialPrice = 0.0; // number of pieces * netweight or bycount
			int pieces = 0;

			if (itemProd->isRandomWeight())
			{
				pieces = 1;
				partialPrice = float(itemTrans->getTotalNetWeight()) / 100  * (float(itemProd->getUnitPrice()) / 10);
			}
			if (itemProd->isByCount() || itemProd->isFixedWeight())
			{
				pieces = itemProd->getByCount();
				partialPrice = pieces * itemProd->getUnitPrice();
			}
			productData += std::to_string(partialPrice) + ",";
			productData += std::to_string(pieces) + ",";

			if (i == ptm->getTransactionCount() - 1)
				productData += std::to_string(itemProd->getProductNum());
			else
				productData += std::to_string(itemProd->getProductNum()) + ",";

			productSum = productSum + pieces; 
			itemTrans = ptm->getNextTransaction();
		
		}
	}
	data += productData; // product data
	data += "],";
	data += std::to_string(productSum) + ","; // total items
	data += std::to_string(ptm->getMasterProductTransaction()->getTotalPrice()) + "\""; // total price
	
	delete hdt;
	return data; 
}
