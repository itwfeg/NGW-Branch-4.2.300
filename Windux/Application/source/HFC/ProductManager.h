/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "ProductDevEvent.h"
#include "Product.h"
#include "ProductTextCfg.h"

class SessionMessage;
class DBSession;
class Product;
class ProductTransaction;
class ProductTransactionMaster;
class HfcProductCompleteMsg;
class HfcProductPackageCompleteMsg;
class HfcProductLabelFormSizeMsg;
class HfcWeigherStatus;
class WriteSingleField;
class HfcProductUpdateValuesMsg;
class CutTestManager;
class RemotePrintRequest;
class RemotePrintTemplateRequest;


#include "DBProductsRM.h"
#include "DBProductStatisticsRM.h"
#include "DBProduct.h"
#include "DBQuery.h"

#include <list>

class PresentationMethod;
#if 0
class SoftKeyLists;
#endif

#include "DBCategory.h"

#include "DBProductStatistic.h"
class ProductStatistic;


#include "LabelType.h"
#include "OpModeEvent.h"

class GenericHfcObject;


#include "CSmartPtr.h"

namespace hfc
{
	class HFCManagerContainer;
	class Transaction;
}


class ProductManager : public DataManager
{
	public:

		/* The Product Manager will load the Product templates when the Session
		 * instantiates the Product Manager.
		 * todo - the Product templates could be static, since they are the same
		 *        for all Sessions.
		 */
		ProductManager();
		ProductManager(hfc::HFCManagerContainer *container);
		virtual ~ProductManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		void handleMessage(SessionMessage *msg);

		/* DataManager virtual methods. */
		Product* getFirstRecord() { return firstProduct(); }
		Product* getNextRecord()  { return nextProduct(); }
		EvolDeviceType getDevice() const { return ProductDev; }
		UINT8 getWriteEvent() const { return ProductWrite; }
		UINT8 getDeleteEvent() const { return ProductDelete; }
		UINT8 getDeleteAllEvent() const { return ProductDeleteAll; }

		Product *firstProduct();
		Product *nextProduct();
		int countAllProducts();
		int calculateAverageSpace();

		Product *getCurrentProduct();
		void setCurrentProduct(Product *prod);

		void clearCurrentProduct(bool notifyUI = false, bool returnToStartScreen = true);

		void handleDeleteAll();
		void handleDeleteAll(HobRecordNum deptNum);
		void handleDeleteAllCommodityClass();
		void handleSaveRecord(Product *recordToSave);


		void handleWeight(HfcWeigherStatus *weight);
		void printLabel();
		void startPrintingAllLabels();
		bool findPrintingProduct(const Product *prodToFind, Util::CSmartPtr<Product> &printingProduct);
		bool removeAPrintingProduct(const Product *prodToFind);
		bool clearPrintingProductsList();

		void checkForReportingClassError();
		bool hasLabelTypeChanged(short printerNum);
		bool setUpLabelTypes();
		void resetUpLabelTypes(OpModeEvent newMode);

		// Negative Side Effect: After the call to this routine, the area referred to by labelType may have been deleted.
		// The client is not informed of this side effect. The client must test the return value with LabelCorrected.
		// When LabelCorrected is returned, the client MUST call Product::getLabelType(printerNum)!
		LabelMediaReturn labelTypeErrorTest(LabelType *labelType, LabelType **correctedLabelType, short printerNum, HobRecordNum deptNumOfProduct = 0, bool forceErrorTestInAnyMode = false);



		void checkForPLUGraphicsError();
		void checkForGraphicError(short index);

		short getPrinterNumFromFieldOffset(short fieldCode);


		LabelMediaReturn labelMediaCheck();


		ProductStatistic *firstProductStatistic();
		ProductStatistic *nextProductStatistic();
		int countAllProductStatistics();
		void handleDeleteAllProductStatistic();
		void purgeOldProductStatistics(int daysToKeepStatistics);


		void assignCoolListToProduct(edb::DBCoolList *dbCoolList);

		void destroyQueriesAndIters();

		void applyTransactionTypeToUpcNumber();

		bool validateProductForPlatterTare();
		void handlePlatterTare(HfcWeigherStatus *weigherStatus);

		bool isWaitingForLoginBeforeAddToAccum() const; // FT-809
		bool isWaitingForLoginBeforeAddToProductionEntry() const;
		bool isWaitingForLoginBeforePrinting() const;
		bool isWaitingForLoginBeforeWriteSingleField() const;
		void cancelWaitingForLogin();
		void continueProcessingWriteSingleField();
		
