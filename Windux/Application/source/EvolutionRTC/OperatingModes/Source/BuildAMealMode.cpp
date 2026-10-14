
#include "OpModeEvent.h"
#include "BuildAMealMode.h"

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

#include "ProductTransaction.h"
#include "ProductTransactionMaster.h"
#include "LabelType.h"
#include "ResourceHeader.h"
#include "LabelStockDescriptor.h"
#include "FormRecord.h"

#include "Messages/UIDialogMsg.h"

#include "Configuration.h"
#include "HfcPrinterStockMetrics.h"
#include "HfcWeigherStatus.h"
#include "ProductManager.h"
#include "ProductStatusMsg.h"

using namespace std;

#include "Integer.h"
#include "Hex.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int BuildAMealMode::dataType = 0;
std::string BuildAMealMode::dataTypeName("BuildAMealMode"); 
#endif

BuildAMealMode::BuildAMealMode(Product *pCurrentProduct,
								 //WeigherManager *pWeigherManager,
								 //PrinterManager *pFirstPrinter,
								 //PrinterManager *pSecondPrinter,
								 //PrinterManager *pThirdPrinter,
								 //LabelApplierManager *pLabelApplierMananger,
								 //WrapperManager *pWrapperManager) :	AccumulateItemsMode(
								Hobart::CEvolMsgReceiver *pWeigherManager,
								Hobart::CEvolMsgReceiver *pFirstPrinter,
								Hobart::CEvolMsgReceiver *pSecondPrinter,
								Hobart::CEvolMsgReceiver *pThirdPrinter,
								Hobart::CEvolMsgReceiver *pLabelApplierMananger,
								Hobart::CEvolMsgReceiver *pWrapperManager) :	AccumulateItemsMode(
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

	registerEvent(ProductDev, ProductSetMealWeight);
	registerEvent(ProductDev, ProductReadAccumulatedMasterTrans);
	setupCfgParams();
}

BuildAMealMode::~BuildAMealMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

///	emptyProductTransactionQ();
	deregisterAllEvents();
	ShutdownNicely();

	if (ptm)
	{
		delete ptm;
		ptm = 0;
	}
}				


void BuildAMealMode::handleProductMessage(ProductEvent productEvent, const char *recvBuffer)
{
	switch (productEvent)
	{
		// Operator has added the item to the Accumulated Item list
		case ProductUpdate:
		{
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

			HobString desc = pNextProduct->getDescription();
			desc.stripFontSequences();
			pNextProduct->setDescription(desc);

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

			break;
		}

		// Assign the weight to the meal
		case ProductSetMealWeight:
		{
			Debug::trace("BuildAMealMode.StateMachine", "ProductChange - handling ProductSetMealWeight");
			if (ptm)
			{
				HfcWeigherStatus weight(recvBuffer);
				ProductTransaction *masterTrans = ptm->getMasterProductTransaction();
				masterTrans->setTotalNetWeight(weight.getWeight());
			}
			break;
		}

		// Read the master trans for this accumulation so as to get the total price for the meal.
		// This is needed for Build-A-Meal, when sides are not free, so the total price gets updated on the customer display
		case ProductReadAccumulatedMasterTrans:
		{
			Debug::trace("BuildAMealMode", "ProductChange - handling ProductReadAccumulatedMasterTrans");
			if (ptm)
			{
				ProductStatusMsg masterTrans(this, ptm->getMasterProduct(), ptm->getMasterProductTransaction(), ProductStatusMsg::ProductTransactionUpdate);
				dispatchEvent(&masterTrans);
			}
			break;
		}


		default:
		{
			AccumulateItemsMode::handleProductMessage(productEvent, recvBuffer);
			break;
		}
	}					
}


void BuildAMealMode::setupCfgParams()
{
	//BuildAMeal has specific defaults for these parameters, not what they are set to in the Configuration db table
	cfgEnableItemStripMode = true;
	cfgEnableAccumItemsTotalsOnly = false;
	cfgAccumBarcodeUseFirstItem = true;
}
