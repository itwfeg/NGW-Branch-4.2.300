
/**
 * @file CANPrinterManager.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Header file CAN printer devices prepack / service scale
 * Created on: Oct 24, 2018
 * @addtogroup CAN
 * @{
 */
#ifndef CANPRINTERMANAGER_H_
#define CANPRINTERMANAGER_H_
#include "PrinterManager.h"
#include "CEvolMsgReceiver.h"
#include "CHobartTimer.h"
#include "Printer.h"
#include "KpcSystemEvents.h"
#include "SystemEvent.h"

#include "HobString.h"
#include "MtdScaleData.h"
#include "MtdNGWWrapperData.h"
#include "RS485Manager.h"


class CANPrinterManager : public PrinterManager
{
	public:

		// --- Constructors and destructor ---
		CANPrinterManager( Hobart::CEvolMsgReceiver *pTranslator_, PrinterType id_ = FirstPrinter,
				LabelWidth labelWidth_ = UFWLabel, UINT16 bufferSize_ = 8192,
				UINT16 printheadSize_ = 640, UINT32 transferSize_ = 0, PrinterKind prKind_ = KPCPrinter );

		~CANPrinterManager();


		virtual void timeoutProcess( __int32 timer_id );
		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

		bool isVirtualPrintingEnabled() const;
		void setVirtualPrintingEnabled(bool enabled);


		virtual PrinterType getPrinterType() const { return stationId; }

		bool createTestLabel(PrTestLabelType newTestLabelType);
		UINT16 *testLabelData;

	protected:
		ULONG printerProbeTime;

		int probeTimeoutCntr_;

	private:
		//static const ULONG TWOSECONDS = 2;
		static const ULONG FIVESECONDS = 5;

	    Hobart::CEvolMsgReceiver *pTranslator;
	    PrinterType stationId;
		UINT32 transferSize;

		virtual void startPrintingLabel(PrinterType labelNumToPrint);
		RS485Manager *pRS485Manager;

		Util::CSmartPtr<Util::CTLSThread> cPrinterThread;

		virtual void registerEvents();

		virtual DWORD Run();

		virtual void maskCommand(UINT8 sensorMask, UINT8 sensor2Mask, UINT8 userMask);
		virtual void teachCommand(PrinterCommandType command, PrinterModeType mode);

		virtual void handlePrinterMessage(PrEvent prEvent, const char *recvBuffer, UINT16 bufSize);
		virtual void handlePrinterSysMessage(SysEvent sysEvent, const char *recvBuffer);
		void handleKpcSystemError(KpcSystemEvent event, const char *recvBuffer);

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
