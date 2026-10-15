#ifndef PRINTERMANAGER_H
#define PRINTERMANAGER_H



#include <queue>
#include <deque>
#include <map>
#include <vector>
#include <string>
#include <functional>

#include "Printer.h"
#include "PrinterPrinterType.h"

class Product;
class Graphic;
class ProductTransaction;
class LabelType;
class RendererManager;

#include "LabelStockDescriptor.h"
#include "ImageControl.h"
#include "BitMap.h"
#include "Pen.h"

#include "SystemEvent.h"
#include "RendererEvent.h"

#include "PrinterSystemConfiguration.h"
#include "PrinterConfiguration.h"
#include "PrinterStatus.h"
#include "PrinterSensors.h"
#include "PrinterStockMetrics.h"
#include "PrinterCutterStatus.h"
#include "PrinterPreImage.h"
class PrinterTeachTable;
class PrinterCommand;

class Configuration;
#include "HobString.h"
#include "CEvolMsgReceiver.h"
#include "CHobartTimer.h"
#include "MtdScaleData.h"
#include "MtdNGWWrapperData.h"
#include "RS485Manager.h"
#include "KpcSystemEvents.h"
#include "OpModeEvent.h"	//SOF-3847 3848
#include "ConfigurationEvent.h"
#include "LabelApplierEvent.h"
#include <SessionEvent.h>	//SOF-5482

class PrinterManager : public Hobart::CEvolMsgReceiver, public Hobart::CHobartTimer, public Util::CRunnableClass
{
	public:
		// --- Types and subtypes ---
		typedef enum PrinterState
		{
			PrinterUnsized,
			PrinterTryingToSize,
			PrinterFinding,
			PrinterReadyToPreImage,
			PrinterPreImaging,
			PrinterReadyToPrint,
			PrinterPostImaging,
			PrinterReadyToXfer,
			PrinterXferringLabel,
			PrinterTakingLabel,
			PrinterLabelIsReady,
			PrinterHeadDisengaged,
			PrinterPriorityError,
			PrinterPriorityErrorStop,		//SOF-3861
			PrinterPriorityErrorRestart,	//SOF-3861
			PrinterTakingLabelCombined,
			PrinterWaitToExpelAndCut,		//SOF-5066
			PrinterWaitInterlock			//SOF-5083
		} PrinterState_t;

		typedef std::map<std::string, BitMap *, std::greater<std::string> > bitmapsContainer;
		typedef bitmapsContainer::iterator bitmapsContainerIterator;
		typedef std::map<std::string, Graphic *, std::greater<std::string> > graphicsContainer;
		typedef graphicsContainer::iterator graphicsContainerIterator;

		typedef std::vector<BitMap*> labelBitMapsContainer;
		typedef labelBitMapsContainer::iterator labelBitMapsIterator;


		// --- Constructors and destructor ---
		PrinterManager(Hobart::CEvolMsgReceiver *pMyPrinter, PrinterTeachTable *pMyTeachTable,
				PrinterType printerType = FirstPrinter, LabelWidth labelWidth = UFWLabel,
				UINT16 bufferSize = 8192, UINT16 printheadSize = 640, UINT16 physicalAddress = 0x4646,
				USBD_NODE_ID usbNodeId = (USBD_NODE_ID) -1, PrinterKind prKind = EppPrinter, ULONG transferSize_ = 0 );
		~PrinterManager();


	public:
		bool isVirtualPrintingEnabled() const;
		void setVirtualPrintingEnabled(bool enabled);

		bool isExternalLabelPrinterManager() const;   // PJR-444 - SOF-5063 - Print wider labels on Zebra printer
		bool isFakePrinterManager() const;
		PrinterKind getPrinterKind() const;
		PrinterType getPrinterType() const;

		bool isLabelToBePrinted(PrinterType printerNum);
		void setLabelToBePrinted(PrinterType printerNum, bool printIt);
		void setAllLabelsToBePrinted(bool printIt);

		USBD_NODE_ID getUsbNodeId() const;

		bool getStreamingLabels() const;
		void setStreamingLabels(bool streaming);

		bool isContinuousStock() const;
		bool isDieCutloaded() const;
		
		bool createTestLabel(PrTestLabelType newTestLabelType);
		UINT16 *getTestLabelData() const;
		UINT16 *pBmpLabelBuffer;

		bool isCutterInstalledAndEnabled() const;

	protected:
		
		ULONG takeLabelTime;
		bool takeLabelError;

		ULONG printerProbeTime;

