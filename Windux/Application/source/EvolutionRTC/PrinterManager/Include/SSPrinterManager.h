
/**
 * @file SSPrinterManager.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Header file Self service scale printer manager
 * Created on: Oct 24, 2018
 * @addtogroup CAN
 * @{
 */
#ifndef SSPRINTERMANAGER_H_
#define SSPRINTERMANAGER_H_
#include "CEvolMsgReceiver.h"
#include "CHobartTimer.h"
#include "PrinterManager.h"
#include "Printer.h"
#include "KpcSystemEvents.h"
#include "SystemEvent.h"

#include "HobString.h"
#include "MtdScaleData.h"
#include "MtdNGWWrapperData.h"
#include "RS485Manager.h"


class SSPrinterManager : public PrinterManager
{
	public:

		// --- Constructors and destructor ---
		SSPrinterManager( Hobart::CEvolMsgReceiver *pTranslator_, PrinterType id_ = FirstPrinter,
				LabelWidth labelWidth_ = UFWLabel, UINT16 bufferSize_ = 8192,
				UINT16 printheadSize_ = 640, UINT32 transferSize_ = 0, PrinterKind prKind_ = KPCPrinter );

		~SSPrinterManager();


		virtual void timeoutProcess( __int32 timer_id );
		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

		virtual bool isVirtualPrintingEnabled() const;
		virtual void setVirtualPrintingEnabled(bool enabled);


		virtual PrinterType getPrinterType() const { return stationId; }

		virtual bool createTestLabel(PrTestLabelType newTestLabelType);


	protected:
		ULONG printerProbeTime;

		int probeTimeoutCntr_;

	private:
		//static const ULONG TWOSECONDS = 2;
		static const ULONG FIVESECONDS = 5;

		LabelWidth labelWidth;
	    UINT16 bufferSize;
		UINT16 printheadSize;
		PrinterKind prKind;

		Hobart::CEvolMsgReceiver *pTranslator;
	    PrinterType stationId;
		UINT32 transferSize;

		PrinterTeachTable *pMyTeachTable;
		bool isCutterInstalled;				// physically installed and attached
		bool isCutterEnabled;				// it can be physically installed, but this will turn it off and on.
		DotWearHeadStatus lastDotStatus;
		
		UINT16 measuredSize;
		UINT16 actualSize;

		bool outOfDataBuffers;
		UINT16 centeringOffset;

		UINT32 labelDataSize;			
		UINT16 *labelData;				
		UINT16 *pHeelBuffer;
		UINT16 *pHeelBufferStart;
		Configuration *pConfig;
		bool printLabelCentered;

		Product *pCurrentProduct;
		Util::CSmartPtr<Product> currentProductSmartPtr;
		Util::CSmartPtr<Product> newProductSmartPtr;
		typedef struct productTransactionId
		{
			ProductTransaction *pProductTransaction;
			HobRecordNum transactionNum;	
			UINT8 packId;					/*either physical or virtual*/
			Util::CSmartPtr<Product> productSmartPtr;
		}   productTransactionId_t;

		std::deque<productTransactionId> productTransactionQ;

		ProductTransaction *pCurrentProductTransaction;
		UINT8 currentPackId;
		HobRecordNum currentTransNum;		

		virtual void startPrintingLabel(PrinterType labelNumToPrint);
		RS485Manager *pRS485Manager;

		Util::CSmartPtr<Util::CTLSThread> cPrinterThread;
		RendererManager *pMyRenderer;
		UINT16 myRendererSessionId;

		LabelType *pCurrentLabel;

		bool labelPresent;
		bool printerResetting;
		bool printerCalibrating;
		bool printerAdvancing;

		bool enableSizing;

		bool noCutOption;
		bool reTeach;	
				
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
		
		ImageControl* imageControlBlock[NewMaxPrinterTypes];  
		UINT16 *pDestinationData;

		bool pendingLabelXfer;

		PrinterStockMetrics printerStockMetrics;
		LabelStockDescriptor stockLoaded;

		void CopyLabelStockDescriptor( PrinterType whichPrinter );
		
		LabelStockDescriptor printer1Descriptor;
		LabelStockDescriptor printer2Descriptor;
		LabelStockDescriptor printer3Descriptor;
		LabelStockDescriptor printer4Descriptor;  

		BitMap bitMap[NewMaxPrinterTypes];  
		BitMap rotatedBitMap[MaxPrinterTypes];
		Pen pen;

		BitMap storeGraphicBitMap;
		BitMap userGraphicBitMap1;
		BitMap userGraphicBitMap2;
		BitMap userGraphicBitMap3;
		BitMap userGraphicBitMap4;

		PrinterStatus lastStatusMsg;
		PrinterSensors lastSensorsMsg;
		PrinterCutterStatus	lastCutterStatusMsg;

		// Copied from PrinterManager.h - not sure why they are not here to begin with. May have to #ifdef WINNT them in
		PrinterConfiguration printerConfig;
		bool testLabelPrintInProgress;

		bool executingCommand;
		std::queue<PrinterCommand *> commandQ;

		bitmapsContainer indirectBitmaps;
		graphicsContainer indirectGraphics;

		labelBitMapsContainer embeddedLabelBitMaps_;

		bool streamingLabels;
		bool virtualPrintingEnabled;
		PrinterType currentVirtuallyPrintingLabelNum;
		virtual bool preImageALabel(LabelType *pCurrentLabel, PrinterType whichPrinter);
		

		bool usingContinuousStock;
		bool labelWidthStatus;

		ULONG previousPrinterCounts_;
		ULONG printerCounts_;
		ULONG codeCheckerFrequency_ = 100;
		static const size_t NUM_LABEL_BUFFERS = 8;
		static const ULONG FOURSECONDS = 10000;

		std::vector<UINT8> marginalBits;

		unsigned char *labelBuffer_[NUM_LABEL_BUFFERS];
		size_t labelBufferIndex_;

		bool resetPrinter;
		bool requestSensorReading;

		virtual void registerEvents();

		virtual DWORD Run();

		virtual void maskCommand(UINT8 sensorMask, UINT8 sensor2Mask, UINT8 userMask);
		virtual void teachCommand(PrinterCommandType command, PrinterModeType mode);

		virtual void handlePrinterMessage(PrEvent prEvent, const char *recvBuffer, UINT16 bufSize);
		virtual void handlePrinterSysMessage(SysEvent sysEvent, const char *recvBuffer);
		void handleKpcSystemError(KpcSystemEvent event, const char *recvBuffer);

		virtual void sendPrinterReset( UINT8 location );
		virtual void sendPrinterConfig(PrConfigType configType);
		virtual void sendPrinterFlush();
		virtual void sendPrinterEnableTakeup();

		virtual void sendRequestLabelTransfer(UINT16 bytesToXmit, UINT16 *pUint16, bool sendSetupXferMessage = true, bool cancelPendingTransfer = false);

		virtual void sendLabelDataHead();
		virtual void sendLabelDataHeel();
		virtual void sendPrinterType();


		virtual void requestStatusMsg();
		virtual void requestInquireMsg();
		virtual void requestSensorMsg();

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
			PrinterTest
		};


};










#endif
