#ifndef OPERATINGMODE_H
#define OPERATINGMODE_H



#include <string>
#include <queue>
#include <map>

#include "evolution.h" 
#include "ProductDevEvent.h"
#include "WeigherEvent.h"
#include "LabelApplierEvent.h"
#include "PrinterEvent.h"
#include "WrapperEvent.h"

#include "Weigher.h"
#include "Printer.h"
#include "Wrapper.h"
#include "LabelApplier.h"
#include "LabelApplierSetup.h"
#include "WrapperTest.h"
#include "WrapperSetWrapMode.h"
#include "OperationModeStatus.h"

#include "CSmartPtr.h"

#include "Configuration.h"

class WeigherManager;
class PrinterManager;
class LabelApplierManager;
class WrapperManager;
class Product;
class ProductTransaction;
class OpModePackage;
class PrinterLabelComplete;

class OperatingMode : public Hobart::CEvolMsgReceiver 
{
	typedef OperatingMode _Myt;
	typedef Hobart::CEvolMsgReceiver _Mybase;

public:
	OperatingMode(
		std::string tName,
		Product *pCurrentProduct = NULL,
		Hobart::CEvolMsgReceiver *pWeigherManager = NULL,
		Hobart::CEvolMsgReceiver *pFirstPrinter = NULL,
		Hobart::CEvolMsgReceiver *pSecondPrinter = NULL,
		Hobart::CEvolMsgReceiver *pThirdPrinter = NULL,
		Hobart::CEvolMsgReceiver *pLabelApplierManager = NULL,
		Hobart::CEvolMsgReceiver *pWrapperManager = NULL );

	OperatingMode(
		std::string tName,
		const Util::CSmartPtr<Product> &pCurrentProductSmartPtr,
		Hobart::CEvolMsgReceiver *pWeigherManager = NULL,
		Hobart::CEvolMsgReceiver *pFirstPrinter = NULL,
		Hobart::CEvolMsgReceiver *pSecondPrinter = NULL,
		Hobart::CEvolMsgReceiver *pThirdPrinter = NULL,
		Hobart::CEvolMsgReceiver *pLabelApplierManager = NULL,
		Hobart::CEvolMsgReceiver *pWrapperManager = NULL );

	virtual ~OperatingMode();

	virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

	virtual void firstPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr);
	virtual void firstPrinterDetached();

	virtual void secondPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr);
	virtual void secondPrinterDetached();

	virtual void thirdPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr);
	virtual void thirdPrinterDetached();

	virtual void weigherAttached(Hobart::CEvolMsgReceiver *pWeigherManager);
	virtual void weigherDetached();

	virtual void labelApplierAttached(Hobart::CEvolMsgReceiver *pLabelApplierManager);
	virtual void labelApplierDetached();

	virtual void wrapperAttached(Hobart::CEvolMsgReceiver *pWrapperManager, WrapperSetWrapMode::WrapMode wrapMode = WrapperSetWrapMode::UnknownMode);
	virtual void wrapperDetached();

	typedef enum OpModeType
	{
		UnknownOpMode,
		IdleMode,
		ManualMode,
		IlaMode,
		SemiWeighMode,
		AutoWeighMode,
		UwsWeighmode,
		EvolWeighMode,
		ServiceMode
	} OpModeType_t;

	OperatingMode::OpModeType getModeType();

