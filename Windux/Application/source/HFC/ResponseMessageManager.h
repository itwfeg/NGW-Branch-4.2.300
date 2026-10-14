/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef RESPONSEMESSAGEMANAGER_H
#define RESPONSEMESSAGEMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "HfcResponse.h"
#include "HobList.h"
#include "Printer.h"

#include <chrono>
#include <list>
#include <map>
#include <algorithm>

class Session;
class SessionMessage;
class HfcWeigherStatus;
class HfcPrinterStatus;
class HfcLabelApplierStatus;
class HfcWrapperStatus;
class HfcEvolWrapperStatus;
class HfcOperationModeStatus;


class ResponseMessageManager 
{
	public:

		class Response
		{
			public:
				Response(ErrorEvent dev, int code, short style, const HobString &listData = "")
					: errorDevice(dev), errorCode(code), responseStyle(style), listSerializedData(listData) {}
				~Response() {}

				ErrorEvent getErrorDevice() const {return errorDevice;}
				int getErrorCode() const {return errorCode;}
				short getResponseStyle() const {return responseStyle;}
				const HobString &getListSerializedData() const {return listSerializedData;}

				// --- Overridden operators ---
				friend bool operator==(const Response& resp1, const Response& resp2)
				{
					return ((resp1.errorDevice == ErrUnknown || resp1.errorDevice == resp2.errorDevice) &&
					        (resp1.errorCode == -1 || resp1.errorCode == resp2.errorCode)
								/*&&
					        (resp1.responseStyle == -1 || resp1.responseStyle == resp2.responseStyle)*/);
				}

				friend bool operator!=(const Response& resp1, const Response& resp2)
						{ return !operator==(resp1, resp2); }

			private:
				ErrorEvent errorDevice;
				int errorCode;
				short responseStyle;
				HobString listSerializedData;
		};
		typedef std::list<Response> responseContainer_type;

		
		ResponseMessageManager(Session *session = 0);
		virtual ~ResponseMessageManager();

		bool manageResponseAnswer(HfcResponse *answer);

		bool checkPrinterStatuses();

		void manageSelfServeMessages();
		void manageStatus(HfcWeigherStatus *status);

		void manageStatus(HfcPrinterStatus *newStatus, HfcPrinterStatus *currentStatus = 0);
		bool addPrinterMessage(int errorCode, short responseStyle = HfcResponse::RESPONSESTYLE_OK, PrinterType prType = FirstPrinter);
		bool removePrinterMessage(int errorCode, short responseStyle, PrinterType prType);
		void manageCurrentPrinterStatus(HfcPrinterStatus* currentStatus, int printerNum);

		void manageStatus(HfcLabelApplierStatus *status);
		void manageStatus(HfcWrapperStatus *status);
		void manageStatus(HfcEvolWrapperStatus *status);
		void manageStatus(HfcOperationModeStatus *status);
		
		bool addMessage(ErrorEvent errorDevice, int errorCode, short responseStyle = HfcResponse::RESPONSESTYLE_OK, const HobList *msgData = 0, bool allowDuplicates = false);
		bool removeMessage(ErrorEvent errorDevice, int errorCode, short responseStyle = HfcResponse::RESPONSESTYLE_OK);
		bool removeAllMessages(ErrorEvent errorDevice = ErrUnknown, short responseStyle = -1, bool keepSelfServeOutOfServiceMessages = false);

		bool displayingResponse() const;
		bool displayingStatusLineMessage() const;

		void resendMessage(short responseStyle);

		bool shouldBeep(ErrorEvent errorDevice, int errorCode, short responseStyle) const;

		void setDisplayingDialogs(bool displaying);

		void show();


	private:
		static const int SelfServiceErrorOffset = 500;
		static const int PrinterErrorOffset = 100;

		Session *mySession;

		responseContainer_type responses;
		responseContainer_type statusLineResponses;

		bool displayingDialogs;
		HfcPrinterStatus* newPrinterStatus;
		HfcPrinterStatus* currentPrinterStatus;

		bool addMessageToQueue(Response r, responseContainer_type *queue, bool allowDuplicates = false);
		bool removeMessageFromQueue(Response r, responseContainer_type *queue);
		
		void sendMessage(ErrorEvent errorDevice, int errorCode, short responseStyle, const HobString &msgData = "");

		bool isSelfServeOutOfServiceMessage(int errorCode);
		bool isSelfServeSevereWarningMessage(int errorCode);                          // new line
		int translateSelfServeOutOfServiceMessage(int errorCode);

		bool isPrinterMessage(int errorCode);
		int translatePrinterMessage(int errorCode, PrinterType prType);

		void showQueue(responseContainer_type *queue);

		bool sentNegWeightMessNotRemovedYet;
		bool sentOutOfTiltMessNotRemovedYet;

		typedef std::chrono::system_clock ClockType;
		typedef std::chrono::milliseconds DurationMillisecondsType;

		typedef std::pair<ErrorEvent, int> ErrorDeviceCode;
		typedef std::map<ErrorDeviceCode, ClockType::time_point> ResponseTimestampMap;

		ResponseTimestampMap responseTimes;

		typedef std::map<ErrorDeviceCode, DurationMillisecondsType> PreventSpamMessageMap;

		PreventSpamMessageMap messageWaitTimes;
		bool canopyError;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //RESPONSEMESSAGEMANAGER_H