		//#define PR_DEFAULTS_TEST
		#ifdef PR_DEFAULTS_TEST
		unsigned long printerTestMsgTime;
		#endif

		int probeTimeoutCntr_;

		ULONG requestSensorTime;

		PrinterStateType trackSensors;

		ULONG printheadPowerTime;

		ULONG continuousLabelTime;

		PrinterState printerState;
		PrinterModeType printerMode;
		bool printHeelFirst;

		bool imageFlip;
#if defined ( __linux__ )
		unsigned int adjTopMargin;	//SOF-4592
#else
		int adjTopMargin;			//SOF-4592
#endif
		ULONG transferSize;
		bool wasDotWearEnabled;

#ifdef PR_DEFAULTS_TEST
		int	testMsgCount;
		int testCounter;
		int failedErases;
#endif

		virtual void timeoutProcess( __int32 timer_id );
		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

		bool isWrapperPrinter() const;
		bool isCounterScalePrinter() const;

	private:

		static const size_t NUM_LABEL_BUFFERS = 8;
		static const ULONG FOURSECONDS = 10000;

		std::vector<UINT8> marginalBits;

		typedef struct productTransactionId
		{
			ProductTransaction *pProductTransaction;
			HobRecordNum transactionNum;	//	PT # 164 - Redundant, but use in case pProductTransaction pointer is deleted by the Product.
			UINT8 packId;					/*either physical or virtual*/
			Util::CSmartPtr<Product> productSmartPtr;
		}   productTransactionId_t;

		Util::CSmartPtr<Util::CTLSThread>	printerManagerThread;

		PrinterSystemConfiguration printerSysConfig;
		HobString printerTraceString;

		bool receivedPrinterInquire;
		bool requestedPrinterInquire;
		bool resetPrinter;
		bool resetUPCBarcode;				// if the price og UPC barcode > 100, it can be swapped to something else.  Afterwards reset it
		
		bool printLabelCentered;
		bool getLabelWidthFromStatus;
		bool initialDotWearComplete;

	    Hobart::CEvolMsgReceiver *pMyPrinter;
		PrinterType myPrinterType;
		PrinterKind prKind;

		PrinterConfiguration printerConfig;
		bool requestSensorReading;
		PrinterTeachTable *pMyTeachTable;
		bool isCutterInstalled;				// physically installed and attached
		bool isCutterEnabled;				// it can be physically installed, but this will turn it off and on.
		DotWearHeadStatus lastDotStatus;
		UINT16 printheadSize;
		UINT16 physicalAddress;
		USBD_NODE_ID usbNodeId;

		UINT16 measuredSize;
		UINT16 actualSize;

		bool outOfDataBuffers;
		UINT16 bufferSize;

		UINT16 centeringOffset;

		LabelWidth labelWidth;
		UINT32 labelDataSize;			// zzz lap - used in conjunction with labelData pointer directly below.
		UINT16 *labelData;				// zzz lap - points into the buffer to be sent to the printer for printing.  There probably does not need to be multiple of these, since we can only actually print one label at a time.  This pointer will be used to point to the label that is in the process of being printed.
		UINT16 *pHeelBuffer;
		UINT16 *pHeelBufferStart;
		Configuration *pConfig;


		RendererManager *pMyRenderer;
		UINT16 myRendererSessionId;

		Product *pCurrentProduct;
		Util::CSmartPtr<Product> currentProductSmartPtr;
		Util::CSmartPtr<Product> newProductSmartPtr;

		std::deque<productTransactionId> productTransactionQ;

		ProductTransaction *pCurrentProductTransaction;
		UINT8 currentPackId;
		HobRecordNum currentTransNum;		//	PT # 164 - Keep track of the current transaction number in case  pCurrentProductTransaction pointer is deleted by the product.

		LabelType *pCurrentLabel;

		bool labelPresent;
		bool printerResetting;
		bool printerCalibrating;
		bool printerAdvancing;

		bool enableSizing;

		bool noCutOption;	//SOF-3847 3848
		bool reTeach;		//SOF-3933
		
		bool firstBoot;		//SOF-5130
		#if defined ( __linux__ )
		#define BOARD_SITE_FILE_NAME "/etc/hobart/boardsite"
		#endif
		
		INT32 linesToPrint;

		bool sendTestLabel;
		bool printingVoidLabel;
		bool continuousTestLabels;
		PrTestLabelType testLabelType;
		PrTestLabelType saveTestLabelType;
		PrinterCommandOptions applySecurityLabel;

