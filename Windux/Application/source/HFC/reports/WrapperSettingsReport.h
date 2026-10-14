#ifndef WRAPPERSETTINGSREPORT_H
#define WRAPPERSETTINGSREPORT_H



#include "AbstractReport.h"
#include "util/types.h"


class HfcStartReport;
class WrapperSystemStatus;
class WrapperStatus;
class WrapperInquire;
class WrapperPackCodeInfo;


class WrapperSettingsReport : public AbstractReport
{
	public:
		// --- Types and subtypes ---
		typedef enum
		{
			ReceivedNone        = 0x00,

			ReceivedSysStatus   = 0x01,
			ReceivedWrStatus    = 0x02,
			ReceivedWrInquire   = 0x04,

			ReceivedAll         = ReceivedSysStatus | ReceivedWrStatus | ReceivedWrInquire
		} MessagesReceived;


		// --- Constructors and destructor ---
		WrapperSettingsReport(const HfcStartReport* requestMessage, ReportManager* reportManager,
				PrinterType type = FirstPrinter);
		~WrapperSettingsReport();


		// --- Accessor methods ---
		const std::string& getName() const { return _name_; }

		const std::string& getContentFontFamily() const { return _contentFontFamily_; }

		int getContentFontSize() const { return _contentFontSize_; }


		// --- Processing methods ---


	protected:
		void doInitialize();

		void doHandleWrapperMessage(UINT8 event, const char* buffer);
		void doHandleWrapperSystemMessage(UINT8 event, const char* buffer);


	private:
		// --- Object Data Members ---
		int packCode_;
		Flags received_;
		std::string powerupTestResultsContent_;
		std::string wrapperStatusSettingsContent_;
		std::string wrapperInquireSettingsContent_;
		std::string packCodeSettingsContent_;

		// --- Class Data Members ---
		static const std::string _name_;
		static const std::string _contentFontFamily_;
		static const int _contentFontSize_;


		// --- Processing methods ---
		void composeReport();
		void serializeSystemStatus(const WrapperSystemStatus& stat);
		void serializeWrapperStatus(const WrapperStatus& stat);
		void serializeWrapperInquire(const WrapperInquire& wi);
		void serializePackCode(const WrapperPackCodeInfo& packCode, std::string& content);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif //WRAPPERSETTINGSREPORT_H