protected:
	// variables used for Q depth in printers
	std::queue< Util::CSmartPtr<Product> > productQ;

	typedef std::map<HobRecordNum, OpModePackage *> PackageMap;
	PackageMap packageMap;

	std::string tName;
	const Product *pCurrentProduct;
	const ProductTransaction *pCurrentProductTransaction;
	const ProductTransaction *pPendingPackage;

	// These pointers are used by the modes when the Product
	// has labels on printers not in the customary order. For
	// example, a product may use the secondary printer only.
	Hobart::CEvolMsgReceiver *pWeigherManager;
	Hobart::CEvolMsgReceiver *pFirstPrinter;
	Hobart::CEvolMsgReceiver *pSecondPrinter;
	Hobart::CEvolMsgReceiver *pThirdPrinter;
	Hobart::CEvolMsgReceiver *pLabelApplierManager;
	Hobart::CEvolMsgReceiver *pWrapperManager;

	typedef enum PrinterScenarios
	{
		NoPrinters,
		PrimaryPrinterOnly,
		SecondPrinterOnly,
		ThirdPrinterOnly,

		PrimaryAndSecondPrinter,
		PrimaryAndThirdPrinter,
		SecondAndThirdPrinter,
		PrimarySecondAndThirdPrinter,

		PrimaryPrinterBakeryCLA,					//	Primary printer on a BakeryCLA station
		SecondPrinterBakeryCLA,						//	Second printer on a BakeryCLA station
		PrimaryPrinterBakeryCLAAndSecondPrinter,	//	Primary printer on a BakeryCLA station and a Second Printer on the AWS station
		PrimaryPrinterAndSecondPrinterBakeryCLA,	//	Second printer on a BakeryCLA station and a Primary Printer on the AWS station

		SupportTextOnPrimary,
		SupportTextOnSecondary,
		SupportTextOnTertiary
	}PrinterScenarios_t;

	PrinterScenarios printerScenario;
	LabelApplierSetup laSetup;

	virtual void changeProduct();
	virtual void changeProduct2(const Product *pProduct);
	virtual void setPrinterScenario();

	virtual bool isFirstPrinterManagerFake();
	virtual bool isSecondPrinterManagerFake();
	virtual bool isThirdPrinterManagerFake();

	// Sends message to WeigherManager to pass along weigher status messages.
	virtual void unlockWeigher();
	// Sends message to WeigherManager to stop passing along weigher status
	// messages.
	virtual void lockWeigher();
	virtual void setTakeLabelTime(PrinterType printerType, UINT32 tlTime);
	virtual void setTakeLabelTime(PrinterType printerType);
	virtual bool getNewProduct();
	virtual void emptyProductQ();
	virtual void preImagePrinters();
	//virtual void preImagePrinter(PrinterType printerType, PrinterManager *pMgr);
	//virtual void postImagePrinter(PrinterType printerType, PrinterManager *pMgr, UINT8 packId = 0);
	virtual void preImagePrinter(PrinterType printerType, Hobart::CEvolMsgReceiver *pMgr);
	virtual void postImagePrinter(PrinterType printerType, Hobart::CEvolMsgReceiver *pMgr, UINT8 packId = 0);
	virtual void generateNewPackage(const ProductTransaction *pPT);
	virtual void generateNewPackage2(const ProductTransaction *pPT);
	virtual const OpModePackage *getPackage(UINT16 packId);
	virtual void deletePrinterPackage(PrinterType prType, UINT16 packId);
	const ProductTransaction *removePackId(UINT16 packId);
	virtual UINT8 getPackId(const ProductTransaction *pPT);
	virtual void invalidatePackage(const ProductTransaction *pPT);
	virtual UINT8 incPackId();
	virtual void sendProductComplete(const ProductTransaction *pCPT, bool packageSuccess = true);
	virtual bool updateProductComplete(PrinterLabelComplete &prLabelComplete);

	virtual void handleProductMessage(ProductEvent productEvent, const char *recvBuffer);
	virtual void handlePrinterMessage(PrEvent prEvent, const char *recvBuffer);
	virtual void handleWrapperMessage(WrEvent wrEvent, const char *recvBuffer);
	virtual void handleLabelApplierMessage(LaEvent laEvent, const char *recvBuffer);
	virtual void handleWeigherMessage(WgEvent wgEvent, const char *recvBuffer);

	bool isPrinterHeadUp();
	void updatePrinterHeadUp(PrinterType printerType);
	void updateSizeCmdComplete(PrinterType printerType);

	virtual void emptyPackageMap(bool alsoSendProductCompleteMsg = true);

	void sendVoidLabel(PrinterType prType, PrinterManager *pPrinter);

	void setWeigherMode(WgOperatingMode opMode);
	void requestWeigherStatus();

	void setWrapperMode(WrOpMode opMode);
	void sendWrapperTest(WrapperTest::WrTestCode testCode);
	void sendWrapperAbort();
	void requestWrapperStatus();
	void requestWrapperInquire();

	void setLabelApplierMode(LaOperatingMode opMode);
	void setLabelApplierOff();
	void requestLabelApplierStatus();

	virtual void handlePrinterOverflow(UINT16 maxLabels);
	virtual bool packageMapOverflow(PrinterType prType, UINT16 maxLabels);
	virtual void sendPrinterReset(PrinterType prType);
	virtual void sendPrinterResetImmediate(PrinterType prType);
	virtual void sendPrinterOverflow(PrinterType prType);
	virtual void resetWrapperMode();
	virtual void setNewEvolWrapperError(const EvolWrapperErrorInfo::EvolWrapperError &wrapperError = EvolWrapperErrorInfo::NoWrapperError,
										const EvolWrapperErrorInfo::EvolWrapperErrorSource &wrapperErrorSource = EvolWrapperErrorInfo::NoWrapperErrorSource);
	virtual void setNewWrapperError(const WrapperErrorInfo::WrapperErrorSource &wrapperError = WrapperErrorInfo::NoErrorSource);
	virtual void clearPressStart();
	virtual void setPressStart();
	virtual void clearGhostPackage();
	virtual void setGhostPackage();
	virtual void clearPackageOverWeight();
	virtual void setPackageOverWeight();
	virtual void resetWrapper();
	virtual void sendLabelReady(LaStationID stationId, UINT8 packageId);
	virtual void sendPackageReady(UINT8 packageId);
	virtual void sendLabelTaken(LaStationID stationId, UINT8 packageId);
	virtual void requestLabelApplierInquire();
	virtual void resetLabelApplier();
	virtual void sendLabelApplierOn();
	virtual void sendLabelApplierCloseWindow(UINT16 packId);
	virtual void sendLabelApplierSetup(const Product &product);
	virtual LaPosLoc setLabelPlacement(LaPosLoc laPosition);

	virtual void handleWrapperStatus(const char *recvBuffer);
	virtual void handleWrapperEvolStatus(const char *recvBuffer);
	virtual void handleWrapperInquire(const char *recvBuffer);
	virtual void handleWrapperControl(const char *recvBuffer);
	virtual void requestLabelSize();

	virtual void handlePrinterConfigInOpMode(const char *recvBuffer);

	typedef enum AWM_WrapStates
	{
		awmWrapInitial,
		awmWrapChangingProduct,
		awmWrapOpenWindow,
		awmWrapWrap,
		awmWrapPrinterHeadUp,
		awmWrapError
	} AWM_WrapStates_t;

	AWM_WrapStates wrapStateMachine;

	bool weigherLocked;
	UINT32 takeLabelTime;

	UINT8 packId;

	bool wrapOnlyMode;
	bool machineRunning;
	bool largeMotion;

	typedef enum WM_LaStates
	{
		wmLaInitial,
		wmLaIdle,
		wmLaLabelReady,
		wmLaPackageReady,
		wmLaLabelTaken,
		wmLaIlaVoidLabel,
		wmLaError,
		wmLaDischargeError
	} WM_LaStates_t;

	WM_LaStates laStateMachine1;
	WM_LaStates laStateMachine2;
	WM_LaStates laStateMachine3;

	OperationModeStatus opModeStatus;
	OpModeType modeType;

	UINT16 firstPrinterLabelLength;
	UINT16 secondPrinterLabelLength;
	UINT16 thirdPrinterLabelLength;

	bool ghostPackage;
	HobWeight getMaxAWSPackageWeight() const;
	bool packageWeightIsTooHeavyForAWS(HobWeight pkgWeight) const;

	void sendPkgDischargeError(PrinterType prType, bool setIt = true);

	Util::CSmartPtr<Product> pCurrentProductSmartPtr;


private:
	typedef enum PrinterHeadUpState
	{
		PrinterNoHeadUp,
		PrinterHeadUp_1,
		PrinterHeadUp_2,
		PrinterHeadUp_3,
		PrinterHeadUp_12,
		PrinterHeadUp_13,
		PrinterHeadUp_23,
		PrinterHeadUp_123
	} PrinterHeadUpState_t;

	PrinterHeadUpState printerHeadUpState;

	static const HobWeight MaxAWSPkgWtAvoir   = 1500;		// 15.00 pounds
	static const HobWeight MaxAWSPkgWtAvoirDR = 15000;		// 15.000 pounds, Dual Range Support (haberrc)
	static const HobWeight MaxAWSPkgWtMetric  = 6820;		// 6.820 kg (6.820 * 2.2 lb/kg = 15.00 lb)

	Configuration *pConfig_;

	WeigherManager *weigher_;
	PrinterManager *firstPrinter_;
	PrinterManager *secondPrinter_;
	PrinterManager *thirdPrinter_;
	LabelApplierManager *labelApplier_;
	WrapperManager *wrapper_;

#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif
};
#endif	//OPERATINGMODE_H