		ULONG tstLabelWaitCnts_;			
		static const unsigned int stopTimeout_ = 50;	// 50 * 10 palsleep = 500mS

		UINT16 *testLabelData;
		UINT32 testLabelDataSize;

		INT32 ticksPerSecond;

		INT32 xferLabelStart;
		INT32 xferLabelFinish;
		
		ImageControl* imageControlBlock[NewMaxPrinterTypes];  // using NewMaxPrinterTypes for ExtraPrinter_UsedForSelfServeScreenBarcodeLabel,  PJR-383
		UINT16 *pDestinationData;

		bool pendingLabelXfer;

		bool pjr413HasQRLabel;

		PrinterStockMetrics printerStockMetrics;
		LabelStockDescriptor stockLoaded;

		//Windux IR #82

		void CopyLabelStockDescriptor( PrinterType whichPrinter );
		
		LabelStockDescriptor printer1Descriptor;
		LabelStockDescriptor printer2Descriptor;
		LabelStockDescriptor printer3Descriptor;
		LabelStockDescriptor printer4Descriptor;  // added for ExtraPrinter_UsedForSelfServeScreenBarcodeLabel,  PJR-383

		BitMap *bitMap[NewMaxPrinterTypes];  // using NewMaxPrinterTypes for ExtraPrinter_UsedForSelfServeScreenBarcodeLabel,  PJR-383
		BitMap *rotatedBitMap[MaxPrinterTypes];
		Pen pen;

		BitMap storeGraphicBitMap;
		BitMap userGraphicBitMap1;
		BitMap userGraphicBitMap2;
		BitMap userGraphicBitMap3;
		BitMap userGraphicBitMap4;

		PrinterStatus lastStatusMsg;
		PrinterSensors lastSensorsMsg;
		PrinterCutterStatus	lastCutterStatusMsg;

		bool executingCommand;
		std::queue<PrinterCommand *> commandQ;

		bitmapsContainer indirectBitmaps;
		graphicsContainer indirectGraphics;

		labelBitMapsContainer embeddedLabelBitMaps_;

		bool virtualPrintingEnabled;
		PrinterType currentVirtuallyPrintingLabelNum;
		bool preImageALabel(LabelType *pCurrentLabel, PrinterType whichPrinter);
		void startPrintingLabel(PrinterType labelNumToPrint);

		bool usingContinuousStock;
		//SOF-5355 hack hack!! in for demo with jim 	bool printToScreenFlag_;	 
		ULONG previousPrinterCounts_;
		ULONG printerCounts_;
		ULONG codeCheckerFrequency_ = 100;

		unsigned char *labelBuffer_[NUM_LABEL_BUFFERS];
		size_t labelBufferIndex_;

		static const unsigned int _edgeMargin_;

		RS485Manager *pRS485Manager;		

#ifdef FAKE_SOCKET_WEIGHER
		bool agentAttached_;
#endif
		bool mfgKey_;	//SOF-5482

		PrinterManager();

		virtual DWORD Run();

#ifdef FAKE_SOCKET_WEIGHER
		void sendRequestVirtualLabel(UINT16 bytesToXmit, UINT16 *pUint16, INT32 linesToPrint);
#endif

		void teachPrinter(PrinterModeType mode, UINT8 sensorMask = LabelTakenBit, UINT8 sensor2Mask = OutOfDataBuffersBit,
			UINT8 userMask = TakingBit);
		void teachPrinterNormalSizing();
		void maskCommand(UINT8 sensorMask, UINT8 sensor2Mask, UINT8 userMask);
		void teachCommand(PrinterCommandType command, PrinterModeType mode);
		PrinterCommand * popCommandQ();
		void queueCommand(PrinterCommandType printerCommandType = PrintCommand,
						PrinterCommandOptions printerCommandOptions = PrNoSecurityLabel,
						PrinterIndirection printerIndirection = PrNoIndirection,
						UINT16 value = 0);
		void handlePrinterMessage(PrEvent prEvent, const char *recvBuffer, UINT16 bufSize);
		void handlePrinterSysMessage(SysEvent sysEvent, const char *recvBuffer);
		void handleKpcSystemError(KpcSystemEvent event, const char *recvBuffer);
		void handleRendererMessage(RdEvent rdEvent, const char *recvBuffer);
		void handleModeMessage( OpModeEvent mEvent, const char *recvBuffer);
		bool handleMode( OpModeEvent mEvent );
		void handleConfigMessage( ConfigurationEvent cEvent,const char *recvBuffer );	//SOF-3980
        void handleApplierMessage( LaEvent event, const char *recvBuffer );
        void handleSessionDevMessage(SessionEvent e, const char *recvBuffer);	//SOF-5482