		bool hasPrintingProduct() const;
		void setHasProductionEntryItems(bool);
		bool hasProductionEntryItems() const;
		void setHasAccumulateItems(bool);
		bool hasAccumulateItems() const;

		bool isPrintingRemoteLabel() const;

		Util::CSmartPtr<Product> getCurrentProductSmartPtr() const;
        CutTestManager *getCutTestManager();

		void assignCOOLTextToCurrentProduct(HobRecordNum currentTextNum, HobRecordNum newTextNum);

		int bamMaxSides;
		int bamSidesLeft;
		bool bamAllSidesFree;
		bool bamUnlimitedSides;

		void clearPick5ValueOutOfRange();

		void startRecordingSweetheartWeight(ProductTransaction *trans);
		void stopRecordingSweetheartWeight(bool writeSweetheartWeightToDbIfItExists = true);
		void handleSweetheartWeight(HfcWeigherStatus *weight);
		void donePostImagingSelfServeBarcodeLabelImage(int filenameCounter);
		
		void setSecondayLabelTypeEnable(bool state);
		bool getSecondayLabelTypeEnable(void);


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		edb::DBQuery<edb::DBProduct> *allProductsQuery;
		edb::DBQuery<edb::DBProduct>::const_iterator *allProductsIter;

		Product	*currentProduct;

		Util::CSmartPtr<Product> currentProductSmartPtr;
		Util::CSmartPtr<Product> currentProductSmartPtrSave;

		Util::CSmartPtr<Product> sweetheartWeightProductSmartPtr;
		ProductTransaction *sweetheartWeightTransaction;
		 
		Product *dummyProduct;

		bool currentProductCanAcceptWeight;
		bool productionEntryItemsAdded;
		bool accumulateItemsAdded;

		bool resubmitPrintJob;

		std::list< Util::CSmartPtr<Product> > printingProducts;

		bool isLargeMotion;

		bool waitingForStableWeightToPrint;

		bool waitingForLoginBeforeAddToAccum; // FT-809
		bool waitingForLoginBeforeAddToProductionEntry;		
		Product *productWaitingForProductionEntryLogin;
		bool waitingForLoginBeforePrinting;
		bool waitingForLoginBeforeWriteSingleField;
		WriteSingleField *productUpdateWaitingForLogin;

		bool saveOffReadInPluHasForcedByCount;			// SOF - 3510

		std::vector<int> mainLabelTypeNum;
		std::vector<int> altLabelTypeNum;
//		std::vector<int> altFamilyLabelTypeNum;
//		std::vector<int> activeLabelTypeNum;

		void init();

		void handleProductReadMessage(Product *msgProduct);
		void handleProductReadAndAddSideItemForBuildAMeal(Product *msgProduct);
		void addCurrentItemToAccumOrMeal();

		void handleProductWriteMessage(Product *modifiedProduct);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(Product *msgProduct = 0);
		void handleUpdateSupervisorTransaction();
//void handleProductSaveMessage(Product *msgProduct = 0);
		void handleProductRestore(SessionMessage *msg);
		void handleProductCreateMessage(Product *msgProduct);

		int determineRequiredAccessLevel(int fieldCode);
		void handleProductWriteSingleFieldMessage(SessionMessage *msg);

		void updateWeightAndNotify(HfcWeigherStatus *weight = 0, bool forceNotification = true);
		void updateBrowserAndCustomerDisplay(HfcProductUpdateValuesMsg & hpuvm);
		void handleProductReadCurrentTotalValuesMessage();
		void handleProductWriteCurrentTotalValuesMessage(SessionMessage *msg);
		bool checkPrinterStatuses();

		// Printing methods
		void handleProductPrintKey1(bool fppTestPrint = false);
		void handleProductPrintKey2();
		void handleProductPrintKey3();

		void handleProductComplete(HfcProductCompleteMsg *productCompleteMsg);
		void handlePackageComplete(HfcProductPackageCompleteMsg * packageCompleteMsg);
		void handleCancelPrinting();
		//FT1001 void resendProductChange();

		void handleProductTree();
		void handleProductGetByDescription(SessionMessage *msg);
		void handleProductReadLifeDataMessage();

		void handleProductShowTextLinks(SessionMessage *msg);
		void handleProductReadSupportRecord(SessionMessage *msg);
		void handleProductReadSupportData(SessionMessage *msg);
		void handleProductWriteSupportData(SessionMessage *msg);
		void handleProductChangeSupportData(SessionMessage *msg);
		void handleProductReadNutriLabelMessage();
        bool hasNutrifactsLabelDataMissing();
		void handleProductToggleLabelTypeMessage(SessionMessage *msg);
		void handleProductToggleSecLabelTypeMessage(SessionMessage *msg);