		void emptyProductTransactionQ(bool dispatchCompleteMessage);
		void emptyIndirectGraphics();
		void emptyEmbeddedLabelBitMaps();
		void preImageLabel();
		void updateFormPreImageFields(const ImageControl& ic);
		void updateFormPostImageFields(const ImageControl& ic);
		std::string cleanTrailingCurrencyMarks(std::string priceStr);
		HobString cleanHobTrailingCurrencyMarks(HobString priceStr);

		void sendPrinterFlush();
		void sendPrinterEnableTakeup();
		void sendPrinterKind();
		void sendPrinterReset(UINT8 location);
		void sendRequestLabelTransfer(UINT16 bytesToXmit, UINT16 *pUint16, bool sendSetupXferMessage = true, bool cancelPendingTransfer = false);
		void sendLabelData();
		void sendLabelDataHead();
		void sendLabelDataHeel();
		void flipLabelBufferHeel(int size);
		UINT16 flipWord(UINT16 labelWord);
		void sendPrinterType();

		void requestWakeupMsg();
		void requestStatusMsg();
		void requestInquireMsg();
		void requestSensorMsg();
		void requestSystemWakeupMsg();
		void requestSystemStatusMsg();
		void requestSystemInquireMsg();
		void requestHeadDotStatus();
		void requestHeadDotWear();
		void requestCutterStatus();

		void sendPrinterConfig(PrConfigType configType);
		void sendPrinterSystemConfig(SysConfigType sysConfigType);

		//	PT # 164 - Pass in the current transaction number in case pPT pointer is deleted by the product.
		void dispatchLabelComplete(const ProductTransaction *pPT, HobRecordNum transNum, bool labelOk);
		void dispatchLabelReady(const ProductTransaction *pPT, HobRecordNum transNum, UINT8 packId);
		void dispatchLabelTaken(const ProductTransaction *pPT, HobRecordNum transNum, UINT8 packId);

		//bool createTestLabel(PrTestLabelType newTestLabelType);
		void printTestLabel();
		void updateHorizontalOffset();
		void createCheckerboardLabel(UINT32 startingRow, UINT32 length);
		void createVerticalLinesLabel(UINT32 startingRow, UINT32 length);
		void createHorizontalBarsLabel(UINT32 startingRow, UINT32 length);
		void createHighPowerLabel(UINT32 startingRow, UINT32 length);
		void createLabelSensorLabel(UINT32 startingRow, UINT32 length);
		void flipLabelBufferHeelForBMPImage(int size);

		void receivedPrinterFeedback();

		bool printerBusy();
		void emptyCommandQ();
		void clearLabelDimensions();
		void checkLabelWidth(LabelWidth printerStatusLabelWidth);

		void processEmbeddedImageControlBlocks(ImageControl* ic, LabelType* label);

		void bitSet(UINT16 startBit, UINT16 numBits, UINT8 *buffer);

		bool isProductTransactionQEmpty();
		void peekProductTransactionQ(productTransactionId *pProdPackId);
		void popProductTransactionQ(productTransactionId *pProdPackId);
		bool removeProductTransactionFromQ(ProductTransaction *pPT);
		void pushProductTransactionQ(ProductTransaction *pPT, UINT8 packId, Util::CSmartPtr<Product> pProductSmartPtr);

		void adjustMargins( const ImageControl* ic, unsigned char *buffer );

		int testPrinter(int i);

		void writeScaleDataThread();
		bool isCombinedVirtualPrint();
		
		bool testLabelPrintInProgress;

		mutable Util::CSmartCriticalSection  printerManagerCS;

		enum PrinterManagerTimer
		{
			takeLabelTimerExpired = 1,
			printerProbe,
			sensorTimerExpired,
			printheadPowerTimerExpired,
			continuousLabelTimerExpired,
			labelStatisticsTimer,
			testLabelTimerTimeout,
			statisticsTimer,
			PrinterTests		
		};

		bool labelsToBePrinted[MaxPrinterTypes];

		bool streamingLabels;
		bool cassetteTest;
		int isLabelFormAccumulateItemsItemLabelType(UINT16 verticalJustifyNum) const;

		std::list<ImageControl*> doomedImageControl;
		std::list<BitMap*> doomedBitMap;

		void deleteImageControl( ImageControl *obj, int count = 16 );
		void deleteBitMap( BitMap *obj, int count = 32 );


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif //PRINTERMANAGER_H