		void handlePrintProductSupportLabel(ProdSupportTextType supportType, ProductTextPrinterChoices whichPrinter, HobRecordNum whichLabelNum);

		void updateProductionPlanningApplication(ProductTransaction *prodTransaction);
		Util::CSmartPtr<std::string> buildProductionPlanningURI(const std::string& uri, hfc::Transaction * transaction, bool increment = true);
		Util::CSmartPtr<std::string> buildProductionPlanningURI(const std::string& uriString, HobRecordNum plu, HobWeight weight, int quantity);
		std::string buildProductionPlanningJSONDocument(ProductTransaction *transaction, bool increment = true, ProductTransaction *masterTrans = NULL);
		std::string buildProductionPlanningJSONDocument(ProductTransactionMaster *transactions, bool increment = true);
		void handleProductTransactionTypeChanged();
		void handleProductLabelFormSize(HfcProductLabelFormSizeMsg *labelFormSizeMsg);
		
		void saveCOOLInfoToProduct(short fieldCode);

		// Remote Print methods
		int handleRemotePrintProduct(RemotePrintRequest* remotePrintRequest);
		int handleRemotePrintTemplate(RemotePrintTemplateRequest *rptr);
		int handleRemoteReadProductMessage(Product *msgProduct);
		int handleRemotePrintStartPrinting();
		void handleRemoteProductComplete(HfcProductCompleteMsg *productCompleteMsg);
		void remotePrintReturnMessage(int returnVal, int labelsPrinted);

		void handleRemoteByCountUpdate(int newByCount);
		void handleRemoteUnitPriceUpdate(HobPrice newPrice);
		int handleRemoteLabelUpdate(HobRecordNum newLabelTypeNum);
		int handleRemoteLabelSingleFieldMessage(SessionMessage *msg);

		void clearRemotePrintProduct();
		int checkForRemoteSizingStatus(LabelType *labelType, bool requiresExactFit);
		int returnRemotePrintError(int errorNum);

		static const bool REMOTE_PRINT_SUPPORT_BYCOUNT = false;
		static const bool REMOTE_PRINT_SUPPORT_UNITPRICE = false;
		static const bool REMOTE_PRINT_SUPPORT_CHANGE_LABEL = false;
		static const bool REMOTE_PRINT_SUPPORT_EXPANDED_TEXT = false;


		PresentationMethod *currentPresentationMethod;
		short currentPresentationMethodDataType;

#if 0
		SoftKeyLists *currentGridData;

		void addDataToSoftKeyList(SoftKeyLists *currentGridData, DBQuery<DBProduct> *productsQuery, DBQuery<DBCategory> *categoriesQuery = 0);
		void handleProductFindSoftKeysBySearchString(SessionMessage *msg);
		void handleProductReadMostCommonSoftKeyList();
		void handleProductReadSoftKeyList(SessionMessage *msg);
#endif
		void handleProductReadAllProductGraphics();

		edb::DBQuery<edb::DBProductStatistic> *allProductStatisticsQuery;
		edb::DBQuery<edb::DBProductStatistic>::const_iterator *allProductStatisticsIter;

		void handleRestoreProductStatistic(ProductStatistic *restoreStatistic);

		void destroyProductsQueryAndIter();
		void destroyProductStatisticsQueryAndIter();


		void handleApplyPriceDiscount(Product *msgProduct, int event);
		void handleResetPrice();

		OpModeEvent opModePriorToPrintMultipleLabelsMode;
		void handlePrintMultipleLabels();
		void handleLeavePrintMultipleLabelsMode();

		bool productionEntryContainsFPPItems;
		
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif

		bool processingKeyboardNetWt;
		void switchModeForKeyboardNetWt();
		void switchModeBackAfterKeyboardNetWt();

		void adjustLabelRotations(Product *newProduct);
		short offsetLabelRotation(short curRotation, short offsetRotation);

		void handleChangeRotationPlacement(GenericHfcObject *gho);

		bool pick5ValueOutOfRange;

		void setupForToggleLabelTypeMessage();

		void imageSelfServeBarcodeLabel(const HfcProductUpdateValuesMsg &hpuvm);
		void deleteSelfServeBarcodeLabel();

		//SOF-5092 Disable secondary label Flashkey
		bool secLabelTypeEnable;
		long origSecLabelTypeNum;
};

#endif //PRODUCTMANAGER_H
